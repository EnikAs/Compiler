#include "frontend.h"
#include "backend.h"
#include "elfhdr.h"

int main()
{
    
    FILE* inputfile = fopen("input.txt", "r");

    buffer* buf = (buffer*) calloc(1, sizeof(buffer)); 
    CheckPtr(buf, "buffer callocation error!");
    
    tkn_arr* tk_array = GetAllTokens(inputfile, buf);

    fclose (inputfile);
    
    PrintAllTokens(tk_array);
    
    Tree* tree = (Tree*) calloc(1 , sizeof(Tree));
    
    tree->peak = (Node*) calloc(1, sizeof(Node));
    
    tree->peak = GetG(tk_array);
    
    TreeDump(tree->peak);

    GenerateELFFile(tree->peak);

    //system("C:/VSCprogs/Processor/CPU/cpu.exe");

    

    free(tree->peak);
    free(tree);
    free(tk_array->node);
    free(tk_array);
    free(buf->data);
    free(buf);

    printf("END !");

    //system("edb --run romalox.dick");

    
    /*
    FILE* code_file = fopen("romalox.dick", "w");
    
    unsigned char mas[200] = {};
    int ip = 0;
    Elf64_Ehdr ehdr = {};
    Elf64_Phdr phdr = {};

    ELFhdrCtor(&ehdr);
    PhdrCtor(&phdr);

    for (int i = 0 ; i < sizeof(Elf64_Ehdr) ; i++)
    {
        mas[ip] = ((unsigned char*)&ehdr)[i];
        ip++;
    }

    //int memsz = ip - sizeof(int64_t) 
	//	- sizeof(Elf64_Ehdr)- sizeof(Elf64_Phdr);

    int memsz = 44;

    phdr.p_memsz
	       	= memsz;

	phdr.p_filesz
	       	= memsz;

    for (int i = 0 ; i < sizeof(Elf64_Phdr) ; i++)
    {
        mas[ip] = ((unsigned char*)&phdr)[i];
        ip++;
    }
    unsigned char code[] = { 0xB8, 0x04, 0x00, 0x00, 0x00, 0xBB, 0x01, 0x00, 0x00, 0x00, 0xB9, 0x23, 0x01, 0x00, 0x00, 0xBA, 0x05, 0x00, 0x00, 0x00, 0xCD, 0x80, 0xB8, 0x01, 0x00, 0x00, 0x00, 0xBB, 0x00, 0x00, 0x00, 0x00, 0xCD, 0x80,0xB8, 0x04, 0x00, 0x00, 0x00, 0xBB, 0x01, 0x00, 0x00, 0x00 };

    for (int i = 0 ; i < sizeof(code) ; i++)
    {
        mas[ip] = code[i];
        ip++;
    }

    fwrite(mas, sizeof(char), sizeof(mas), code_file);

    fclose(code_file);

    system("chmod +x romalox.dick");
*/
    return 0;
}