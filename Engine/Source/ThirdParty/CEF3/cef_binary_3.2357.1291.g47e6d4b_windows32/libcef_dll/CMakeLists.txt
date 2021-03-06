# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

add_definitions(-DUSING_CEF_SHARED)

# Append platform specific sources to a list of sources.
macro(LIBCEF_APPEND_PLATFORM_SOURCES name_of_list)
  if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin" AND ${name_of_list}_MACOSX)
    list(APPEND ${name_of_list} ${${name_of_list}_MACOSX})
  endif()
  if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" AND ${name_of_list}_LINUX)
    list(APPEND ${name_of_list} ${${name_of_list}_LINUX})
  endif()
  if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" AND ${name_of_list}_WINDOWS)
    list(APPEND ${name_of_list} ${${name_of_list}_WINDOWS})
  endif()
endmacro()

set(LIBCEF_SRCS
  transfer_util.cc
  transfer_util.h
  )
source_group(libcef_dll FILES ${LIBCEF_SRCS})

set(LIBCEF_BASE_SRCS
  base/cef_atomicops_x86_gcc.cc
  base/cef_bind_helpers.cc
  base/cef_callback_helpers.cc
  base/cef_callback_internal.cc
  base/cef_lock.cc
  base/cef_lock_impl.cc
  base/cef_logging.cc
  base/cef_ref_counted.cc
  base/cef_string16.cc
  base/cef_thread_checker_impl.cc
  base/cef_thread_collision_warner.cc
  base/cef_weak_ptr.cc
  )
source_group(libcef_dll\\\\base FILES ${LIBCEF_BASE_SRCS})

