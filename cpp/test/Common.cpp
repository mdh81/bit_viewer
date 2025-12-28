#include "Common.h"
#include "gtest/gtest.h"

#include <ranges>
#include <array>
#include <string>

namespace bb = bits_and_bytes;

namespace {
    std::array constexpr HEX_MAP {
        "0000",
        "0001",
        "0010",
        "0011",
        "0100",
        "0101",
        "0110",
        "0111",
        "1000",
        "1001",
        "1010",
        "1011",
        "1100",
        "1101",
        "1110",
        "1111"
    };
}

TEST(Common, WillTrimStringsCorrectly) {
    ASSERT_EQ("100", bb::trim("    100  "));
    ASSERT_EQ("0xAF", bb::trim(" 0xAF"));
    ASSERT_EQ("0xAF", bb::trim("0xAF     "));
    ASSERT_EQ("0x3D", bb::trim("0x3D"));
    ASSERT_EQ("", bb::trim(""));
    ASSERT_EQ("", bb::trim("    "));
    ASSERT_EQ("1", bb::trim("1"));
}

TEST(Common, WillNormalizeStringsCorrectly) {
    ASSERT_EQ(" 1 0 0 ", bb::normalize("  1  0 0 "));
    ASSERT_EQ("0xAF", bb::normalize("0xAF"));
    ASSERT_EQ("0x AF AF ", bb::normalize("0x  AF  AF  "));
}

TEST(Common, WillCanonicalizeHexStringsCorrectly) {
    ASSERT_EQ("100", bb::canonicalize("  1  0 0 "));
    ASSERT_EQ("AF", bb::canonicalize("0xAF", true));
    ASSERT_EQ("AFAF", bb::canonicalize("0x  AF  AF ", true));
}

TEST(Common, WillValidateHexadecimalStringsCorrectly) {
    ASSERT_NO_THROW(bb::validateHex("0xAF"));
    ASSERT_NO_THROW(bb::validateHex("0xAF AF"));
    ASSERT_THROW(
        try {
            bb::validateHex("0xA3 YZ");
        } catch (bb::BitFormatException& ex) {
            ASSERT_STREQ("0xA3 YZ is not a valid hexadecimal value.", ex.what());
            throw;
        }, bb::BitFormatException);
    ASSERT_NO_THROW(bb::validateHex("0x FFFF FFFF FFFF FFFF")); // NOLINT: Spelling ignored
    ASSERT_THROW(
        try {
            bb::validateHex("0x AA30 FFFF FFFF FFFF FFFF"); // NOLINT: Spelling ignored
        } catch (bb::BitFormatException& ex) {
            ASSERT_STREQ("0x AA30 FFFF FFFF FFFF FFFF is not a valid hexadecimal value. The largest data type supported " // NOLINT: Spelling ignored
                "by this library is 64-bits", ex.what());
            throw;
        }, bb::BitFormatException);
    ASSERT_THROW(
        try {
            bb::validateHex("1111 1100 1001");
        } catch (bb::BitFormatException& ex) {
            ASSERT_STREQ("1111 1100 1001 is not a valid hexadecimal value.", ex.what());
            throw;
        }, bb::BitFormatException);
}

TEST(Common, WillValidateBinaryStringsCorrectly) {
    ASSERT_NO_THROW(bb::canonicalizeBinaryString("10101111"));
    ASSERT_NO_THROW(bb::canonicalizeBinaryString("10101111 10101111"));
    ASSERT_THROW(
        try {
            bb::canonicalizeBinaryString("0A01111000");
        } catch (bb::BitFormatException& ex) {
            ASSERT_STREQ("0A01111000 is not a valid binary value.", ex.what());
            throw;
        }, bb::BitFormatException);
    ASSERT_NO_THROW(bb::canonicalizeBinaryString(" 1111 1111 1111 1000 1101 0001 1000 0101 1111 1111 1111 1111 1111 1111 1000 0001 "));
    ASSERT_THROW(
        std::string binStr(65, '1');
        try {
            bb::canonicalizeBinaryString(binStr);
        } catch (bb::BitFormatException& ex) {
            ASSERT_EQ(std::format("{} is not a valid binary value. The largest data type supported " // NOLINT: Spelling ignored
                "by this library is 64-bits", binStr), ex.what());
            throw;
        }, bb::BitFormatException);
}

