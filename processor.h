#pragma once

#include <string>
#include <map>
#include "definitions.h"
#include "memory.h"
#include "instruction_base.h"

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
    static constexpr Byte POINTER_INDICATOR = 0xF8;

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
        SP.value--;
        Byte toReturn = memory[SP.value];
        memory[SP.value] = 0x00;
        return toReturn;
    }

    void push_stack(Byte toWrite, MEM& memory)
    {
        memory[SP.value++] = toWrite;
    }

    uint64_t get_register_value(Byte encoding);
    void set_register_value(Byte encoding, uint64_t value);
};