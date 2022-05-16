#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED

#include <elf.h>
#include "tree.h"

const int VAR_MAX_CUNT = 100;
const int MAX_CODE_LEN = 500;
const int MAX_FUNC_CUNT = 100;
const int MAX_SKIP_CUNT = 500;

enum move_dx_key
{
    MINUS = 1488,
    PLUS
};

struct ELFfile
{
    unsigned char code[MAX_CODE_LEN] = {};
    int ip = 0;

    Elf64_Ehdr ehdr = {};
    Elf64_Phdr phdr = {};

    FILE* exe_file = {};
};

struct func_jmp_mas
{
    int func_hash = 0;
    int func_ip = 0;
};

struct JMPtable
{
    func_jmp_mas func_jmp[MAX_FUNC_CUNT] = {};
    int tmp_func = 0;
    int skip_jmp[MAX_SKIP_CUNT] = {};
    int tmp_skip = 0;   
};

struct var_list
{
    int var_hash = 0;
    int var_dx_shift = 0;
};

struct var_lists
{
    var_list* var = NULL;
    int free = 1;
};

int     ELFCtor                 (ELFfile* fdata);

int     GenerateELFFile         (Node* node);

int     VisitPrintCommands      (Node* node, var_lists* vr_lists, FILE* com_file, ELFfile* fdata, JMPtable* jtable);

int     FindVariable            (var_lists* vr_list, int hash);

int     FindFunction            (JMPtable* jtable, int hash);

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