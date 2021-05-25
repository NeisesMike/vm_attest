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
uint8_t onehundredmb[1024*1024*100];


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
/*
   void cake_send()
   {
   int8_t* packagePtr = malloc(sizeof(int8_t)*N);
//int8_t* packagePtr = (int8_t*)fib_buf;
printf("sending to cake: ");
for (int i = 0; i < N; i++) {
printf("%d ", ((int8_t*)fib_buf)[i*4]);
packagePtr[i] = ((int8_t*)fib_buf)[i*4];
}
printf("\n");

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
*/

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

bool matchEOFAdjacentString(int startIndex)
{
    uint8_t magicByteArray[10];
    char magicString[10] = "debug_line";
    for(int i=0; i<10; i++)
    {
        magicByteArray[i] = (uint8_t)magicString[i];
    }
    for(int i=0; i<10; i++)
    {
        if(onehundredmb[startIndex+i] != magicByteArray[i])
        {
            return false;
        }
    }
    return true;
}

int lookForwardForEOF(int startIndex)
{
    for(int i=startIndex; i<1024*1000*100; i++)
    {
        if(onehundredmb[i] == (uint8_t)'d')
        {
            if(matchEOFAdjacentString(i))
            {
                printf("Found Approximate End of ELF at %d\n", i);
                //printf("Size of ELF portion is %d\n", i-startIndex);
                return i;
            }
        }
    }
    return -1;
}

bool matchELFHeader(int startIndex)
{
    // build the magic byte string
    uint8_t byte0 = (uint8_t)0x7f;
    uint8_t byte1 = (uint8_t)0x45;
    uint8_t byte2 = (uint8_t)0x4c;
    uint8_t byte3 = (uint8_t)0x46;
    uint8_t byte4 = (uint8_t)0x02;
    uint8_t onebyte = (uint8_t)0x01;
    uint8_t nullbyte = (uint8_t)0x00;

    uint8_t magicBytes[16];
    magicBytes[0] = byte0;
    magicBytes[1] = byte1;
    magicBytes[2] = byte2;
    magicBytes[3] = byte3;
    magicBytes[4] = byte4;
    magicBytes[5] = onebyte;
    magicBytes[6] = onebyte;
    for(int i=7; i<16; i++)
    {
        magicBytes[i] = nullbyte;
    }

    // look for the magic byte string
    for(int i=0; i<16; i++)
    {
        if(onehundredmb[startIndex+i] != magicBytes[i])
        {
            return false;
        }
    }
    return true;
}

int lookBackForELFMagicNumber(int startIndex)
{
    uint8_t firstByte = (uint8_t)0x7f;
    for(int i=0; i<startIndex; i++)
    {
        if(onehundredmb[startIndex-i] == firstByte)
        {
            if(matchELFHeader(startIndex-i))
            {
                printf("Found start of ELF at %d\n", startIndex-i);
                //printf("Size of ELF portion is %d\n", i);
                return startIndex-i;
            }
        }
    }
    return -1;
}

void checkForELF(int startIndex)
{
    uint8_t magicByteArray[53];
    char magicString[53] = "/build-q/apps/aadl-eventdata-direct-new/vm/connection";
    for(int i=0; i<53; i++)
    {
        magicByteArray[i] = (uint8_t)magicString[i];
    }
    for(int i=0; i<53; i++)
    {
        if(onehundredmb[startIndex+i] != magicByteArray[i])
        {
            return;
        }
    }
    printf("\n\nFound the magic string at %d\n", startIndex);
    int sof = lookBackForELFMagicNumber(startIndex);
    int eof = lookForwardForEOF(sof);

    if(sof < startIndex && startIndex < eof)
    {
        printf("ELF start: %d\nELF end: %d\nELF size: %d\n", sof, eof, eof-sof);
    }
    else
    {
        printf("That magic string didn't fall within the ELF boundaries...\n");
    }
}

