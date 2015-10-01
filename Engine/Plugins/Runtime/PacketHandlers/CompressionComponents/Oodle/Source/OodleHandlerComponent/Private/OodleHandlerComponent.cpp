// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "OodleHandlerComponentPCH.h"


DEFINE_LOG_CATEGORY(OodleHandlerComponentLog);


#if HAS_OODLE_SDK
#define OODLE_INI_SECTION TEXT("OodleHandlerComponent")


// @todo #JohnB: Find a better solution than this, for handling the maximum packet size. This is far higher than it needs to be.
#define MAX_OODLE_PACKET_SIZE 65535


DEFINE_STAT(STAT_Oodle_BytesRaw);
DEFINE_STAT(STAT_Oodle_BytesCompressed);


FString GOodleSaveDir = TEXT("");


// @todo #JohnB: You need to integrate the dictionary generation into a commandlet, to support FArchive-format dictionaries
//					(alternative - and do it this way first! - adjust OodleTrainer to read/write the FArchive version files)

// @todo #JohnB: You need context-sensitive data capturing; e.g. you do NOT want to be capturing voice channel data


// Saved compressor model file :
// must match example_packet - or whatever you use to write the state file
struct OodleNetwork1_SavedStates_Header
{
#define ON1_MAGIC	0x11235801
	U32 magic;
	U32 compressor;
	U32 ht_bits;
	U32 dic_size;
	U32 oodle_header_version;
	U32 dic_complen;
	U32 statecompacted_size;
	U32 statecompacted_complen;

	OodleNetwork1_SavedStates_Header()
		: magic(0)
		, compressor(0)
		, ht_bits(0)
		, dic_size(0)
		, oodle_header_version(0)
		, dic_complen(0)
		, statecompacted_size(0)
		, statecompacted_complen(0)
	{
	}
};

// OODLE
OodleHandlerComponent::OodleHandlerComponent()
	: BytesSaved(0)
#if EXAMPLE_CAPTURE_FORMAT
	, PacketLogFile(NULL)
	, bPacketLogAppend(false)
#else
	, InPacketLog(NULL)
	, OutPacketLog(NULL)
#endif
	, Mode(EOodleHandlerMode::Release)
	, ServerDictionary()
	, ClientDictionary()
#if 0
	, Dictionary(NULL)
	, SharedDictionaryData(NULL)
	, CompressorState(NULL)
#endif
{
	SetActive(true);
}

OodleHandlerComponent::~OodleHandlerComponent()
{
#if EXAMPLE_CAPTURE_FORMAT
	// @todo #JohnB: PacketLogfile is not deleted. Doesn't matter though, as this is being deprecated.
#else
	if (OutPacketLog != NULL)
	{
		OutPacketLog->Close();
		OutPacketLog->DeleteInnerArchive();

		delete OutPacketLog;

		OutPacketLog = NULL;
	}

	if (InPacketLog != NULL)
	{
		InPacketLog->Close();
		InPacketLog->DeleteInnerArchive();

		delete InPacketLog;

		InPacketLog = NULL;
	}
#endif

	if (ServerDictionary.Dictionary != NULL)
	{
		free(ServerDictionary.Dictionary);
	}

	if (ServerDictionary.SharedDictionaryData != NULL)
	{
		free(ServerDictionary.SharedDictionaryData);
	}

	if (ServerDictionary.CompressorState != NULL)
	{
		free(ServerDictionary.CompressorState);
	}


	if (ClientDictionary.Dictionary != NULL)
	{
		free(ClientDictionary.Dictionary);
	}

	if (ClientDictionary.SharedDictionaryData != NULL)
	{
		free(ClientDictionary.SharedDictionaryData);
	}

	if (ClientDictionary.CompressorState != NULL)
	{
		free(ClientDictionary.CompressorState);
	}

	Oodle_Shutdown_NoThreads();
}

