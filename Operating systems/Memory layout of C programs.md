# Memory layout of c programs

Typical memory layout of the C program is below, 

1. Text section (executeable instruction)
2. initialized data (Static and global variables)
3. Uninitialized data (BSS)
4. Heap (Dynamically allocted memory)
5. Stack 

![image](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/2a5f6c21-b90a-41c9-ba2f-28f6f3079bbb)

## TEXT SEGMENT:
Text segment is knows as code segment or Text which contains the executable instructions. Its shareable.
single copy is enough. Mostly its read only. Its kept at the bottom to avoid stack or heap overflow causes overwrite.

## DATA SEGMENT:
Data segment has two sections. 
**1. Initialized data segment :**
This section contains the initialized static and global data variables.
Example, 
static int = 10;
char* name ="ABCD";

**2. Unitialized data segment :**
This section contains unitialized static and global data variables. Its called as bss (block started by symbols).
The data in this segment initilaized kernel as 0.
Example, 
static int i;
int j;

## HEAP :
This section contains dynamically allocated memory. It grows in the upward direction. 
The Heap area is shared by all shared libraries and dynamically loaded modules in a process.

## STACK:
This stack section contains program stack , LIFO structure which is usually located in the higher part of the memory.
**stack pointer** register tracks the top of the stack. This section contains local variables, return address, function agruments.
