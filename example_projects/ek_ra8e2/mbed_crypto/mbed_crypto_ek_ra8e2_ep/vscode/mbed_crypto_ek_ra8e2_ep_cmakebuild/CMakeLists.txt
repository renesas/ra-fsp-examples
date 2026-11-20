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
project(mbed_crypto_ek_ra8e2_ep_cmakebuild
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
	${CMAKE_CURRENT_SOURCE_DIR}/ra_cfg/arm
	${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/include
	${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library
	${CMAKE_CURRENT_SOURCE_DIR}/ra_cfg/arm/littlefs
	${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/littlefs
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/private/inc
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/public/inc
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/common
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/inc
	${CMAKE_CURRENT_SOURCE_DIR}/ra_cfg/arm/mbedtls
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_warmstart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mbed_crypto_ep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mbed_crypto_example_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/hal_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/pin_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/vector_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/aes_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/aes_alt_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/aes_vendor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/asymmetric_vendor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/ccm_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/ccm_alt_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/chacha20_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/chacha20_alt_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/chachapoly_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/chachapoly_alt_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/cipher_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/cmac_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/ctr_drbg_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/ecdh_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/ecdsa_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/ecdsa_alt_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/ecp_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/ecp_alt_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/ecp_curves_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/gcm_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/gcm_alt_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/mldsa_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/mlkem_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/platform_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/rm_psa_crypto.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/rsa_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/rsa_alt_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/sha256_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/sha256_alt_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/sha3_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/sha3_alt_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/sha512_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/sha512_alt_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/trng_entropy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/vendor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_littlefs_flash/rm_littlefs_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sci_b_uart/r_sci_b_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/DomainParams.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/change_endian_long.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func008.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func012.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func013.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func016.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func017.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func027.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func028.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func031.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func043.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func044.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func048.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func049.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func052.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func053.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func054.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func055.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func057.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func058.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func059.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func061.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func062.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func063.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func065.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func068.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func070.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func071.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func073.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func074.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func075.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func076.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func077.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func078.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func079.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func081.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func082.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func083.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func086.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func087.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func088.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func089.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func090.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func091.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func092.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func093.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func094.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func095.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func100.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func101.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func102.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func103.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func202.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func209.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func214.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func215.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func216.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func302.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func303.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func304.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func305.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func310.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func311.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func312.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func313.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func314.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func315.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func316.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func317.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func318.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func319.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func320.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func321.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func322.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func323.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func324.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func401.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func402.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func403.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func404.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func405.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func406.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func501.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func502.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_func503.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p00.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p07.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p08.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p0a.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p0b.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p11.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p12.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p13.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p14.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p15.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p17.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p18.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p19.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p1a.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p1b.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p20.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p21.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p29a.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p29f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p29i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p29t.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p29u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p2b.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p2c_r2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p2d_r2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p2e.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p31.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p32a.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p32f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p32i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p32t.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p32u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p34a.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p34f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p34i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p34t.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p34u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p36a.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p36f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p36i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p36t.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p36u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p3a.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p3b.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p3c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p3d.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p3e.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p3f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p40.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p41f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p41i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p41u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p44f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p44i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p44u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p47f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p47i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p47u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p50f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p50i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p50u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p53.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p54.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p56.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p57.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p6e.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p70.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p72.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p73f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p73i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p73r.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p73s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p73u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p74.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p75.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p75f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p75i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p75r.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p75s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p75u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p76f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p76i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p76u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p79.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p7a.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p7b.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p7c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p7d.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p7e.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p7f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p81.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p82.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p83a.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p83f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p83i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p83t.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p83u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p85a.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p85f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p85i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p85t.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p85u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p87f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p87i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p87u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p89f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p89i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p89u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p95f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p95i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p95u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p98f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p98i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p98u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p9a.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p9b.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_p9c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pa1f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pa1i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pa1u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pa4f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pa4i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pa4u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pa7f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pa7i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pa7u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pb0f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pb0i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pb0u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pb3f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pb3i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pb3u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pb6f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pb6i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pb6u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pb9f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pb9i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pb9u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pc2f.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pc2i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pc2u.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pdcf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pdci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pdcu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pf0.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pf1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pf4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pf5.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pf6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/hw_sce_p_pf9.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/primitive/s_flash2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/crypto_procedures/src/rsip_e51a/plainkey/adaptors/r_sce_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_ioport/r_ioport.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_flash_hp/r_flash_hp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/ra8e2/bsp_linker.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/board/ra8e2_ek/board_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/board/ra8e2_ek/board_leds.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/aes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/aesce.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/aesni.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/aria.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/asn1parse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/asn1write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/base64.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/bignum.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/bignum_core.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/bignum_mod.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/bignum_mod_raw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/block_cipher.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/camellia.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ccm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/chacha20.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/chachapoly.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/cipher.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/cipher_wrap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/cmac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/constant_time.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ctr_drbg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/debug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/des.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/dhm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ecdh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ecdsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ecjpake.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ecp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ecp_curves.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ecp_curves_new.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/entropy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/entropy_poll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/gcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/hkdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/hmac_drbg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/lmots.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/lms.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/md.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/md5.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/memory_buffer_alloc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/mps_reader.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/mps_trace.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/net_sockets.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/nist_kw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/oid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/padlock.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/pem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/pk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/pk_ecc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/pk_wrap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/pkcs12.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/pkcs5.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/pkcs7.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/pkparse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/pkwrite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/platform.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/platform_util.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/poly1305.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_aead.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_cipher.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_driver_wrappers_no_static.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_ecp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_ffdh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_hash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_mac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_mldsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_mlkem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_pake.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_random.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_rsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_se.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_slot_management.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_crypto_storage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_its_file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/psa_util.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ripemd160.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/rsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/rsa_alt_helpers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/sha1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/sha3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/sha512.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/threading.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/timing.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/version.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/version_features.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/littlefs/lfs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/littlefs/lfs_util.c
)
