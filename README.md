# Compiler

This project is a direct continuation of the language, which translated the program written in my language into the assembler code of my virtual processor.

But this compiler translates code written in my language into X86-64 architectural instructions and creates an executable file based on them.

For example, a program that calculates the factorial of 5 for 1000000 times has become **140** times faster.
## Syntax

+ **Each line must end with a `;` character.**
+ **Programm must end with `$` character.**
    + **Instead of `if`, i have `tortbl`**
    + **Instead of `while`, i have `pinzhak`**
    + **Instead of `else`, i have `lattE`**
    + **Instead of `return`, i have `zvOnit`**
    + **Instead of `=`, i have `lOzhit`**
    + **Instead of `{`, i have `word_stress`**
    + **Instead of `}`, i have `for_weak_people`**

As you can see, this is the language of wrong stresses in words, and just wrong words in Russian.

**But `=`, `{`, `}` can also be used.**

------------------------------------------
+ **Standart function print() have only one(!) variable (or constant) as input and displays its value on the screen**

+ **Standart function scan() have only one(!) variable as input, it takes a integer(!) value from the keyboard and puts it in a variable**

+ **I also have a standard sqrt() function that takes one value (variable or constant) as input and returns its square root.**

**Other parts of syntax are similar with C syntax**

-----------------------------------------------
# Examples of translation for basic constructions

## If

```
tortbl (x == 0)
{
    x = x + 1;
}
lattE
{
    x = 0
}
```
**Translates into**

``` asm

        mov rbx, 0
        mov rax, [rbp - 8]
        cmp rax, rbx
        jne L1                  ; jump to else
        mov rax, 0x64
        mov [rbp - 8], rax
        jmp L2
L1:                             ; jump to skip else
        mov rax, 0
        mov [rbp - 8], rax
L2:
```
**Actually, its translated into this, but i will skip binary version next**
``` asm
48 c7 c3 00 00 00 00      ;mov rbx, 0
48 8b 85 f8 ff ff ff      ;mov rax, [rbp - 8]
48 39 d8                  ;cmp rax, rbx
0f 85 13 00 00 00         ;jne 0x13
48 c7 c0 64 00 00 00      ;mov rax, 0x64
48 89 85 f8 ff ff ff      ;mov [rbp - 8], rax
e9 0e 00 00 00            ;jmp 0x0e
48 c7 c0 00 00 00 00      ;mov rax, 0
48 89 85 f8 ff ff ff      ;mov [rbp - 8], rax
```
## While

```
pinzhak (x >= 0)
{
    x = x - 1;
}
```
**Translates into**

```
L2:
        mov rbx, 0
        mov rax, [rbp - 8]
        cmp rax, rbx
        jl L1
        mov rax, [rbp - 8]
        mov rbx, 0x64
        sub rax, rbx
        mov [rbp - 8], rax
        jmp L2
L1:
```
## Functions

+ **Аll arguments are passed through the stack.**
+ **Functions returns its values in `rax` register.**

**Example of sum(x, y) function**
``` cpp
sum(x, y)
{
    zvOnit x + y;
}
x = 5;
y = 12;

z = sum(x, y);
$
```
``` asm
            ; at the start I have standart functions
            ; You can check code at asm_instructions.h

            push rbp
            mov rbp, rsp
            sub rsp, 0x18
sum:
            jmp skip_sum
            push rbp
            mov rbp, rsp
            sub rsp, 0x10
            mov rax, [rbp + 0x10]       ; rax = x;
            mov rbx, [rbp + 0x18]       ; rbx = y;
            add rax, rbx                ; rax = x + y
            mov rsp, rbp
            pop rbp
            ret
skip_sum:
            mov rax, 0x1f4
            mov [rbp - 8], rax          ; x = 0x1f4 = 0d500
            mov rax, 0x4b0
            mov [rbp - 0x10], rax       ; y = 0x4b0 = 0d1200
            push qword [rbp - 8]
            push qword [rbp - 0x10]
            call sum
            pop r12                     ; pop sum args from stack
            pop r12                     ; pop sum args from stack
            mov [rbp - 0x18], rax
            mov eax, 1  
            mov ebx, 0
            int 0x80                    ;exit
```

## Adition

**As you can see, all values ​​are multiplied by a factor of 100 (in the examples, 0x64 is used instead of 1).**

**This done to have a fractional part of the numbers, since I work with integers.**

**So, if you will try to output 1/3, print() will give you "0.33"**

## Optimisation 

**I also made a backend optimization that simplifies the counting of variables whose value is known before the program is executed**

**For example**
``` cpp
    x = 10;
    y = x + 5;
```
**Before execution, you can say that `x` will be equal to 10 and `y` will be 15.**
**And because of that, asm code of my programm will be:**
``` asm 
          mov rax, 0x3e8
          mov [rbp - 8], rax
          mov rax, 0x5dc
          mov [rbp - 0x10], rax
```
**Instead of**
``` asm
          mov rax, 0x3e8
          mov [rbp - 8], rax
          mov rax, [rbp - 8]
          mov rbx, 0x1f4
          add rax, rbx
          mov [rbp- 0x10], rax

```














