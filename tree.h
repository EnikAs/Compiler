#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <iostream>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>

typedef char* telem_t;

const int MAX_STR_SIZE = 1000;

#define PRINT_LINE printf("I'm at line %d at function %s\n", __LINE__, __func__);

const int ERRORR = 0;

enum kostyl
{
    RIGHT,
    LEFT
};

enum input
{
    INCORRECT_INPUT = 228,
    CORRECT_INPUT
};

enum memory_check
{
    CALLOC_ERROR = 322,
    MALLOC_ERROR,
    MEMORY_IS_OK
};

enum node_data_type
{   
    CONSTANT = 21,
    VARIABLE,
    OPERATOR,
    REL_OPERATOR,
    BRACKET,
    DOLLAR,
    IF,
    WHILE,
    ELSE,
    COMMA,
    SCAN,
    RETURN,
    PRINT
};

enum exept_node_data_type
{
    DESISION = 228,
    STATEMENT,
    DEFINE,
    FUNCTION,
    CALL,
    PARAMETER,
    PARAMETER_CALL,
};

enum operator_type
{
    MUL = '*',
    SUB = '-',
    ADD = '+',
    DIV = '/',
    DEG = '^',
    SIN = 's',
    EQL = '=',
    COS = 'c'
};

enum trash_skip_key
{
    LETTERS,
    ZERO_ONLY
};

union union_node_data
{
    double  dbl = 0;
    char    ch;
    char*   str;
};

struct Node
{
    Node* left = NULL;
    Node* right = NULL;
    union_node_data data = {};
    int data_type = 0;
    int data_lng = 0; // размер data в языке
};

struct Tree
{
    Node* peak = NULL;
};

int     VisitPrintFileIN         (const Node* node, FILE* treefile);

int     TreeDump                 (Node* node);

int     TreeSetNode              (Node* node, telem_t value, Node* leftptr, Node* rightptr);

Tree*   TreeReadFileIN           (FILE* treefile);

int     scanf_data_diffrent_type (const char* buffer, Node* node, int* buf_pos);

int     TreeReadNodeIN           (const char* buffer, Node* main_node, int* buf_pos);

int     FreeTheTree              (Tree* tree);

int     TreeNodesFree            (Node* node);

void    CreateTreeFile           (FILE* inputfile, FILE* outputfile);

int     kostyl_trig              (const char* buffer, Node* main_node, int* buf_pos, int key, int key_tr);

size_t  scanf_file_size          (FILE* input_file);

void    CheckPtr                 (void* ptr, const char* error);


#endif