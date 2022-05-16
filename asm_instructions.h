#ifndef ASM_INSTR_H_INC
#define ASM_INSTR_H_INC

#define $ARRAY_SIZE(array) (sizeof(array)/sizeof((array)[0]))

#define $WRITE_OPCDE(opcodes)					            \
	do                                                      \
    {							                            \
		for (size_t i = 0; i != $ARRAY_SIZE(opcodes) ; i++) \
			fdata->code[fdata->ip++] = opcodes[i];          \
	} while(0)

#define $WRITE_CONST(const, type)				            \
	do                                              		\
    {						                        		\
		*(type*)(fdata->code + fdata->ip) = const;    		\
		fdata->ip += sizeof(type);		            		\
	} while(0);

#define $WRITE_CONST_IP(const, type, ip)					\
	do														\
	{														\
		*(type*)(fdata->code + ip) = const;					\
	} while (0);											\

// push regs
const unsigned char push_rax[] = {0x50};
const unsigned char push_rsp[] = {0x54};
const unsigned char push_rbp[] = {0x55};
const unsigned char push_mem_rbp[] = {0xFF, 0xB5}; // need arg as int (4 byte)
const unsigned char push_r12[] = {0x41, 0x54};
const unsigned char push_r11[] = {0x41, 0x53};
// push constant
const unsigned char push_constant[] = {0x68};

// pop
const unsigned char pop_rax[] = {0x58};
const unsigned char pop_rbx[] = {0x5b};
const unsigned char pop_rsp[] = {0x5c};
const unsigned char pop_rbp[] = {0x5d};
const unsigned char pop_r12[] = {0x41, 0x5C};
const unsigned char pop_mem_rbp[] = {0x8F, 0x85};

// mov in [rbp - something]
const unsigned char mov_mem_rbp_rax[] = {0x48, 0x89, 0x85}; //need arg as int (4 byte)
const unsigned char mov_mem_rbp_rbx[] = {0x48, 0x89, 0x9D}; //need arg as int (4 byte)

// mov
const unsigned char mov_r11_rbp[] = {0x49, 0x89, 0xEB};
const unsigned char mov_rbp_rsp[] = {0x48, 0x89, 0xE5};
const unsigned char mov_rsp_rbp[] = {0x48, 0x89, 0xEC};
const unsigned char mov_rbp_r11[] = {0x4C, 0x89, 0xDD};
const unsigned char mov_rax_mem_rbp[] = {0x48, 0x8B, 0x85};
const unsigned char mov_rax_const[] = {0x48, 0xC7, 0xC0};
const unsigned char mov_rbx_const[] = {0x48, 0xC7, 0xC3};
const unsigned char mov_rbx_mem_rbp[] = {0x48, 0x8B, 0x9D};
const unsigned char mov_rbx_rax[] = {0x48, 0x89, 0xC3};

// jmp
const unsigned char jmp[] = {0xe9};//need arg as int (4 byte) (all jmps)
const unsigned char je[]  = {0x0f, 0x84};
const unsigned char jne[] = {0x0f, 0x85};
const unsigned char jae[] = {0x0f, 0x83};
const unsigned char jbe[] = {0x0f, 0x86};
const unsigned char ja[]  = {0x0f, 0x87};
const unsigned char jb[]  = {0x0f, 0x82};

// sub
const unsigned char sub_rsp_constant[] = {0x48, 0x81, 0xEC};//need arg as int (4 byte) (all jmps)
const unsigned char sub_rax_rbx[] = {0x48, 0x29, 0xD8};
const unsigned char sub_rsp[] = {0x48, 0x81, 0xec};//need arg as int (4 byte)

// add
const unsigned char add_rax_rbx[] = {0x48, 0x01, 0xD8};
const unsigned char add_rsp[] = {0x48, 0x81, 0xC4};//need arg as int (4 byte)

// mul
const unsigned char imul_rax_rdx[] = {0x48, 0x0F, 0xAF, 0xC3};

//div
const unsigned char div_rbx[] = {0x48, 0xF7, 0xF3};

// spesial
const unsigned char call[] = {0xe8};
const unsigned char ret[] = {0xC3};
const unsigned char xor_rdx_rdx[] = {0x48, 0x31, 0xD2};
const unsigned char cmp_rax_rbx[] = {0x48, 0x39, 0xD8};
const unsigned char nop[] = {0x90};


#endif