#include "frontend.h"

static void    space_skip  (buffer* buf);

static int     get_bracket (buffer* buf, tkn_arr* tk_array);

static int     get_op      (buffer* buf, tkn_arr* tk_array);

static int     get_num     (buffer* buf, tkn_arr* tk_array);

static int     get_word    (buffer* buf, tkn_arr* tk_array);

static int     isnumber    (char value);

static int     isoperator  (char value);

static int     isbracket   (char value);

static int     iscomma     (char value);


#define $VALUE_IS_OPERATOR (value == '-' || value == '+' || value == '*' || value == '/' || value == '^' || value == '=' || value == '<' || value == '>' || value == ';'|| value == '!')

#define $BUF_CUR_ELEM (buf->data[buf->pos])

#define $CUR_TKN_DATA_LNG (tk_array->node[tk_array->current_node].data_lng)
#define $CUR_TKN_DATA_STR (tk_array->node[tk_array->current_node].data.str)
#define $CUR_TKN_DATA_DBL (tk_array->node[tk_array->current_node].data.dbl)
#define $CUR_TKN_DATA_CHR (tk_array->node[tk_array->current_node].data.ch)
#define $CUR_TKN_DATA_TYP (tk_array->node[tk_array->current_node].data_type)

#define $CURRENT_TOKEN (tk_array->node[tk_array->current_node])
#define $CURRENT (tk_array->current_node)

//#define $300$ printf("I was in function %s\n", __func__);
//#define $300$ printf("cur = %d\n", $CURRENT);
#define $300$ 

#define $Require(ch) Require(ch, tk_array)

