# Discord-asm
## _Discord assembly bot_

Tired of restrictive bot commands? Want to have a command for every one of your needs? Unlimited freedom? Look no further! With discord-asm you can create any command straight from the discord text bar! Sounds too good to be true? Well, its true! All you need is a good understanding of low level CPU architecture and a pretty good understanding of NASM-like assembly and you're good to go!

## What is discord-asm?
On a more serious note, discord-asm is a bot that interprets user written assembly code and executes it in things called "environments". Environmetns have access to the servers ins-and-outs. Like, creating channels, kicking users, removing messages in a given channel, sending messages in a given channel, banning users, creating roles, adding roles to members, etc. Basically, anything a regular old bot can do. The server owner can configure environments to allow or disallow things users can access inside an environment.

> Note that many features are currently not implemented. Check [Instructions](#instructions) for all of the currently implemented instructions.

## Examples
Hello World
```x86asm
&
mov si, STR
call printf
jmp EXIT

printf:
  str_loop:
    mov al, [si]
    cmp al, 0x00
    jne print_char
    ret

  print_char:
    prtc [si]
    add si, 0x01
    jmp str_loop

STR: db "Hello", 0x20, "World!!!!", 0x00

EXIT:
```

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

### Flags
The CPU uses flags for logical operatios. These are the currently implemented flags.

| Flag | Description                                    |
|------|------------------------------------------------|
| CF   | Set during by cmp, used by logical operations. |

## Instructions

> Coming soon

# Developers

## Formats
### UserSettings

List is in order of elements:
1. User ID (u64)
2. dumpMemory (bool)
3. dumpFull (bool)

Optionally the user is also able to preload registers in the environment (for use as parameters for example). This is encoded by first appending the number of registers affected and then all the affected registers after eachother in this format: `[registerIdentifier] [preloadedValue]`.

For example: `93472039409823709 0 1 2 2 64 11 600`.
