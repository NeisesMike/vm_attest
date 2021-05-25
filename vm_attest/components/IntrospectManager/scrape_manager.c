
#include <string.h>
#include <camkes.h>
#include <stdio.h>

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

typedef struct {
    uint8_t numBytes;
    uint8_t data[64];
} HeaderEntry;

typedef struct {
    uint8_t numEntries;
    HeaderEntry* list[100];
} ProgramHeaderTable;

typedef struct {
    uint8_t numEntries;
    HeaderEntry* list[100];
} SectionHeaderTable;


/*
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
        if(linuxMem[startIndex+i] != magicByteArray[i])
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
        if(linuxMem[i] == (uint8_t)'d')
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
        if(linuxMem[startIndex+i] != magicByteArray[i])
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

int lookBackForELFMagicNumber(int startIndex)
{
    uint8_t firstByte = (uint8_t)0x7f;
    for(int i=0; i<startIndex; i++)
    {
        if(linuxMem[startIndex-i] == firstByte)
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
*/

bool matchELFHeader(uint8_t* linuxMem, int startIndex)
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
        if(linuxMem[startIndex+i] != magicBytes[i])
        {
            return false;
        }
    }
    return true;
}

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
bool tryReadELF(uint8_t* linuxMem, int startIndex, ELF64Header* thisHeader)
{
    if(!matchELFHeader(linuxMem, startIndex))
    {
        return false;
    }

    uint32_t readHalf(int start)
    {
        uint32_t result = (uint32_t)linuxMem[start+1] << 8
            | linuxMem[start];
        return result;
    }

    uint32_t readWord(int start)
    {
        uint32_t result = (uint32_t)linuxMem[start+3] << 24
            | linuxMem[start+2] << 16
            | linuxMem[start+1] << 8
            | linuxMem[start];
        return result;
    }

    uint64_t readAddr(int start)
    {
        uint64_t result = (uint64_t)linuxMem[start+8] << 56
            | linuxMem[start+7] << 48
            | linuxMem[start+6] << 40
            | linuxMem[start+5] << 32
            | linuxMem[start+4] << 24
            | linuxMem[start+3] << 16
            | linuxMem[start+1] << 8
            | linuxMem[start];
        return result;
    }

    for(int i=0; i<16; i++)
    {
        thisHeader->ident[i] = linuxMem[startIndex+i];

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

ProgramHeaderTable* getProgramHeaderTable(uint8_t* linuxMem, ELF64Header* thisHeader)
{
    ProgramHeaderTable* programHeaderList = malloc(sizeof(ProgramHeaderTable));
    programHeaderList->numEntries = thisHeader->phnum;

    uint8_t* memoryHead = linuxMem;
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
    }
    return programHeaderList;
}

SectionHeaderTable* getSectionHeaderTable(uint8_t* linuxMem, ELF64Header* thisHeader)
{
    SectionHeaderTable* sectionHeaderList = malloc(sizeof(SectionHeaderTable));
    sectionHeaderList->numEntries = thisHeader->shnum;

    uint8_t* memoryHead = linuxMem;
    memoryHead += thisHeader->startIndex;
    memoryHead += thisHeader->shoff;

    for(int i=0; i<sectionHeaderList->numEntries; i++)
    {
        HeaderEntry* thisEntry = malloc(sizeof(HeaderEntry));
        thisEntry->numBytes = thisHeader->shentsize;

        uint8_t* entryHead = (uint8_t*)thisEntry->data;
        for(int j=0; j<thisEntry->numBytes; j++)
        {
            entryHead[j] = memoryHead[(thisEntry->numBytes)*i + j];
        }
        sectionHeaderList->list[i] = thisEntry;
    }
    return sectionHeaderList;
}

void printHeaderEntry(HeaderEntry* entry)
{
    for(int i=0; i<entry->numBytes; i++)
    {
        printf("%02x",entry->data[i]);
    }
    printf("\n");
}

void printProgramHeaders(ProgramHeaderTable* progHList)
{
    int numEntries = progHList->numEntries;
    if(numEntries == 0)
    {
        printf("This Program Header Table is empty!\n\n");
        return;
    }
    for(int i=0; i<numEntries; i++)
    {
        printHeaderEntry(progHList->list[i]);
    }
    printf("\n");
}

void printSectionHeaders(SectionHeaderTable* sectHList)
{
    int numEntries = sectHList->numEntries;
    if(numEntries == 0)
    {
        printf("This Section Header Table is empty!\n\n");
        return;
    }
    for(int i=0; i<numEntries; i++)
    {
        printHeaderEntry(sectHList->list[i]);
    }
    printf("\n");
}

