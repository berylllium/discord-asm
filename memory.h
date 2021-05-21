#pragma once

#include "definitions.h"

struct MEM
{
    static constexpr Word MAX_MEM = 0xFFFF;
    Byte* data = new Byte[MAX_MEM];

    MEM()
    {
        for (Word i = 0; i < MAX_MEM; i++)
        {
            data[i] = 0;
        }
    }

    ~MEM()
    {
        delete[] data;
    }

    Byte operator[](Word address) const // Read byte
    {
        if (address < MAX_MEM) return data[address];
        //else throw std::overflow_error("tried indexing address greater than 0xFFFF");
    }

    Byte& operator[](Word address) // Write byte
    {
        if (address < MAX_MEM) return data[address];
        //else throw std::overflow_error("tried indexing address greater than 0xFFFF");
    }
};
