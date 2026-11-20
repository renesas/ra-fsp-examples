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
project(ethernet_ek_ra8d2_ep_cmakebuild
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
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/include
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_freertos_plus_tcp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ethernet_ep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ethernet_example_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_warmstart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/net_thread_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/hal_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/net_thread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/pin_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/vector_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_freertos_port/port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_freertos_plus_tcp/NetworkInterface.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sci_b_uart/r_sci_b_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_rmac_phy/targets/VSC8541/r_rmac_phy_target_vsc8541.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_rmac_phy/targets/KSZ8091RNB/r_rmac_phy_target_ksz8091rnb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_rmac_phy/targets/KSZ8041/r_rmac_phy_target_ksz8041.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_rmac_phy/targets/ICS1894/r_rmac_phy_target_ics1894.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_rmac_phy/targets/GPY111/r_rmac_phy_target_gpy111.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_rmac_phy/targets/DP83620/r_rmac_phy_target_dp83620.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_rmac_phy/r_rmac_phy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_rmac/r_rmac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_layer3_switch/r_layer3_switch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_ioport/r_ioport.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/ra8d2/bsp_linker.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/board/ra8d2_ek/board_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/board/ra8d2_ek/board_leds.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/portable/MemMang/heap_4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_ARP.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_BitConfig.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_DHCP.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_DHCPv6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS_Cache.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS_Callback.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS_Networking.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS_Parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_ICMP.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_IP.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_IP_Timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_IP_Utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_IPv4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_IPv4_Sockets.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_IPv4_Utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_IPv6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_IPv6_Sockets.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_IPv6_Utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_ND.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_RA.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_Routing.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_Sockets.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_Stream_Buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_IP.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_IP_IPv4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_IP_IPv6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_Reception.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_State_Handling.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_State_Handling_IPv4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_State_Handling_IPv6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_Transmission.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_Transmission_IPv4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_Transmission_IPv6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_Utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_Utils_IPv4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_Utils_IPv6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_WIN.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_Tiny_TCP.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_UDP_IP.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_UDP_IPv4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/source/FreeRTOS_UDP_IPv6.c
)