//==========================================================================
void CheckPtr(void* ptr, const char* error)
{
    if (ptr == NULL)
    {
        printf("%s", error);
        assert(0);
    }
}
//==========================================================================
tkn_arr* GetAllTokens(FILE* inputfile, buffer* buf)
{
    assert(inputfile);
    assert(buf);

    buf->pos = 0;
    buf->size = scanf_file_size(inputfile);
    printf("%d ", buf->size);
    buf->data = (char*) calloc(buf->size, sizeof(char));
    CheckPtr(buf->data, "Buffer callocation error!");

    fread(buf->data, sizeof(char), buf->size, inputfile);

    tkn_arr* tk_array = (tkn_arr*) calloc(1, sizeof(tkn_arr));
    CheckPtr(tk_array, "token tk_array callocation error!");

    tk_array->node = (Node*) calloc(MAX_TKN_CUNT, sizeof(Node));

    GetTokens(buf, tk_array);

    return tk_array;
}
//==========================================================================
void GetTokens (buffer* buf, tkn_arr* tk_array)
{
    while( $BUF_CUR_ELEM != '$')
    {
        if      (isalpha($BUF_CUR_ELEM))
        {
            $CUR_TKN_DATA_LNG = get_word(buf, tk_array);
            $CURRENT += 1;

            tk_array->n_cunt += 1;
        }
        else if (isnumber($BUF_CUR_ELEM))
        {
            $CUR_TKN_DATA_LNG = get_num(buf, tk_array);
            $CURRENT += 1;

            tk_array->n_cunt += 1;
        }
        else if (isoperator($BUF_CUR_ELEM))
        {
            $CUR_TKN_DATA_LNG = get_op(buf, tk_array);
            $CURRENT += 1;

            tk_array->n_cunt += 1;
        }
        else if (isbracket($BUF_CUR_ELEM))
        {
            $CUR_TKN_DATA_LNG = get_bracket(buf, tk_array);
            $CURRENT += 1;

            tk_array->n_cunt += 1;
        }
        else if (iscomma($BUF_CUR_ELEM))
        {
            $CUR_TKN_DATA_LNG = 1;
            $CUR_TKN_DATA_CHR = ',';
            $CUR_TKN_DATA_TYP = COMMA;

            $CURRENT += 1;
            
            buf->pos += 1;

            tk_array->n_cunt += 1;
        }
        else if (isspace($BUF_CUR_ELEM))
        {
            space_skip(buf);
        }
        else 
        {
            printf("%c\n", $BUF_CUR_ELEM);
            assert(0 && "INCORRECT INPUT bitch!");
        }
    }
    $CUR_TKN_DATA_CHR = '$';
    $CUR_TKN_DATA_TYP = DOLLAR;
    $CUR_TKN_DATA_LNG = 1;
    tk_array->n_cunt += 1;

}
//==========================================================================
void    space_skip  (buffer* buf)
{
    while(isspace($BUF_CUR_ELEM))
    {
        buf->pos += 1;
    }
}
//==========================================================================
int     get_bracket (buffer* buf, tkn_arr* tk_array)
{
    $CUR_TKN_DATA_CHR = $BUF_CUR_ELEM;

    buf->pos += 1;

    $CUR_TKN_DATA_TYP = BRACKET;

    return 1;
}
//==========================================================================
int     get_op      (buffer* buf, tkn_arr* tk_array)
{
    int cunt = 0;
    char* op = NULL;
    op = &($BUF_CUR_ELEM);

    $CUR_TKN_DATA_CHR = $BUF_CUR_ELEM;

    while(isoperator($BUF_CUR_ELEM))
    {
        cunt += 1;
        buf->pos += 1;
    }
    if (cunt > 1 || $CUR_TKN_DATA_CHR == '<' || $CUR_TKN_DATA_CHR == '>')
    {
        $CUR_TKN_DATA_TYP = REL_OPERATOR;
        $CUR_TKN_DATA_STR = op;
    }

    else
        $CUR_TKN_DATA_TYP = OPERATOR;

    return cunt;
}
//==========================================================================
int     get_num     (buffer* buf, tkn_arr* tk_array)
{
    int cunt = 0;
    double tmp_dbl = 0;

    while(isnumber($BUF_CUR_ELEM))
    {
        cunt += 1;

        tmp_dbl *= 10;
        tmp_dbl += $BUF_CUR_ELEM - '0';

        buf->pos += 1;
    }

    $CUR_TKN_DATA_DBL = tmp_dbl;

    $CUR_TKN_DATA_TYP = CONSTANT;

    return cunt;
}
//==========================================================================
int     get_word    (buffer* buf, tkn_arr* tk_array)
{
    int cunt = 0;

    $CUR_TKN_DATA_STR = &($BUF_CUR_ELEM);
    
    while (isalpha($BUF_CUR_ELEM) || $BUF_CUR_ELEM == '_'||$BUF_CUR_ELEM == '\'')
    {
        cunt += 1;
        buf->pos += 1;
    }

    if (strncmp($CUR_TKN_DATA_STR, "for_weak_people", 15) == EQUAL)
        {
            $CUR_TKN_DATA_CHR = '}';
            $CUR_TKN_DATA_TYP = BRACKET;
            cunt = 1;

            return cunt;
        }
    
    else if (strncmp($CUR_TKN_DATA_STR, "word_stress", 11) == EQUAL)
        {
            $CUR_TKN_DATA_CHR = '{';
            $CUR_TKN_DATA_TYP = BRACKET;
            cunt = 1;
            
            return cunt;
        }

    else if (strncmp($CUR_TKN_DATA_STR, "tortbl", 6) == EQUAL)
    {
        $CUR_TKN_DATA_TYP = IF;
    }
    else if (strncmp($CUR_TKN_DATA_STR, "pinzhak", 7) == EQUAL)
    {
        $CUR_TKN_DATA_TYP = WHILE;
    }
    else if (strncmp($CUR_TKN_DATA_STR, "lattE", 5) == EQUAL)
    {
        $CUR_TKN_DATA_TYP = ELSE;
    }
    else if (strncmp($CUR_TKN_DATA_STR, "zvOnit", 6) == EQUAL)
    {
        $CUR_TKN_DATA_TYP = RETURN;
    }
    else if (strncmp($CUR_TKN_DATA_STR, "lOzhit'", 6) == EQUAL)
    {
        $CUR_TKN_DATA_TYP = OPERATOR;
        $CUR_TKN_DATA_CHR = '=';
    }
    else
    {
        $CUR_TKN_DATA_TYP = VARIABLE;
    }
    return cunt;
}
//==========================================================================
int     isnumber    (char value)
{
    if (value <= '9' && value >= '0')
        return 1;
    
    else 
        return 0;
}
//==========================================================================
int     isoperator  (char value)
{
    if ($VALUE_IS_OPERATOR)
        return 1;

    else
        return 0;

}
//==========================================================================
int     isbracket   (char value)
{
    if (value == '(' || value == ')' || value == '{' || value == '}')
        return 1;

    else
        return 0;
}
//==========================================================================
int     iscomma (char value)
{
    if (value == ',')
    {
        return 1;
    }
    else 
        return 0;
}
//==========================================================================
void PrintAllTokens (tkn_arr* tk_array)
{
    for(int i = 0 ; i < tk_array->n_cunt ; i++)
    {
        assert(i < tk_array->n_cunt);
        assert(i >= 0);
        switch(tk_array->node[i].data_type)
        {
            case CONSTANT:
                printf("%d) data = %.1lf|| type = CONSTANT      || data lng = %d\n", i, tk_array->node[i].data.dbl, tk_array->node[i].data_lng);
                break;
            case IF:
                printf("%d) data = %.*s|| type = KEYWORD       || data lng = %d\n", i, tk_array->node[i].data_lng, tk_array->node[i].data.str, tk_array->node[i].data_lng);
                break;
            case WHILE:
                printf("%d) data = %.*s|| type = KEYWORD       || data lng = %d\n", i, tk_array->node[i].data_lng, tk_array->node[i].data.str, tk_array->node[i].data_lng);
                break;
            case ELSE:
                printf("%d) data = %.*s|| type = KEYWORD       || data lng = %d\n", i, tk_array->node[i].data_lng, tk_array->node[i].data.str, tk_array->node[i].data_lng);
                break;
            case RETURN:
                printf("%d) data = %.*s|| type = KEYWORD       || data lng = %d\n", i, tk_array->node[i].data_lng, tk_array->node[i].data.str, tk_array->node[i].data_lng);
                break;
            case OPERATOR:
                printf("%d) data = %c  || type = OPERATOR      || data lng = %d\n", i, tk_array->node[i].data.ch, tk_array->node[i].data_lng);
                break;
            case COMMA:
                printf("%d) data = %c  || type = COMMA         || data lng = %d\n", i, tk_array->node[i].data.ch, tk_array->node[i].data_lng);
                break;
            case REL_OPERATOR:
                printf("%d) data = %.*s|| type = REL_OPERATOR  || data lng = %d\n", i, tk_array->node[i].data_lng, tk_array->node[i].data.str, tk_array->node[i].data_lng);
                break;
            case VARIABLE:
                printf("%d) data = %.*s|| type = VARIABLE      || data lng = %d\n", i, tk_array->node[i].data_lng, tk_array->node[i].data.str, tk_array->node[i].data_lng);
                break;
            case BRACKET:
                printf("%d) data = %c  || type = BRACKET       || data lng = %d\n", i, tk_array->node[i].data.ch, tk_array->node[i].data_lng);
                break;
            case DOLLAR:
                printf("%d) data = %c  || type = DOLLAR        || data lng = %d\n", i, tk_array->node[i].data.ch, tk_array->node[i].data_lng);
                break;
        }
    }
}
//==========================================================================
Node* GetG (tkn_arr* tk_array)
{
    $CURRENT = 0;

    Node* var = NULL;

    var = GetStmts(tk_array);
        
    if ($CUR_TKN_DATA_CHR == '$')
    {
        printf("IT IS THE END!!!");
    }
    else
    {
        printf("It is not '$', it is %c", $CUR_TKN_DATA_CHR);
        SyntaxERROR(__func__);
    }
    if (var == NULL)
    {
        assert(ERRORR && "Something goes wrong, tree->peak = NULL");
    }   
    else 
    {
        printf("Correct end of GetG !\n");
    }

    return var; 
}
//==========================================================================
Node* GetStmts (tkn_arr* tk_array)
{
    if ($CUR_TKN_DATA_TYP == DOLLAR || ($CUR_TKN_DATA_CHR == '}' && $CUR_TKN_DATA_TYP == BRACKET))
    {
        return NULL;
    }
    else 
    {
        Node* stmt_node   = NULL;
		stmt_node         = CreateNode(STATEMENT);

		Node* stmt        = GetStmt(tk_array);
		Node* stmts_after = GetStmts(tk_array);

        stmt_node->left   = stmts_after;
        stmt_node->right  = stmt;

		return stmt_node;
    }

    assert(0);
    return NULL;
}
//==========================================================================
Node* GetStmt (tkn_arr* tk_array)
{$300$
    
    switch($CUR_TKN_DATA_TYP)
    {
        case VARIABLE:
        {   
            int start_current = 0;
            start_current = $CURRENT;
            $CURRENT += 1;
            if ($CUR_TKN_DATA_CHR == '(' && $CUR_TKN_DATA_TYP == BRACKET)
            {
                $CURRENT += 1;
                while ($CUR_TKN_DATA_CHR != ')' && $CUR_TKN_DATA_TYP != BRACKET)
                {
                    assert($CURRENT < tk_array->n_cunt);
                    $CURRENT += 1;
                }
                $CURRENT += 1;

                if ($CUR_TKN_DATA_CHR == '{' && $CUR_TKN_DATA_TYP == BRACKET)
                {
                    $CURRENT = start_current;

                    Node* define_node = NULL;
                    Node* func_node = NULL;

                    define_node = CreateNode(DEFINE);
                    func_node = CreateNode(FUNCTION);

                    define_node->left = func_node;

                    func_node->left = &($CURRENT_TOKEN);
                    
                    $CURRENT += 1;
                    $Require('(');
                    func_node->right = GetArgs(tk_array);
                    $Require(')');

                    $Require('{');
                    define_node->right = GetStmts(tk_array);
                    $Require('}');

                    return define_node;
                }
                else 
                {
                    $CURRENT = start_current;

                    Node* call_node = NULL;

                    call_node = CreateNode(CALL);

                    call_node->left = &($CURRENT_TOKEN);

                    $CURRENT += 1;

                    $Require('(');
                    call_node->right = GetArgsCall(tk_array);
                    $Require(')');
                    $Require(';');

                    return call_node;
                }
                    
            }
            else 
                $CURRENT = start_current;
            
        }
        case CONSTANT:
        {   
            Node* left_part_stmt = GetE(tk_array);

            Node* op_node = &($CURRENT_TOKEN);

            if ($CUR_TKN_DATA_CHR != '=' && $CUR_TKN_DATA_TYP != REL_OPERATOR)
            {   
                printf("full jopa!\n");
                SyntaxERROR(__func__);
            }
            $CURRENT += 1;

            op_node->right = GetE(tk_array);
            op_node->left = left_part_stmt;

            if ($CUR_TKN_DATA_CHR == ')' && $CUR_TKN_DATA_TYP == BRACKET);
            else
                $Require(';');

            return op_node;
            break;
        }
        case IF:
        {
            Node* if_node = &($CURRENT_TOKEN);
            $CURRENT += 1;

            $Require('(');
            if_node->left = GetStmt(tk_array);
            $Require(')');

            $Require('{');
            Node* desision = CreateNode(DESISION);
            if_node->right = desision;
            desision->left = GetStmts(tk_array);
            $Require('}');

            if ($CUR_TKN_DATA_TYP == ELSE)           
            {
                $CURRENT += 1;

                $Require('{');
                desision->right = GetStmts(tk_array);
                $Require('}');
            }

            return if_node;
            break;
        }
        case WHILE:
        { 
            Node* while_node = NULL;
            while_node = &($CURRENT_TOKEN);

            $CURRENT += 1;

            $Require('(');
            while_node->left = GetStmt(tk_array);
            $Require(')');

            $Require('{');
            while_node->right = GetStmts(tk_array);
            $Require('}');

            return while_node;
            break;
        }
        case RETURN:
        {
            Node* return_node = &($CURRENT_TOKEN);
            $CURRENT += 1;
            return_node->left = GetE(tk_array);
            $Require(';');

            return return_node;
        }
        default:
        {
            assert(0);
            return 0;
        }
    }
    assert(0);
    return NULL;
}
//==========================================================================
Node* GetE (tkn_arr* tk_array)
{
    Node* val = NULL;
    val = GetT(tk_array);
    
    Node* op = NULL;
    Node* old_op = NULL;
    op = val;
    old_op = val;

    while($CUR_TKN_DATA_TYP == OPERATOR && ($CUR_TKN_DATA_CHR == '+' || $CUR_TKN_DATA_CHR == '-'))
    {
        old_op = op;
        op = &($CURRENT_TOKEN);

        $CURRENT += 1;
        
        Node* val2 = NULL;
        val2 = GetT(tk_array);
        
        op->left = old_op; // TODO bind_l_r(old_op, op, val2)
        
        op->right = val2;
    }
    
    return op;
}
//==========================================================================
Node* GetT (tkn_arr* tk_array)
{
    Node* val = NULL;
    val = GetP(tk_array);
    
    Node* op = NULL;
    Node* old_op = NULL;
    op = val;
    old_op = val;

    while($CUR_TKN_DATA_TYP == OPERATOR && ($CUR_TKN_DATA_CHR == '*' || $CUR_TKN_DATA_CHR == '/'))
    {
        old_op = op;
        op = &($CURRENT_TOKEN);

        $CURRENT += 1;

        Node* val2 = NULL;
        val2 = GetP(tk_array);
        
        
        op->left = old_op; // TODO bind_l_r(old_op, op, val2)
        op->right = val2;
    }
    
    
    return op;
}
//==========================================================================
Node* GetP (tkn_arr* tk_array)
{
    if ($CUR_TKN_DATA_CHR == '(' && $CUR_TKN_DATA_TYP == BRACKET)
    {
        $Require('(');
        Node* val = NULL;
        val = GetE(tk_array);
        $Require(')');

        return val;
    }
    else 
    {
        Node* val2 = GetN(tk_array);
        
        return val2;
    }
}
//==========================================================================
Node* GetN (tkn_arr* tk_array)
{
    Node* val = NULL;

    if ($CUR_TKN_DATA_TYP == DOLLAR)
    {
        return val;
    }
    if ($CUR_TKN_DATA_TYP == CONSTANT)
    {
        val = &($CURRENT_TOKEN);
        $CURRENT += 1;
    }

    else if ($CUR_TKN_DATA_TYP == VARIABLE)
    {
        val = GetV(tk_array);
    }
    
    else
    {
        printf("In func GetN there is a synt error, char = '%c' dbl = %lf\n", $CUR_TKN_DATA_CHR, $CUR_TKN_DATA_DBL);
        SyntaxERROR(__func__);
    }

    return val;
}
//==========================================================================
Node* GetV (tkn_arr* tk_array)
{
    Node* val = &($CURRENT_TOKEN);

    $CURRENT += 1;

    if ($CUR_TKN_DATA_CHR == '(' && $CUR_TKN_DATA_TYP == BRACKET)
    {
        $CURRENT += 1;

        Node* call_node = CreateNode(CALL);

        call_node->left = val;

        if ($CUR_TKN_DATA_CHR == ')' && $CUR_TKN_DATA_TYP == BRACKET)
        {
            $CURRENT += 1;

            return call_node;
        }

        else
        {
            call_node->right = GetArgsCall(tk_array);
            $Require(')');
            return call_node;
        }
    }
    
    return val;
}
//==========================================================================
Node* GetArgs (tkn_arr* tk_array)
{
    if ($CUR_TKN_DATA_CHR == ')' && $CUR_TKN_DATA_TYP == BRACKET)
    {
        return NULL;
    }

    else
    {
        if ($CUR_TKN_DATA_CHR == ',' && $CUR_TKN_DATA_TYP == COMMA)
            $CURRENT += 1;
        
        Node* param_node = CreateNode(PARAMETER);
		
		Node* param  = GetArg(tk_array);
		Node* params_after = GetArgs(tk_array);

        param_node->left  = params_after;
        param_node->right = param;

		return param_node;
    }

}
//==========================================================================
Node* GetArgsCall (tkn_arr* tk_array)
{
    if ($CUR_TKN_DATA_CHR == ')' && $CUR_TKN_DATA_TYP == BRACKET)
    {
        return NULL;
    }

    else
    {
        if ($CUR_TKN_DATA_CHR == ',' && $CUR_TKN_DATA_TYP == COMMA)
            $CURRENT += 1;
        
        Node* param_node = CreateNode(PARAMETER_CALL);
		
		Node* param  = GetArg(tk_array);
		Node* params_after = GetArgsCall(tk_array);

        param_node->left  = params_after;
        param_node->right = param;

		return param_node;
    }

}
//==========================================================================
Node* GetArg (tkn_arr* tk_array)
{
    Node* var = GetE(tk_array);
    
    return var;
}
//==========================================================================
[[noreturn]] void SyntaxERROR (const char* s)
{
    printf("Ha ha oshibsya in function %s\n", s);
    assert(0);
}
//==========================================================================
void Require (char ch, tkn_arr* tk_array)
{
    if (isbracket(ch))
    {
        if ($CUR_TKN_DATA_CHR == ch && $CUR_TKN_DATA_TYP == BRACKET)
            $CURRENT += 1;
        
        else
        {
            printf("Incorrect input, expected '%c' but have '%c'\n", ch, $CUR_TKN_DATA_CHR);
            assert(0);
        }
    }
    else if ($CUR_TKN_DATA_CHR == ch)
    {
        $CURRENT += 1;
    }
    else
    {
        printf("Incorrect input, expected '%c' but have '%c'\n", ch, $CUR_TKN_DATA_CHR);
        assert(0);
    }
}
//==========================================================================
Node* CreateNode (int node_type)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    node->data_type = node_type;
    
    return node;
}
//==========================================================================
#include "rev_frontend.h"

