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
project(tml_fpb_ra0l1_ep_cmakebuild
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
# e2studio Managed-Build (config Debug) actually compiles with "-Oz", not RASC's own assumed default for this config.
list(REMOVE_ITEM RASC_DEBUG_FLAGS "-O0" "-O1" "-O2" "-O3" "-Os" "-Oz")
list(APPEND RASC_DEBUG_FLAGS "-Oz")
# Source .cproject (config Debug) is configured "-funsigned-char", not RASC's default "-fsigned-char".
list(TRANSFORM RASC_CMAKE_C_FLAGS REPLACE "^-fsigned-char$" "-funsigned-char")
list(TRANSFORM RASC_CMAKE_CXX_FLAGS REPLACE "^-fsigned-char$" "-funsigned-char")
list(TRANSFORM RASC_CMAKE_ASM_FLAGS REPLACE "^-fsigned-char$" "-funsigned-char")
list(TRANSFORM RASC_CMAKE_EXE_LINKER_FLAGS REPLACE "^-fsigned-char$" "-funsigned-char")
# e2studio Managed-Build actually compiles with "-mcpu=cortex-m23" (RASC's own -mcpu may be missing a device extension modifier like "+nopacbti").
list(TRANSFORM RASC_CMAKE_C_FLAGS REPLACE "^-mcpu=.*$" "-mcpu=cortex-m23")
list(TRANSFORM RASC_CMAKE_CXX_FLAGS REPLACE "^-mcpu=.*$" "-mcpu=cortex-m23")
list(TRANSFORM RASC_CMAKE_ASM_FLAGS REPLACE "^-mcpu=.*$" "-mcpu=cortex-m23")
list(TRANSFORM RASC_CMAKE_EXE_LINKER_FLAGS REPLACE "^-mcpu=.*$" "-mcpu=cortex-m23")
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


target_link_libraries(${PROJECT_NAME}.elf PRIVATE
    crt0
    c++abi
)



# --- Match e2studio Managed-Build link-time optimization flag ---
# RASC's own per-config target_link_options() calls above never include one
target_link_options(${PROJECT_NAME}.elf PRIVATE
    $<$<CONFIG:Debug>:-Oz>
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/elc_ep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_warmstart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lpm_ep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tml_ep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tml_example_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/elc_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/hal_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/pin_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/vector_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_uarta/r_uarta.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_tml/r_tml.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_lpm/r_lpm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_ioport/r_ioport.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_elc/r_elc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/ra0l1/bsp_linker.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/board/ra0l1_fpb/board_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/board/ra0l1_fpb/board_leds.c
)