void OodleHandlerComponent::InitFirstRunConfig()
{
	// Check that the OodleHandlerComponent section exists, and if not, init with defaults
	if (!GConfig->DoesSectionExist(OODLE_INI_SECTION, GEngineIni))
	{
		GConfig->SetString(OODLE_INI_SECTION, TEXT("Mode"), TEXT("Training"), GEngineIni);
		GConfig->SetString(OODLE_INI_SECTION, TEXT("PacketLogFile"), TEXT("PacketDump"), GEngineIni);

#if EXAMPLE_CAPTURE_FORMAT
		GConfig->SetBool(OODLE_INI_SECTION, TEXT("bPacketLogAppend"), false, GEngineIni);
#endif

		GConfig->SetString(OODLE_INI_SECTION, TEXT("ServerDictionary"), TEXT(""), GEngineIni);
		GConfig->SetString(OODLE_INI_SECTION, TEXT("ClientDictionary"), TEXT(""), GEngineIni);
	}
}

void OodleHandlerComponent::Initialize()
{
	InitFirstRunConfig();

#if EXAMPLE_CAPTURE_FORMAT
	// Class config variables
	GConfig->GetBool(OODLE_INI_SECTION, TEXT("bPacketLogAppend"), bPacketLogAppend, GEngineIni);
#endif

	// Mode
	FString ReadMode;
	GConfig->GetString(OODLE_INI_SECTION, TEXT("Mode"), ReadMode, GEngineIni);

	if(ReadMode == TEXT("Training"))
	{
		Mode = EOodleHandlerMode::Training;
	}
	else if(ReadMode == TEXT("Release"))
	{
		Mode = EOodleHandlerMode::Release;
	}
	// Default
	else
	{
		Mode = EOodleHandlerMode::Release;
	}

	switch(Mode)
	{
	case EOodleHandlerMode::Training:
		{
			if (Handler->Mode == Handler::Mode::Server)
			{
				IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
				FString ReadOutputLogDirectory = FPaths::Combine(*GOodleSaveDir, TEXT("Server"));
				FString BaseFilename;

				PlatformFile.CreateDirectoryTree(*ReadOutputLogDirectory);
				PlatformFile.CreateDirectoryTree(*FPaths::Combine(*ReadOutputLogDirectory, TEXT("Input")));
				PlatformFile.CreateDirectoryTree(*FPaths::Combine(*ReadOutputLogDirectory, TEXT("Output")));
				GConfig->GetString(OODLE_INI_SECTION, TEXT("PacketLogFile"), BaseFilename, GEngineIni);

				BaseFilename = FPaths::GetBaseFilename(BaseFilename);


#if EXAMPLE_CAPTURE_FORMAT
				FString FinalFilePath;

				// When bPacketLogAppend is used, a static log filename is used
				if (bPacketLogAppend)
				{
					FString PreExtFilePath = FPaths::Combine(*ReadOutputLogDirectory, *BaseFilename);

					// Try0: PacketDump.ucap, Try1: PacketDump_1.ucap, Try#:PacketDump_#.ucap
					FinalFilePath = PreExtFilePath + CAPTURE_EXT;

					for (int32 i=1; ; i++)
					{
						// If not appending to a file, make sure to disable this bool - otherwise the file header won't be written
						bPacketLogAppend = PlatformFile.FileExists(*FinalFilePath);
						PacketLogFile = PlatformFile.OpenWrite(*FinalFilePath, true);

						if (PacketLogFile != NULL)
						{
							break;
						}


						FinalFilePath = PreExtFilePath + FString::Printf(TEXT("_%i"), i) + CAPTURE_EXT;
					}
				}
				// When non-append logging is used, the log filename uses a timestamp
				else
				{
					BaseFilename = BaseFilename + TEXT("_") + FDateTime::Now().ToString();

					FString PreExtFilePath = FPaths::Combine(*ReadOutputLogDirectory, *BaseFilename);
					FinalFilePath = PreExtFilePath + CAPTURE_EXT;

					for (int32 i=1; PlatformFile.FileExists(*FinalFilePath); i++)
					{
						FinalFilePath = PreExtFilePath + FString::Printf(TEXT("_%i"), i) + CAPTURE_EXT;
					}

					// Try0: PacketDump_2015.09.23-20.12.54.ucap, Try1: PacketDump_2015.09.23-20.12.54_1.ucap, ...
					PacketLogFile = PlatformFile.OpenWrite(*FinalFilePath);
				}

				if (PacketLogFile == NULL)
				{
					LowLevelFatalError(TEXT("Failed to create file %s"), *FinalFilePath);
					return;
				}
#else
				BaseFilename = BaseFilename + TEXT("_") + FDateTime::Now().ToString();

				FString PreExtInFilePath = FPaths::Combine(*ReadOutputLogDirectory, TEXT("Input"), *(BaseFilename + TEXT("_Input")));
				FString PreExtOutFilePath = FPaths::Combine(*ReadOutputLogDirectory, TEXT("Output"), *(BaseFilename + TEXT("_Output")));
				FString InPath = PreExtInFilePath + CAPTURE_EXT;
				FString OutPath = PreExtOutFilePath + CAPTURE_EXT;

				// Ensure the In/Out filenames are unique
				for (int32 i=1; PlatformFile.FileExists(*InPath) || PlatformFile.FileExists(*OutPath); i++)
				{
					InPath = PreExtInFilePath + FString::Printf(TEXT("_%i"), i) + CAPTURE_EXT;
					OutPath = PreExtOutFilePath + FString::Printf(TEXT("_%i"), i) + CAPTURE_EXT;
				}

				FArchive* InArc = IFileManager::Get().CreateFileWriter(*InPath);
				FArchive* OutArc = (InArc != NULL ? IFileManager::Get().CreateFileWriter(*OutPath) : NULL);

				InPacketLog = (InArc != NULL ? new FPacketCaptureArchive(*InArc) : NULL);
				OutPacketLog = (OutArc != NULL ? new FPacketCaptureArchive(*OutArc) : NULL);


				if (InPacketLog == NULL || OutPacketLog == NULL)
				{
					LowLevelFatalError(TEXT("Failed to create files '%s' and '%s'"), *InPath, *OutPath);
					return;
				}
#endif


#if EXAMPLE_CAPTURE_FORMAT
				// Only write the initial header, if not appending
				if (!bPacketLogAppend)
				{
					int32 NumberOfChannels = 1;
			
					PacketLogFile->Write(reinterpret_cast<const uint8*>(&NumberOfChannels), sizeof(NumberOfChannels));
				}
#else
				InPacketLog->SerializeCaptureHeader();
				OutPacketLog->SerializeCaptureHeader();
#endif
			}

			break;
		}

	case EOodleHandlerMode::Release:
		{
			FString ServerDictionaryPath;
			FString ClientDictionaryPath;

			GConfig->GetString(OODLE_INI_SECTION, TEXT("ServerDictionary"), ServerDictionaryPath, GEngineIni);
			GConfig->GetString(OODLE_INI_SECTION, TEXT("ClientDictionary"), ClientDictionaryPath, GEngineIni);

			if (ServerDictionaryPath.Len() < 0 || ClientDictionaryPath.Len() < 0)
			{
				LowLevelFatalError(TEXT("Specify both Server/Client dictionaries for Oodle compressor in DefaultEngine.ini"));
				return;
			}


			// Path must be within game directory, e.g: "Content/Oodle/Output.udic" becomes "ShooterGam/Content/Oodle/Output.udic"
			ServerDictionaryPath = FPaths::Combine(*FPaths::GameDir(), *ServerDictionaryPath);
			ClientDictionaryPath = FPaths::Combine(*FPaths::GameDir(), *ClientDictionaryPath);

			FPaths::CollapseRelativeDirectories(ServerDictionaryPath);
			FPaths::CollapseRelativeDirectories(ClientDictionaryPath);

			// Don't allow directory traversal to escape the game directory
			if (!ServerDictionaryPath.StartsWith(FPaths::GameDir()) || !ClientDictionaryPath.StartsWith(FPaths::GameDir()))
			{
				LowLevelFatalError(TEXT("DictionaryFile not allowed to use ../ paths to escape game directory."));
				return;
			}


			InitializeDictionary(ServerDictionaryPath, ServerDictionary);
			InitializeDictionary(ClientDictionaryPath, ClientDictionary);


			break;
		}

	default:
		break;
	}

	Initialized();
}

