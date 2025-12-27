#pragma once

#include <algorithm>
#include <cstdint>
#include <format>
#include <ios>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>

namespace bits_and_bytes {

    enum class Order : uint8_t {
       LittleEndian,
       BigEndian
    };

    enum class Format : uint8_t {
        Binary,
        HexaDecimal,
    };

    enum class HexFormat : uint8_t {
        UpperCase,
        LowerCase,
    };

    enum class BitUnit {
        Nibble,
        Byte,
        None
    };

    [[nodiscard]] inline uint8_t asValue(BitUnit const& bitUnit) {
        return bitUnit == BitUnit::Nibble ? 4 : 8;
    }

    enum class LeadingZeroes : uint8_t {
        Suppress,
        Include,
    };

    unsigned char constexpr NUM_BITS_IN_ONE_BYTE {8U};
    unsigned char constexpr NUM_BITS_IN_ONE_NIBBLE {4U};
    char constexpr DEFAULT_GROUP_DELIMITER {' '};

    struct StringFormat {
        Order order;
        Format format;
        HexFormat hexFormat;
        BitUnit bitUnit;
        LeadingZeroes leadingZeroes;
        char groupDelimiter;
    };

    inline StringFormat constexpr DEFAULT_STRING_FORMAT {
        Order::BigEndian,
        Format::Binary,
        HexFormat::UpperCase,
        BitUnit::Nibble,
        LeadingZeroes::Include,
        DEFAULT_GROUP_DELIMITER
    };

    uint8_t constexpr SIXTEEN {16};
    uint8_t constexpr NINE {9};
    uint8_t constexpr TEN {10};
    uint8_t constexpr TWO {2};
    uint8_t constexpr EIGHT {8};
    uint8_t constexpr SIXTYFOUR {64};
    inline std::regex const HEX_REGEX {"[a-fA-F0-9]{1,16}"};
    inline std::regex const BIN_REGEX {"[0-1]{1,64}"};

    struct BitFormatException final : std::runtime_error {
        explicit BitFormatException(std::string const& message) : std::runtime_error(message) {}
    };

    inline std::string_view trim(std::string_view const bitString) {
        if (bitString.empty()) return {};
        auto const start = bitString.find_first_not_of(' ');
        if (start == std::string_view::npos) return {}; // Special case: Input is all spaces
        auto const end= bitString.find_last_not_of(' ');
        return start <= end ? bitString.substr(start, end - start + 1) : std::string_view{};
    }

    inline std::string normalize(std::string_view const bitString) {
        std::string normalized;
        normalized.reserve(bitString.length());
        bool prvSpace{};
        for (char const c : bitString) {
            if (c == ' ') {
                if (!prvSpace) normalized.push_back(' ');
                prvSpace = true;
            } else {
                normalized.push_back(c);
                prvSpace = false;
            }
        }
        return normalized;
    }

    inline std::string canonicalize(std::string_view bitString, bool const isHex = false) {
        if (isHex) {
            if (!bitString.starts_with("0x")) {
                throw BitFormatException(std::format("{} is not a valid hexadecimal value.", bitString));
            }
            bitString.remove_prefix(TWO); // Remove prefix 0x to retain just the bits
        }
        std::string bits;
        bits.reserve(bitString.size());
        std::ranges::copy_if(bitString, std::back_inserter(bits), [](char const c) { return c != ' '; });
        return bits;
    }

    inline std::string validateHex(std::string_view const hexString) {
        auto const normalized = normalize(trim(hexString));
        auto const bits = canonicalize(normalized, true);
        if ( !std::regex_match(bits,HEX_REGEX)) {
            std::string suffix {bits.length() > SIXTEEN ? " The largest data type supported by this library is 64-bits" : ""};
            throw BitFormatException(
                   std::format("{} is not a valid hexadecimal value.{}", normalized, suffix)
            );
        }
        return bits;
    }

    inline std::string validateBinary(std::string_view binaryString) {
        auto const normalized = normalize(trim(binaryString));
        auto const bits = canonicalize(binaryString);
        if (!std::regex_match(bits, BIN_REGEX)) {
            std::string suffix {bits.length() > SIXTYFOUR ? " The largest data type supported by this library is 64-bits" : ""};
            throw BitFormatException(
                std::format("{} is not a valid binary value.{}", normalized, suffix)
            );
        }
        return bits;
    }

    inline std::string nibbleAsBits(char const hexDigit) {
        uint8_t decimal{};
        if (auto const digit = static_cast<char>(std::tolower(static_cast<unsigned char>(hexDigit))); digit >= '0' && digit <= '9') {
            decimal = digit - '0';
        } else if (digit >= 'a' && digit <= 'f') {
            decimal = digit - 'a' + TEN;
        } else {
            throw BitFormatException(std::format("{} is not a valid hexadecimal digit", hexDigit));
        }
        std::string nibbleString(NUM_BITS_IN_ONE_NIBBLE, '0');
        for (auto& c : std::ranges::reverse_view(nibbleString)) {
            c = decimal & 1 ? '1' : '0';
            decimal >>= 1;
            if (!decimal) break;
        }
        return nibbleString;
    }

    inline std::string convertHexStringToBinaryString(std::string_view const hexString) {
        auto const canonicalBitString = validateHex(hexString);
        std::string binaryString;
        binaryString.reserve(canonicalBitString.length() * NUM_BITS_IN_ONE_NIBBLE);
        for (auto const hexDigit : canonicalBitString) {
            binaryString += nibbleAsBits(hexDigit);
        }
        return binaryString;
    }

    template<typename NumericType>
    [[nodiscard]] std::string zeroExtend(std::string_view const bitString) {
        std::string canonicalBitString;
        if (bitString.starts_with("0x")) {
            canonicalBitString = convertHexStringToBinaryString(bitString);
        } else {
            canonicalBitString = validateBinary(bitString);
        }
        if (auto constexpr maxBits = sizeof(NumericType) * EIGHT; canonicalBitString.length() < maxBits) {
            std::string zeroExtended(maxBits, '0');
            for (uint8_t i = 0, len = canonicalBitString.length(), j = maxBits - len; i < len; ++i, ++j) {
                zeroExtended[j] = canonicalBitString[i];
            }
            return zeroExtended;
        }
        return canonicalBitString;
    }

}