TEST(Common, WillConvertNibbleToBitsCorrectly) {
    for (auto const hexDigit : std::ranges::iota_view('0', static_cast<char>('9' + 1))) {
        ASSERT_EQ(HEX_MAP.at(hexDigit - '0'), bb::nibbleAsBits(hexDigit));
    }
    for (auto const hexDigit : std::ranges::iota_view('a', static_cast<char>('f' + 1))) {
        ASSERT_EQ(HEX_MAP.at(hexDigit - 'a' + bb::TEN), bb::nibbleAsBits(hexDigit));
    }
    for (auto const hexDigit : std::ranges::iota_view('A', static_cast<char>('F' + 1))) {
        ASSERT_EQ(HEX_MAP.at(hexDigit - 'A' + bb::TEN), bb::nibbleAsBits(hexDigit));
    }
}

TEST(Common, WillConvertHexadecimalToBinaryCorrectly) {
    ASSERT_EQ("0000", bb::convertHexToCanonicalBinaryString("0x0"));
    ASSERT_EQ("1000", bb::convertHexToCanonicalBinaryString("0x8"));
    ASSERT_EQ("1010", bb::convertHexToCanonicalBinaryString("0xA"));
    ASSERT_EQ("11111010", bb::convertHexToCanonicalBinaryString("0xFA"));
    ASSERT_EQ(std::string(16, '1'), bb::convertHexToCanonicalBinaryString("0xFFFF"));
    //TODO: Negative tests
}

TEST(Common, WillZeroExtendCorrectly) {
    ASSERT_EQ("00000000", bb::zeroExtend<uint8_t>("0"));
    ASSERT_EQ("00000101", bb::zeroExtend<uint8_t>("101"));
    ASSERT_EQ("00001000", bb::zeroExtend<uint8_t>("1000"));
    ASSERT_EQ("00011000", bb::zeroExtend<int8_t>("11000"));
    ASSERT_EQ("00011000", bb::zeroExtend<int8_t>("00011000"));
    ASSERT_EQ("10101111111100000000000000000000000000010001", bb::zeroExtend<int32_t>("0xAFF 0000 0011"));
}

TEST(Common, WillConvertNibbleToHexDigitCorrectly) {
    ASSERT_EQ('A', bb::asHexDigit("1010"));
    ASSERT_EQ('F', bb::asHexDigit("1111"));
    ASSERT_EQ('0', bb::asHexDigit("0000"));
    ASSERT_EQ('9', bb::asHexDigit("1001"));
    ASSERT_THROW(
        try {
            auto ret [[maybe_unused]] = bb::asHexDigit("0");
        } catch (...) {
            throw;
        }, bb::BitFormatException
    );
    ASSERT_THROW(
        try {
            auto ret [[maybe_unused]] = bb::asHexDigit("");
        } catch (...) {
            throw;
        }, bb::BitFormatException
    );
    ASSERT_THROW(
        try {
            auto ret [[maybe_unused]] = bb::asHexDigit("ABC");
        } catch (...) {
            throw;
        }, bb::BitFormatException
    );
    ASSERT_THROW(
        try {
            auto ret [[maybe_unused]] = bb::asHexDigit("11111");
        } catch (...) {
            throw;
        }, bb::BitFormatException
    );
}

TEST(Common, WillConvertBinaryToHexString) {
    ASSERT_EQ("0x00", bb::convertBinaryToHexString("0000 0000"));
    ASSERT_EQ("0x5", bb::convertBinaryToHexString("0101"));
    ASSERT_THROW(
        try {
            auto ret [[maybe_unused]] = bb::convertBinaryToHexString("00 101");
        } catch (...) {
            throw;
        }, bb::BitFormatException
    ) << "Expected bit format exception for binary strings that are not sequence of nibbles";
    ASSERT_THROW(
        try {
            auto ret [[maybe_unused]] = bb::convertBinaryToHexString(std::string(65, '1'));
        } catch (...) {
            throw;
        }, bb::BitFormatException
    ) << "Expected bit format exception for binary strings containing more than 64 bits";
}
