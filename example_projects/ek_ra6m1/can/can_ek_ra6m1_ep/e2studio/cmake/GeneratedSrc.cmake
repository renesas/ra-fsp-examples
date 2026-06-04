# This file was automatically generated and updated by RASC and should not be edited by the user.
# Use CMakeLists.txt to override the settings in this file 

if ((NOT RENESAS_IDE) OR (NOT RENESAS_IDE STREQUAL "e2studio"))

# Pre-build step: run RASC to generate project content if configuration.xml is changed
	message("Running RASC for generating project ${PROJECT_NAME} content since modification is detected in configuration.xml:")
	message("${RASC_EXE_PATH} -nosplash --launcher.suppressErrors --generate --devicefamily ra --compiler GCC --toolchainversion ${CMAKE_C_COMPILER_VERSION} --buildconfiguration ${CMAKE_BUILD_TYPE} ${CMAKE_CURRENT_SOURCE_DIR}/configuration.xml")

	execute_process(
		COMMAND
			${RASC_EXE_PATH} -nosplash --launcher.suppressErrors --generate --devicefamily ra --compiler GCC --toolchainversion ${CMAKE_C_COMPILER_VERSION} --buildconfiguration ${CMAKE_BUILD_TYPE} ${CMAKE_CURRENT_SOURCE_DIR}/configuration.xml
		OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/rasc_conf_cmd_out.txt
		ERROR_FILE  ${CMAKE_CURRENT_BINARY_DIR}/rasc_conf_cmd_err.txt
		RESULT_VARIABLE resultVar
		TIMEOUT 600
	)

	if(resultVar AND NOT resultVar EQUAL 0)
		message(FATAL_ERROR "RASC Execution is failed: ${resultVar}, check rasc_conf_cmd_err.txt")
	endif()

endif()

#source directories
file(GLOB_RECURSE Source_Files 
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/*.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/*.S
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/*.asm
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/*.sx
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/*.msa
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/*.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/*.S
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/*.asm
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/*.sx
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen/*.msa
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.S
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.sx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.msa)

# Exclude files from source build
list(REMOVE_ITEM Source_Files ${EXCLUDED_SOURCE_FILES})


file(GLOB_RECURSE Companion_Source_Files
    )
# Exclude files from source build
list(REMOVE_ITEM Source_Files ${EXCLUDED_SOURCE_FILES})

SET(ALL_FILES ${Source_Files})

SET(COMPANION_FILES ${Companion_Source_Files})

add_executable(${PROJECT_NAME}.elf ${ALL_FILES})


target_compile_options(${PROJECT_NAME}.elf
                       PRIVATE
                       $<$<CONFIG:Debug>:${RASC_DEBUG_FLAGS}>
                       $<$<CONFIG:Release>:${RASC_RELEASE_FLAGS}>
                       $<$<CONFIG:MinSizeRel>:${RASC_MIN_SIZE_RELEASE_FLAGS}>
                       $<$<CONFIG:RelWithDebInfo>:${RASC_RELEASE_WITH_DEBUG_INFO}>)

target_compile_options(${PROJECT_NAME}.elf PRIVATE  $<$<COMPILE_LANGUAGE:ASM>:${RASC_CMAKE_ASM_FLAGS}>)
target_compile_options(${PROJECT_NAME}.elf PRIVATE  $<$<COMPILE_LANGUAGE:C>:${RASC_CMAKE_C_FLAGS}>)
target_compile_options(${PROJECT_NAME}.elf PRIVATE  $<$<COMPILE_LANGUAGE:CXX>:${RASC_CMAKE_CXX_FLAGS}>)

target_link_options(${PROJECT_NAME}.elf PRIVATE $<$<LINK_LANGUAGE:ASM>:${RASC_CMAKE_EXE_LINKER_FLAGS}>)
target_link_options(${PROJECT_NAME}.elf PRIVATE $<$<LINK_LANGUAGE:C>:${RASC_CMAKE_EXE_LINKER_FLAGS}>)
target_link_options(${PROJECT_NAME}.elf PRIVATE $<$<LINK_LANGUAGE:CXX>:${RASC_CMAKE_EXE_LINKER_FLAGS}>)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE ${RASC_CMAKE_DEFINITIONS})

target_include_directories(${PROJECT_NAME}.elf
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/inc/api
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/fsp/inc/instances
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/CMSIS_5/CMSIS/Core/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_gen
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_cfg/fsp_cfg
    ${CMAKE_CURRENT_SOURCE_DIR}/ra_cfg/fsp_cfg/bsp
    ${CMAKE_CURRENT_SOURCE_DIR}/"."
    ${CMAKE_CURRENT_SOURCE_DIR}/ra/arm/CMSIS_6/CMSIS/Core/Include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/
)

target_link_directories(${PROJECT_NAME}.elf
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/script
)

target_link_libraries(${PROJECT_NAME}.elf
    PRIVATE
    
)

# suppress compiler warnings for companion source files only
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
    # also downgrade errors to warnings with GCC 14 and later versions
    set_source_files_properties(${COMPANION_FILES} PROPERTIES COMPILE_FLAGS
        "-w -Wno-error=declaration-missing-parameter-type -Wno-error=implicit-function-declaration -Wno-error=implicit-int -Wno-error=incompatible-pointer-types -Wno-error=int-conversion -Wno-error=return-mismatch"
    )
else()
    set_source_files_properties(${COMPANION_FILES} PROPERTIES COMPILE_FLAGS "-w")
endif()

add_custom_command(
    TARGET
        ${PROJECT_NAME}.elf
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O srec ${PROJECT_NAME}.elf ${PROJECT_NAME}.srec
    COMMENT "Creating S-record file in ${PROJECT_BINARY_DIR}"
)


if ((NOT RENESAS_IDE) OR (NOT RENESAS_IDE STREQUAL "e2studio"))


	set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/configuration.xml)

	# Post-build step: run RASC to generate the SmartBundle file
	add_custom_command(
	    OUTPUT
	        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.sbd
	    DEPENDS
	        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf
	    COMMAND
	        echo "Running RASC post-build to generate Smart Bundle file for ${PROJECT_NAME}:"
	    COMMAND
	        echo ${RASC_EXE_PATH} -nosplash --launcher.suppressErrors --gensmartbundleandpartition --devicefamily ra --compiler GCC --toolchainversion ${CMAKE_C_COMPILER_VERSION} --buildconfiguration ${CMAKE_BUILD_TYPE} ${CMAKE_CURRENT_SOURCE_DIR}/configuration.xml ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf 
	    COMMAND
	        ${RASC_EXE_PATH} -nosplash --launcher.suppressErrors --gensmartbundleandpartition --devicefamily ra --compiler GCC --toolchainversion ${CMAKE_C_COMPILER_VERSION} --buildconfiguration ${CMAKE_BUILD_TYPE} ${CMAKE_CURRENT_SOURCE_DIR}/configuration.xml ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf  2> rasc_cmd_log.txt
	)

	add_custom_target(generate_sbd_${PROJECT_NAME} ALL
		DEPENDS
			${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.sbd
		VERBATIM
	)

endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/script/PostBuild.cmake)
	include(${CMAKE_CURRENT_SOURCE_DIR}/script/PostBuild.cmake)
endif()
