#include "elfhdr.h"
#include <cstdio>

int ELFhdrCtor(Elf64_Ehdr* ehdr)
{
    if (ehdr == NULL)
    {
        printf("Error in ELFhdrCtor (bad ehdr ptr)!");
        return -1;
    }

    ehdr->e_ident[EI_MAG0]       = ELFMAG0;
    ehdr->e_ident[EI_MAG1]       = ELFMAG1;
    ehdr->e_ident[EI_MAG2]       = ELFMAG2;
    ehdr->e_ident[EI_MAG3]       = ELFMAG3;
    ehdr->e_ident[EI_CLASS]      = ELFCLASS64;
    ehdr->e_ident[EI_DATA]       = ELFDATA2LSB;
    ehdr->e_ident[EI_VERSION]    = EV_CURRENT;
    ehdr->e_ident[EI_OSABI]      = ELFOSABI_NONE;
    ehdr->e_ident[EI_ABIVERSION] = 0;
    ehdr->e_ident[EI_PAD]        = 0;

    ehdr->e_type	  = ET_DYN;
	ehdr->e_machine   = EM_X86_64;
	ehdr->e_version   = EV_CURRENT;
	ehdr->e_entry     = 0x8048080;
	ehdr->e_phoff     = 64;
	ehdr->e_shoff     = 0;
	ehdr->e_flags     = 0x0;
	ehdr->e_ehsize    = sizeof(Elf64_Ehdr);
	ehdr->e_phentsize = sizeof(Elf64_Phdr);
	ehdr->e_phnum     = 0x1;
	ehdr->e_shentsize = 0x0;
	ehdr->e_shnum     = 0x0;
	ehdr->e_shstrndx  = 0x0;

    return 0;
}

int PhdrCtor(Elf64_Phdr* phdr)
{
	if (!phdr)
    {
        printf("Error in PhdrCtor (bad phdr ptr)!");
		return -1;
    }

    phdr->p_type   = PT_LOAD;	/* Segment type */
    phdr->p_flags  = PF_X + PF_R;/* Segment flags */
    phdr->p_offset = 0x80;	/* Segment file offset */
    phdr->p_vaddr  = 0x8048080;	/* Segment virtual address */
    phdr->p_paddr  = 0x0;	/* Segment physical address */
    phdr->p_filesz = 0x7;	/* Segment size in file */
    phdr->p_memsz  = 0x7;	/* Segment size in memory */
    phdr->p_align  = 0x1000;	/* Segment alignment */

	return 0;
}