set(LIBCEF_CPPTOC_SRCS
  cpptoc/app_cpptoc.cc
  cpptoc/app_cpptoc.h
  cpptoc/base_cpptoc.h
  cpptoc/browser_process_handler_cpptoc.cc
  cpptoc/browser_process_handler_cpptoc.h
  cpptoc/client_cpptoc.cc
  cpptoc/client_cpptoc.h
  cpptoc/completion_callback_cpptoc.cc
  cpptoc/completion_callback_cpptoc.h
  cpptoc/context_menu_handler_cpptoc.cc
  cpptoc/context_menu_handler_cpptoc.h
  cpptoc/cookie_visitor_cpptoc.cc
  cpptoc/cookie_visitor_cpptoc.h
  cpptoc/cpptoc.h
  cpptoc/delete_cookies_callback_cpptoc.cc
  cpptoc/delete_cookies_callback_cpptoc.h
  cpptoc/dialog_handler_cpptoc.cc
  cpptoc/dialog_handler_cpptoc.h
  cpptoc/display_handler_cpptoc.cc
  cpptoc/display_handler_cpptoc.h
  cpptoc/domvisitor_cpptoc.cc
  cpptoc/domvisitor_cpptoc.h
  cpptoc/download_handler_cpptoc.cc
  cpptoc/download_handler_cpptoc.h
  cpptoc/drag_handler_cpptoc.cc
  cpptoc/drag_handler_cpptoc.h
  cpptoc/end_tracing_callback_cpptoc.cc
  cpptoc/end_tracing_callback_cpptoc.h
  cpptoc/find_handler_cpptoc.cc
  cpptoc/find_handler_cpptoc.h
  cpptoc/focus_handler_cpptoc.cc
  cpptoc/focus_handler_cpptoc.h
  cpptoc/geolocation_handler_cpptoc.cc
  cpptoc/geolocation_handler_cpptoc.h
  cpptoc/get_geolocation_callback_cpptoc.cc
  cpptoc/get_geolocation_callback_cpptoc.h
  cpptoc/jsdialog_handler_cpptoc.cc
  cpptoc/jsdialog_handler_cpptoc.h
  cpptoc/keyboard_handler_cpptoc.cc
  cpptoc/keyboard_handler_cpptoc.h
  cpptoc/life_span_handler_cpptoc.cc
  cpptoc/life_span_handler_cpptoc.h
  cpptoc/load_handler_cpptoc.cc
  cpptoc/load_handler_cpptoc.h
  cpptoc/navigation_entry_visitor_cpptoc.cc
  cpptoc/navigation_entry_visitor_cpptoc.h
  cpptoc/print_handler_cpptoc.cc
  cpptoc/print_handler_cpptoc.h
  cpptoc/read_handler_cpptoc.cc
  cpptoc/read_handler_cpptoc.h
  cpptoc/render_handler_cpptoc.cc
  cpptoc/render_handler_cpptoc.h
  cpptoc/render_process_handler_cpptoc.cc
  cpptoc/render_process_handler_cpptoc.h
  cpptoc/request_context_handler_cpptoc.cc
  cpptoc/request_context_handler_cpptoc.h
  cpptoc/request_handler_cpptoc.cc
  cpptoc/request_handler_cpptoc.h
  cpptoc/resource_bundle_handler_cpptoc.cc
  cpptoc/resource_bundle_handler_cpptoc.h
  cpptoc/resource_handler_cpptoc.cc
  cpptoc/resource_handler_cpptoc.h
  cpptoc/run_file_dialog_callback_cpptoc.cc
  cpptoc/run_file_dialog_callback_cpptoc.h
  cpptoc/scheme_handler_factory_cpptoc.cc
  cpptoc/scheme_handler_factory_cpptoc.h
  cpptoc/set_cookie_callback_cpptoc.cc
  cpptoc/set_cookie_callback_cpptoc.h
  cpptoc/string_visitor_cpptoc.cc
  cpptoc/string_visitor_cpptoc.h
  cpptoc/task_cpptoc.cc
  cpptoc/task_cpptoc.h
  cpptoc/urlrequest_client_cpptoc.cc
  cpptoc/urlrequest_client_cpptoc.h
  cpptoc/v8accessor_cpptoc.cc
  cpptoc/v8accessor_cpptoc.h
  cpptoc/v8handler_cpptoc.cc
  cpptoc/v8handler_cpptoc.h
  cpptoc/web_plugin_info_visitor_cpptoc.cc
  cpptoc/web_plugin_info_visitor_cpptoc.h
  cpptoc/web_plugin_unstable_callback_cpptoc.cc
  cpptoc/web_plugin_unstable_callback_cpptoc.h
  cpptoc/write_handler_cpptoc.cc
  cpptoc/write_handler_cpptoc.h
  )
source_group(libcef_dll\\\\cpptoc FILES ${LIBCEF_CPPTOC_SRCS})