void OodleHandlerComponent::InitializeDictionary(FString FilePath, FOodleDictionary& OutDictionary)
{
	TArray<uint8> FileInMemory;

	// @todo #JohnB: Need a way to verify the state file, is actually a state file (do this once converting to FArchive)
	if (FFileHelper::LoadFileToArray(FileInMemory, *FilePath))
	{
		// parse header :
		OodleNetwork1_SavedStates_Header* SavedDictionaryFileData = (OodleNetwork1_SavedStates_Header*)FileInMemory.GetData();

		if (SavedDictionaryFileData->magic == ON1_MAGIC && SavedDictionaryFileData->oodle_header_version == OODLE_HEADER_VERSION)
		{
			// this code only loads compressed states currently
			check(SavedDictionaryFileData->compressor != -1);


			S32 StateHTBits = SavedDictionaryFileData->ht_bits;
			SINTa DictionarySize = SavedDictionaryFileData->dic_size;

			SINTa DictionaryCompLen = SavedDictionaryFileData->dic_complen;
			SINTa CompressorStateCompactedSize = SavedDictionaryFileData->statecompacted_size;
			SINTa CompressorStateCompactedLength = SavedDictionaryFileData->statecompacted_complen;

			SINTa SharedSize = OodleNetwork1_Shared_Size(StateHTBits);
			SINTa CompressorStateSize = OodleNetwork1UDP_State_Size();

			check(DictionarySize >= DictionaryCompLen);
			check(CompressorStateCompactedSize >= CompressorStateCompactedLength);

			check(CompressorStateCompactedSize > 0 && CompressorStateCompactedSize < OodleNetwork1UDP_StateCompacted_MaxSize());

			check(FileInMemory.Num() == (S64)sizeof(OodleNetwork1_SavedStates_Header) + DictionaryCompLen +
					CompressorStateCompactedLength);

			//OodleLog_Printf_v1("OodleNetwork1UDP Loading; dic comp %d , state %d->%d\n",DictionaryCompLen,
			//						CompressorStateCompactedSize, CompressorStateCompactedLength);

			//-------------------------------------------
			// decompress Dictionary and CompressorStatecompacted

			OutDictionary.Dictionary = malloc(DictionarySize);

			void * DictionaryCompressed = SavedDictionaryFileData + 1;

			SINTa DecompressedDictionarySize = OodleLZ_Decompress(DictionaryCompressed, DictionaryCompLen, OutDictionary.Dictionary,
																	DictionarySize);

			check(DecompressedDictionarySize == DictionarySize);

			OodleNetwork1UDP_StateCompacted* UDPNewCompacted = (OodleNetwork1UDP_StateCompacted *)malloc(CompressorStateCompactedSize);

			void * CompressorStatecompacted_comp_ptr = (U8 *)DictionaryCompressed + DictionaryCompLen;

			SINTa decomp_statecompacted_size = OodleLZ_Decompress(CompressorStatecompacted_comp_ptr, CompressorStateCompactedLength,
																	UDPNewCompacted, CompressorStateCompactedSize);

			check(decomp_statecompacted_size == CompressorStateCompactedSize);

			//----------------------------------------------
			// Uncompact the "Compacted" state into a usable state

			OutDictionary.CompressorState = (OodleNetwork1UDP_State *)malloc(CompressorStateSize);
			check(OutDictionary.CompressorState != NULL);

			OodleNetwork1UDP_State_Uncompact(OutDictionary.CompressorState, UDPNewCompacted);
			free(UDPNewCompacted);

			//----------------------------------------------
			// fill out SharedDictionaryData from the dictionary

			OutDictionary.SharedDictionaryData = (OodleNetwork1_Shared *)malloc(SharedSize);
			check(OutDictionary.SharedDictionaryData != NULL);
			OodleNetwork1_Shared_SetWindow(OutDictionary.SharedDictionaryData, StateHTBits, OutDictionary.Dictionary, (S32)DictionarySize);

			//-----------------------------------------------------
	
			//SET_DWORD_STAT(STAT_Oodle_DicSize, DictionarySize);
		}
		else if (SavedDictionaryFileData->magic != ON1_MAGIC)
		{
			UE_LOG(OodleHandlerComponentLog, Warning, TEXT("OodleNetCompressor: state_file ON1_MAGIC mismatch"));
		}
		else //if (SavedDictionaryFileData->oodle_header_version != OODLE_HEADER_VERSION)
		{
			UE_LOG(OodleHandlerComponentLog, Warning,
					TEXT("OodleNetCompressor: state_file OODLE_HEADER_VERSION mismatch. Was: %u, Expected: %u"),
					(uint32)SavedDictionaryFileData->oodle_header_version, (uint32)OODLE_HEADER_VERSION);
		}
	}
	else
	{
		LowLevelFatalError(TEXT("Incorrect DictionaryFile Provided"));
	}
}