typedef struct {
    uint8_t startIndex;
    uint8_t ident[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} ELF64Header;

void printELF64Header(ELF64Header* inputHeader)
{
    printf("ELF64 Header Data\n");
    printf("Type: %d\n", inputHeader->type);
    printf("Machine: %d\n", inputHeader->machine);
    printf("Version: %d\n", inputHeader->version);
    printf("Entry Address: %08x\n", inputHeader->entry);
    printf("Program Header Table Offset: %d\n", inputHeader->phoff);
    printf("Section Header Table Offset: %d\n", inputHeader->shoff);
    printf("Flags: %04x\n", inputHeader->flags);
    printf("ELF header size: %d\n", inputHeader->ehsize);
    printf("Program header table entry size: %d\n", inputHeader->phentsize);
    printf("Program header table num entries: %d\n", inputHeader->phnum);
    printf("Section header size: %d\n", inputHeader->shentsize);
    printf("Section header num entries: %d\n", inputHeader->shnum);
    printf("Section name string table index: %d\n\n", inputHeader->shstrndx);
}

// return true if ELF scraped
bool tryReadELF(int startIndex, ELF64Header* thisHeader)
{
    if(!matchELFHeader(startIndex))
    {
        return false;
    }

    uint32_t readHalf(int start)
    {
        uint32_t result = (uint32_t)onehundredmb[start+1] << 8
            | onehundredmb[start];
        return result;
    }

    uint32_t readWord(int start)
    {
        uint32_t result = (uint32_t)onehundredmb[start+3] << 24
            | onehundredmb[start+2] << 16
            | onehundredmb[start+1] << 8
            | onehundredmb[start];
        return result;
    }

    uint64_t readAddr(int start)
    {
        uint64_t result = (uint64_t)onehundredmb[start+8] << 56
            | onehundredmb[start+7] << 48
            | onehundredmb[start+6] << 40
            | onehundredmb[start+5] << 32
            | onehundredmb[start+4] << 24
            | onehundredmb[start+3] << 16
            | onehundredmb[start+1] << 8
            | onehundredmb[start];
        return result;
    }

    for(int i=0; i<16; i++)
    {
        thisHeader->ident[i] = onehundredmb[startIndex+i];

    }

    uint16_t eType = readHalf(startIndex+16);
    if(5 < eType && eType < 0xfe00)
    {
        return false;
    }
    thisHeader->type = eType;

    thisHeader->startIndex = startIndex; 
    
    thisHeader->machine = readHalf(startIndex+18);
    thisHeader->version = readWord(startIndex+20);
    thisHeader->entry = readAddr(startIndex+24);
    thisHeader->phoff = readWord(startIndex+32);
    thisHeader->shoff = readWord(startIndex+40);
    thisHeader->flags = readWord(startIndex+48);
    thisHeader->ehsize = readHalf(startIndex+52);
    thisHeader->phentsize = readHalf(startIndex+54);
    thisHeader->phnum = readHalf(startIndex+56);
    thisHeader->shentsize = readHalf(startIndex+58);
    thisHeader->shnum = readHalf(startIndex+60);
    thisHeader->shstrndx = readHalf(startIndex+62);

    return true;
}

typedef struct {
    uint8_t numBytes;
    uint8_t data[64];
} HeaderEntry;

typedef struct {
    uint8_t numEntries;
    HeaderEntry* list[100];
} ProgramHeaderList;

typedef struct {
    uint8_t numEntries;
    HeaderEntry* list[100];
} SectionHeaderList;

ProgramHeaderList* getProgramSections(ELF64Header* thisHeader)
{
    ProgramHeaderList* programHeaderList = malloc(sizeof(ProgramHeaderList));
    programHeaderList->numEntries = thisHeader->phnum;

    uint8_t* memoryHead = (uint8_t*)onehundredmb;
    memoryHead += thisHeader->startIndex;
    memoryHead += thisHeader->phoff;

    for(int i=0; i<programHeaderList->numEntries; i++)
    {
        HeaderEntry* thisEntry = malloc(sizeof(HeaderEntry));
        thisEntry->numBytes = thisHeader->phentsize;

        uint8_t* entryHead = (uint8_t*)thisEntry->data;
        for(int j=0; j<thisEntry->numBytes; j++)
        {
            entryHead[j] = memoryHead[(thisEntry->numBytes)*i + j];
        }
        programHeaderList->list[i] = thisEntry;
        //printHeaderEntry(thisEntry);
    }
    return programHeaderList;
}

SectionHeaderList getSectionHeaders(ELF64Header* thisHeader)
{
    uint8_t* head = (uint8_t*)onehundredmb;
    head += thisHeader->startIndex;
    head += thisHeader->shoff;
}

void printHeaderEntry(HeaderEntry* entry)
{
    for(int i=0; i<entry->numBytes; i++)
    {
        printf("%02x",entry->data[i]);
    }
    printf("\n");
}

void printProgramHeaders(ProgramHeaderList* progHList)
{
    for(int i=0; i<progHList->numEntries; i++)
    {
        printHeaderEntry(progHList->list[i]);
    }
    printf("\n");
}

int run(void)
{
    printf("\n\nbing!\n\n");
    while (1) {
        ready_wait();

        memcpy(onehundredmb, ((char *)memdev), sizeof(uint8_t) * 1024 * 1000 * 100);

        const int headersUpperBound = 100;
        ELF64Header* ELFList[headersUpperBound];

        for(int i=0; i<headersUpperBound; i++)
        {
            ELFList[i] = malloc(sizeof(ELF64Header));
        }

        //initELFList(scrapedELFs, headersUpperBound);

        int numELFs = 0;
        for (int j = 0; j < 1024*1000*100; j++)
        {
            if(onehundredmb[j] == (uint8_t)0x7f)
            {
                if(tryReadELF(j, ELFList[numELFs]))
                {
                    numELFs++;
                }
            }
        }
        printf("\n\nWe scraped %d ELFs! ", numELFs);
        /*
        printf("Here is the first one:\n\n");
        printELF64Header(ELFList[0]);
        */

        /*
        int maxPHSize = 0;
        int maxSSize = 0;
        for(int i=0; i<numELFs; i++)
        {
            if(ELFList[i]->phentsize > maxPHSize)
            {
                maxPHSize = ELFList[i]->phentsize;
            }
            if(ELFList[i]->shentsize > maxSSize)
            {
                maxSSize = ELFList[i]->shentsize;
            }
        }
        printf("Max Prog entry size: %d\nMax Section entry size: %d\n", maxPHSize, maxSSize);
        */

        /*
        ProgramHeaderList progHeaders = getProgramSections(ELFList[0]);
        printProgramHeaders(progHeaders);
        */

    
        ProgramHeaderList* allProgHeaders[numELFs];
        for(int i=0; i<numELFs; i++)
        {
            printf("Program Header Entries for Scraped ELF: %d\n", i);
            allProgHeaders[i] = getProgramSections(ELFList[i]);
            printProgramHeaders(allProgHeaders[i]);
        }




        //======================
        // clean up our garbage
        //======================
        for(int i=0; i<numELFs; i++)
        {
            ProgramHeaderList* thisHList = allProgHeaders[i];
            for(int j=0; j<thisHList->numEntries; j++)
            {
                HeaderEntry* thisEntry = thisHList->list[j];
                free(thisEntry);
            }
            free(thisHList);
        }
        for(int i=0; i<headersUpperBound; i++)
        {
            if(ELFList[i])
            {
                free(ELFList[i]);
                continue;
            }
            break;
        }

        done_emit_underlying();
        done_emit();
    }

    return 0;
}
