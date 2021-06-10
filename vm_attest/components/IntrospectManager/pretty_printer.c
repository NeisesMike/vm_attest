
#include <string.h>
#include <camkes.h>
#include <stdio.h>

#include<inttypes.h>

#include "type_defs.c"
#include "scrape_manager.c"

void printELF64Header(ELF64Header* inputHeader)
{
    printf("ELF64 Header Data\n");
    printf("startIndex: %d\n", inputHeader->startIndex);
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

void printHeaderEntry(HeaderEntry* entry)
{
    for(int i=0; i<entry->numBytes; i++)
    {
        printf("%02x",entry->data[i]);
    }
    printf("\n");
}

void printSectionContents(SectionHeader64* thisHeader, int fileStartIndex)
{
    int payloadOffset = fileStartIndex + thisHeader->offset;
    int payloadSize = thisHeader->size;

    printf("Section Contents:\n");
    for(int i=0; i<payloadSize; i++)
    {
        printf("%c", Linux_Mem[payloadOffset + i]);
    }
}

// return whether we were able to print as expected
bool printSectionHeader(SectionHeader64* header, ELF64Header* thisELF)
{
    SectionHeader64* strTabHeader = getSectionHeaderTable(thisELF)->list[thisELF->shstrndx];

    //get name as a string
    char* sectionName = (char*)Linux_Mem;
    int offset = thisELF->startIndex + strTabHeader->offset + header->name;
    if(thisELF->startIndex < offset && offset < LINUX_MEMORY_SIZE)
    {
        if(header->name == 0)
        {
            sectionName = "(NULL)";
        }
        else
        {
            sectionName += offset;
        }
    }
    else
    {
        sectionName = "NOT_FOUND";
        return false;
    }

    //printf("Section header contents:\n");
    printf("Section Name:      %s\n", sectionName);
    /*
    printf("Type:      %04x\n", header->type);
    printf("Flags:     %08x\n", header->flags);
    printf("Addr:      %08x\n", header->addr);
    printf("Offset:    %08x\n", header->offset);
    printf("Size:      %08x\n", header->size);
    printf("Link:      %04x\n", header->link);
    printf("Info:      %04x\n", header->info);
    printf("Addralign: %08x\n", header->addralign);
    printf("Entsize:   %08x\n", header->entsize);
    printf("\n");
    */

    if(strcmp(sectionName, ".rodata") == 0)
    {
        printf("We found some read-only data! Here it is:\n");
        printf("\n\n===========================================================\n");
        printf("===========================================================\n\n");
        printSectionContents(header, thisELF->startIndex);
        printf("\n\n===========================================================\n");
        printf("===========================================================\n\n");
    }
    return true;
}

void printProgramHeaderRaw(ProgramHeader64* header, ELF64Header* thisELF)
{
    printf("Program header contents:\n");
    printf("Type:      %04x\n", header->type);
    printf("Flags:     %08x\n", header->flags);
    printf("Offset:    %08x\n", header->offset);
    printf("Vaddr:      %08x\n", header->vaddr);
    printf("Paddr:      %08x\n", header->paddr);
    printf("Filesz:      %08x\n", header->filesz);
    printf("Memsz:      %08x\n", header->memsz);
    printf("Align:      %08x\n", header->align);
    printf("\n");
}

void printProgramHeaders(ELF64Header* thisELF)
{
    ProgramHeaderTable* progHTable = getProgramHeaderTable(thisELF);
    int numEntries = progHTable->numEntries;
    if(numEntries == 0)
    {
        printf("This Program Header Table is empty!\n\n");
        return;
    }
    for(int i=0; i<numEntries; i++)
    {
        printProgramHeaderRaw(progHTable->list[i], thisELF);
    }
    printf("\n");
}

// return whether this ELF was busted
bool printAllSectionHeaders(ELF64Header* thisELF)
{
    SectionHeaderTable* sectHTable = getSectionHeaderTable(thisELF);

    int numEntries = sectHTable->numEntries;
    if(numEntries == 0)
    {
        printf("This Section Header Table is empty!\n\n");
        return true;
    }

    printf("This Section Header Table has %d entries\n", numEntries);
    for(int i=0; i<numEntries; i++)
    {
        if(!printSectionHeader(sectHTable->list[i], thisELF))
        {
            printf("But the entries were busted...\n");
            return false;
        }
    }
    
    printf("\n");
    return true;
}

void printSectionHeaderRaw(SectionHeader64* header, ELF64Header* thisELF)
{
    printf("Section header contents:\n");
    printf("Section Name: %08x\n", header->name);
    printf("Type:      %08x\n", header->type);
    printf("Flags:     %016x\n", header->flags);
    printf("Addr:      %016x\n", header->addr);
    printf("Offset:    %016x\n", header->offset);
    printf("Size:      %016x\n", header->size);
    printf("Link:      %08x\n", header->link);
    printf("Info:      %08x\n", header->info);
    printf("Addralign: %016x\n", header->addralign);
    printf("Entsize:   %016x\n", header->entsize);
    printf("\n");
}

void printAllSectionHeadersRaw(ELF64Header* thisELF)
{
    SectionHeaderTable* sectHTable = getSectionHeaderTable(thisELF);
    int stringTableIndex = thisELF->shstrndx;

    int numEntries = sectHTable->numEntries;
    if(numEntries == 0)
    {
        printf("This Section Header Table is empty!\n\n");
        return;
    }

    printf("This Section Header Table has %d entries\n", numEntries);
    for(int i=0; i<numEntries; i++)
    {
        printSectionHeaderRaw(sectHTable->list[i], thisELF);
    }
    
    printf("\n");
}


