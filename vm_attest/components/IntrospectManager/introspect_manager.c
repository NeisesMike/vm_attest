/*
 * Copyright 2020, Data61
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

#define RAM_BASE 0x40000000
#define N 10

uint32_t fib_buf[N];


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
*/

// Write to cakeml
void cake_send()
{
    int8_t* packagePtr = malloc(sizeof(int8_t)*N);
    //int8_t* packagePtr = (int8_t*)fib_buf;
    for (int i = 0; i < N; i++) {
        printf("%d\n", ((int8_t*)fib_buf)[i*4]);
        packagePtr[i] = ((int8_t*)fib_buf)[i*4];
    }
    
    MUTEXOP(cake_dispatch_sem_wait())
    memset(cake_write_port, '\0', 4096);
    strcpy(cake_write_port, (char*)packagePtr);
    MUTEXOP(cake_dispatch_sem_post())
    cake_periodic_dispatch_notification_emit();

    return;
}
void cake_send_old( int8_t* package )
{
    MUTEXOP(cake_dispatch_sem_wait())
    memset(cake_write_port, '\0', 4096);
    strcpy(cake_write_port, (char*)package);
    MUTEXOP(cake_dispatch_sem_post())
    cake_periodic_dispatch_notification_emit();
    return;
}

// Read from cakeml
int8_t* cake_recv()
{
    MUTEXOP(cake_dispatch_sem_wait())
    char str[4096];
    sprintf(str, "%s\n", cake_write_port);
    int8_t* ret = (int8_t*)str;
    MUTEXOP(cake_dispatch_sem_post());
    return( ret );
}

/*
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
    
    return 0;
}
    */


int run(void)
{
    printf("\n\nbing!\n\n");
    while (1) {
        ready_wait();

        seL4_Word paddr = *(seL4_Word *)introspect_data;
        printf("paddr in component 0x%x\n", paddr);

        seL4_Word offset = paddr - RAM_BASE;

        printf("offset in component 0x%x\n", offset);

        memcpy(fib_buf, ((char *)memdev + offset), sizeof(uint32_t) * N);

        //print data from inside linux process
        for (int i = 0; i < 10; i++) {
            printf("camkes_fib[%d]@%p = %d, ", i, (fib_buf + i), fib_buf[i]);
        }

        printf("\n");

        //send data from inside linux over to cake
        cake_send();
        done_emit_underlying();
        int8_t* cakePackage = cake_recv();
        printf("Got back from Cake: %s\n", (char*)cakePackage);

        done_emit();
    }

    return 0;
}