bool OodleHandlerComponent::IsValid() const
{
	return true;
}

void OodleHandlerComponent::Incoming(FBitReader& Packet)
{
	switch(Mode)
	{
	case EOodleHandlerMode::Training:
		{
			if(Handler->Mode == Handler::Mode::Server)
			{
				uint32 SizeOfPacket = Packet.GetNumBytes();

#if EXAMPLE_CAPTURE_FORMAT
				int32 channel = 0;

				PacketLogFile->Write(reinterpret_cast<const uint8*>(&channel), sizeof(channel));
				PacketLogFile->Write(reinterpret_cast<const uint8*>(&SizeOfPacket), sizeof(SizeOfPacket));
				PacketLogFile->Write(reinterpret_cast<const uint8*>(Packet.GetData()), SizeOfPacket);
#else
				void* PacketData = Packet.GetData();
				InPacketLog->SerializePacket(PacketData, SizeOfPacket);
#endif
			}
			break;
		}

	case EOodleHandlerMode::Release:
		{
			uint32 DecompressedLength;
			Packet.SerializeIntPacked(DecompressedLength);

			if (ensure(DecompressedLength < MAX_OODLE_PACKET_SIZE))
			{
				// @todo #JohnB: You want to tweak the size of 'MAX_OODLE_PACKET_SIZE'
				static uint8 CompressedData[MAX_OODLE_PACKET_SIZE];
				static uint8 DecompressedData[MAX_OODLE_PACKET_SIZE];
				const int32 CompressedLength = Packet.GetBytesLeft();
				FOodleDictionary* CurDict = ((Handler->Mode == Handler::Mode::Server) ? &ClientDictionary : &ServerDictionary);

				Packet.Serialize(CompressedData, CompressedLength);

				check(OodleNetwork1UDP_Decode(CurDict->CompressorState, CurDict->SharedDictionaryData, CompressedData, CompressedLength,
												DecompressedData, DecompressedLength));


				// @todo #JohnB: Decompress directly into FBitReader's buffer
				FBitReader UnCompressedPacket(DecompressedData, DecompressedLength * 8);
				Packet = UnCompressedPacket;
			}


			break;
		}
	}
}

