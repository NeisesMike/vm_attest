
#============================================================================
# begin platam stuff
#============================================================================

find_program (aarch-gcc aarch64-linux-gnu-gcc)
set(CMAKE_C_COMPILER ${aarch-gcc})

add_subdirectory(cakeml-am/system/crypto)

DeclareCAmkESComponent(CakeServer
    SOURCES
        cakeml-am/system/dataports/dataport_ffi.c
        #destination.c 
        sb_destination_t_impl.c
        #sampling_ports/sp_int8_t.c
        #${CMAKE_CURRENT_BINARY_DIR}/cakeServer.S
        cakeServer.S
    INCLUDES
        components/destination_t_impl/includes
        sampling_ports
        includes
    LIBS
        "crypto;camkescakeml"
)

#============================================================================
# end platam stuff
# begin useram stuff
#============================================================================
