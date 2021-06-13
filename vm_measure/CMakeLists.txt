#
# Copyright 2020, Data61
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

project(camkes-vm-introspect C)

include(${CAMKES_ARM_VM_HELPERS_PATH})
find_package(camkes-vm-linux REQUIRED)
include(${CAMKES_VM_LINUX_HELPERS_PATH})
include(${CAMKES_VM_LINUX_MODULE_HELPERS_PATH})
include(${CAMKES_VM_LINUX_SOURCE_HELPERS_PATH})
include(${CAMKES_VM_LINUX_MODULE_HELPERS_PATH})
include(ExternalProject)
include(external-project-helpers)

# Setup Linux Sources
GetDefaultLinuxMajor(linux_major)
GetDefaultLinuxMinor(linux_minor)
GetDefaultLinuxMd5(linux_md5)
# Download and Configure our Linux sources
DownloadLinux(${linux_major} ${linux_minor} ${linux_md5} vm_linux_extract_dir download_vm_linux)
set(linux_config "${CAMKES_VM_LINUX_DIR}/linux_configs/${linux_major}.${linux_minor}/32/config")
set(linux_symvers "${CAMKES_VM_LINUX_DIR}/linux_configs/${linux_major}.${linux_minor}/32/Module.symvers")
ConfigureLinux(${vm_linux_extract_dir} ${linux_config} ${linux_symvers} configure_vm_linux
    DEPENDS download_vm_linux
)
# Add the external poke module project
ExternalProject_Add(poke-module
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/poke-module
    BUILD_ALWAYS ON
    STAMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/poke-module-stamp
    EXCLUDE_FROM_ALL
    INSTALL_COMMAND ""
    DEPENDS download_vm_linux configure_vm_linux
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_TOOLCHAIN_FILE=${LINUX_64BIT_TOOLCHAIN}
        -DLINUX_KERNEL_DIR=${vm_linux_extract_dir}
        -DMODULE_HELPERS_FILE=${CAMKES_VM_LINUX_DIR}/linux-module-helpers.cmake
)
# Add our module binary to the overlay
AddExternalProjFilesToOverlay(
    poke-module
    ${CMAKE_CURRENT_BINARY_DIR}/poke-module
    overlay
    "lib/modules/4.14.87/kernel/drivers/vmm"
    FILES
    poke.ko
)
	
AddFileToOverlayDir(
    "introspect_mike_init"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/init_scripts/introspect_mike_init
    "etc/init.d"
    overlay
)




if("${KernelARMPlatform}" STREQUAL "exynos5422")
    set(cpp_flags "-DKERNELARMPLATFORM_EXYNOS5422")
    set(linux_repo "https://github.com/hardkernel/linux.git")
    set(linux_tag "4.14.87-153")
    set(linux_arch "arm")
    set(linux_cross_compile "arm-linux-gnueabi-")

    find_package(camkes-vm-linux REQUIRED)

    include(${CAMKES_VM_LINUX_HELPERS_PATH})

    set(cpp_flags "-DKERNELARMPLATFORM_EXYNOS5422")

    # Generate overlayed rootfs
    set(rootfs_file "${CAMKES_VM_IMAGES_DIR}/exynos5422/rootfs.cpio.gz")

elseif("${KernelARMPlatform}" STREQUAL "qemu-arm-virt")
    set(cpp_flags "-DKERNELARMPLATFORM_QEMU-ARM-VIRT")
    set(linux_repo "https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git")
    set(linux_tag "v4.9.189")
    set(linux_arch "arm64")
    set(linux_cross_compile "aarch64-linux-gnu-")

    find_package(camkes-vm-linux REQUIRED)

    include(${CAMKES_VM_LINUX_HELPERS_PATH})

    include(simulation)

    set(SIMULATION ON CACHE BOOL "Generate simulation script to run qemu with the proper arguments")

    if(SIMULATION)
        GenerateSimulateScript()
    endif()

endif()

#AddToFileServer("linux" "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/linux")
#set(rootfs_file "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/rootfs_crossvm.cpio.gz")
GetArchDefaultLinuxKernelFile("64" default_kernel_file)
GetArchDefaultLinuxRootfsFile("64" default_rootfs_file)

# Decompress Linux Kernel image and add to file server
DecompressLinuxKernel(extract_linux_kernel decompressed_kernel ${default_kernel_file})
AddToFileServer("linux" ${decompressed_kernel} DEPENDS extract_linux_kernel)

# Add rootfs images into file server
AddToFileServer("rootfs.cpio" ${default_rootfs_file})



ExternalProject_Add(
    introspect-app
    URL
    file:///${CAMKES_VM_LINUX_DIR}/camkes-linux-artifacts/camkes-linux-apps/camkes-introspect-app/pkgs/introspect
    BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/introspect-app
    INSTALL_COMMAND
    ""
    BUILD_ALWAYS
    ON
    EXCLUDE_FROM_ALL
    CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)

AddExternalProjFilesToOverlay(
    introspect-app
    ${CMAKE_CURRENT_BINARY_DIR}/introspect-app
    overlay
    "usr/bin"
    FILES
    introspect
)

# Add script to initialise dataport module
AddFileToOverlayDir(
    "S90crossvm_module_init"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/init_scripts/introspect_dataport_init
    "etc/init.d"
    overlay
)

AddFileToOverlayDir(
    "S91crossvm_test"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/init_scripts/introspect_vm_test
    "etc/init.d"
    overlay
)

# Construct new rootfs
#AddOverlayDirToRootfs(
#    overlay
#    ${rootfs_file}
#    "buildroot"
#    "rootfs_install"
#    output_overlayed_rootfs_location
#    rootfs_target
#    GZIP
#)
#AddToFileServer("linux-initrd" ${output_overlayed_rootfs_location} DEPENDS rootfs_target)

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

CAmkESAddImportPath(${KernelARMPlatform})

# Define our VM Component with out cross vm dataports glue code
DeclareCAmkESComponent(VM SOURCES src/cross_vm_connection.c src/init_dataport_ram.c)

#DeclareCAmkESComponent(introspect_memdev SOURCES components/introspect_component.c)

#add_subdirectory(components/Router)
#add_subdirectory(components/CakeServer)
add_subdirectory(components/IntrospectManager)

# Declare root server
DeclareCAmkESRootserver(
    vm_attest.camkes
    CPP_FLAGS
    ${cpp_flags}
    CPP_INCLUDES
    ${CAMKES_VM_DIR}/components/VM_Arm
)
GenerateCAmkESRootserver()