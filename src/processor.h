#pragma once

#include <string>
#include <map>
#include "definitions.h"
#include "mymemory.h"
#include "instruction_base.h"

#pragma warning(disable : 4333)

struct Register
{
    Word value;

    Register()
    {
        value = 0;
    }

    Byte lower()
    {
        return (Byte)value & 0x00FF;
    }

    Byte upper()
    {
        return (Byte)value >> 8;
    }

    void set_lower(Byte lower)
    {
        value = (value & 0xFF00) | lower;
    }

    void set_upper(Byte upper)
    {
        value = (value & 0x00FF) | (upper << 8);
    }
};

struct CPU
{
public:

    Word PC; // Program Counter

    Register AX, BX, CX, DX, SI, DI, SP,
        BP;

    uint64_t R8, R9, R10, R11, R12, R13,
        R14, R15;

    Byte CF : 1;

public:

    static std::map<std::string, std::map<ADDRESSING_MODES, Byte>> instructionOpCodes;
    static std::map<std::string, Byte> registerEncoding;

    CPU()
    {
        reset();
    }

    ~CPU()
    {

    }

    void reset()
    {
        PC = 0x0200;
        SP.value = 0x0100;
        R8 = R9 = R10 = R11 = R12 = R13 = R14 = R15 = 0;
        CF = 0;
    }

    Byte read_byte(MEM& memory)
    {
        return memory[PC++];
    }

    void write_byte(Byte toWrite, MEM& memory)
    {
        memory[PC++] = toWrite;
    }

    Byte pull_stack(MEM& memory)
    {
        Byte toReturn = memory[SP.value];
        memory[--SP.value] = 0x00;
        return toReturn;
    }

    Word pull_stack_word(MEM& memory)
    {
        Byte a = memory[SP.value];
        Byte b = memory[SP.value];
        memory[--SP.value] = 0x00;
        memory[--SP.value] = 0x00;

        return a | (b << 8);
    }

    void push_stack(Byte toWrite, MEM& memory)
    {
        memory[SP.value++] = toWrite;
    }

    void push_stack(Word toWrite, MEM& memory)
    {
        memory[SP.value++] = toWrite & 0x00FF;
        memory[SP.value++] = toWrite >> 8;
    }

    uint64_t get_register_value(Byte encoding);
    void set_register_value(Byte encoding, uint64_t value);
};