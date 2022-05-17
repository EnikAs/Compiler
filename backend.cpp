#include "backend.h"
#include "asm_instructions.h"
#include "elfhdr.h"

const int MAGIC4 = 4; // this magic number is used for the desired jump offset.
const int TRASH = 228;

#define $ND_CHR node->data.ch
#define $ND_DBL node->data.dbl
#define $ND_STR node->data.str
#define $ND_LNG node->data.lng
#define $ND_TYP node->data_type

#define $CUR_FUNC jtable->func_jmp[jtable->tmp_func]


// These defines are made to prevent the repetition of the same pieces of code. 
// They don't make the code easier to read. Sorry for that!
// Dont forget about "else" before visit next node)
#define $R_N_D_CONST_VISIT                          \
            if ($R->data_type == CONSTANT)          \
                {                                   \
                    $WRITE_OPCDE(mov_rbx_const);    \
                    $VISIT($R);                     \
                }

#define $R_N_D_VAR_VISIT                            \
            else if($R->data_type == VARIABLE)      \
                {                                   \
                    $WRITE_OPCDE(mov_rbx_mem_rbp);  \
                    $VISIT($R);                     \
                }

#define $L_N_D_CONST_VISIT                          \
            if ($L->data_type == CONSTANT)          \
                {                                   \
                    $WRITE_OPCDE(mov_rax_const);    \
                    $VISIT($L);                     \
                }       

#define $L_N_D_VAR_VISIT                            \
            else if($L->data_type == VARIABLE)      \
                {                                   \
                    $WRITE_OPCDE(mov_rax_mem_rbp);  \
                    $VISIT($L);                     \
                }


#define $FREE_VAR vr_lists->var[vr_lists->free]
#define $FREE     vr_lists->free

#define $R node->right
#define $L node->left

#define $VISIT(node)                              \
if (node)                                         \
    VisitWriteCommands(node, vr_lists, com_file, fdata, jtable)  

#define $VISIT_NEW_LIST(node, NL)                 \
if (node)                                         \
    VisitWriteCommands(node, NL, com_file, fdata, jtable)        

#define $PRINT(...) fprintf(com_file, __VA_ARGS__)
//#define $PRINT(...) printf(__VA_ARGS__)

