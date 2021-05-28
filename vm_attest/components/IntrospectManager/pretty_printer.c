
#include <string.h>
#include <camkes.h>
#include <stdio.h>

#include<inttypes.h>

#include "type_defs.c"

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

void printHeaderEntry(HeaderEntry* entry)
{
    for(int i=0; i<entry->numBytes; i++)
    {
        printf("%02x",entry->data[i]);
    }
    printf("\n");
}

void printSectionHeader(SectionHeader64* header, SectionHeader64* sectionNameStringTableHeader, int fileStartIndex)
{
    printf("Section header contents:\n");

    //get name as a string
    char* sectionName = (char*)Linux_Mem;
    sectionName += (fileStartIndex + sectionNameStringTableHeader->offset + header->name);

    printf("Name:      %s\n", sectionName);
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

void printAllSectionHeaders(SectionHeaderTable* sectHList, int stringTableIndex, int fileStartIndex)
{
    int numEntries = sectHList->numEntries;
    if(numEntries == 0)
    {
        printf("This Section Header Table is empty!\n\n");
        return;
    }

    SectionHeader64* sectionNameStringTable = sectHList->list[stringTableIndex];

    printf("We'll print %d entries\n", numEntries);
    for(int i=0; i<numEntries; i++)
    {
        printSectionHeader(sectHList->list[i], sectionNameStringTable, fileStartIndex);
    }
    
    printf("\n");
}

void printSectionNameStringTable(SectionHeader64* stringTableHeader, int fileStartIndex)
{
    int strTabSectOffset = fileStartIndex + stringTableHeader->offset;
    int strTabSectSize = stringTableHeader->size;

    printf("String Table Contents:\n");
    for(int i=0; i<strTabSectSize; i++)
    {
        printf("%c", Linux_Mem[strTabSectOffset + i]);
    }
}

