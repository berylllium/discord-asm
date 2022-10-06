#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <deque>
#include "definitions.hpp"

namespace asmutils
{
    int get_bits(uint64_t number);

    std::vector<Byte> int_to_little_endian_byte_array(uint64_t value);

    bool valid_hex_string(std::string hex);

    // TODO: Please do something better than this to get register sizes. This will
    // fail when the register map is edited in any way.
    bool compare_register_size(Byte encoding, int minSize);
    int get_register_size(Byte encoding);

    std::vector<std::string> split(std::string text, char delim);
    std::deque<std::string> split_deque(std::string text, char delim);

    template<class K, class V>
    bool map_contains_value(std::map<K, V> map, V value);

    template<class K, class V>
    K map_get_key_from_value(std::map<K, V> map, V value);

    // Errors

    std::string throw_undefined_registry_exception(std::string line, std::string registry);
    std::string throw_undefined_instruction_exception(std::string line, std::string instruction);
    std::string throw_possible_overflow_exception(std::string line, int provided, int expected);
    std::string throw_instruction_exception(std::string line, int provided, int expected);
    std::string throw_hex_string_format_exception(std::string line, std::string provided);
    std::string throw_label_already_defined_exception(std::string line, std::string label, Word address);
    std::string throw_label_not_found_exception(Word address, std::string label);
}
