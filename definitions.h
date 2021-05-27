#pragma once

#include <cstdint>

#undef ABSOLUTE

using Byte = uint8_t;
using Word = uint16_t;

enum class ADDRESSING_MODES { IMPLICIT, IMMEDIATE, ABSOLUTE, REGISTRY, REGISTRY_POINTER };