void OodleHandlerComponent::Outgoing(FBitWriter& Packet)
{
	switch(Mode)
	{
	case EOodleHandlerMode::Training:
		{
			if(Handler->Mode == Handler::Mode::Server)
			{
				uint32 SizeOfPacket = Packet.GetNumBytes();

#if EXAMPLE_CAPTURE_FORMAT
				int32 channel = 0;

				PacketLogFile->Write(reinterpret_cast<const uint8*>(&channel), sizeof(channel));
				PacketLogFile->Write(reinterpret_cast<const uint8*>(&SizeOfPacket), sizeof(SizeOfPacket));
				PacketLogFile->Write(reinterpret_cast<const uint8*>(Packet.GetData()), SizeOfPacket);
#else
				OutPacketLog->SerializePacket((void*)Packet.GetData(), SizeOfPacket);
#endif
			}
			break;
		}

	case EOodleHandlerMode::Release:
		{
			// Add size
			uint32 UncompressedLength = Packet.GetNumBytes();

			check(UncompressedLength < MAX_OODLE_PACKET_SIZE);

			if (UncompressedLength > 0)
			{
				// @todo #JohnB: You should be able to share the same two buffers between Incoming and Outgoing, as an optimization
				static uint8 UncompressedData[MAX_OODLE_PACKET_SIZE];
				static uint8 CompressedData[MAX_OODLE_PACKET_SIZE];
				FOodleDictionary* CurDict = ((Handler->Mode == Handler::Mode::Server) ? &ServerDictionary : &ClientDictionary);

				check(OodleLZ_GetCompressedBufferSizeNeeded(UncompressedLength) < MAX_OODLE_PACKET_SIZE);

				memcpy(UncompressedData, Packet.GetData(), UncompressedLength);

				SINTa CompressedLengthSINT = OodleNetwork1UDP_Encode(CurDict->CompressorState, CurDict->SharedDictionaryData,
																		UncompressedData, UncompressedLength, CompressedData);

				uint32 CompressedLength = static_cast<uint32>(CompressedLengthSINT);

				Packet.Reset();

				// @todo #JohnB: Compress directly into a (deliberately oversized) FBitWriter buffer, which you can shrink afterwards
				Packet.SerializeIntPacked(UncompressedLength);
				Packet.Serialize(CompressedData, CompressedLength);


				BytesSaved += UncompressedLength - CompressedLength;

				//UE_LOG(OodleHandlerComponentLog, Log, TEXT("Oodle Compressed: UnCompressed: %i Compressed: %i Total: %i"),
				//			UncompressedLength, CompressedLength, BytesSaved);

				INC_DWORD_STAT_BY(STAT_Oodle_BytesRaw, UncompressedLength);
				INC_DWORD_STAT_BY(STAT_Oodle_BytesCompressed, CompressedLength);
			}
			else
			{
				Packet.SerializeIntPacked(UncompressedLength);
			}

			break;
		}
	}
}

