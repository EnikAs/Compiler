#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED

#include <elf.h>
#include "tree.h"

const int VAR_MAX_CUNT = 100;
const int MAX_CODE_LEN = 2048;
const int MAX_FUNC_CUNT = 100;
const int SIZE_OF_REG = 8;

enum var_def
{
    UNDEFINED = 0,
    DEFINED = 1
};

enum while_flag
{
    NOT_IN_WHILE = 0,
    IN_WHILE = 1,
};

enum std_funcs
{
    SCAN_HASH = 2031059512,
    PRINT_HASH = -1817107093,
    SQRT_HASH = 786779368,

    STD_FUNC_DETECTED,
    STD_FUNC_NOT_DETECTED
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
};

struct var_list
{
    int var_hash = 0;
    int var_value = 0;
    int def_flag = UNDEFINED;
};

struct var_lists
{
    var_list* var = NULL;
    int free = 1;
    int while_flag = NOT_IN_WHILE;
};

int     ELFCtor                 (ELFfile* fdata);

int     GenerateELFFile         (Node* node);

int     VisitWriteCommands      (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

int     FindVariable            (var_lists* vr_list, int hash);

int     FindFunction            (JMPtable* jtable, int hash);

void    FixPhdr                 (ELFfile* fdata);

int     murmurHash              (char* key, unsigned int len);

#endif

/*

// zvOnit = return
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