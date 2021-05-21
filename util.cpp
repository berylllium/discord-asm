#include "util.h"

int asmutils::get_bits(uint64_t number)
{
    int i = 0;

    while (number)
    {
        number = number >> 1;
        i++;
    }

    return i;
}

std::vector<Byte> asmutils::int_to_little_endian_byte_array(uint64_t value)
{
    std::vector<Byte> bytes;

    while (value != 0)
    {
        Byte byte = value & 0xFF;
        bytes.push_back(byte);

        value >>= 8;
    }

    return bytes;
}


bool asmutils::valid_hex_string(std::string hex)
{
    return hex.compare(0, 2, "0x") == 0
        && hex.size() > 2
        && hex.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
}

bool asmutils::compare_register_size(Byte encoding, int minSize)
{
    if (minSize <= 8) return true;
    else if (minSize <= 16) return encoding >= 0x08;
    else if (minSize <= 64) return encoding >= 0x10;
}

int asmutils::get_register_size(Byte encoding)
{
    if (encoding <= 0x07) return 8;
    else if (encoding <= 0x0F) return 16;
    else return 64;
}

template<class K, class V>
bool asmutils::map_contains_value(std::map<K, V> map, V value)
{
    for (auto const& x : map)
    {
        if (x.second == value) return true;
    }

    return false;
}

template<class K, class V>
K asmutils::map_get_key_from_value(std::map<K, V> map, V value)
{
    for (auto const& x : map)
    {
        if (x.second == value) return x.first;
    }

    return NULL;
}

std::string asmutils::throw_undefined_registry_exception(std::string line, std::string registry)
{
    std::stringstream ss;
    ss << "Exception while compiling code. At line: `" << line << "`. UndefinedRegistryException: Given registry: `" << registry << "`, was not found.";

    return ss.str();
}

std::string asmutils::throw_undefined_instruction_exception(std::string line, std::string instruction)
{
    std::stringstream ss;
    ss << "Exception while compiling code. At line: `" << line << "`. UndefinedInstructionException: Given instruction: `" << instruction << "`, was not found.";

    return ss.str();
}

std::string asmutils::throw_possible_overflow_exception(std::string line, int provided, int expected)
{
    std::stringstream ss;
    ss << "Exception while compiling code. At line: `" << line << "`. PossibleOverflowException: Max: `" << expected << "` bits, provided: `" << provided << "` bits.";

    return ss.str();
}

std::string asmutils::throw_instruction_exception(std::string line, int provided, int expected)
{
    std::stringstream ss;
    ss << "Exception while compiling code. At line: `" << line << "`. InstructionException: Provided: `" << provided << "` instructions, expected: `" << expected << " instructions`.";

    return ss.str();
}

std::string asmutils::throw_hex_string_format_exception(std::string line, std::string provided)
{
    std::stringstream ss;
    ss << "Exception while compiling code. At line: `" << line << "`. HexStringFormatException: Provided (`" << provided << "`) hex number is not a valid hex number.";

    return ss.str();
}

std::string asmutils::throw_label_already_defined_exception(std::string line, std::string label, Word address)
{
    std::stringstream ss;
    ss << "Exception while compiling code. At line: `" << line << "`. LabelAlreadyProvidedException: Provided label (`" << label << "`) is already defined to point at 0x" << std::hex << std::setw(4) << std::setfill('0') << address << ".";

    return ss.str();
}

std::string asmutils::throw_label_not_found_exception(Word address, std::string label)
{
    std::stringstream ss;
    ss << "Exception while compiling code. At address: `" << std::hex << std::setw(4) << std::setfill('0') << address << "`. LabelNotFoundException: Provided label (`" << label << "`) has not been defined.";

    return ss.str();
}
