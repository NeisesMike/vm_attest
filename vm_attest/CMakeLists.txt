#
# Copyright 2019, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(DATA61_BSD)
#

cmake_minimum_required(VERSION 3.8.2)

project(vm_attest C)

# needed to build the crypto lib correctly
set(CAmkES "true")

add_definitions(-DCAMKES)
find_program(CAKEML_COMPILER NAMES "cake32")
find_program( odroid_cross_compiler arm-linux-gnueabihf-gcc )

find_package(camkes-vm-linux REQUIRED)
#find_package(camkes-vm REQUIRED)
#find_package(camkes-vm-images REQUIRED)
#find_package(camkes-arm-vm REQUIRED)

include(../../arm_vm_helpers.cmake)
include(${CAMKES_VM_LINUX_HELPERS_PATH})
include(${CAMKES_VM_LINUX_MODULE_HELPERS_PATH})
include(${CAMKES_VM_LINUX_SOURCE_HELPERS_PATH})
include(ExternalProject)
include(external-project-helpers)

#camkes_arm_vm_import_project()

message("The value of CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
message("The value of CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")



# Taken and modified from tools/seL4/cmake-tool/helpers/external-project-helpers.cmake
# Function to add a given file to an overlay directory.
# filename: The preferred name of the file in the overlay
# file_location: Absolute path to the file we are adding the overlay
# root_location: The location that the file we are adding will exist in the VM's root filesystem
# overlay_name: Overlay target name
function(MyAddFileToOverlayDir filename file_location root_location overlay_name)
    # Get any existing dependencies when adding the file into the overlay
    cmake_parse_arguments(PARSE_ARGV 4 ROOTFS_FILE_OVERLAY
        ""
        ""
        "DEPENDS"
    )
    if (NOT "${ROOTFS_FILE_OVERLAY_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "Unknown arguments to MyAddFileToOverlay")
    endif()
    set(rootfs_output_dir "${CMAKE_CURRENT_BINARY_DIR}/${overlay_name}")
    if(TARGET ${overlay_name})
        get_target_property(rootfs_output_dir ${overlay_name} ROOTFS_OUTPUT_DIR)
    else()
        add_custom_target(${overlay_name})
        set_property(TARGET ${overlay_name} APPEND PROPERTY ROOTFS_OUTPUT_DIR "${rootfs_output_dir}")
    endif()
    # Copy the file into the rootfs output directory at 'root_location'
    add_custom_command(OUTPUT ${rootfs_output_dir}/${root_location}/${filename}
        COMMAND ${CMAKE_COMMAND} -E copy "${file_location}" "${rootfs_output_dir}/${root_location}/${filename}"
        VERBATIM
        DEPENDS ${file_location} ${ROOTFS_FILE_OVERLAY_DEPENDS}
    )
    # Create a semi-unique target name based on the files location
    set(copy_target_name "copy_${rootfs_output_dir}/${root_location}/${filename}")
    string(REGEX REPLACE "/" "_" copy_target_name ${copy_target_name})
    string(MD5 copy_target_name ${copy_target_name})
    # Create a custom target for the copy command
    add_custom_target(${copy_target_name} DEPENDS "${rootfs_output_dir}/${root_location}/${filename}")
    # Add the copy command as a dependency for the overlay directory
    set_property(TARGET ${overlay_name} APPEND PROPERTY DEPENDS "${copy_target_name}")
    set_property(TARGET ${overlay_name} APPEND PROPERTY DEPENDS "${file_location}")
endfunction(MyAddFileToOverlayDir)

# Taken and modified from tools/seL4/cmake-tool/helpers/external-project-helpers.cmake
# Function for declaring target object files that are produced by an external project. This adds a custom_command
# that forces a stale check on the object file after the External projects install step.
# external_proj_target: Target name of the external project
# external_prog_output_dir: Location of binary or install directory for external project
# FILES: List of object files that exist in the external project
function(MyDeclareExternalProjObjectFiles external_proj_target external_proj_output_dir)
    # Parse the given files object files
    cmake_parse_arguments(PARSE_ARGV 2 EXT_OBJECT "" "" "FILES")
    if(NOT "${EXT_OBJECT_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "Unknown arguments to DeclareExternalProjObjectFiles")
    endif()
    # Necessary for FILES to be passed
    if(NOT EXT_OBJECT_FILES)
        message(FATAL_ERROR "NO FILES declared for ${external_proj_target}")
    endif()
    # Get external project binary and stamp dir properties
    ExternalProject_Get_property(${external_proj_target} STAMP_DIR)
    foreach(obj_file IN LISTS EXT_OBJECT_FILES)
        # Generate a unique name based on the object files location
        set(file_path ${external_proj_output_dir}/${obj_file})
        list(APPEND objfiles ${file_path})
    endforeach()
    if(NOT (TARGET ${external_proj_target}-install))
        ExternalProject_Add_StepTargets(${external_proj_target} install)
    endif()
    add_custom_command(
        OUTPUT ${objfiles}
        COMMAND true
        DEPENDS ${external_proj_target}-install ${STAMP_DIR}/${external_proj_target}-install
    )
endfunction(MyDeclareExternalProjObjectFiles)

# Taken and modified from projects/camkes-vm-linux/vm-linux-helpers.cmake
# Wrapper function to declare object files in an external project and add them to the targeted overlay
# external_target: Target of external project
# external_install_dir: The install/binary directory where the files are located
# overlay_target: The overlay target we are adding files to
# overlay_root_location: The location in the overlay the file is to be installed. Note all files passed in a given
# call of this helper will install in the same root location.
# FILES: A list of files to declare from an external project and add to the given overlay
function(MyAddExternalProjFilesToOverlay external_target external_install_dir overlay_target overlay_root_location)
    # Get the external project files to add the the overlay
    cmake_parse_arguments(PARSE_ARGV 4 EXTERNAL_PROJ_OVERLAY
        ""
        ""
        "FILES"
    )
    # Error checking
    if (NOT "${EXTERNAL_PROJ_OVERLAY_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "Unknown arguments to AddExternalProjFilesToOverlay")
    endif()
    # Necessary to provide a least one file
    if(NOT EXTERNAL_PROJ_OVERLAY_FILES)
        message(FATAL_ERROR "NO FILES declared in AddExternalProjFilesToOverlay")
    endif()
    # Declare the project files
    MyDeclareExternalProjObjectFiles(${external_target} ${external_install_dir}
        FILES ${EXTERNAL_PROJ_OVERLAY_FILES})
    # Iterate adding each file to the overlay
    foreach(file IN LISTS EXTERNAL_PROJ_OVERLAY_FILES)
        get_filename_component(file_name ${file} NAME)
        MyAddFileToOverlayDir("${file_name}" ${external_install_dir}/${file} "${overlay_root_location}" ${overlay_target}
            DEPENDS ${external_target})
    endforeach()
endfunction(MyAddExternalProjFilesToOverlay)




# =============== new =====================
# Create our CPP Flags based on ARM VM config variables
set(rootfs_address "0x4d700000")
set(dtb_file "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/linux-dtb")
# ============= end new ===================



# Create our CPP Flags based on ARM VM config variables
set(cpp_flags "-DKERNELARMPLATFORM_EXYNOS5422")
set(linux_repo "https://github.com/hardkernel/linux.git")
set(linux_tag "4.14.87-153")
set(linux_arch "arm")
set(linux_cross_compile "arm-linux-gnueabihf-")





if(BUILD_CROSSVM)
    set(rootfs_file "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/rootfs.cpio.gz")
    # Checkout and configure linux to build crossvm module
    ExternalProject_Add(
        checkout_linux
        GIT_REPOSITORY
        ${linux_repo}
        GIT_TAG
        ${linux_tag}
        GIT_SHALLOW
        1
        GIT_PROGRESS
        1
        BUILD_COMMAND
        ""
        INSTALL_COMMAND
        ""
        CONFIGURE_COMMAND
        ""
        SOURCE_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/linux_out
    )
    # Linux config and symvers are to be copied to unpacked archive
    set(linux_config "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/linux_configs/config")
    set(linux_symvers "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/linux_configs/Module.symvers")
    # Configure unpacked archive with config and symvers
    ConfigureLinux(
        ${CMAKE_CURRENT_BINARY_DIR}/linux_out
        ${linux_config}
        ${linux_symvers}
        configure_vm_linux
        ARCH
        ${linux_arch}
        CROSS_COMPILE
        ${linux_cross_compile}
        DEPENDS
        checkout_linux
    )

    # Compile CrossVM Dataport Module
    DefineLinuxModule(
        ${CAMKES_VM_LINUX_DIR}/camkes-linux-artifacts/camkes-linux-modules/camkes-connector-modules/connection
        output_module
        output_module_target
        KERNEL_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/linux_out
        ARCH
        ${linux_arch}
        CROSS_COMPILE
        ${linux_cross_compile}
        DEPENDS
        checkout_linux
        configure_vm_linux
    )
    AddFileToOverlayDir(
        "connection.ko"
        ${output_module}
        "lib/modules/4.14.87/kernel/drivers/vmm"
        overlay
        DEPENDS
        output_module_target
    )

    # Complile CrossVM Dataport Apps
    ExternalProject_Add(
        dataport-apps
        URL
        file:///${CAMKES_VM_LINUX_DIR}/camkes-linux-artifacts/camkes-linux-apps/camkes-connector-apps/pkgs/dataport
        BINARY_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/dataport_apps
        INSTALL_COMMAND
        ""
        BUILD_ALWAYS
        ON
        EXCLUDE_FROM_ALL
        CMAKE_ARGS
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    )
    AddExternalProjFilesToOverlay(
        dataport-apps
        ${CMAKE_CURRENT_BINARY_DIR}/dataport_apps
        overlay
        "usr/bin"
        FILES
        dataport_read
        dataport_write
    )

    # Complile CrossVM Event Apps
    ExternalProject_Add(
        event-apps
        URL
        file:///${CAMKES_VM_LINUX_DIR}/camkes-linux-artifacts/camkes-linux-apps/camkes-connector-apps/pkgs/emits_event
        BINARY_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/emits_event_apps
        INSTALL_COMMAND
        ""
        BUILD_ALWAYS
        ON
        EXCLUDE_FROM_ALL
        CMAKE_ARGS
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    )
    AddExternalProjFilesToOverlay(
        event-apps
        ${CMAKE_CURRENT_BINARY_DIR}/emits_event_apps
        overlay
        "usr/bin"
        FILES
        emits_event_emit
    )

    # Complile CrossVM Consume Event Apps
    ExternalProject_Add(
        consume-event-apps
        URL
        file:///${CAMKES_VM_LINUX_DIR}/camkes-linux-artifacts/camkes-linux-apps/camkes-connector-apps/pkgs/consumes_event
        BINARY_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/consume_event_apps
        INSTALL_COMMAND
        ""
        BUILD_ALWAYS
        ON
        EXCLUDE_FROM_ALL
        CMAKE_ARGS
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    )
    AddExternalProjFilesToOverlay(
        consume-event-apps
        ${CMAKE_CURRENT_BINARY_DIR}/consume_event_apps
        overlay
        "usr/bin"
        FILES
        consumes_event_wait
    )

    # Add script to initialise dataport module
    AddFileToOverlayDir(
        "S90crossvm_module_init"
        ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/init_scripts/cross_vm_module_init
        "etc/init.d"
        overlay
    )
else()
    # User pre-configured rootfs file with crossvm modules and apps installed
    set(rootfs_file "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/rootfs_crossvm.cpio.gz")
endif()

#-----------------------------

# Add script to test dataport module
AddFileToOverlayDir(
    "S91crossvm_test"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/init_scripts/cross_vm_test
    "etc/init.d"
    overlay
)

AddFileToOverlayDir(
    "interfaces"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/network_interfaces/interfaces
    "etc/network"
    overlay
)


CAmkESAddImportPath(${KernelARMPlatform})

# Define our VM Component with out cross vm dataports glue code
DeclareCAmkESComponent(VM SOURCES src/cross_vm_connections.c)

# Define our demo component that shares a dataport with the VM
add_subdirectory(components/Router)

#===========================================
# BEGIN DECLARATION OF CAKE COMPONENTS
#===========================================

find_program (arm-hf-gcc arm-linux-gnueabihf-gcc)
set(CMAKE_C_COMPILER ${arm-hf-gcc})

add_subdirectory(components/CakeServer/cakeml_am/system/crypto)

DeclareCAmkESComponent(CakeServer
    SOURCES
        components/CakeServer/destination.c 
        components/CakeServer/sb_destination_t_impl.c
        sampling_ports/sp_int8_t.c
        ${CMAKE_CURRENT_BINARY_DIR}/cakeServer.S
    INCLUDES
        components/destination_t_impl/includes
        /usr/lib/gcc-cross/arm-linux-gnueabihf/8/include
        sampling_ports
        includes
    LIBS
        "crypto"
)


#===========================================
# END DECLARATION OF CAKE COMPONENTS
#===========================================

find_program(CAKE NAMES cake32 cake PATHS "~/cake-x64-x32/" "/cake-x64-x32/" DOC "32-bit targeting CakeML compiler")
if("${CAKE}" STREQUAL "CAKE-NOTFOUND")
    message(FATAL_ERROR "Could not find a 32-bit targeting CakeML compiler. Please ensure cake32 is on the system path.")
endif()
ExternalProject_Add(
    useram
    SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/cakeml-am
    BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/cakeml-am
    INSTALL_COMMAND
    ""
    BUILD_ALWAYS
    ON
    EXCLUDE_FROM_ALL
    CMAKE_ARGS
        -DCMAKE_C_COMPILER=${odroid_cross_compiler}
        -DCMAKE_C_FLAGS=${BASE_C_FLAGS}
        -DAM_QUEUE_LIB=${CMAKE_CURRENT_SOURCE_DIR}/../am_queue
        -DCAKE=${CAKE}
)

MyAddExternalProjFilesToOverlay(
    useram
    ${CMAKE_CURRENT_BINARY_DIR}/cakeml-am
    overlay
    "usr/bin"
    FILES
    groundstation
)

# Construct new rootfs
AddOverlayDirToRootfs(
    overlay
    ${rootfs_file}
    "buildroot"
    "rootfs_install"
    output_overlayed_rootfs_location
    rootfs_target
    GZIP
)

# =============== new =====================
# Updated dtb based on generated initrd
UpdateDtbFromInitrd(
    ${dtb_file}
    ${output_overlayed_rootfs_location}
    ${rootfs_address}
    dtb_gen_target
    output_dtb_location
    DEPENDS
    rootfs_target
)

AddToFileServer("linux-dtb" "${output_dtb_location}" DEPENDS dtb_gen_target)
AddToFileServer("linux" "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/linux")
# ============= end new ===================
AddToFileServer("linux-initrd" ${output_overlayed_rootfs_location} DEPENDS rootfs_target)


AddCamkesCPPFlag(
    cpp_flags
    CONFIG_VARS
    VmEmmc2NoDMA
    VmVUSB
    VmVchan
    Tk1DeviceFwd
    Tk1Insecure
    VmVirtioNetVirtqueue
)

DefineCAmkESVMFileServer()

#=====================
# mike end
#=====================


# Declare root server
DeclareCAmkESRootserver(
    vm_attest.camkes
    CPP_FLAGS
    ${cpp_flags}
    CPP_INCLUDES
    ${CMAKE_CURRENT_LIST_DIR}/../../components/VM
)