set(LIBCEF_CTOCPP_SRCS
  ctocpp/auth_callback_ctocpp.cc
  ctocpp/auth_callback_ctocpp.h
  ctocpp/base_ctocpp.h
  ctocpp/before_download_callback_ctocpp.cc
  ctocpp/before_download_callback_ctocpp.h
  ctocpp/binary_value_ctocpp.cc
  ctocpp/binary_value_ctocpp.h
  ctocpp/browser_ctocpp.cc
  ctocpp/browser_ctocpp.h
  ctocpp/browser_host_ctocpp.cc
  ctocpp/browser_host_ctocpp.h
  ctocpp/callback_ctocpp.cc
  ctocpp/callback_ctocpp.h
  ctocpp/command_line_ctocpp.cc
  ctocpp/command_line_ctocpp.h
  ctocpp/context_menu_params_ctocpp.cc
  ctocpp/context_menu_params_ctocpp.h
  ctocpp/cookie_manager_ctocpp.cc
  ctocpp/cookie_manager_ctocpp.h
  ctocpp/ctocpp.h
  ctocpp/dictionary_value_ctocpp.cc
  ctocpp/dictionary_value_ctocpp.h
  ctocpp/domdocument_ctocpp.cc
  ctocpp/domdocument_ctocpp.h
  ctocpp/domnode_ctocpp.cc
  ctocpp/domnode_ctocpp.h
  ctocpp/download_item_callback_ctocpp.cc
  ctocpp/download_item_callback_ctocpp.h
  ctocpp/download_item_ctocpp.cc
  ctocpp/download_item_ctocpp.h
  ctocpp/drag_data_ctocpp.cc
  ctocpp/drag_data_ctocpp.h
  ctocpp/file_dialog_callback_ctocpp.cc
  ctocpp/file_dialog_callback_ctocpp.h
  ctocpp/frame_ctocpp.cc
  ctocpp/frame_ctocpp.h
  ctocpp/geolocation_callback_ctocpp.cc
  ctocpp/geolocation_callback_ctocpp.h
  ctocpp/jsdialog_callback_ctocpp.cc
  ctocpp/jsdialog_callback_ctocpp.h
  ctocpp/list_value_ctocpp.cc
  ctocpp/list_value_ctocpp.h
  ctocpp/menu_model_ctocpp.cc
  ctocpp/menu_model_ctocpp.h
  ctocpp/navigation_entry_ctocpp.cc
  ctocpp/navigation_entry_ctocpp.h
  ctocpp/post_data_ctocpp.cc
  ctocpp/post_data_ctocpp.h
  ctocpp/post_data_element_ctocpp.cc
  ctocpp/post_data_element_ctocpp.h
  ctocpp/print_dialog_callback_ctocpp.cc
  ctocpp/print_dialog_callback_ctocpp.h
  ctocpp/print_job_callback_ctocpp.cc
  ctocpp/print_job_callback_ctocpp.h
  ctocpp/print_settings_ctocpp.cc
  ctocpp/print_settings_ctocpp.h
  ctocpp/process_message_ctocpp.cc
  ctocpp/process_message_ctocpp.h
  ctocpp/request_callback_ctocpp.cc
  ctocpp/request_callback_ctocpp.h
  ctocpp/request_context_ctocpp.cc
  ctocpp/request_context_ctocpp.h
  ctocpp/request_ctocpp.cc
  ctocpp/request_ctocpp.h
  ctocpp/response_ctocpp.cc
  ctocpp/response_ctocpp.h
  ctocpp/scheme_registrar_ctocpp.cc
  ctocpp/scheme_registrar_ctocpp.h
  ctocpp/sslcert_principal_ctocpp.cc
  ctocpp/sslcert_principal_ctocpp.h
  ctocpp/sslinfo_ctocpp.cc
  ctocpp/sslinfo_ctocpp.h
  ctocpp/stream_reader_ctocpp.cc
  ctocpp/stream_reader_ctocpp.h
  ctocpp/stream_writer_ctocpp.cc
  ctocpp/stream_writer_ctocpp.h
  ctocpp/task_runner_ctocpp.cc
  ctocpp/task_runner_ctocpp.h
  ctocpp/urlrequest_ctocpp.cc
  ctocpp/urlrequest_ctocpp.h
  ctocpp/v8context_ctocpp.cc
  ctocpp/v8context_ctocpp.h
  ctocpp/v8exception_ctocpp.cc
  ctocpp/v8exception_ctocpp.h
  ctocpp/v8stack_frame_ctocpp.cc
  ctocpp/v8stack_frame_ctocpp.h
  ctocpp/v8stack_trace_ctocpp.cc
  ctocpp/v8stack_trace_ctocpp.h
  ctocpp/v8value_ctocpp.cc
  ctocpp/v8value_ctocpp.h
  ctocpp/value_ctocpp.cc
  ctocpp/value_ctocpp.h
  ctocpp/web_plugin_info_ctocpp.cc
  ctocpp/web_plugin_info_ctocpp.h
  ctocpp/xml_reader_ctocpp.cc
  ctocpp/xml_reader_ctocpp.h
  ctocpp/zip_reader_ctocpp.cc
  ctocpp/zip_reader_ctocpp.h
  )
source_group(libcef_dll\\\\ctocpp FILES ${LIBCEF_CTOCPP_SRCS})

