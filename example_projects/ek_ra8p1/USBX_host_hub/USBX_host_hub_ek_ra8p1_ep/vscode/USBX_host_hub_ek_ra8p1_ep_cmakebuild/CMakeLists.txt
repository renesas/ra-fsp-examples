# Please see README.md for build instructions

cmake_minimum_required(VERSION 3.16.4)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_SYSTEM_PROCESSOR arm)

# Use add_library() with the STATIC option to name the source file in the generated project.
# This avoids running the linker and is intended for use with cross-compiling toolchains that
# cannot link without custom flags or linker scripts
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo MinSizeRel)

if(BUILD_KIT)
	message(NOTICE "Build Kit is defined as ${BUILD_KIT}")
endif()

message(NOTICE "CMAKE_CURRENT_BINARY_DIR is defined as ${CMAKE_CURRENT_BINARY_DIR}")

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(isMultiConfig)
    message(NOTICE "Multi-config generator(${CMAKE_GENERATOR}) is active")
else()
    message(NOTICE "Single-config generator(${CMAKE_GENERATOR}) is active")
	if(DEFINED CMAKE_BUILD_TYPE)
	 	message(NOTICE "CMAKE_BUILD_TYPE is defined as ${CMAKE_BUILD_TYPE}")
	elseif(DEFINED BUILD_TYPE)
		set(CMAKE_BUILD_TYPE ${BUILD_TYPE})
		message(NOTICE "CMAKE_BUILD_TYPE is set to BUILD_TYPE ENV ${CMAKE_BUILD_TYPE}")
	else()
		message(FATAL_ERROR "Please specify the CMAKE_BUILD_TYPE with -DCMAKE_BUILD_TYPE=")
	endif()
    #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
	message(NOTICE "CMAKE_RUNTIME_OUTPUT_DIRECTORY is defined as ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

cmake_minimum_required(VERSION 3.16.4)

# Project configuration
project(USBX_host_hub_ek_ra8p1_ep_cmakebuild
	VERSION 1.0.0
	LANGUAGES C CXX ASM)

set(CMAKE_CROSSCOMPILING "TRUE")
set(CMAKE_EXPORT_COMPILE_COMMANDS "TRUE")

include(${CMAKE_CURRENT_LIST_DIR}/Config.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/GeneratedCfg.cmake)
# --- Match e2studio Managed-Build compiler flags ---
# The stray size-optimization flag is "-Os" on the LLVM/clang_arm toolchain but
# "-Oz" on the GCC/gcc-arm-embedded toolchain - remove both to cover either.
list(REMOVE_ITEM RASC_CMAKE_C_FLAGS "-Os" "-Oz")
list(REMOVE_ITEM RASC_CMAKE_CXX_FLAGS "-Os" "-Oz")
list(REMOVE_ITEM RASC_CMAKE_ASM_FLAGS "-Os" "-Oz")
# e2studio Managed-Build (config Debug) actually compiles with "-Os", not RASC's own assumed default for this config.
list(REMOVE_ITEM RASC_DEBUG_FLAGS "-O0" "-O1" "-O2" "-O3" "-Os" "-Oz")
list(APPEND RASC_DEBUG_FLAGS "-Os")
if(DEFINED RASC_C_DEBUG_FLAGS)
    string(REPLACE " " ";" _rasc_opt "${RASC_C_DEBUG_FLAGS}")
    list(REMOVE_ITEM _rasc_opt "-O0" "-O1" "-O2" "-O3" "-Os" "-Oz")
    list(APPEND _rasc_opt "-Os")
    string(REPLACE ";" " " RASC_C_DEBUG_FLAGS "${_rasc_opt}")
endif()
if(DEFINED RASC_CXX_DEBUG_FLAGS)
    string(REPLACE " " ";" _rasc_opt "${RASC_CXX_DEBUG_FLAGS}")
    list(REMOVE_ITEM _rasc_opt "-O0" "-O1" "-O2" "-O3" "-Os" "-Oz")
    list(APPEND _rasc_opt "-Os")
    string(REPLACE ";" " " RASC_CXX_DEBUG_FLAGS "${_rasc_opt}")
endif()
# Source .cproject (config Debug) is configured "-funsigned-char", not RASC's default "-fsigned-char".
list(TRANSFORM RASC_CMAKE_C_FLAGS REPLACE "^-fsigned-char$" "-funsigned-char")
list(TRANSFORM RASC_CMAKE_CXX_FLAGS REPLACE "^-fsigned-char$" "-funsigned-char")
list(TRANSFORM RASC_CMAKE_ASM_FLAGS REPLACE "^-fsigned-char$" "-funsigned-char")
list(TRANSFORM RASC_CMAKE_EXE_LINKER_FLAGS REPLACE "^-fsigned-char$" "-funsigned-char")
# e2studio Managed-Build actually compiles with "-mcpu=cortex-m85" (RASC's own -mcpu may be missing a device extension modifier like "+nopacbti").
list(TRANSFORM RASC_CMAKE_C_FLAGS REPLACE "^-mcpu=.*$" "-mcpu=cortex-m85")
list(TRANSFORM RASC_CMAKE_CXX_FLAGS REPLACE "^-mcpu=.*$" "-mcpu=cortex-m85")
list(TRANSFORM RASC_CMAKE_ASM_FLAGS REPLACE "^-mcpu=.*$" "-mcpu=cortex-m85")
list(TRANSFORM RASC_CMAKE_EXE_LINKER_FLAGS REPLACE "^-mcpu=.*$" "-mcpu=cortex-m85")
# Match e2studio Managed-Build's __FILE__ expansion (affects assert()/configASSERT() in RTOS projects)
list(APPEND RASC_CMAKE_C_FLAGS "-fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=../")
list(APPEND RASC_CMAKE_CXX_FLAGS "-fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=../")


# If required user can amend RASC generated flags here, e.g. list(REMOVE_ITEM RASC_CMAKE_C_FLAGS "-Wall")

include(${CMAKE_CURRENT_LIST_DIR}/cmake/GeneratedSrc.cmake)

target_include_directories(${PROJECT_NAME}.elf
    BEFORE
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)



target_include_directories(${PROJECT_NAME}.elf
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/.
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/inc
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_usbx_port
	${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/inc
	${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/inc
)

target_link_options(${PROJECT_NAME}.elf
    PRIVATE
        $<$<CONFIG:Debug>:
            "SHELL:-Wl,-z,norelro"
        >
)

target_link_options(${PROJECT_NAME}.elf
    PRIVATE
        $<$<CONFIG:Release>:
            "SHELL:-Wl,-z,norelro"
        >
)


target_compile_options(${PROJECT_NAME}.elf PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-flax-vector-conversions>
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-flax-vector-conversions>
)

target_link_libraries(${PROJECT_NAME}.elf PRIVATE
    crt0
    c++abi
)



# --- Match e2studio Managed-Build link-time optimization flag ---
# RASC's own per-config target_link_options() calls above never include one
target_link_options(${PROJECT_NAME}.elf PRIVATE
    $<$<CONFIG:Debug>:-Os>
)


# --- Always also emit .hex, matching e2studio Managed-Build output ---
# Some RASC-converted projects only emit .srec/.bin regardless of build
# config; harmless duplicate if GeneratedSrc.cmake already emits .hex too.
add_custom_command(
    TARGET ${PROJECT_NAME}.elf
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMENT "Creating hex file in ${PROJECT_BINARY_DIR}"
)


# --- Match e2studio Managed-Build object link order ---
set_property(TARGET ${PROJECT_NAME}.elf PROPERTY SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/console/syscalls_gcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/console/syscalls_llvm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/console/uart_console.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_warmstart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/terminal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/terminal_thread_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usbx_host_hid_key_board_hub_thread_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usbx_host_hid_mouse_hub_thread_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usbx_host_hub_ep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usbx_host_hub_example_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/hal_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/pin_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/terminal_thread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/usbx_host_hid_key_board_hub_thread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/usbx_host_hid_mouse_hub_thread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/vector_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_client_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_client_search.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_configure.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_descriptor_parse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_field_decompress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_global_item_parse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_idle_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_idle_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_idle_set_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_instance_clean.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_interrupt_endpoint_search.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_item_data_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_keyboard_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_keyboard_callback.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_keyboard_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_keyboard_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_keyboard_ioctl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_keyboard_key_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_keyboard_tasks_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_keyboard_thread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_local_item_parse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_main_item_parse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_mouse_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_mouse_buttons_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_mouse_callback.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_mouse_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_mouse_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_mouse_position_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_mouse_wheel_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_periodic_report_start.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_periodic_report_stop.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_remote_control_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_remote_control_callback.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_remote_control_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_remote_control_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_remote_control_usage_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_report_add.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_report_callback_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_report_compress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_report_decompress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_report_descriptor_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_report_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_report_id_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_report_item_analyse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_report_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_report_set_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_resources_free.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_tasks_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hid_transfer_request_completed.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_change_detect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_change_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_configure.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_descriptor_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_feature.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_hub_change_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_interrupt_endpoint_start.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_port_change_connection_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_port_change_enable_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_port_change_over_current_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_port_change_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_port_change_reset_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_port_change_suspend_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_port_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_ports_power.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_status_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/usbx_host_classes/src/ux_host_class_hub_transfer_request_completed.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_address_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_endpoint_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_endpoint_destroy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_endpoint_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_endpoint_stall.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_endpoint_status.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_frame_number_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_function.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_initialize_complete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_state_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_transfer_abort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_transfer_request.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_dcd_sim_slave_transfer_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_class_dpump_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_class_dpump_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_class_dpump_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_class_dpump_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_class_dpump_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_class_dpump_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_class_dpump_read_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_class_dpump_thread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_class_dpump_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_class_dpump_write_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_alternate_setting_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_alternate_setting_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_class_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_class_unregister.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_clear_feature.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_configuration_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_configuration_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_control_request_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_descriptor_send.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_disconnect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_endpoint_stall.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_get_status.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_host_wakeup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_interface_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_interface_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_interface_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_interface_start.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_microsoft_extension_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_set_feature.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_tasks_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_transfer_abort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_transfer_all_request_abort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_transfer_request.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_transfer_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_device_stack_uninitialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_asynch_queue_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_asynch_schedule.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_asynchronous_endpoint_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_asynchronous_endpoint_destroy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_controller_disable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_ed_obtain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_ed_td_clean.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_endpoint_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_frame_number_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_frame_number_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_interrupt_endpoint_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_iso_queue_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_iso_schedule.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_isochronous_endpoint_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_isochronous_td_obtain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_least_traffic_list_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_periodic_endpoint_destroy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_periodic_schedule.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_periodic_tree_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_port_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_port_status_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_regular_td_obtain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_request_bulk_transfer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_request_control_transfer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_request_interupt_transfer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_request_isochronous_transfer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_request_transfer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_timer_function.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_transaction_schedule.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_transfer_abort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_transfer_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_hcd_sim_host_uninitialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_class_dpump_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_class_dpump_configure.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_class_dpump_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_class_dpump_endpoints_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_class_dpump_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_class_dpump_ioctl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_class_dpump_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_class_dpump_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_bandwidth_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_bandwidth_claim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_bandwidth_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_class_call.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_class_device_scan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_class_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_class_instance_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_class_instance_destroy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_class_instance_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_class_instance_verify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_class_interface_scan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_class_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_class_unregister.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_configuration_descriptor_parse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_configuration_enumerate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_configuration_instance_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_configuration_instance_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_configuration_interface_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_configuration_interface_scan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_configuration_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_delay_ms.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_device_address_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_device_configuration_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_device_configuration_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_device_configuration_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_device_configuration_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_device_configuration_select.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_device_descriptor_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_device_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_device_remove.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_device_resources_free.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_endpoint_instance_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_endpoint_instance_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_endpoint_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_endpoint_transfer_abort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_enum_thread_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_hcd_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_hcd_thread_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_hcd_transfer_request.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_hcd_unregister.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_hnp_polling_thread_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_interface_endpoint_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_interface_instance_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_interface_instance_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_interface_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_interface_setting_select.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_interfaces_scan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_new_configuration_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_new_device_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_new_device_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_new_endpoint_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_new_interface_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_rh_change_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_rh_device_extraction.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_rh_device_insertion.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_role_swap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_tasks_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_transfer_request.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_transfer_request_abort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_transfer_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_host_stack_uninitialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_system_error_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_system_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_system_tasks_run.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_system_uninitialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_trace_event_insert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_trace_event_update.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_trace_object_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_trace_object_unregister.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_debug_callback_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_debug_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_delay_ms.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_descriptor_pack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_descriptor_parse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_error_callback_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_event_flags_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_event_flags_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_event_flags_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_event_flags_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_long_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_long_get_big_endian.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_long_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_long_put_big_endian.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_memory_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_memory_allocate_add_safe.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_memory_allocate_mulc_safe.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_memory_allocate_mulv_safe.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_memory_byte_pool_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_memory_byte_pool_search.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_memory_compare.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_memory_copy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_memory_free.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_memory_free_block_best_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_memory_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_mutex_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_mutex_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_mutex_off.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_mutex_on.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_pci_class_scan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_pci_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_pci_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_physical_address.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_semaphore_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_semaphore_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_semaphore_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_semaphore_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_set_interrupt_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_short_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_short_get_big_endian.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_short_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_short_put_big_endian.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_string_length_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_string_length_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_string_to_unicode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_thread_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_thread_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_thread_identify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_thread_relinquish.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_thread_resume.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_thread_schedule_other.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_thread_sleep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_thread_suspend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_timer_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_timer_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_unicode_to_string.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/usbx/common/core/src/ux_utility_virtual_address.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_block_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_block_pool_cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_block_pool_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_block_pool_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_block_pool_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_block_pool_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_block_pool_performance_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_block_pool_performance_system_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_block_pool_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_block_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_byte_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_byte_pool_cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_byte_pool_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_byte_pool_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_byte_pool_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_byte_pool_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_byte_pool_performance_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_byte_pool_performance_system_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_byte_pool_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_byte_pool_search.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_byte_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_event_flags_cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_event_flags_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_event_flags_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_event_flags_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_event_flags_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_event_flags_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_event_flags_performance_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_event_flags_performance_system_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_event_flags_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_event_flags_set_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_initialize_high_level.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_initialize_kernel_enter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_initialize_kernel_setup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_mutex_cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_mutex_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_mutex_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_mutex_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_mutex_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_mutex_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_mutex_performance_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_mutex_performance_system_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_mutex_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_mutex_priority_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_mutex_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_flush.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_front_send.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_performance_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_performance_system_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_receive.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_send.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_queue_send_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_semaphore_ceiling_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_semaphore_cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_semaphore_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_semaphore_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_semaphore_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_semaphore_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_semaphore_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_semaphore_performance_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_semaphore_performance_system_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_semaphore_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_semaphore_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_semaphore_put_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_entry_exit_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_identify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_performance_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_performance_system_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_preemption_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_priority_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_relinquish.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_resume.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_shell_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_sleep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_stack_analyze.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_stack_error_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_stack_error_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_suspend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_system_preempt_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_system_resume.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_system_suspend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_terminate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_time_slice.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_time_slice_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_timeout.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_thread_wait_abort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_time_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_time_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_expiration_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_performance_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_performance_system_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_system_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_system_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_timer_thread_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_trace_buffer_full_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_trace_disable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_trace_enable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_trace_event_filter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_trace_event_unfilter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_trace_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_trace_interrupt_control.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_trace_isr_enter_insert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_trace_isr_exit_insert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_trace_object_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_trace_object_unregister.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/tx_trace_user_event_insert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_block_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_block_pool_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_block_pool_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_block_pool_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_block_pool_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_block_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_byte_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_byte_pool_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_byte_pool_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_byte_pool_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_byte_pool_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_byte_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_event_flags_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_event_flags_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_event_flags_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_event_flags_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_event_flags_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_event_flags_set_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_mutex_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_mutex_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_mutex_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_mutex_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_mutex_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_mutex_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_queue_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_queue_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_queue_flush.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_queue_front_send.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_queue_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_queue_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_queue_receive.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_queue_send.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_queue_send_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_semaphore_ceiling_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_semaphore_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_semaphore_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_semaphore_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_semaphore_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_semaphore_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_semaphore_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_semaphore_put_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_entry_exit_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_preemption_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_priority_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_relinquish.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_resume.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_suspend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_terminate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_time_slice_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_thread_wait_abort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_timer_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_timer_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_timer_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_timer_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_timer_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/threadx/common/src/txe_timer_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_usbx_port/rm_usbx_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_iar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_initialize_low_level.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_isr_end.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_isr_start.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_port_wait_thread_ready.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_thread_interrupt_control.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_thread_interrupt_disable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_thread_interrupt_restore.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_thread_schedule.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_thread_secure_stack_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_thread_secure_stack_free.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_thread_stack_build.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_thread_system_return.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/tx_timer_interrupt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/txe_thread_secure_stack_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_threadx_port/txe_thread_secure_stack_free.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/hw/r_usb_creg_abs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/hw/r_usb_creg_access.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/hw/r_usb_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/hw/r_usb_hostelectrical.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/hw/r_usb_hreg_abs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/hw/r_usb_hreg_access.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/hw/r_usb_mcu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/hw/r_usb_preg_abs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/hw/r_usb_preg_access.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_cdataio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_clibusbip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_cstd_rtos.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_hbc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_hcontrolrw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_hdriver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_hhubsys.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_hintfifo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_hinthandler_usbip0.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_hinthandler_usbip1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_hlibusbip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_hmanager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_hscheduler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_hsignal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_hstdfunction.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_pbc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_pcontrolrw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_pdriver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_pintfifo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_pinthandler_usbip0.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_plibusbip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_psignal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_pstdfunction.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/src/driver/r_usb_pstdrequest.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_usb_basic/r_usb_basic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sci_b_uart/r_sci_b_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_ioport/r_ioport.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/ra8p1/bsp_linker.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_clocks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_delay.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_group_irq.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_guard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_ipc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_irq.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_macl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_ospi_b.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_register_protection.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_sbrk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_sdram.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/all/bsp_security.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/startup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/system.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/board/ra8p1_ek/board_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/board/ra8p1_ek/board_leds.c
)
