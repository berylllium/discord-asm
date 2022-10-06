# discord-asm

## _**Discord assembly** bot_

Tired of restrictive bot commands? Want to have a command for every one of your needs? Unlimited freedom? Look no further! With **discord-asm** you can create any command straight from the discord text bar! Sounds too good to be true? Well, its true! All you need is a good understanding of low level CPU architecture and a pretty good understanding of NASM-like assembly and you're good to go!

## Table of Contents
1. [What is discord-asm?](#what-is-discord-asm)
2. [Examples](#examples)
3. [Environments](#environments)
4. [Instructions](#instructions)
5. [Developers](#developers)


## What is discord-asm?
On a more serious note, discord-asm, short for discord assembly, is a bot that interprets user written assembly code and executes it in objects called "environments". Environments have access to the servers ins-and-outs. Like, creating channels, kicking users, removing messages in a given channel, sending messages in a given channel, banning users, creating roles, adding roles to members, etc. Basically, anything a regular old bot can do. The server owner can configure environments to allow or disallow things users can access inside an environment.

> Note that many features are currently not implemented. Check [Instructions](#instructions) for all of the currently implemented instructions.

## Examples
### Hello World
```x86asm
&
mov bx, STR
call printf
jmp EXIT

printf:             
  str_loop:
    mov al, [bx]
    cmp al, 0x00
    jne print_char
    ret

  print_char:
    prtc [bx]
    add bx, 0x01
    jmp str_loop

STR: db "Hello", 0x20, "World!!!!", 0x00

EXIT:
```
In this example we make a program with a function which can be called to with the `call` instruction. Before this function can be called, the `bx` register must be populated with the address in memory of the first character of the string. The string must also be zero terminated.

Let's first look at the `STR` label. As you may know, labels define memory addresses in text form. These labels can be jumped to or can be moved into registers in the form of memory addresses. This label defines the memory address of the first byte of memory of the hello world string; this is because the pseudeo-instruction `db` is used right after it. `db` writes literals into memory: the string literal "Hello" gets chopped into its constituent char-bytes and gets stored in memory. Next up the byte-literal `0x20` gets stored into memory, `0x20` represents the space character in UTF8. As we can see, the string is terminated with a `0x00` byte literal.

### Fibonacci Sequence

```x86asm
&
mov dx 0x0A   # Use ten iterations
mov ax 0x00
mov bx 0x01

LOOP:
  mov cx 0x00
  add cx ax
  add cx bx
  
  prtn cx
  prtc 0x20  # Print a space
  
  mov ax bx
  mov bx cx

  dec dx
  cmp dx 0x00
  jne LOOP
```

The code above prints out the Fibonacci sequence. The Fibonacci sequence is a sequence where each number is the addition of the two previous numbers; thus: `0 1 2 3 5 8 13 21 34 55 89 ... etc`.

## Environments
You can imagine environments as a standalone CPU. Each time the user sends assembly code, a new environment gets created and populated with the given information.
### Architecture
#### Memory
Each environment has its own memory with a 16-bit addressing-bus, meaning 65,5 kb of addressable memory. Overflow will result in immediate failure of the environment (unlike real CPUs). The program counter (from now on referred to as PC) starts at memory address 0x0200, this is also where all the bytecode gets assembled to during assembly. The stack starts from 0x0000, meaning that you have access to  512 bytes of stack memory. Furthermore, the CPU is [little-endian](https://en.wikipedia.org/wiki/Endianness).

### Registers
The CPU has 4 16-bit general purpose registers, each having their own "pseudo-register" accessing their lower and higher bytes respectively.

| Register | Bits | Lower Byte Register | Higher Byte Register |
|----------|:----:|---------------------|----------------------|
| AX       | 16   | AL                  | AH                   |
| BX       | 16   | BL                  | BH                   |
| CX       | 16   | CL                  | CH                   |
| DX       | 16   | DL                  | DH                   |

There are also 4 more 16-bit task specific registers, but these can be used as general purpose registers aswell, be sure to push and pop them to the stack before you use them though!

| Register | Bits | Description                           |
|----------|:----:|---------------------------------------|
| SI       | 16   | Source index                          |
| DI       | 16   | Destination index                     |
| SP       | 16   | Stack pointer                         |
| BP       | 16   | Base pointer (meant for stack frames) |

And last but not least, the 64-bit registers. 

| Register | Bits |
|----------|:----:|
| R8       | 64   |
| R9       | 64   |
| R10      | 64   |
| R11      | 64   |
| R12      | 64   |
| R13      | 64   |
| R14      | 64   |
| R15      | 64   |
> Be aware that the 64-bit registers are not affected by [pusha]() and [popa](). There are 8 64-bit registers, meaning 512 bytes among all of them. The stack is 512 bytes too.

### Register Encoding
|  AL  |  BL  |  CL  |  DL  |  AH  |  BH  |  CH  |  DH  |
|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
| 0x00 | 0x01 | 0x02 | 0x03 | 0x04 | 0x05 | 0x06 | 0x07 |

|  AX  |  BX  |  CX  |  DX  |  SI  |  DI  |  SP  |  BP  |
|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
| 0x08 | 0x09 | 0x0A | 0x0B | 0x0C | 0x0D | 0x0E | 0x0F |

|  R8  |  R9  |  R10 |  R11 |  R12 |  R13 |  R14 |  R15 |
|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
| 0x10 | 0x11 | 0x12 | 0x13 | 0x14 | 0x15 | 0x16 | 0x17 |

### Flags
The CPU uses flags for logical operatios. These are the currently implemented flags.

| Flag | Description                                    |
|------|------------------------------------------------|
| CF   | Set during by cmp, used by logical operations. |


## Instructions

|               |               |             |             |             |             |             |               |
|:-------------:|:-------------:|:-----------:|:-----------:|:-----------:|:-----------:|:-----------:|:-------------:|
|  [add](#add)  | [call](#call) | [cmp](#cmp) | [jmp](#jmp) | [jne](#jne) | [jeq](#jeq) | [mov](#mov) | [prtc](#prtc) |
| [prtn](#prtn) |  [ret](#ret)  | [dec](#dec) | [inc](#inc) |             |             |             |               |


### add

Adds the first second argument to the first argument. The first argument has to be a register, the second can be another register, a register pointer or a numeric literal.

Example: `add ax 0x32`

### call
First saves the current value of the program counter to register `di`, then jumps to the given label. Used in conjunction with the [ret](#ret) instruction.

Example: `call main`

### cmp

Compares the first argument with the second. Both arguments can be registers, register pointers, or numeric literals. If compared values are the same, the `CF` flag is set to 1. Used in conjunction with the [jne](#jne) and [jeq](#jeq) instructions.

Example: `cmp ax 0x01`

### jmp

Jumps to the provided label.

Example: `jmp main`

### jne

Jumps to the provided label if the `CF` flag is set to 0. 

Example: `jne main`

### jeq

Jumps to the provided label if the `CF` flag is set to 1.

Example: `jeq main`

### mov

Moves* the value of the second argument to the first argument. The first argument can only be a register, the second can be a register, register pointer or a numeric literal.

Example: `mov ax bx`

> *: Movement implies the value in the second argument gets set to zero, this is however not the case. The value gets copied. 

### prtc

Prints a character. The character gets written to the console buffer, and after the environment has finished running, gets dumped to the discord channel the environment was created in. The characters are encoded in UTF8 encoding, and can only be one byte in size.

Example: `prtc [ax]`

### prtn

Prints a number. The number gets written to the console buffer, and after the environment has finished running, gets dumped to the discord channel the environment was created in.

Example: `prtn ax`

### ret

Returns to the address within the `di` register, which can be manually set or via the [call](#call) instruction. Does not take any arguments.

Example: `ret`

> **Wanrning:** if `call` hasn't been called before, or if the `di` register has not been set manually before calling `ret`, the program counter will be set to 0, which is the beginning of the stack. This will result in a crash.

### dec

Decreases the value of the target operand by one.

Example: `dec ax`

### inc

Increases the value of the target operand by one.

Example: `inc ax`

# Developers

## Formats
### UserSettings

List is in order of elements:
1. User ID (u64)
2. dumpMemory (bool)
3. dumpFull (bool)

Optionally the user is also able to preload registers in the environment (for use as parameters for example). This is encoded by first appending the number of registers affected and then all the affected registers after eachother in this format: `[registerIdentifier] [preloadedValue]`.

For example: `93472039409823709 0 1 2 2 64 11 600`.