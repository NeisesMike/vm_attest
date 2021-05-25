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

#include "scrape_manager.c"

#define RAM_BASE 0x40000000
#define N 10

uint8_t onehundredmb[1024*1024*100];

int run(void)
{
    printf("\n\nbing!\n\n");
    while (1) {
        ready_wait();

        memcpy(onehundredmb, ((char *)memdev), sizeof(uint8_t) * 1024 * 1000 * 100);

        uint8_t* linuxMemory = (uint8_t*)onehundredmb;

        const int headersUpperBound = 100;
        ELF64Header* ELFList[headersUpperBound];

        for(int i=0; i<headersUpperBound; i++)
        {
            ELFList[i] = malloc(sizeof(ELF64Header));
        }

        int numELFs = 0;
        for (int j = 0; j < 1024*1000*100; j++)
        {
            if(onehundredmb[j] == (uint8_t)0x7f)
            {
                if(tryReadELF(linuxMemory, j, ELFList[numELFs]))
                {
                    numELFs++;
                }
            }
        }
        printf("\n\nWe scraped %d ELFs!\n\n", numELFs);
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
            printf("Program Header Entries for Scraped ELF: %d\n", i);
            programHeaderTables[i] = getProgramHeaderTable(linuxMemory, ELFList[i]);
            printProgramHeaders(programHeaderTables[i]);

            printf("Section Header Entries for Scraped ELF: %d\n", i);
            sectionHeaderTables[i] = getSectionHeaderTable(linuxMemory, ELFList[i]);
            printSectionHeaders(sectionHeaderTables[i]);
        }




        //======================
        // clean up our garbage
        //======================
        for(int i=0; i<numELFs; i++)
        {
            ProgramHeaderTable* thisProgramHeaderTable = programHeaderTables[i];
            for(int j=0; j<thisProgramHeaderTable->numEntries; j++)
            {
                HeaderEntry* thisEntry = thisProgramHeaderTable->list[j];
                free(thisEntry);
            }
            free(thisProgramHeaderTable);

            SectionHeaderTable* thisSectionHeaderTable = sectionHeaderTables[i];
            for(int j=0; j<thisSectionHeaderTable->numEntries; j++)
            {
                HeaderEntry* thisEntry = thisSectionHeaderTable->list[j];
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
