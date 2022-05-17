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
const unsigned char push_constant[] = {0x68};// need arg as int (4 byte)

// pop
const unsigned char pop_rax[] = {0x58};
const unsigned char pop_rbx[] = {0x5b};
const unsigned char pop_rsp[] = {0x5c};
const unsigned char pop_rbp[] = {0x5d};
const unsigned char pop_r12[] = {0x41, 0x5C};
const unsigned char pop_mem_rbp[] = {0x8F, 0x85};// need arg as int (4 byte)

// mov in [rbp - something]
const unsigned char mov_mem_rbp_rax[] = {0x48, 0x89, 0x85}; //need arg as int (4 byte)
const unsigned char mov_mem_rbp_rbx[] = {0x48, 0x89, 0x9D}; //need arg as int (4 byte)

// mov
const unsigned char mov_r11_rbp[] = {0x49, 0x89, 0xEB};
const unsigned char mov_rbp_rsp[] = {0x48, 0x89, 0xE5};
const unsigned char mov_rsp_rbp[] = {0x48, 0x89, 0xEC};
const unsigned char mov_rbp_r11[] = {0x4C, 0x89, 0xDD};
const unsigned char mov_rax_mem_rbp[] = {0x48, 0x8B, 0x85};// need arg as int (4 byte)
const unsigned char mov_rax_const[] = {0x48, 0xC7, 0xC0};// need arg as int (4 byte)
const unsigned char mov_rbx_const[] = {0x48, 0xC7, 0xC3};// need arg as int (4 byte)
const unsigned char mov_rbx_mem_rbp[] = {0x48, 0x8B, 0x9D};// need arg as int (4 byte)
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
const unsigned char my_exit[] = {0xB8, 0x01, 0x00, 0x00, 0x00, 0xBB, 0x00, 0x00, 0x00, 0x00, 0xCD, 0x80};

//standart functions

