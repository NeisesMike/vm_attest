
#include <string.h>
#include <camkes.h>
#include <stdio.h>

//#define LINUX_MEMORY_SIZE 1024*1024*100
// This constant is known via inspection
#define LINUX_MEMORY_SIZE 104943616
uint8_t Linux_Mem[LINUX_MEMORY_SIZE];

typedef struct {
    int startIndex;
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
    uint32_t name;
    uint32_t type;
    uint64_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t addralign;
    uint64_t entsize;
} SectionHeader64;

typedef struct {
    uint8_t numEntries;
    SectionHeader64* list[100];
} SectionHeaderTable;


typedef struct {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
} ProgramHeader64;

typedef struct {
    uint8_t numEntries;
    ProgramHeader64* list[100];
} ProgramHeaderTable;

typedef struct {
    uint32_t name;
    unsigned char info;
    unsigned char other;
    uint16_t shndx;
    uint64_t value;
    uint64_t size;
} ELF64Sym;

typedef struct {
    uint8_t numEntries;
    ELF64Sym* list[500];
} SymTab;

