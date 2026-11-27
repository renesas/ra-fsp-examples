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
project(wifi_fpb_ra6e2_ep_cmakebuild
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
# Source .cproject (config Debug) is configured "-funsigned-char", not RASC's default "-fsigned-char".
list(TRANSFORM RASC_CMAKE_C_FLAGS REPLACE "^-fsigned-char$" "-funsigned-char")
list(TRANSFORM RASC_CMAKE_CXX_FLAGS REPLACE "^-fsigned-char$" "-funsigned-char")
list(TRANSFORM RASC_CMAKE_ASM_FLAGS REPLACE "^-fsigned-char$" "-funsigned-char")
list(TRANSFORM RASC_CMAKE_EXE_LINKER_FLAGS REPLACE "^-fsigned-char$" "-funsigned-char")
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
	${CMAKE_CURRENT_SOURCE_DIR}/ra_cfg/arm/littlefs
	${CMAKE_CURRENT_SOURCE_DIR}/ra_cfg/arm/mbedtls
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Utilities/logging
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/VisualStudio_StaticProjects/MbedTLS
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Application-Protocols/coreMQTT/source/interface
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Application-Protocols/network_transport
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/corePKCS11/source/dependency/3rdparty/mbedtls_utils
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/amazon-freertos/demos/dev_mode_key_provisioning/include
	${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/littlefs
	${CMAKE_CURRENT_SOURCE_DIR}/ra_cfg/arm
	${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library
	${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/include
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/trng
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/common
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_aws_sockets_wrapper_wifi_silex
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_psa_crypto/inc
	${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_wifi_onchip_silex
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/corePKCS11/source/dependency/3rdparty/pkcs11/published/2-40-errata-1
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/corePKCS11/source/include
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/corePKCS11/source/dependency/3rdparty/pkcs11
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Application-Protocols/coreHTTP/source/interface
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Application-Protocols/coreHTTP/source/include
	${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Application-Protocols/coreHTTP/source/dependency/3rdparty/llhttp/include
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp_socket.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/wifi_ep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/wifi_example_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/wifi_thread_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/hal_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/pin_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/vector_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/wifi_thread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_wifi_onchip_silex/rm_wifi_api_silex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_wifi_onchip_silex/rm_wifi_onchip_silex.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_mbedtls/x509_crt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_littlefs_flash/rm_littlefs_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_freertos_port/port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_aws_sockets_wrapper_wifi_silex/tcp_sockets_wrapper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/rm_aws_pkcs11_pal_littlefs/rm_aws_pkcs11_pal_littlefs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sci_uart/r_sci_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/trng/r_sce_trng.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_sce/trng/r_sce_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_ioport/r_ioport.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_flash_hp/r_flash_hp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/r_dtc/r_dtc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/src/bsp/mcu/ra6e2/bsp_linker.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/board/ra6e2_fpb/board_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/board/ra6e2_fpb/board_leds.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/amazon-freertos/demos/dev_mode_key_provisioning/src/aws_dev_mode_key_provisioning.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/portable/MemMang/heap_4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS/Source/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/VisualStudio_StaticProjects/MbedTLS/mbedtls_freertos_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/corePKCS11/source/portable/mbedtls/core_pkcs11_mbedtls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/corePKCS11/source/dependency/3rdparty/mbedtls_utils/mbedtls_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/corePKCS11/source/core_pkcs11.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/corePKCS11/source/core_pki_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Application-Protocols/network_transport/mbedtls_bio_tcp_sockets_wrapper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Application-Protocols/network_transport/mbedtls_pk_pkcs11.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Application-Protocols/network_transport/transport_mbedtls_pkcs11.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Application-Protocols/coreHTTP/source/dependency/3rdparty/llhttp/src/api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Application-Protocols/coreHTTP/source/dependency/3rdparty/llhttp/src/http.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Application-Protocols/coreHTTP/source/dependency/3rdparty/llhttp/src/llhttp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/aws/FreeRTOS/FreeRTOS-Plus/Source/Application-Protocols/coreHTTP/source/core_http_client.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_cache.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_ciphersuites.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_cookie.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_msg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_ticket.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_tls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_tls12_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_tls12_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_tls13_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_tls13_generic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_tls13_keys.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/ssl_tls13_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/threading.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/timing.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/version.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/version_features.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/x509.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/x509_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/x509_crl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/x509_csr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/x509write_crt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/mbedtls/library/x509write_csr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/littlefs/lfs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/littlefs/lfs_util.c
)