// MODULE INTERFACE
TSharedPtr<HandlerComponent> FOodleComponentModuleInterface::CreateComponentInstance(FString& Options)
{
	return MakeShareable(new OodleHandlerComponent);
}

void FOodleComponentModuleInterface::StartupModule()
{
	// Use an absolute path for this, as we want all relative paths, to be relative to this folder
	GOodleSaveDir = FPaths::ConvertRelativePathToFull(FPaths::Combine(*FPaths::GameSavedDir(), TEXT("Oodle")));


	// Load the Oodle library
	FString OodleBinaryPath = FPaths::EngineDir() / TEXT("Binaries/ThirdParty/NotForLicensees/Oodle/");
	FString OodleBinaryFile;

#if PLATFORM_WINDOWS
	{
	#if PLATFORM_64BITS
		OodleBinaryPath += TEXT("Win64/");
		OodleBinaryFile = TEXT("oodle_145_win64.dll");
	#else
		OodleBinaryPath += TEXT("Win32/");
		OodleBinaryFile = TEXT("oodle_145_win32.dll");
	#endif


		FPlatformProcess::PushDllDirectory(*OodleBinaryPath);

		OodleDllHandle = FPlatformProcess::GetDllHandle(*(OodleBinaryPath + OodleBinaryFile));

		FPlatformProcess::PopDllDirectory(*OodleBinaryPath);
	}
#else
	// @todo #JohnB
#endif

	check(OodleDllHandle != NULL);


	OodleInitOptions Options;

	Oodle_Init_GetDefaults_Minimal(OODLE_HEADER_VERSION, &Options);

	// @todo #JohnB: Implement toggle for logfile (from config), and implement/test the log filename path (don't forget char conversion)
	//Options.m_OodleInit_Log = true;
	//Options.m_OodleInit_Log_Header = ?;
	//Options.m_OodleInit_Log_FileName = ?;
	//Options.m_OodleInit_Log_FlushEachWrite = FParse::Param(FCommandLine::Get(), TEXT("FORCELOGFLUSH"));

	// @todo #JohnB: Consider threading in future
	Oodle_Init_NoThreads(OODLE_HEADER_VERSION, &Options);
}

void FOodleComponentModuleInterface::ShutdownModule()
{
	Oodle_Shutdown();

	if (OodleDllHandle != NULL)
	{
		FPlatformProcess::FreeDllHandle(OodleDllHandle);
		OodleDllHandle = NULL;
	}
}
#else
TSharedPtr<HandlerComponent> FOodleComponentModuleInterface::CreateComponentInstance(FString& Options)
{
	return TSharedPtr<HandlerComponent>(NULL);
}

void FOodleComponentModuleInterface::StartupModule()
{
}

void FOodleComponentModuleInterface::ShutdownModule()
{
}
#endif

IMPLEMENT_MODULE( FOodleComponentModuleInterface, OodleHandlerComponent );