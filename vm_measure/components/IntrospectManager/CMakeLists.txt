
cmake_minimum_required(VERSION 3.8.2)

project(IntrospectManager C)

DeclareCAmkESComponent( IntrospectManager
    SOURCES
        introspect_manager.c
        #sampling_ports/sp_int8_t.c
    INCLUDES
        includes
        #sampling_ports
    LIBS
        sel4_autoconf
)