//==========================================================================
int ELFCtor(ELFfile* fdata)
{
    fdata->exe_file = fopen("roma.lox", "w");

    if (fdata->exe_file == NULL)
        assert(0 && "EXECUTABLE FILE PTR IS NULL !!");

    ELFhdrCtor(&fdata->ehdr);
    PhdrCtor(&fdata->phdr);

    return 0;
}
//==========================================================================
int GenerateELFFile(Node* node)
{
    var_lists* vr_lists = (var_lists*) calloc(1, sizeof(var_lists));
    vr_lists->free = 1;

    vr_lists->var = (var_list*) calloc(VAR_MAX_CUNT, sizeof(var_list));

    FILE* com_file = fopen("asm_file.txt", "w");

    ELFfile* fdata = (ELFfile*) calloc(1, sizeof(ELFfile));

    ELFCtor(fdata);

    fdata->ip = 0;

    JMPtable* jtable = (JMPtable*) calloc(1, sizeof(JMPtable));
    
    $WRITE_CONST(0, double);           // fill code buffer with trash for memory alignment

    $WRITE_OPCDE(push_rbp);
    $WRITE_OPCDE(mov_rbp_rsp);
    $WRITE_OPCDE(sub_rsp);
    int tmp_ip = fdata->ip;
    $WRITE_CONST(TRASH, int);

    std_func_add(fdata, jtable);
    
    VisitWriteCommands(node, vr_lists, com_file, fdata, jtable);
    
    $WRITE_CONST_IP((vr_lists->free-1) * 8, int, tmp_ip);

    $WRITE_OPCDE(my_exit);


    printf("%d <- ip at the end\n", fdata->ip);
    int memsz = fdata->ip;
    
    fdata->phdr.p_filesz = memsz;
    fdata->phdr.p_memsz = memsz;
    
    fwrite((char*)(&fdata->ehdr), sizeof(char), sizeof(Elf64_Ehdr), fdata->exe_file);
    fwrite((char*)(&fdata->phdr), sizeof(char), sizeof(Elf64_Phdr), fdata->exe_file);
    fwrite((char*)(&fdata->code), sizeof(char), fdata->ip + 2, fdata->exe_file);
    
    system("chmod +x roma.lox");

    
    
    free(vr_lists->var);
    free(vr_lists);
    free(jtable);
    fclose(com_file);
    fclose(fdata->exe_file);

    system("./roma.lox");
    
    return 0;
}
//==========================================================================
int VisitWriteCommands (Node* node, var_lists* vr_lists, FILE* com_file, ELFfile* fdata, JMPtable* jtable)
{//TODO бэк-энд оптимизация!
    switch($ND_TYP)
    {
        case STATEMENT://rework doesnt needed
        {
            $VISIT($R);
            $VISIT($L);

            break;
        }
        case OPERATOR://DONE!!!!! Слишком большие кейсы полный рефакторинг
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

                if (tmp_var_num == -1)
                {
                    $FREE_VAR.var_hash     = tmp_var_hash;
                    $FREE_VAR.var_dx_shift = $FREE;
                    tmp_var_num            = $FREE;

                    $FREE += 1;
                }
                
                $WRITE_OPCDE(mov_mem_rbp_rax);
                $WRITE_CONST(tmp_var_num * (-8), int);

                return 0;
            }

            $L_N_D_CONST_VISIT
            $L_N_D_VAR_VISIT
            
            else
                $VISIT($L);
            
            if ($R->data_type == CALL)
            {
                $WRITE_OPCDE(push_rax);

                $VISIT($R);

                $WRITE_OPCDE(mov_rbx_rax);
                $WRITE_OPCDE(pop_rax);
            }

            else
            {
                if ($R->data_type == CONSTANT)
                {
                    $WRITE_OPCDE(mov_rbx_const);
                    $VISIT($R);
                }

                else if($R->data_type == VARIABLE)
                {
                    $WRITE_OPCDE(mov_rbx_mem_rbp);
                    $VISIT($R);
                }
                else
                {
                    $WRITE_OPCDE(push_rax);
                    $VISIT($R);
                    $WRITE_OPCDE(mov_rbx_rax);
                    $WRITE_OPCDE(pop_rax);
                }
            }
            switch ($ND_CHR)
            {
                case MUL:
                {
                    $WRITE_OPCDE(imul_rax_rdx);   
                    break;
                }
                case SUB:
                {
                    $WRITE_OPCDE(sub_rax_rbx);
                    break;
                }
                case ADD:
                {
                    $WRITE_OPCDE(add_rax_rbx);
                    break;
                }
                case DIV:
                {
                    $WRITE_OPCDE(xor_rdx_rdx);
                    $WRITE_OPCDE(div_rbx);
                    break;
                }
                case DEG:
                {
                    assert(ERRORR && "Sorry, but exponentiation is not allowed))");
                }
                default:
                {
                    assert(ERRORR && "case operator incorrect input");
                }
            }
            break;
        }
        case VARIABLE://reworked
        {
            int tmp_var_hash = murmurHash($ND_STR, node->data_lng);
            int tmp_var_num  = FindVariable(vr_lists, tmp_var_hash);

            if (tmp_var_num == -1)
            {
                $FREE_VAR.var_hash     = tmp_var_hash;
                $FREE_VAR.var_dx_shift = $FREE;
                tmp_var_num            = $FREE;

                $FREE += 1;
            }

            $WRITE_CONST(tmp_var_num * (-8), int);
            break;
        }
        case CONSTANT://reworked
        {
            int temp = $ND_DBL;
            $WRITE_CONST(temp, int);

            break;
        }
        case DEFINE://done
        {
            $WRITE_OPCDE(jmp);
            int jmp_ip = fdata->ip;
            $WRITE_CONST(fdata->ip, int);

            var_lists vr_lists_new = {};
            vr_lists_new.var = (var_list*) calloc(VAR_MAX_CUNT, sizeof(var_list));
            vr_lists_new.free = 1;

            $VISIT_NEW_LIST($L, &vr_lists_new);


            $WRITE_OPCDE(mov_r11_rbp);
            $WRITE_OPCDE(pop_r12);
            $WRITE_OPCDE(mov_rbp_rsp);

            int arg_num = 1;
            for (int i = vr_lists_new.free - 2 ; i >= 0 ; i--)
            {
                $WRITE_OPCDE(pop_mem_rbp);
                $WRITE_CONST((1 + arg_num)*(-8), int);
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
            
            $WRITE_CONST_IP(fdata->ip - jmp_ip - MAGIC4, int, jmp_ip);

            free(vr_lists_new.var);
            break;
        }
        case FUNCTION://reworked
        {
            $CUR_FUNC.func_hash = murmurHash($L->data.str, $L->data_lng); 
            $CUR_FUNC.func_ip = fdata->ip;
            jtable->tmp_func += 1;

            $VISIT($R);

            break;
        }
        case PARAMETER://norm vrodeby
        {
            int tmp_hash = murmurHash(node->right->data.str, node->right->data_lng);

            $FREE_VAR.var_dx_shift = $FREE;
            $FREE_VAR.var_hash     = tmp_hash;

            $FREE += 1;

            $VISIT($L);

            break;
        }
        case PARAMETER_CALL://done
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
            break;
        }
        case CALL://done
        {
            if (strncmp(node->left->data.str, "scan", 4) == 0)
            { 
                int hash = murmurHash($L->data.str, $L->data_lng);
                int func_ip = FindFunction(jtable, hash);
                if (func_ip == -1)
                    assert(0 && "YOU CAN'T USE FUNCTIONS BEFORE THEY'RE DEFINED!!!");

                $WRITE_OPCDE(call);
                $WRITE_CONST(func_ip - fdata->ip - MAGIC4, int);

                if ($R->right->data_type != VARIABLE)
                    assert(0 && "INCORRECT INPUT, SCAN MUST HAVE VARIABLE AS A PARAMETER!");

                $WRITE_OPCDE(mov_mem_rbp_rax);
                $VISIT($R->right);
            }
            
            else if (strncmp(node->left->data.str, "print", 5) == 0)
            {
                int hash = murmurHash($L->data.str, $L->data_lng);
                int func_ip = FindFunction(jtable, hash);
                if (func_ip == -1)
                    assert(0 && "YOU CAN'T USE FUNCTIONS BEFORE THEY'RE DEFINED!!!");

                if ($R->right->data_type == CONSTANT)          
                    {                                   
                        $WRITE_OPCDE(mov_rax_const);    
                        $VISIT($R->right);                     
                    }

                else if($R->right->data_type == VARIABLE)      
                    {                                   
                        $WRITE_OPCDE(mov_rax_mem_rbp);  
                        $VISIT($R->right);                     
                    }

                $WRITE_OPCDE(call);
                $WRITE_CONST(func_ip - fdata->ip - MAGIC4, int);
            }

            else if (strncmp(node->left->data.str, "sqrt", 4) == 0)
            {
                int hash = murmurHash($L->data.str, $L->data_lng);
                int func_ip = FindFunction(jtable, hash);
                if (func_ip == -1)
                    assert(0 && "YOU CAN'T USE FUNCTIONS BEFORE THEY'RE DEFINED!!!");

                $WRITE_OPCDE(mov_rax_mem_rbp);
                $VISIT($R->right);

                $WRITE_OPCDE(call);
                $WRITE_CONST(func_ip - fdata->ip - MAGIC4, int);
            }
            
            else
            {
                $VISIT($R);

                int hash = murmurHash($L->data.str, $L->data_lng);
                int func_ip = FindFunction(jtable, hash);
                if (func_ip == -1)
                    assert(0 && "YOU CAN'T USE FUNCTIONS BEFORE THEY'RE DEFINED!!!");

                $WRITE_OPCDE(call);
                $WRITE_CONST(func_ip - fdata->ip - MAGIC4, int);//
            }

            break;
        }
        case RETURN://done
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
            break;
        }
        case IF://done
        {
            $VISIT($L);
            $VISIT($R);

            break;
        }
        case DESISION://done
        {
            int rel_jmp_ip = fdata->ip;
            $WRITE_CONST(TRASH, int);
            $VISIT($L);
            $WRITE_OPCDE(jmp);            //jmp на скип else после тру ифа
            int else_skip_jmp = fdata->ip;
            $WRITE_CONST(TRASH, int);
            $WRITE_CONST_IP(fdata->ip - rel_jmp_ip - MAGIC4, int, rel_jmp_ip);

            $VISIT($R);

            $WRITE_CONST_IP(fdata->ip - else_skip_jmp - MAGIC4, int, else_skip_jmp);

            break;
        }
        case REL_OPERATOR:// nothing to do
        {
            $R_N_D_CONST_VISIT
            $R_N_D_VAR_VISIT

            else
                $VISIT($R);

            $L_N_D_CONST_VISIT
            $L_N_D_VAR_VISIT
            
            else
                $VISIT($L);

            $WRITE_OPCDE(cmp_rax_rbx);

            //  Here I use jumps with the opposite condition !!! Стрцмп кринж

            if (strncmp($ND_STR, "==", 2) == 0)
            {
                $WRITE_OPCDE(jne);
            }
            else if (strncmp($ND_STR, "!=", 2) == 0)
            {
                $WRITE_OPCDE(je);
            }
            else if (strncmp($ND_STR, ">=", 2) == 0)
            {
                $WRITE_OPCDE(jb);
            }
            else if (strncmp($ND_STR, "<=", 2) == 0)
            {
                $WRITE_OPCDE(ja);
            }
            else if (strncmp($ND_STR, ">", 1) == 0)
            {
                $WRITE_OPCDE(jbe);
            }
            else if (strncmp($ND_STR, "<", 1) == 0)
            {
                $WRITE_OPCDE(jae);
            }
            else
            {
                assert(0 && "OOooops incorrect relative operator !");
            }
            break;
        }
        case WHILE://done
        {
            int while_strt_ip = fdata->ip;

            $VISIT($L);
            int while_end_ip = fdata->ip;
            $WRITE_CONST(TRASH, int);// trash jmp at the end

            $VISIT($R); // while body
            
            $WRITE_OPCDE(jmp); // jmp at the start

            $WRITE_CONST(while_strt_ip - fdata->ip - MAGIC4, int);

            $WRITE_CONST_IP(fdata->ip - while_end_ip - MAGIC4, int, while_end_ip);
        
            break;
        }
        defaul:
        {
            assert(0 && "Shit happen(s) (undefined case in VisitWriteCommands switch)");
        }
    }// Эссэ "Ночной кодинг))"
    return 0;
}
//==========================================================================
int FindVariable (var_lists* vr_lists, int hash)
{
    for (int i = 0 ; i < VAR_MAX_CUNT ; i++)
    {
        if (vr_lists->var[i].var_hash == hash)
            return i;
    }

    return -1;
}
//==========================================================================
int FindFunction(JMPtable* jtable, int hash)
{
    for(int i = 0 ; i < jtable->tmp_func ; i++)
    {
        if (jtable->func_jmp[i].func_hash == hash)
            return jtable->func_jmp[i].func_ip;
    }
    return -1;
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
    $WRITE_CONST_IP(fdata->ip - jmp_ip - MAGIC4, int, jmp_ip);              \
} while (0);

//==========================================================================
int std_func_add(ELFfile* fdata, JMPtable* jtable)
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