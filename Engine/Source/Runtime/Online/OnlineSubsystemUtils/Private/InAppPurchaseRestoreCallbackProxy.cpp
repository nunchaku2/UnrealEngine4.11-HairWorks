// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "OnlineSubsystemUtilsPrivatePCH.h"
#include "InAppPurchaseRestoreCallbackProxy.h"

//////////////////////////////////////////////////////////////////////////
// UInAppPurchaseRestoreCallbackProxy

UInAppPurchaseRestoreCallbackProxy::UInAppPurchaseRestoreCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WorldPtr = nullptr;
}


void UInAppPurchaseRestoreCallbackProxy::Trigger(APlayerController* PlayerController)
{
	bFailedToEvenSubmit = true;

	WorldPtr = (PlayerController != nullptr) ? PlayerController->GetWorld() : nullptr;
	if (APlayerState* PlayerState = (PlayerController != nullptr) ? PlayerController->PlayerState : nullptr)
	{
		if (IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get())
		{
			IOnlineStorePtr StoreInterface = OnlineSub->GetStoreInterface();
			if (StoreInterface.IsValid())
			{
				bFailedToEvenSubmit = false;

				// Register the completion callback
				InAppPurchaseRestoreCompleteDelegate = FOnInAppPurchaseCompleteDelegate::CreateUObject(this, &UInAppPurchaseRestoreCallbackProxy::OnInAppPurchaseRestoreComplete);
				InAppPurchaseRestoreCompleteDelegateHandle = StoreInterface->AddOnInAppPurchaseRestoreCompleteDelegate_Handle(InAppPurchaseRestoreCompleteDelegate);

				// Set-up, and trigger the transaction through the store interface
				ReadObject = MakeShareable(new FOnlineInAppPurchaseRestoreRead());
				FOnlineInAppPurchaseRestoreReadRef ReadObjectRef = ReadObject.ToSharedRef();
				StoreInterface->RestorePurchases(ReadObjectRef);
			}
			else
			{
				FFrame::KismetExecutionMessage(TEXT("UInAppPurchaseRestoreCallbackProxy::Trigger - In-App Purchases are not supported by Online Subsystem"), ELogVerbosity::Warning);
			}
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("UInAppPurchaseRestoreCallbackProxy::Trigger - Invalid or uninitialized OnlineSubsystem"), ELogVerbosity::Warning);
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("UInAppPurchaseRestoreCallbackProxy::Trigger - Invalid player state"), ELogVerbosity::Warning);
	}

	if (bFailedToEvenSubmit && (PlayerController != NULL))
	{
		OnInAppPurchaseRestoreComplete(EInAppPurchaseState::Failed);
	}
}


void UInAppPurchaseRestoreCallbackProxy::OnInAppPurchaseRestoreComplete(EInAppPurchaseState::Type CompletionState)
{
	RemoveDelegate();
	SavedPurchaseState = CompletionState;
	if (CompletionState == EInAppPurchaseState::Restored)
	{
		SavedProductInformation = ReadObject->ProvidedRestoreInformation;
	}
    
	if (UWorld* World = WorldPtr.Get())
	{
		DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.DelayInAppPurchaseRestoreComplete"), STAT_FSimpleDelegateGraphTask_DelayInAppPurchaseRestoreComplete, STATGROUP_TaskGraphTasks);

		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateLambda([=](){

				OnInAppPurchaseRestoreComplete_Delayed();

			}),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DelayInAppPurchaseRestoreComplete), 
			nullptr, 
			ENamedThreads::GameThread
		);
    }

	ReadObject = NULL;
}


void UInAppPurchaseRestoreCallbackProxy::OnInAppPurchaseRestoreComplete_Delayed()
{    
	if (SavedPurchaseState == EInAppPurchaseState::Restored)
	{
		OnSuccess.Broadcast(SavedPurchaseState, SavedProductInformation);
	}
	else
	{
		OnFailure.Broadcast(SavedPurchaseState, SavedProductInformation);
	}
}


void UInAppPurchaseRestoreCallbackProxy::RemoveDelegate()
{
	if (!bFailedToEvenSubmit)
	{
		if (IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get())
		{
			IOnlineStorePtr InAppPurchases = OnlineSub->GetStoreInterface();
			if (InAppPurchases.IsValid())
			{
				InAppPurchases->ClearOnInAppPurchaseRestoreCompleteDelegate_Handle(InAppPurchaseRestoreCompleteDelegateHandle);
			}
		}
	}
}


void UInAppPurchaseRestoreCallbackProxy::BeginDestroy()
{
	ReadObject = NULL;
	RemoveDelegate();

	Super::BeginDestroy();
}


UInAppPurchaseRestoreCallbackProxy* UInAppPurchaseRestoreCallbackProxy::CreateProxyObjectForInAppPurchaseRestore(class APlayerController* PlayerController)
{
	UInAppPurchaseRestoreCallbackProxy* Proxy = NewObject<UInAppPurchaseRestoreCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->Trigger(PlayerController);
	return Proxy;
}
