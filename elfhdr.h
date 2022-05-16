#ifndef ELFHDR_H_INC
#define ELFHDR_H_INC

#include <elf.h>

int ELFhdrCtor(Elf64_Ehdr* ehdr);

int PhdrCtor(Elf64_Phdr* phdr);


#endif