int VisitPrintTree (Node* node)
{
    printf("(");

    if(node->left)
        VisitPrintTree(node->left);

    switch(node->data_type)
    {
        case STATEMENT:
            printf("statement");
            break;
        case DEFINE:
            printf("define");
            break;
        case FUNCTION:
            printf("function");
            break;
        case OPERATOR:
            printf("%c", node->data.ch);
            break;
        case REL_OPERATOR:
            printf("%.*s", node->data_lng, node->data.str);
            break;
        case CALL:
            printf("call");
            break;
        case VARIABLE:
            printf("%.*s", node->data_lng, node->data.str);
            break;
        case CONSTANT:
            printf("%lf", node->data.dbl);
            break;
        case PARAMETER:
            printf("parametr");
            break;
        case PARAMETER_CALL:
            printf("parametr");
            break;
    }    

    if (node->right)
        VisitPrintTree(node->right);

    printf(")");
}








#undef $VALUE_IS_OPERATOR

#undef $BUF_CUR_ELEM

#undef $CUR_TKN_DATA_LNG
#undef $CUR_TKN_DATA_STR
#undef $CUR_TKN_DATA_DBL
#undef $CUR_TKN_DATA_CHR
#undef $CUR_TKN_DATA_TYP

#undef $CURRENT_TOKEN
#undef $CURRENT
#undef $300$

#undef $Require

// zvOnit = retuen
// krasivEe = while
// tortbl = if
//  
//      
//  
//  
//  
//  