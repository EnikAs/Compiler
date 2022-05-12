#include "frontend.h"
#include "backend.h"

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

    GenerateAsmCode(tree->peak);

    system("C:/VSCprogs/Processor/CPU/cpu.exe");

    

    free(tree->peak);
    free(tree);
    free(tk_array->node);
    free(tk_array);
    free(buf->data);
    free(buf);

    printf("END !");
    
    return 0;
}