set(LIBCEF_INCLUDE_SRCS
  ../include/cef_app.h
  ../include/cef_auth_callback.h
  ../include/cef_base.h
  ../include/cef_browser.h
  ../include/cef_browser_process_handler.h
  ../include/cef_callback.h
  ../include/cef_client.h
  ../include/cef_command_line.h
  ../include/cef_context_menu_handler.h
  ../include/cef_cookie.h
  ../include/cef_dialog_handler.h
  ../include/cef_display_handler.h
  ../include/cef_dom.h
  ../include/cef_download_handler.h
  ../include/cef_download_item.h
  ../include/cef_drag_data.h
  ../include/cef_drag_handler.h
  ../include/cef_find_handler.h
  ../include/cef_focus_handler.h
  ../include/cef_frame.h
  ../include/cef_geolocation.h
  ../include/cef_geolocation_handler.h
  ../include/cef_jsdialog_handler.h
  ../include/cef_keyboard_handler.h
  ../include/cef_life_span_handler.h
  ../include/cef_load_handler.h
  ../include/cef_menu_model.h
  ../include/cef_navigation_entry.h
  ../include/cef_origin_whitelist.h
  ../include/cef_pack_resources.h
  ../include/cef_pack_strings.h
  ../include/cef_parser.h
  ../include/cef_path_util.h
  ../include/cef_print_handler.h
  ../include/cef_print_settings.h
  ../include/cef_process_message.h
  ../include/cef_process_util.h
  ../include/cef_render_handler.h
  ../include/cef_render_process_handler.h
  ../include/cef_request.h
  ../include/cef_request_context.h
  ../include/cef_request_context_handler.h
  ../include/cef_request_handler.h
  ../include/cef_resource_bundle_handler.h
  ../include/cef_resource_handler.h
  ../include/cef_response.h
  ../include/cef_runnable.h
  ../include/cef_scheme.h
  ../include/cef_ssl_info.h
  ../include/cef_stream.h
  ../include/cef_string_visitor.h
  ../include/cef_task.h
  ../include/cef_trace.h
  ../include/cef_urlrequest.h
  ../include/cef_v8.h
  ../include/cef_values.h
  ../include/cef_version.h
  ../include/cef_web_plugin.h
  ../include/cef_xml_reader.h
  ../include/cef_zip_reader.h
  )
set(LIBCEF_INCLUDE_SRCS_MACOSX
  ../include/cef_application_mac.h
  )
set(LIBCEF_INCLUDE_SRCS_WINDOWS
  ../include/cef_sandbox_win.h
  )
LIBCEF_APPEND_PLATFORM_SOURCES(LIBCEF_INCLUDE_SRCS)
source_group(include FILES ${LIBCEF_INCLUDE_SRCS})

set(LIBCEF_INCLUDE_BASE_SRCS
  ../include/base/cef_atomic_ref_count.h
  ../include/base/cef_atomicops.h
  ../include/base/cef_basictypes.h
  ../include/base/cef_bind.h
  ../include/base/cef_bind_helpers.h
  ../include/base/cef_build.h
  ../include/base/cef_callback.h
  ../include/base/cef_callback_forward.h
  ../include/base/cef_callback_helpers.h
  ../include/base/cef_callback_list.h
  ../include/base/cef_cancelable_callback.h
  ../include/base/cef_lock.h
  ../include/base/cef_logging.h
  ../include/base/cef_macros.h
  ../include/base/cef_move.h
  ../include/base/cef_platform_thread.h
  ../include/base/cef_ref_counted.h
  ../include/base/cef_scoped_ptr.h
  ../include/base/cef_string16.h
  ../include/base/cef_template_util.h
  ../include/base/cef_thread_checker.h
  ../include/base/cef_thread_collision_warner.h
  ../include/base/cef_trace_event.h
  ../include/base/cef_tuple.h
  ../include/base/cef_weak_ptr.h
  )
source_group(include\\\\base FILES ${LIBCEF_INCLUDE_BASE_SRCS})

