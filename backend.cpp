#include "backend.h"
#include "asm_instructions.h"
#include "elfhdr.h"

const int TRASH = 228;
const int BREAK = 0;
const int CONITINUE = 1;

#define $ND_CHR node->data.ch
#define $ND_DBL node->data.dbl
#define $ND_STR node->data.str
#define $ND_LNG node->data_lng
#define $ND_TYP node->data_type

#define $CUR_FUNC jtable->func_jmp[jtable->tmp_func]

#define $FREE_VAR vr_lists->var[vr_lists->free]
#define $FREE     vr_lists->free

#define $R node->right
#define $L node->left

#define $VISIT(node)                              \
if (node)                                         \
    VisitWriteCommands(node, vr_lists, fdata, jtable)  

#define $VISIT_NEW_LIST(node, newlist)            \
if (node)                                         \
    VisitWriteCommands(node, newlist, fdata, jtable)

//==========================================================================
static int  std_funcs_add         (ELFfile* fdata, JMPtable* jtable);

static int  RelOpHash             (Node* node);
//==========================================================================
static void visit_check_var_const_use_rbx (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static void visit_check_var_const_use_rax (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);
//==========================================================================
static int VWC_stmt_case          (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_op_case            (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_op_case_equ        (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_var_case           (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_const_case         (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_def_case           (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_func_case          (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_param_case         (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_call_param_case    (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_call_case          (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_call_case_std_func (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable, int func_hash);

static int VWC_ret_case           (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_if_case            (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_desis_case         (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_rel_op_case        (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);

static int VWC_while_case         (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable);
//==========================================================================
int ELFCtor              (ELFfile* fdata)
{
    assert(fdata);

    fdata->exe_file = fopen("roma.lox", "w");

    if (fdata->exe_file == NULL)
        assert(0 && "EXECUTABLE FILE PTR IS NULL !!");

    ELFhdrCtor(&fdata->ehdr);
    PhdrCtor(&fdata->phdr);

    return 0;
}
//==========================================================================
int GenerateELFFile      (Node* node)
{
    assert(node);

    var_lists* vr_lists = (var_lists*) calloc(1, sizeof(var_lists));
    vr_lists->free = 1;

    vr_lists->var = (var_list*) calloc(VAR_MAX_CUNT, sizeof(var_list));

    FILE* com_file = fopen("asm_file.txt", "w");

    ELFfile* fdata = (ELFfile*) calloc(1, sizeof(ELFfile));

    ELFCtor(fdata);

    JMPtable* jtable = (JMPtable*) calloc(1, sizeof(JMPtable));
    
    $WRITE_CONST(0, double);           // fill code buffer with trash for memory alignment
    
    std_funcs_add(fdata, jtable);
    
    $WRITE_OPCDE(push_rbp);
    $WRITE_OPCDE(mov_rbp_rsp);
    $WRITE_OPCDE(sub_rsp);
    int tmp_ip = fdata->ip;
    $WRITE_CONST(TRASH, int);          // subtract from rsp the number of local variables to be able to use stack

    VisitWriteCommands(node, vr_lists, fdata, jtable);
    
    $WRITE_CONST_IP((vr_lists->free - 1) * 8, int, tmp_ip);   // vr_lists->free - 1 = number of local variables

    $WRITE_OPCDE(my_exit);

    printf("%d <- ip at the end\n", fdata->ip);

    FixPhdr(fdata);
    
    fwrite((char*)(&fdata->ehdr), sizeof(char), sizeof(Elf64_Ehdr), fdata->exe_file);
    fwrite((char*)(&fdata->phdr), sizeof(char), sizeof(Elf64_Phdr), fdata->exe_file);
    fwrite((char*)(&fdata->code), sizeof(char), fdata->ip, fdata->exe_file);
    
    system("chmod +x roma.lox");

    free(vr_lists->var);
    free(vr_lists);
    free(jtable);
    fclose(com_file);
    fclose(fdata->exe_file);

    //system("./roma.lox");
    
    return 0;
}
//==========================================================================
int VisitWriteCommands   (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{//TODO бэк-энд оптимизация!
    switch($ND_TYP)
    {
        case STATEMENT://rework doesnt needed
        {
            return VWC_stmt_case(node, vr_lists, fdata, jtable);
        }
        case OPERATOR://DONE!!!!!
        {
            return VWC_op_case(node, vr_lists, fdata, jtable);
        }
        case VARIABLE://reworked
        {
            return VWC_var_case(node, vr_lists, fdata, jtable);
        }
        case CONSTANT://reworked
        {   
            return VWC_const_case(node, vr_lists, fdata, jtable);
        }
        case DEFINE://done
        {
            return VWC_def_case(node, vr_lists, fdata, jtable);
        }
        case FUNCTION://reworked
        {
            return VWC_func_case(node, vr_lists, fdata, jtable);
        }
        case PARAMETER://norm vrodeby
        {
            return VWC_param_case(node, vr_lists, fdata, jtable);
        }
        case PARAMETER_CALL://done
        {
            return VWC_call_param_case(node, vr_lists, fdata, jtable);
        }
        case CALL://done
        {
            return VWC_call_case(node, vr_lists, fdata, jtable);
        }
        case RETURN://done
        {
            return VWC_ret_case(node, vr_lists, fdata, jtable);
        }
        case IF://done
        {
            return VWC_if_case(node, vr_lists, fdata, jtable);
        }
        case DESISION://done
        {
            return VWC_desis_case(node, vr_lists, fdata, jtable);
        }
        case REL_OPERATOR:
        {
            return VWC_rel_op_case(node, vr_lists, fdata, jtable);
        }
        case WHILE://done
        {
            return VWC_while_case(node, vr_lists, fdata, jtable);
        }
        defaul:
        {
            assert(0 && "Shit happen(s) (undefined case in VisitWriteCommands switch)");
        }
    }// Эссэ "Ночной кодинг))"
    return 0;
}
//==========================================================================
int VWC_stmt_case        (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    $VISIT($R);
    $VISIT($L);
    
    return BREAK;
}
//==========================================================================
int VWC_op_case_equ      (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    if ($ND_CHR == EQL)
    {
        if ($R->data_type == CONSTANT)          
            {                                   
                $WRITE_OPCDE(mov_rax_const);    
                $VISIT($R);                     
            }

        else if($R->data_type == VARIABLE)      
            {                                   
                $WRITE_OPCDE(mov_rax_mem_rbp);  
                $VISIT($R);                     
            }
        
        else
            $VISIT($R);

        if ($L->data_type != VARIABLE)
            assert(0 && "INCORRECT INPUT! YOU CAN ONLY ASSIGN A VALUE TO VARIABLE!\n");

        int tmp_var_hash = murmurHash($L->data.str, $L->data_lng);
        int tmp_var_num  = FindVariable(vr_lists, tmp_var_hash);
        
        $WRITE_OPCDE(mov_mem_rbp_rax);
        $WRITE_CONST(tmp_var_num * (-8), int);

        return BREAK;
    }

    visit_check_var_const_use_rax($L, vr_lists, fdata, jtable);
    
    visit_check_var_const_use_rbx($R, vr_lists, fdata, jtable);

    return CONITINUE;
}
//==========================================================================
int VWC_op_case          (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    int break_check = VWC_op_case_equ(node, vr_lists, fdata, jtable);

    if (break_check == BREAK)
        return BREAK;

    switch ($ND_CHR)
    {
        case MUL:
        {
            $WRITE_OPCDE(imul_rax_rdx);
            $WRITE_OPCDE(idiv_rax_100); 
            break;
        }
        case SUB:
        {
            $WRITE_OPCDE(sub_rax_rbx);
            break;
        }
        case ADD:
        {   PRINT_LINE
            $WRITE_OPCDE(add_rax_rbx);
            break;
        }
        case DIV:
        {
            $WRITE_OPCDE(xor_rdx_rdx);
            $WRITE_OPCDE(div_rbx);
            $WRITE_OPCDE(imul_rax_100);
            break;
        }
        case DEG:
        {
            assert(ERRORR && "Sorry, but exponentiation is not allowed))");
        }
        default:
        {
            if ($ND_CHR == '=')
                return 0;
            assert(ERRORR && "case operator incorrect input");
        }
    }
    
    return BREAK;
}
//==========================================================================
int VWC_var_case         (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    int tmp_var_hash = murmurHash($ND_STR, node->data_lng);
    int tmp_var_num  = FindVariable(vr_lists, tmp_var_hash);

    $WRITE_CONST(tmp_var_num * (-8), int);

    return BREAK;
}
//==========================================================================
int VWC_const_case       (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    int temp = $ND_DBL;
    temp *= 100;
    $WRITE_CONST(temp, int);

    return BREAK;
}
//==========================================================================
int VWC_def_case         (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    $WRITE_OPCDE(jmp);                                  // function skip jump
    int func_skip_jmp_ip = fdata->ip;
    $WRITE_CONST(fdata->ip, int);

    var_lists vr_lists_new = {};
    vr_lists_new.var = (var_list*) calloc(VAR_MAX_CUNT, sizeof(var_list));
    vr_lists_new.free = 1;

    $VISIT_NEW_LIST($L, &vr_lists_new);

    $WRITE_OPCDE(mov_r11_rbp);
    $WRITE_OPCDE(pop_r12);
    $WRITE_OPCDE(mov_rbp_rsp);

    int arg_num = 0;
    for (int i = vr_lists_new.free - 2 ; i >= 0 ; i--)
    {
        $WRITE_OPCDE(pop_mem_rbp);
        $WRITE_CONST(arg_num * (-8), int);
        arg_num += 1;
    }

    $WRITE_OPCDE(push_r12);
    $WRITE_OPCDE(push_r11);
    $WRITE_OPCDE(mov_rbp_rsp);
    $WRITE_OPCDE(sub_rsp);
    int ip_sub_rsp = fdata->ip;                         // save current ip to rewrite it lately
    $WRITE_CONST(TRASH, int);

    $VISIT_NEW_LIST($R, &vr_lists_new);

    $WRITE_CONST_IP((vr_lists_new.free -1) * (8), int, ip_sub_rsp);
    $WRITE_OPCDE(mov_rsp_rbp);
    $WRITE_OPCDE(pop_rbp);
    $WRITE_OPCDE(ret);
    
    $WRITE_CONST_IP(fdata->ip - func_skip_jmp_ip - sizeof(int), int, func_skip_jmp_ip);

    free(vr_lists_new.var);

    return BREAK;
}
//==========================================================================
int VWC_func_case        (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    $CUR_FUNC.func_hash = murmurHash($L->data.str, $L->data_lng); 
    $CUR_FUNC.func_ip = fdata->ip;
    jtable->tmp_func += 1;

    $VISIT($R);

    return BREAK;
}
//==========================================================================
int VWC_param_case       (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    int tmp_hash = murmurHash(node->right->data.str, node->right->data_lng);

    $FREE_VAR.var_hash     = tmp_hash;
    $FREE += 1;

    $VISIT($L);

    return BREAK;
}
//==========================================================================
int VWC_call_param_case  (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    if ($R->data_type == CONSTANT)
    {
        $WRITE_OPCDE(push_constant);
        $VISIT($R);
    }

    if($R->data_type == VARIABLE)
    {
        $WRITE_OPCDE(push_mem_rbp);
        $VISIT($R);
    }
    else
    {
        $VISIT($R);
        $WRITE_OPCDE(push_rax);
    }
    
    return BREAK;
}
//==========================================================================
int VWC_call_case        (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    int func_hash = murmurHash($L->data.str, $L->data_lng);
    
    int std_func_flag = VWC_call_case_std_func(node, vr_lists, fdata, jtable, func_hash);

    if (std_func_flag == STD_FUNC_DETECTED)
        return BREAK;

    $VISIT($R);

    int hash = murmurHash($L->data.str, $L->data_lng);
    int func_ip = FindFunction(jtable, hash);
    if (func_ip == -1)
        assert(0 && "YOU CAN'T USE FUNCTIONS BEFORE THEY'RE DEFINED!!!");

    $WRITE_OPCDE(call);
    $WRITE_CONST(func_ip - fdata->ip - sizeof(int), int);//

    return BREAK;
}
//==========================================================================
int VWC_call_case_std_func (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable, int func_hash)
{
    if (func_hash == SCAN_HASH)
    { 
        int hash = murmurHash($L->data.str, $L->data_lng);
        int func_ip = FindFunction(jtable, hash);

        $WRITE_OPCDE(call);
        $WRITE_CONST(func_ip - fdata->ip - sizeof(int), int);

        if ($R->right->data_type != VARIABLE)
            assert(0 && "INCORRECT INPUT, SCAN MUST HAVE VARIABLE AS A PARAMETER!");

        $WRITE_OPCDE(imul_rax_100);

        $WRITE_OPCDE(mov_mem_rbp_rax);
        $VISIT($R->right);

        return STD_FUNC_DETECTED;
    }
    else if (func_hash == PRINT_HASH)
    {
        int hash = murmurHash($L->data.str, $L->data_lng);
        int func_ip = FindFunction(jtable, hash);

        visit_check_var_const_use_rax($R->right, vr_lists, fdata, jtable);

        $WRITE_OPCDE(call);
        $WRITE_CONST(func_ip - fdata->ip - sizeof(int), int);

        return STD_FUNC_DETECTED;
    }
    else if (func_hash == SQRT_HASH)
    {
        int hash = murmurHash($L->data.str, $L->data_lng);
        int func_ip = FindFunction(jtable, hash);

        visit_check_var_const_use_rax($R->right, vr_lists, fdata, jtable);

        $WRITE_OPCDE(imul_rax_100);
        
        $WRITE_OPCDE(call);
        $WRITE_CONST(func_ip - fdata->ip - sizeof(int), int);

        return STD_FUNC_DETECTED;
    }

    return STD_FUNC_NOT_DETECTED;
}
//==========================================================================
int VWC_ret_case         (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    if ($L->data_type == CONSTANT)
    {
        $WRITE_OPCDE(push_constant);
        $VISIT($L);
        $WRITE_OPCDE(pop_rax);
    }
    else if ($L->data_type == VARIABLE)
    {
        $WRITE_OPCDE(push_mem_rbp);
        $VISIT($L);
        $WRITE_OPCDE(pop_rax);
    }
    else
        $VISIT($L);

    $WRITE_OPCDE(mov_rsp_rbp);
    $WRITE_OPCDE(pop_rbp);
    $WRITE_OPCDE(ret);
    
    return BREAK;
}
//==========================================================================
int VWC_if_case          (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    $VISIT($L);
    $VISIT($R);

    return BREAK;
}
//==========================================================================
int VWC_desis_case       (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    int rel_jmp_ip = fdata->ip;
    $WRITE_CONST(TRASH, int);

    $VISIT($L);
    
    $WRITE_OPCDE(jmp);            //jmp на скип else после тру ифа
    int else_skip_jmp = fdata->ip;
    $WRITE_CONST(TRASH, int);
    $WRITE_CONST_IP(fdata->ip - rel_jmp_ip - sizeof(int), int, rel_jmp_ip);

    $VISIT($R);

    $WRITE_CONST_IP(fdata->ip - else_skip_jmp - sizeof(int), int, else_skip_jmp);

    return BREAK;
}
//==========================================================================
int VWC_rel_op_case      (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    visit_check_var_const_use_rbx($R, vr_lists, fdata, jtable);
    
    visit_check_var_const_use_rax($L, vr_lists, fdata, jtable);

    $WRITE_OPCDE(cmp_rax_rbx);

    int rel_hash = RelOpHash(node);
    
    switch(rel_hash)//  Here I use jumps with the opposite condition !!!
    {
        case '==':
        {
            $WRITE_OPCDE(jne);
            break;
        }
        case '!=':
        {
            $WRITE_OPCDE(je);
            break;
        }
        case '>=':
        {
            $WRITE_OPCDE(jl);
            break;
        }
        case '<=':
        {
            $WRITE_OPCDE(jg);
            break;
        }
        case '>':
        {
            $WRITE_OPCDE(jle);
            break;
        }
        case '<':
        {
            $WRITE_OPCDE(jge);
            break;
        }
        default:
        {
            assert(0 && "OOooops incorrect relative operator !");
        }
    }

    return BREAK;
}
//==========================================================================
int VWC_while_case       (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    int while_strt_ip = fdata->ip;

    $VISIT($L);
    int while_end_ip = fdata->ip;
    $WRITE_CONST(TRASH, int);// trash jmp at the end

    $VISIT($R); // while body
    
    $WRITE_OPCDE(jmp); // jmp at the start

    $WRITE_CONST(while_strt_ip - fdata->ip - sizeof(int), int);

    $WRITE_CONST_IP(fdata->ip - while_end_ip - sizeof(int), int, while_end_ip);

    return BREAK;
}
//==========================================================================
void visit_check_var_const_use_rbx (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    if (node->data_type == CONSTANT)// was $R
    {
        $WRITE_OPCDE(mov_rbx_const);
        $VISIT(node);// was $R
    }

    else if(node->data_type == VARIABLE)// was $R
    {
        $WRITE_OPCDE(mov_rbx_mem_rbp);
        $VISIT(node);// was $R
    }
    else
    {
        $WRITE_OPCDE(push_rax);
        $VISIT(node);// was $R
        $WRITE_OPCDE(mov_rbx_rax);
        $WRITE_OPCDE(pop_rax);
    }
}
//==========================================================================
void visit_check_var_const_use_rax (Node* node, var_lists* vr_lists, ELFfile* fdata, JMPtable* jtable)
{
    if (node->data_type == CONSTANT)// was $L
    {
        $WRITE_OPCDE(mov_rax_const);
        $VISIT(node);// was $L
    }
    else if(node->data_type == VARIABLE)// was $L
    {
        $WRITE_OPCDE(mov_rax_mem_rbp);
        $VISIT(node);// was $L
    }
    else
    {
        $VISIT(node);// was $L
    }
}
//==========================================================================
int FindVariable         (var_lists* vr_lists, int hash)
{
    for (int i = 0 ; i < VAR_MAX_CUNT ; i++)
    {
        if (vr_lists->var[i].var_hash == hash)
            return i;
    }

    $FREE_VAR.var_hash     = hash;
    int var_num            = $FREE;

    $FREE += 1;

    return var_num;
}
//==========================================================================
int FindFunction         (JMPtable* jtable, int hash)
{
    for(int i = 0 ; i < jtable->tmp_func ; i++)
    {
        if (jtable->func_jmp[i].func_hash == hash)
            return jtable->func_jmp[i].func_ip;
    }

    assert(ERRORR && "YOU CAN'T USE FUNCTIONS BEFORE THEY'RE DEFINED!!!");
}
//==========================================================================
#define $add_one_std_func(fname)                                            \
do                                                                          \
{                                                                           \
    $WRITE_OPCDE(jmp);                                                      \
    jmp_ip = fdata->ip;                                                     \
    $WRITE_CONST(fdata->ip, int);                                           \
                                                                            \
    $CUR_FUNC.func_hash = murmurHash(fname, strlen(fname));                 \
    $CUR_FUNC.func_ip = fdata->ip;                                          \
    jtable->tmp_func += 1;                                                  \
                                                                            \
    $WRITE_OPCDE(my_##fname);                                               \
    $WRITE_CONST_IP(fdata->ip - jmp_ip - sizeof(int), int, jmp_ip);         \
} while (0);

int std_funcs_add(ELFfile* fdata, JMPtable* jtable)
{
    char scan[]  = "scan";
    char print[] = "print";
    char sqrt[]  = "sqrt";
    int jmp_ip   = 0;

    $add_one_std_func(scan);

    $add_one_std_func(print);

    $add_one_std_func(sqrt);

    return 0;
}
#undef $add_one_std_func
//==========================================================================
void FixPhdr(ELFfile* fdata)
{
    int memsz = fdata->ip;
    
    fdata->phdr.p_filesz = memsz;
    fdata->phdr.p_memsz = memsz;
}
//==========================================================================
int RelOpHash(Node* node)
{
    int hash = 0;

    for (int i = 0 ; i < $ND_LNG ; i++)
    {
        hash *= 256;
        hash += $ND_STR[i];
    }

    return hash;
}
//==========================================================================
int murmurHash (char * key, unsigned int len)
{
    const unsigned int m = 0x5bd1e995;
    const unsigned int seed = 0;
    const int r = 24;

    unsigned int h = seed ^ len;

    const unsigned char* data = (const unsigned char*) key;
    unsigned int k;

    while (len >= 4)
    {
        k  = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len)
    {
        case 3:
        h ^= data[2] << 16;
        case 2:
        h ^= data[1] << 8;
        case 1:
        h ^= data[0];
        h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}
//==========================================================================
#undef $ND_CHR
#undef $ND_DBL
#undef $ND_STR
#undef $ND_LNG
#undef $ND_TYP

#undef $FREE_VAR
#undef $FREE

#undef $R
#undef $L

#undef $VISIT
#undef $VISIT_NEW_LIST
#undef $PRINT