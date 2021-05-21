#pragma once

#include <cstdint>

#undef ABSOLUTE

using Byte = uint8_t;
using Word = uint16_t;

static enum class ADDRESSING_MODES { IMMEDIATE, ABSOLUTE, REGISTRY, REGISTRY_POINTER };
