
#include <string.h>
#include <camkes.h>
#include <stdio.h>

uint32_t readHalf(int start)
{
    uint32_t result = (uint32_t)Linux_Mem[start+1] << 8
        | Linux_Mem[start];
    return result;
}

uint32_t readWord(int start)
{
    uint32_t result = (uint32_t)Linux_Mem[start+3] << 24
        | Linux_Mem[start+2] << 16
        | Linux_Mem[start+1] << 8
        | Linux_Mem[start];
    return result;
}

uint64_t readAddr(int start)
{
    uint64_t result = (uint64_t)Linux_Mem[start+8] << 56
        | Linux_Mem[start+7] << 48
        | Linux_Mem[start+6] << 40
        | Linux_Mem[start+5] << 32
        | Linux_Mem[start+4] << 24
        | Linux_Mem[start+3] << 16
        | Linux_Mem[start+1] << 8
        | Linux_Mem[start];
    return result;
}

SectionHeader64* getSectionHeader(int startIndex)
{
    SectionHeader64* thisSectionHeader = malloc(sizeof(SectionHeader64));

    thisSectionHeader->name = readWord(startIndex+0);
    thisSectionHeader->type = readWord(startIndex+4);
    thisSectionHeader->flags = readAddr(startIndex+8);
    thisSectionHeader->addr = readAddr(startIndex+16);
    thisSectionHeader->offset = readAddr(startIndex+24);
    thisSectionHeader->size = readAddr(startIndex+32);
    thisSectionHeader->link = readWord(startIndex+40);
    thisSectionHeader->info = readWord(startIndex+44);
    thisSectionHeader->addralign = readAddr(startIndex+48);
    thisSectionHeader->entsize = readAddr(startIndex+56);

    return thisSectionHeader;
}

ProgramHeader64* getProgramHeader(int startIndex)
{
    ProgramHeader64* thisProgramHeader = malloc(sizeof(ProgramHeader64));

    thisProgramHeader->type = readWord(startIndex+0);
    thisProgramHeader->flags = readWord(startIndex+4);
    thisProgramHeader->offset = readAddr(startIndex+8);
    thisProgramHeader->vaddr = readAddr(startIndex+16);
    thisProgramHeader->paddr = readAddr(startIndex+24);
    thisProgramHeader->filesz = readAddr(startIndex+32);
    thisProgramHeader->memsz = readAddr(startIndex+40);
    thisProgramHeader->align = readAddr(startIndex+48);

    return thisProgramHeader;
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
        if(Linux_Mem[startIndex+i] != magicBytes[i])
        {
            return false;
        }
    }
    return true;
}

// return true if ELF scraped
bool tryReadELF(int startIndex, ELF64Header* thisHeader)
{
    if(!matchELFHeader(startIndex))
    {
        return false;
    }

    for(int i=0; i<16; i++)
    {
        thisHeader->ident[i] = Linux_Mem[startIndex+i];

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

ProgramHeaderTable* getProgramHeaderTable(ELF64Header* thisHeader)
{
    ProgramHeaderTable* programHeaderList = malloc(sizeof(ProgramHeaderTable));
    programHeaderList->numEntries = thisHeader->phnum;

    uint8_t* memoryHead = Linux_Mem;
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

SectionHeaderTable* getSectionHeaderTable(ELF64Header* thisHeader)
{
    SectionHeaderTable* sectionHeaderList = malloc(sizeof(SectionHeaderTable));
    sectionHeaderList->numEntries = thisHeader->shnum;

    int SectionHeaderOffset = thisHeader->startIndex + thisHeader->shoff;

    for(int i=0; i<sectionHeaderList->numEntries; i++)
    {
        sectionHeaderList->list[i] = getSectionHeader(SectionHeaderOffset);
        SectionHeaderOffset += thisHeader->shentsize;
        /*
        HeaderEntry* thisEntry = malloc(sizeof(HeaderEntry));
        thisEntry->numBytes = thisHeader->shentsize;

        uint8_t* entryHead = (uint8_t*)thisEntry->data;
        for(int j=0; j<thisEntry->numBytes; j++)
        {
            entryHead[j] = memoryHead[(thisEntry->numBytes)*i + j];
        }
        sectionHeaderList->list[i] = thisEntry;
        */
    }
    return sectionHeaderList;
}

SymTab* getSymbolTable(ELF64Header* thisELFHeader)
{
    SectionHeaderTable* SHTable = getSectionHeaderTable(thisELFHeader);
    SectionHeader64* stringTab = SHTable->list[thisELFHeader->shstrndx];
    for(int i=0; i<SHTable->numEntries; i++)
    {
        SectionHeader64* thisSectHeader = SHTable->list[i];
        char* headerName = (char*)Linux_Mem;
        int offset = thisELFHeader->startIndex + stringTab->offset + thisSectHeader->name;
        if(0 <= offset && offset < LINUX_MEMORY_SIZE)
        {
            headerName += offset;
        }
        else
        {
            headerName = "";
        }

        if(strcmp(headerName, ".symtab") == 0)
        {
            printf("Found a symtab!\n");
        }


    }
    return NULL;
}