set(LIBCEF_INCLUDE_BASE_INTERNAL_SRCS
  ../include/base/internal/cef_bind_internal.h
  ../include/base/internal/cef_callback_internal.h
  ../include/base/internal/cef_lock_impl.h
  ../include/base/internal/cef_raw_scoped_refptr_mismatch_checker.h
  ../include/base/internal/cef_thread_checker_impl.h
  )
set(LIBCEF_INCLUDE_BASE_INTERNAL_SRCS_LINUX
  ../include/base/internal/cef_atomicops_atomicword_compat.h
  ../include/base/internal/cef_atomicops_x86_gcc.h
  )
set(LIBCEF_INCLUDE_BASE_INTERNAL_SRCS_MACOSX
  ../include/base/internal/cef_atomicops_atomicword_compat.h
  ../include/base/internal/cef_atomicops_mac.h
  )
set(LIBCEF_INCLUDE_BASE_INTERNAL_SRCS_WINDOWS
  ../include/base/internal/cef_atomicops_x86_msvc.h
  ../include/base/internal/cef_bind_internal_win.h
  )
LIBCEF_APPEND_PLATFORM_SOURCES(LIBCEF_INCLUDE_BASE_INTERNAL_SRCS)
source_group(include\\\\base\\\\internal FILES ${LIBCEF_INCLUDE_BASE_INTERNAL_SRCS})

set(LIBCEF_INCLUDE_CAPI_SRCS
  ../include/capi/cef_app_capi.h
  ../include/capi/cef_auth_callback_capi.h
  ../include/capi/cef_base_capi.h
  ../include/capi/cef_browser_capi.h
  ../include/capi/cef_browser_process_handler_capi.h
  ../include/capi/cef_callback_capi.h
  ../include/capi/cef_client_capi.h
  ../include/capi/cef_command_line_capi.h
  ../include/capi/cef_context_menu_handler_capi.h
  ../include/capi/cef_cookie_capi.h
  ../include/capi/cef_dialog_handler_capi.h
  ../include/capi/cef_display_handler_capi.h
  ../include/capi/cef_dom_capi.h
  ../include/capi/cef_download_handler_capi.h
  ../include/capi/cef_download_item_capi.h
  ../include/capi/cef_drag_data_capi.h
  ../include/capi/cef_drag_handler_capi.h
  ../include/capi/cef_find_handler_capi.h
  ../include/capi/cef_focus_handler_capi.h
  ../include/capi/cef_frame_capi.h
  ../include/capi/cef_geolocation_capi.h
  ../include/capi/cef_geolocation_handler_capi.h
  ../include/capi/cef_jsdialog_handler_capi.h
  ../include/capi/cef_keyboard_handler_capi.h
  ../include/capi/cef_life_span_handler_capi.h
  ../include/capi/cef_load_handler_capi.h
  ../include/capi/cef_menu_model_capi.h
  ../include/capi/cef_navigation_entry_capi.h
  ../include/capi/cef_origin_whitelist_capi.h
  ../include/capi/cef_parser_capi.h
  ../include/capi/cef_path_util_capi.h
  ../include/capi/cef_print_handler_capi.h
  ../include/capi/cef_print_settings_capi.h
  ../include/capi/cef_process_message_capi.h
  ../include/capi/cef_process_util_capi.h
  ../include/capi/cef_render_handler_capi.h
  ../include/capi/cef_render_process_handler_capi.h
  ../include/capi/cef_request_capi.h
  ../include/capi/cef_request_context_capi.h
  ../include/capi/cef_request_context_handler_capi.h
  ../include/capi/cef_request_handler_capi.h
  ../include/capi/cef_resource_bundle_handler_capi.h
  ../include/capi/cef_resource_handler_capi.h
  ../include/capi/cef_response_capi.h
  ../include/capi/cef_scheme_capi.h
  ../include/capi/cef_ssl_info_capi.h
  ../include/capi/cef_stream_capi.h
  ../include/capi/cef_string_visitor_capi.h
  ../include/capi/cef_task_capi.h
  ../include/capi/cef_trace_capi.h
  ../include/capi/cef_urlrequest_capi.h
  ../include/capi/cef_v8_capi.h
  ../include/capi/cef_values_capi.h
  ../include/capi/cef_web_plugin_capi.h
  ../include/capi/cef_xml_reader_capi.h
  ../include/capi/cef_zip_reader_capi.h
  )
