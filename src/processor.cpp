#include "processor.hpp"

//static constexpr Byte // Op Codes
//INS_MOV_AS = 0x01,
//INS_MOV_IM = 0x02,
//INS_CMP_IM = 0x03,
//INS_JNE = 0x04,
//INS_PUSHA = 0x05,
//INS_POPA = 0x06,
//INS_JMP = 0x07,
//INS_CALL = 0x08,
//INS_RET = 0x09,
//INS_PRTC_IM = 0x0A,
//INS_ADD_IM = 0x0B,
//INS_MOV_RV = 0x0C,
//INS_PRTC_RV = 0x0D,
//// Pseudo Ops
//INS_DB = 0xF0;

std::map<std::string, std::map<ADDRESSING_MODES, Byte>> CPU::instructionOpCodes
{
    {
        "mov", 
        { 
            { ADDRESSING_MODES::ABSOLUTE, 0x01 },
            { ADDRESSING_MODES::IMMEDIATE, 0x02 },
            { ADDRESSING_MODES::REGISTRY, 0x03 },
            { ADDRESSING_MODES::REGISTRY_POINTER, 0x04 }
        }
    },
    {
        "cmp",
        {
            { ADDRESSING_MODES::IMMEDIATE, 0x05 }
        }
    },
    {
        "jne",
        {
            { ADDRESSING_MODES::ABSOLUTE, 0x06 }
        }
    },
    {
        "pusha",
        {
            { ADDRESSING_MODES::IMPLICIT, 0x07 }
        }
    },
    {
        "popa",
        {
            { ADDRESSING_MODES::IMPLICIT, 0x08 }
        }
    },
    {
        "jmp",
        {
            { ADDRESSING_MODES::ABSOLUTE, 0x09 }
        }
    },
    {
        "call",
        {
            { ADDRESSING_MODES::ABSOLUTE, 0x0B }
        }
    },
    {
        "ret",
        {
            { ADDRESSING_MODES::IMPLICIT, 0x0C }
        }
    },
    {
        "prtc",
        {
            { ADDRESSING_MODES::IMMEDIATE, 0x0D },
            { ADDRESSING_MODES::REGISTRY, 0x0E },
            { ADDRESSING_MODES::REGISTRY_POINTER, 0x0A }
        }
    },
    {
        "add",
        {
            { ADDRESSING_MODES::IMMEDIATE, 0x0F },
            { ADDRESSING_MODES::REGISTRY, 0x10 }
        }
    },
    {
        "prtn",
        {
            { ADDRESSING_MODES::IMMEDIATE, 0x11 },
            { ADDRESSING_MODES::REGISTRY, 0x12 },
            { ADDRESSING_MODES::REGISTRY_POINTER, 0x13 }
        }
    }
};

std::map<std::string, Byte> CPU::registerEncoding
{
    { "al", 0x00 }, { "bl", 0x01 }, { "cl", 0x02 }, { "dl", 0x03 }, { "ah", 0x04 }, { "bh", 0x05 },
    { "ch", 0x06 }, { "dh", 0x07 }, // 8-bit registers

    { "ax", 0x08 }, { "bx", 0x09 }, { "cx", 0x0A }, { "dx", 0x0B }, { "si", 0x0C }, { "di", 0x0D },
    { "sp", 0x0E }, { "bp", 0x0F }, // 16-bit registers

    { "r8", 0x10 }, { "r9", 0x11 }, { "r10", 0x12 }, { "r11", 0x13 }, { "r12", 0x14 },
    { "r13", 0x15 }, { "r14", 0x16 }, { "r15", 0x17 } // 64-bit registers
}; // I check for sizes with these assigned encoding values in the assembler. 
   // THIS IS VERY ERROR PRONE IF I CHANGE THIS MAP IN ANY WAY.

uint64_t CPU::get_register_value(Byte encoding)
{
    switch (encoding)
    {
    case 0x00: return AX.lower();
    case 0x01: return BX.lower();
    case 0x02: return CX.lower();
    case 0x03: return DX.lower();
    case 0x04: return AX.upper();
    case 0x05: return BX.upper();
    case 0x06: return CX.upper();
    case 0x07: return DX.upper();

    case 0x08: return AX.value;
    case 0x09: return BX.value;
    case 0x0A: return CX.value;
    case 0x0B: return DX.value;
    case 0x0C: return SI.value;
    case 0x0D: return DI.value;
    case 0x0E: return SP.value;
    case 0x0F: return BP.value;

    case 0x10: return R8;
    case 0x11: return R9;
    case 0x12: return R10;
    case 0x13: return R11;
    case 0x14: return R12;
    case 0x15: return R13;
    case 0x16: return R14;
    case 0x17: return R15;
    }
    return 0;
}

void CPU::set_register_value(Byte encoding, uint64_t value)
{
    switch (encoding)
    {
    case 0x00: AX.set_lower(value); break;
    case 0x01: BX.set_lower(value); break;
    case 0x02: CX.set_lower(value); break;
    case 0x03: DX.set_lower(value); break;
    case 0x04: AX.set_upper(value); break;
    case 0x05: BX.set_upper(value); break;
    case 0x06: CX.set_upper(value); break;
    case 0x07: DX.set_upper(value); break;

    case 0x08: AX.value = value; break;
    case 0x09: BX.value = value; break;
    case 0x0A: CX.value = value; break;
    case 0x0B: DX.value = value; break;
    case 0x0C: SI.value = value; break;
    case 0x0D: DI.value = value; break;
    case 0x0E: SP.value = value; break;
    case 0x0F: BP.value = value; break;

    case 0x10: R8 = value; break;
    case 0x11: R9 = value; break;
    case 0x12: R10 = value; break;
    case 0x13: R11 = value; break;
    case 0x14: R12 = value; break;
    case 0x15: R13 = value; break;
    case 0x16: R14 = value; break;
    case 0x17: R15 = value; break;
    }
}
