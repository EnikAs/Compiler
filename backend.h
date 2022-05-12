#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED
#include "tree.h"

const int VAR_MAX_CUNT = 100;


enum move_dx_key
{
    MINUS = 1488,
    PLUS
};

struct var_list
{
    int var_hash = 0;
    int var_dx_shift = 0;
};

struct var_lists
{
    var_list* var = NULL;
    int free = 0;
};

int     GenerateAsmCode         (Node* node);

int     VisitPrintCommands      (Node* node, var_lists* vr_lists, FILE* com_file);

void    Move_dx                 (int shift, FILE* com_file, int key);

int     FindVariable            (var_lists* vr_list, int hash);

int     murmurHash              (char* key, unsigned int len);

#endif

/*
    dx - for variable in RAM
    ex - for return ip
    ax - used for return value
// zvOnit = retuen
// krasivEe = while
// tortbl = if
// lattE = else 
// word_stress = {
// for_weak_people = }  
//  
//  
//  
//  
*/