source_group(include\\\\capi FILES ${LIBCEF_INCLUDE_CAPI_SRCS})

set(LIBCEF_INCLUDE_INTERNAL_SRCS
  ../include/internal/cef_export.h
  ../include/internal/cef_logging_internal.h
  ../include/internal/cef_ptr.h
  ../include/internal/cef_string.h
  ../include/internal/cef_string_list.h
  ../include/internal/cef_string_map.h
  ../include/internal/cef_string_multimap.h
  ../include/internal/cef_string_types.h
  ../include/internal/cef_string_wrappers.h
  ../include/internal/cef_thread_internal.h
  ../include/internal/cef_time.h
  ../include/internal/cef_trace_event_internal.h
  ../include/internal/cef_types.h
  ../include/internal/cef_types_wrappers.h
  )
set(LIBCEF_INCLUDE_INTERNAL_SRCS_LINUX
  ../include/internal/cef_linux.h
  ../include/internal/cef_types_linux.h
  )
set(LIBCEF_INCLUDE_INTERNAL_SRCS_MACOSX
  ../include/internal/cef_mac.h
  ../include/internal/cef_types_mac.h
  )
set(LIBCEF_INCLUDE_INTERNAL_SRCS_WINDOWS
  ../include/internal/cef_types_win.h
  ../include/internal/cef_win.h
  )
LIBCEF_APPEND_PLATFORM_SOURCES(LIBCEF_INCLUDE_INTERNAL_SRCS)
source_group(include\\\\internal FILES ${LIBCEF_INCLUDE_INTERNAL_SRCS})

set(LIBCEF_INCLUDE_WRAPPER_SRCS
  ../include/wrapper/cef_byte_read_handler.h
  ../include/wrapper/cef_closure_task.h
  ../include/wrapper/cef_helpers.h
  ../include/wrapper/cef_message_router.h
  ../include/wrapper/cef_resource_manager.h
  ../include/wrapper/cef_stream_resource_handler.h
  ../include/wrapper/cef_xml_object.h
  ../include/wrapper/cef_zip_archive.h
  )
source_group(include\\\\wrapper FILES ${LIBCEF_INCLUDE_WRAPPER_SRCS})

set(LIBCEF_WRAPPER_SRCS
  wrapper/cef_browser_info_map.h
  wrapper/cef_byte_read_handler.cc
  wrapper/cef_closure_task.cc
  wrapper/cef_message_router.cc
  wrapper/cef_resource_manager.cc
  wrapper/cef_stream_resource_handler.cc
  wrapper/cef_xml_object.cc
  wrapper/cef_zip_archive.cc
  wrapper/libcef_dll_wrapper.cc
  wrapper/libcef_dll_wrapper2.cc
  )
source_group(libcef_dll\\\\wrapper FILES ${LIBCEF_WRAPPER_SRCS})

add_library(libcef_dll_wrapper
  ${LIBCEF_SRCS}
  ${LIBCEF_BASE_SRCS}
  ${LIBCEF_CPPTOC_SRCS}
  ${LIBCEF_CTOCPP_SRCS}
  ${LIBCEF_INCLUDE_SRCS}
  ${LIBCEF_INCLUDE_BASE_SRCS}
  ${LIBCEF_INCLUDE_BASE_INTERNAL_SRCS}
  ${LIBCEF_INCLUDE_CAPI_SRCS}
  ${LIBCEF_INCLUDE_INTERNAL_SRCS}
  ${LIBCEF_INCLUDE_WRAPPER_SRCS}
  ${LIBCEF_WRAPPER_SRCS}
  )

# Remove the default "lib" prefix from the resulting library.
set_target_properties(libcef_dll_wrapper PROPERTIES PREFIX "")