const unsigned char my_scan[] = 
{
				0x55, 											// push  rbp
				0x48, 0x89, 0xe5, 								// mov   rbp, rsp
				0x31, 0xc0, 									// xor   eax, eax
				0x31, 0xff, 									// xor   edi, edi
				0x48, 0x8d, 0x75, 0xf8, 						// lea   rsi, [var_8h]
				0xba, 0x08, 0x00, 0x00, 0x00, 					// mov   edx, 8
				0x0f, 0x05, 									// syscall
				0x48, 0x8b, 0x45, 0xf8,							// mov   rax, qword [var_8h]
				0x48, 0x8d, 0x7d, 0xf8,							// lea   rdi, [var_8h]
				0x48, 0x31, 0xc0,								// xor   rax, rax
				0x48, 0x0f, 0xb6, 0x0f,							// movzx rcx, byte [rdi]
				0x48, 0x83, 0xe9, 0x30,							// sub   rcx, 0x30
				0x72, 0x13,										// jb    0x13
				0x48, 0x83, 0xf9, 0x09,							// cmp   rcx, 9
				0x77, 0x0d,										// ja    0x0d
				0x48, 0x8d, 0x04, 0x80,							// lea   rax,[rax + rax*4]
				0x48, 0x8d, 0x04, 0x41,							// lea   rax,[rcx + rax*2]
				0x48, 0xff, 0xc7, 								// inc   rdi
				0xeb, 0xe3, 									// jmp   0xe3
				0x5d,											// pop   rbp
				0xc3											// ret
};
const unsigned char my_print[] = {
 0x55,                                 /* push rbp */
 0x48, 0x89, 0xe5,                     /* mov rbp, rsp */
 0x49, 0x89, 0xc3,                     /* mov r11, rax */
 0x49, 0xd1, 0xe3,                     /* shl r11, 1 */
 0x72, 0x02,                           /* jb loc.draw_minus */
 0xeb, 0x24,                           /* jmp loc.skip_draw_minus */
 0x50,                                 /* push rax */
 0x6a, 0x2d,                           /* push 0x2d */
 0xba, 0x08, 0x00, 0x00, 0x00,         /* mov edx, 8 */
 0xb8, 0x01, 0x00, 0x00, 0x00,         /* mov eax, 1 */
 0xbf, 0x01, 0x00, 0x00, 0x00,         /* mov edi, 1 */
 0x48, 0x89, 0xe6,                     /* mov rsi, rsp */
 0x0f, 0x05,                           /* syscall */
 0x41, 0x5b,                           /* pop r11 */
 0x58,                                 /* pop rax */
 0x49, 0xc7, 0xc3, 0xff, 0xff, 0xff, 0xff,  /* mov r11, 0xffffffffffffffff */
 0x49, 0xf7, 0xe3,                     /* mul r11 */
 0x49, 0x89, 0xc4,                     /* mov r12, rax */
 0x48, 0x31, 0xd2,                     /* xor rdx, rdx */
 0x41, 0xba, 0x64, 0x00, 0x00, 0x00,   /* mov r10d, 0x64 */
 0x49, 0xf7, 0xf2,                     /* div r10 */
 0xe8, 0x78, 0x00, 0x00, 0x00,         /* call loc.dectoi */
 0x50,                                 /* push rax */
 0xba, 0x08, 0x00, 0x00, 0x00,         /* mov edx, 8 */
 0xb8, 0x01, 0x00, 0x00, 0x00,         /* mov eax, 1 */
 0xbf, 0x01, 0x00, 0x00, 0x00,         /* mov edi, 1 */
 0x48, 0x89, 0xe6,                     /* mov rsi, rsp */
 0x0f, 0x05,                           /* syscall */
 0x58,                                 /* pop rax */
 0x6a, 0x2e,                           /* push 0x2e */
 0xba, 0x08, 0x00, 0x00, 0x00,         /* mov edx, 8 */
 0xb8, 0x01, 0x00, 0x00, 0x00,         /* mov eax, 1 */
 0xbf, 0x01, 0x00, 0x00, 0x00,         /* mov edi, 1 */
 0x48, 0x89, 0xe6,                     /* mov rsi, rsp */
 0x0f, 0x05,                           /* syscall */
 0x41, 0x5b,                           /* pop r11 */
 0x4c, 0x89, 0xe0,                     /* mov rax, r12 */
 0x48, 0x31, 0xd2,                     /* xor rdx, rdx */
 0x41, 0xba, 0x64, 0x00, 0x00, 0x00,   /* mov r10d, 0x64 */
 0x49, 0xf7, 0xf2,                     /* div r10 */
 0x48, 0x89, 0xd0,                     /* mov rax, rdx */
 0xe8, 0x33, 0x00, 0x00, 0x00,         /* call loc.dectoi */
 0x50,                                 /* push rax */
 0xba, 0x08, 0x00, 0x00, 0x00,         /* mov edx, 8 */
 0xb8, 0x01, 0x00, 0x00, 0x00,         /* mov eax, 1 */
 0xbf, 0x01, 0x00, 0x00, 0x00,         /* mov edi, 1 */
 0x48, 0x89, 0xe6,                     /* mov rsi, rsp */
 0x0f, 0x05,                           /* syscall */
 0x58,                                 /* pop rax */
 0x6a, 0x0a,                           /* push 0xa */
 0xba, 0x08, 0x00, 0x00, 0x00,         /* mov edx, 8 */
 0xb8, 0x01, 0x00, 0x00, 0x00,         /* mov eax, 1 */
 0xbf, 0x01, 0x00, 0x00, 0x00,         /* mov edi, 1 */
 0x48, 0x89, 0xe6,                     /* mov rsi, rsp */
 0x0f, 0x05,                           /* syscall */
 0x41, 0x5b,                           /* pop r11 */
 0x48, 0x89, 0xec,                     /* mov rsp, rbp */
 0x5d,                                 /* pop rbp */
 0xc3,                                 /* ret */
 0xbb, 0xcd, 0xcc, 0xcc, 0xcc,         /* mov ebx, 0xcccccccd */
 0x48, 0x31, 0xff,                     /* xor rdi, rdi */
 0x89, 0xc1,                           /* mov ecx, eax */
 0xf7, 0xe3,                           /* mul ebx */
 0xc1, 0xea, 0x03,                     /* shr edx, 3 */
 0x89, 0xd0,                           /* mov eax, edx */
 0x67, 0x8d, 0x14, 0x92,               /* lea edx, [edx + edx*4] */
 0x67, 0x8d, 0x54, 0x12, 0xd0,         /* lea edx, [edx + edx - 0x30] */
 0x29, 0xd1,                           /* sub ecx, edx */
 0x48, 0xc1, 0xe7, 0x08,               /* shl rdi, 8 */
 0x48, 0x01, 0xcf,                     /* add rdi, rcx */
 0x48, 0x83, 0xf8, 0x00,               /* cmp rax, 0 */
 0x75, 0xdf,                           /* jne loc.dectoi.loop */
 0x48, 0x89, 0xf8,                     /* mov rax, rdi */
 0xc3,                                 /* ret */
};
const unsigned char my_sqrt[] = 
{
				0x48, 0x0f, 0xbd, 0xc8,							// bsr  rcx, rax
				0x66, 0x81, 0xe1, 0xfe, 0x00,					// and  cx, 0xfe
				0xba, 0x01, 0x00, 0x00, 0x00,					// mov  edx, 0x1
				0x48, 0xd3, 0xe2,								// shl  rdx, cl
				0x48, 0x89, 0xde,								// mov  rsi, rbx
				0x48, 0x01, 0xd6,								// add  rsi, rdx
				0x48, 0x39, 0xc6,								// cmp  rsi, rax
				0x77, 0x0b,										// ja   0x27
				0x48, 0x29, 0xf0,								// sub  rax, rsi
				0x48, 0xd1, 0xeb,								// shr  rbx, 1
				0x48, 0x01, 0xd3,								// add  rbx, rdx
				0xeb, 0x03,										// jmp  0x2a
				0x48, 0xd1, 0xeb,								// shr  rbx, 1
				0x48, 0xc1, 0xea, 0x02,							// shr  rdx, 0x2
				0x75, 0xe1,										// jne  0x11
				0x48, 0x89, 0xd8,								// mov  rax, rbx
				0xc3 											// ret
	};
#endif