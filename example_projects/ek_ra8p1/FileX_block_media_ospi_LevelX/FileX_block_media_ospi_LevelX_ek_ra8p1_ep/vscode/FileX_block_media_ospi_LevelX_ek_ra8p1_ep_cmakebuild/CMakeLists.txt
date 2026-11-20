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
project(FileX_block_media_ospi_LevelX_ek_ra8p1_ep_cmakebuild
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

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE $<$<CONFIG:Debug>:
    USE_VIRTUAL_COM=1>)

target_include_directories(${PROJECT_NAME}.elf
    BEFORE
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)



target_include_directories(${PROJECT_NAME}.elf
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/.
	${CMAKE_CURRENT_SOURCE_DIR}/ra_cfg/fsp_cfg/azure/fx
	${CMAKE_CURRENT_SOURCE_DIR}/ra_cfg/ra_gen
	${CMAKE_CURRENT_SOURCE_DIR}/ra_cfg/fsp_cfg/middleware
	${CMAKE_CURRENT_SOURCE_DIR}/ra_cfg/fsp_cfg/azure/lx
	${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/inc/ports
	${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/inc
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_thread_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/board_ospi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/console_example_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/console_thread_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filex_ep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filex_example_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_warmstart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/levelx_ep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/terminal_example_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/terminal_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/terminal_thread_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/app_thread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/console_thread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/hal_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/pin_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/terminal_thread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/vector_data.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_256byte_ecc_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_256byte_ecc_compute.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_block_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_block_data_move.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_block_find.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_block_mapping_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_block_status_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_close.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_data_page_copy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_defragment.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_driver_block_erase.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_driver_block_erased_verify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_driver_block_status_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_driver_block_status_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_driver_page_erased_verify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_erase_count_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_extended_cache_enable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_format.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_free_block_list_add.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_mapped_block_list_add.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_mapped_block_list_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_mapped_block_list_remove.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_memory_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_metadata_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_metadata_build.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_metadata_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_open.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_page_ecc_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_page_ecc_compute.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_partial_defragment.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_sector_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_sector_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_sector_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_sectors_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_sectors_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_sectors_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_simulator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nand_flash_system_error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_block_reclaim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_close.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_defragment.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_driver_block_erase.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_driver_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_driver_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_extended_cache_enable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_logical_sector_find.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_next_block_to_erase_find.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_open.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_partial_defragment.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_physical_sector_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_sector_mapping_cache_invalidate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_sector_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_sector_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_sector_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_simulator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/levelx/common/src/lx_nor_flash_system_error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_attributes_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_attributes_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_default_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_default_get_copy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_default_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_entry_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_entry_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_exFAT_entry_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_exFAT_entry_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_exFAT_free_search.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_exFAT_unicode_entry_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_first_entry_find.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_first_full_entry_find.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_free_search.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_information_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_local_path_clear.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_local_path_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_local_path_get_copy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_local_path_restore.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_local_path_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_long_name_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_long_name_get_extended.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_name_extract.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_name_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_next_entry_find.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_next_full_entry_find.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_rename.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_search.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_short_name_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_directory_short_name_get_extended.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_add_FAT_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_add_bitmap_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_add_checksum_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_add_dir_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_apply_logs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_calculate_checksum.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_cleanup_FAT_chain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_create_log_file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_enable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_read_FAT.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_read_directory_sector.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_read_log_file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_recover.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_reset_log_file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_set_FAT_chain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_transaction_end.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_transaction_fail.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_transaction_start.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_fault_tolerant_write_log_file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_attributes_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_attributes_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_best_effort_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_close.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_date_time_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_extended_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_extended_best_effort_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_extended_relative_seek.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_extended_seek.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_extended_truncate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_extended_truncate_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_open.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_relative_seek.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_rename.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_seek.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_truncate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_truncate_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_file_write_notify_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_abort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_boot_info_extract.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_cache_invalidate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_check_FAT_chain_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_check_lost_cluster_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_close.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_close_notify_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_exFAT_format.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_extended_space_available.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_flush.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_format.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_format_oem_name_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_format_type_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_format_volume_id_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_open.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_open_notify_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_space_available.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_volume_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_volume_get_extended.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_volume_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_media_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_partition_offset_calculate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_ram_driver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_system_date_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_system_date_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_system_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_system_time_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_system_time_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_system_timer_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_trace_event_insert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_trace_event_update.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_trace_object_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_trace_object_unregister.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_directory_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_directory_entry_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_directory_entry_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_directory_rename.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_directory_search.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_file_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_file_rename.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_length_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_length_get_extended.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_name_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_name_get_extended.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_short_name_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_unicode_short_name_get_extended.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_16_unsigned_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_16_unsigned_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_32_unsigned_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_32_unsigned_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_64_unsigned_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_64_unsigned_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_FAT_entry_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_FAT_entry_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_FAT_flush.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_FAT_map_flush.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_FAT_sector_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_absolute_path_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_allocate_new_cluster.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_bitmap_cache_prepare.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_bitmap_cache_update.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_bitmap_flush.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_bitmap_free_cluster_find.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_bitmap_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_bitmap_start_sector_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_cluster_free.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_cluster_state_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_cluster_state_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_geometry_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_name_hash_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_size_calculate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_system_area_checksum_verify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_system_area_checksum_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_system_area_format.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_system_sector_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_unicode_name_hash_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_exFAT_upcase_table.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_logical_sector_cache_entry_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_logical_sector_flush.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_logical_sector_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_logical_sector_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_memory_copy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_memory_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_string_length_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fx_utility_token_length_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_attributes_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_attributes_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_default_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_default_get_copy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_default_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_first_entry_find.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_first_full_entry_find.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_information_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_local_path_clear.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_local_path_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_local_path_get_copy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_local_path_restore.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_local_path_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_long_name_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_long_name_get_extended.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_name_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_next_entry_find.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_next_full_entry_find.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_rename.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_short_name_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_directory_short_name_get_extended.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_fault_tolerant_enable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_attributes_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_attributes_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_best_effort_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_close.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_date_time_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_extended_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_extended_best_effort_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_extended_relative_seek.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_extended_seek.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_extended_truncate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_extended_truncate_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_open.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_relative_seek.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_rename.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_seek.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_truncate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_truncate_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_file_write_notify_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_abort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_cache_invalidate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_close.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_close_notify_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_exFAT_format.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_extended_space_available.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_flush.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_format.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_open.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_open_notify_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_space_available.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_volume_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_volume_get_extended.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_volume_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_media_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_system_date_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_system_date_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_system_time_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_system_time_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_unicode_directory_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_unicode_directory_rename.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_unicode_file_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_unicode_file_rename.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_unicode_name_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_unicode_name_get_extended.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_unicode_short_name_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/microsoft/azure-rtos/filex/common/src/fxe_unicode_short_name_get_extended.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_levelx_nor_spi/rm_levelx_nor_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_filex_levelx_nor/rm_filex_levelx_nor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sci_b_uart/r_sci_b_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_ospi_b/r_ospi_b.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_ioport/r_ioport.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_dmac/r_dmac.c
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
