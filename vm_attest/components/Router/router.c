/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#include <string.h>
#include <camkes.h>
#include <stdio.h>
#include "sb_source_t_impl.h"


/*
// Read from vm
int8_t* my_dataport_read()
{
    char str[4096];
    sprintf(str, "%s\n", dest);
    int8_t* ret = (int8_t*)str;
    return( ret );
}

// Write to vm
void my_dataport_write( char* string )
{
    memset(dest, '\0', 4096);
    strcpy(dest, string);
    return;
}

// Write to cakeml
void cake_send( int8_t* package )
{
    MUTEXOP(sb_dispatch_sem_wait())
    memset(sb_write_port, '\0', 4096);
    strcpy(sb_write_port, (char*)package);
    MUTEXOP(sb_dispatch_sem_post())
    sb_dest_periodic_dispatch_notification_emit();
    return;
}

// Read from cakeml
int8_t* cake_recv()
{
    MUTEXOP(sb_dispatch_sem_wait())
    char str[4096];
    sprintf(str, "%s\n", sb_write_port);
    int8_t* ret = (int8_t*)str;
    MUTEXOP(sb_dispatch_sem_post())
    return( ret );
}
*/

int run(void)
{
    //my_dataport_write( "This is a test from a camkes component.\n" );
    printf("ROUTER PRINT!");

    /*
    ** Wait for an event from the vm
    ** Read what was sent
    ** Send back a success response
    ** Send the package to cakeml
    ** Get response from cakeml
    ** Send the cakeml response to the vm
    */
    /*
    while (1) {
        ready_wait();
        int8_t* package = my_dataport_read();
        printf("Got an event and: %s\n", package);
        cake_send( package );
        done_emit_underlying();
        int8_t* cakePackage = cake_recv();
        my_dataport_write( "Greetings!" );
        printf("Got back from Cake: %s\n", (char*)cakePackage);
        my_dataport_write( (char*)cakePackage );
    }
    */
    
    return 0;
}

