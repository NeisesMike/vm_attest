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

#include "pretty_printer.c"

//#include "timer_interface.c"

#define RAM_BASE 0x40000000
#define N 10

//uint8_t Linux_Mem[1024*1024*100];

int run(void)
{
    printf("\n\nbing!\n\n");
    while (1) {
        ready_wait();



        memcpy(Linux_Mem, ((char *)memdev), sizeof(uint8_t) * 1024 * 1000 * 100);


        const int headersUpperBound = 100;
        ELF64Header* ELFList[headersUpperBound];

        for(int i=0; i<headersUpperBound; i++)
        {
            ELFList[i] = malloc(sizeof(ELF64Header));
        }

        int numELFs = 0;
        for (int j = 0; j < 1024*1000*100; j++)
        {
            if(Linux_Mem[j] == (uint8_t)0x7f)
            {
                if(tryReadELF(j, ELFList[numELFs]))
                {
                    numELFs++;
                }
            }
        }

        printf("\n\nWe found %d potential ELFs\n\n", numELFs);

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
    
        ProgramHeaderTable* programHeaderTables[numELFs];
        SectionHeaderTable* sectionHeaderTables[numELFs];
        for(int i=0; i<numELFs; i++)
        {
            ELF64Header* thisELF = ELFList[i];
            SectionHeaderTable* thisSectionHeaderTable = getSectionHeaderTable(thisELF);
            sectionHeaderTables[i] = thisSectionHeaderTable;


            /*
            uint8_t* sectionNameStringTable = thisSectionHeaderTable->list[thisELF->shstrndx];
            printf("Section Header Entries for Scraped ELF: %d\n", i);
            printAllSectionHeaders(sectionHeaderTables[i], sectionNameStringTable);
            */


            /*
            printf("Program Header Entries for Scraped ELF: %d\n", i);
            */
            programHeaderTables[i] = getProgramHeaderTable(thisELF);
        }


        int numGoodELFs = 0;
        for(int i=0; i<numELFs; i++)
        {
            ELF64Header* thisELF = ELFList[i];
            //SectionHeaderTable* thisSHT = sectionHeaderTables[i];

            printELF64Header(thisELF);
            printProgramHeaders(thisELF);
            if(printAllSectionHeaders(thisELF))
            {
                numGoodELFs++;
            }
            printf("================================================\n");
            printf("================================================\n");
            



            //SymTab* thisSymTab = getSymbolTable(thisELF);
            //printELF64Header(thisELF);
            //printAllSectionHeaders(thisSHT, thisELF->shstrndx, thisELF->startIndex);
        }

        printf("\n\nOut of %d potential ELFs, we found %d good ones.\n\n", numELFs, numGoodELFs);

        /*
        printELF64Header(ELFList[2]);
        printAllSectionHeadersRaw(ELFList[2]);
        */


        /*
        SectionHeader64* sectionNameStringTableHeader = SHT0->list[ELF0->shstrndx];
        printSectionContents(sectionNameStringTableHeader, ELF0->startIndex);
        */

        //======================
        // clean up our garbage
        //======================
        for(int i=0; i<numELFs; i++)
        {
            ProgramHeaderTable* thisProgramHeaderTable = programHeaderTables[i];
            for(int j=0; j<thisProgramHeaderTable->numEntries; j++)
            {
                ProgramHeader64* thisEntry = thisProgramHeaderTable->list[j];
                free(thisEntry);
            }
            free(thisProgramHeaderTable);

            SectionHeaderTable* thisSectionHeaderTable = sectionHeaderTables[i];
            for(int j=0; j<thisSectionHeaderTable->numEntries; j++)
            {
                SectionHeader64* thisEntry = thisSectionHeaderTable->list[j];
                free(thisEntry);
            }
            free(thisSectionHeaderTable);
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
