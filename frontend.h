#ifndef FRONTEND_H_INCLUDED
#define FRONTEND_H_INCLUDED

//#include "C:\TXLib\TX\TXLib.h"

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cassert>
#include <sys\stat.h>

#include "tree.h"

const int KOSTYL = 2;
const int EXTRA = 1;
const int MAX_TKN_CUNT = 350;

enum strncmp
{
    EQUAL = 0
};


struct tkn_arr
{
    Node* node = NULL;
    int n_cunt = 0;
    int current_node  = 0;
};

struct buffer
{
    char* data = NULL;
    int size = 0;
    int pos = 0;
};

tkn_arr*    GetAllTokens    (FILE* inputfile, buffer* buf);

void        GetTokens       (buffer* buf, tkn_arr* array);

void        PrintAllTokens  (tkn_arr* array);

Node*       GetG            (tkn_arr* tk_array);

Node*       GetStmts        (tkn_arr* tk_array);

Node*       GetStmt         (tkn_arr* tk_array);

Node*       GetE            (tkn_arr* tk_array);

Node*       GetT            (tkn_arr* tk_array);

Node*       GetP            (tkn_arr* tk_array);

Node*       GetN            (tkn_arr* tk_array);

Node*       GetV            (tkn_arr* tk_array);

Node*       GetArgs         (tkn_arr* tk_array);

Node*       GetArgsCall     (tkn_arr* tk_array);

Node*       GetArg          (tkn_arr* tk_array);

void        Require         (char ch, tkn_arr* tk_array);

void        SyntaxERROR     (const char* s);

Node*       CreateNode      (int node_type);

void        CheckPtr        (void* ptr, const char* error);

#endif