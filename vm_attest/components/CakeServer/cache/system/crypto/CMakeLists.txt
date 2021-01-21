include(ExternalProject)

set(sys "CAmkES")

# Fetch hacl-star v0.2.1
# ExternalProject_Add(hacl-star
#     URL https://github.com/project-everest/hacl-star/releases/download/v0.2.1/hacl-star-v0.2.1.tar.bz2
#     SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/hacl-star
#     CONFIGURE_COMMAND ""
#     BUILD_COMMAND ""
#     INSTALL_COMMAND ""
# )
ExternalProject_Add(hacl-star
    GIT_REPOSITORY https://github.com/project-everest/hacl-star/
    # This was the most recent commit on master, at the time of integration.
    # The next release will likely support ARM cross-compilation (as this commit
    # does), and we should switch to that when it arrives.
    GIT_TAG 50db8e4147258a5dc8e18c940c1b045ce5558723
    GIT_PROGRESS True
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/hacl-star
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

set(hacl_release_dir "${CMAKE_CURRENT_BINARY_DIR}/hacl-star/dist")
set(hacl_path    "${hacl_release_dir}/gcc-compatible")
set(kremlin_path "${hacl_release_dir}/kremlin/include")
set(kremlib_path "${hacl_release_dir}/kremlin/kremlib/dist/minimal")

# file(GLOB hacl_sources ${hacl_path}/Hacl_Hash.c ${hacl_path}/Hacl_Ed25519.c ${hacl_path}/Hacl_Curve25519_51.c)
set(hacl_sources ${hacl_path}/Hacl_Hash.c ${hacl_path}/Hacl_Ed25519.c ${hacl_path}/Hacl_Curve25519_51.c)
set_source_files_properties(${hacl_sources} PROPERTIES GENERATED True)

# Crypto library
include(HaclConfig.cmake)
add_definitions(${comp_defns})
#set(armhf_include_path "/usr/arm-linux-gnueabihf/include")
# /usr/lib/gcc-cross/arm-linux-gnueabihf/8/include
set(mike_include_path /usr/include /usr/lib/gcc-cross/aarch64-linux-gnu/8/include /usr/aarch64-linux-gnu/include)

# add_library(crypto STATIC crypto_ffi.c sys/${sys}/crypto_ffi.c meas.c ${hacl_sources})
add_library(crypto STATIC crypto_ffi.c meas.c sys/${sys}/crypto_ffi.c ${hacl_sources})
include_directories(BEFORE ${hacl_path} ${kremlin_path} ${kremlib_path} /usr/include /usr/lib/gcc-cross/aarch64-linux-gnu/8/include /usr/aarch64-linux-gnu/include /usr/lib/gcc-cross/aarch64-linux-gnu/8/include-fixed)
target_link_libraries(crypto ${cryptoLinks} -static -static-libgcc)
add_dependencies(crypto hacl-star)


# Small standalone executable to generate keys for Ed25519
add_executable(keyGen EXCLUDE_FROM_ALL
    key_gen.c sys/${sys}/crypto_ffi.c
    ${hacl_path}/Hacl_Hash.c ${hacl_path}/Hacl_Ed25519.c ${hacl_path}/Hacl_Curve25519_51.c
)
add_dependencies(keyGen hacl-star)
