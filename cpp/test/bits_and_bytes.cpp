#include "gtest/gtest.h"

#include "bits_and_bytes.h"

class Bits : public testing::Test {
public:
    void SetUp() override {
        bits_and_bytes::BitsBase::stringFormat = bits_and_bytes::DEFAULT_STRING_FORMAT;
    }

    static void disableLeadingZeroes() {
        bits_and_bytes::BitsBase::stringFormat.leadingZeroes = bits_and_bytes::LeadingZeroes::Suppress;
    }

    static void disableBitGrouping() {
        bits_and_bytes::BitsBase::stringFormat.bitUnit = bits_and_bytes::BitUnit::None;
    }

    static void enableHexaDecimalOutput() {
        bits_and_bytes::BitsBase::stringFormat.format = bits_and_bytes::Format::HexaDecimal;
    }

protected:
    inline static bits_and_bytes::StringFormat stringFormat {bits_and_bytes::DEFAULT_STRING_FORMAT };
};

TEST_F(Bits, WillSizeUnsignedIntegerTypesCorrectly) {
    disableLeadingZeroes();
    disableBitGrouping();

    std::string bitStr{};

    bitStr = bits_and_bytes::Bits{uint8_t{0}};
    ASSERT_EQ(1, bitStr.length());

    bitStr = bits_and_bytes::Bits{uint16_t{0xFFFF}};
    ASSERT_EQ(16, bitStr.length());

    bitStr = bits_and_bytes::Bits{uint32_t{0xFFFF'FFFF}};
    ASSERT_EQ(32, bitStr.length());

    bitStr = bits_and_bytes::Bits{uint64_t{0xFFFF'FFFF'FFFF'FFFF}};
    ASSERT_EQ(64, bitStr.length());
}

TEST_F(Bits, WillGroupByNibbleWhenLeadingZeroesAreOff) {
    disableLeadingZeroes();
    ASSERT_STREQ("0", bits_and_bytes::Bits{uint8_t{0}});
    ASSERT_STREQ("10", bits_and_bytes::Bits{int16_t{2}});
    ASSERT_STREQ("1111", bits_and_bytes::Bits{int32_t{15}});
    ASSERT_STREQ("1 0000", bits_and_bytes::Bits{int32_t{16}});
    // With leading zeroes off, unsigned values should produce the same output for all
    // data types that can represent the number
    // NOTE: This invokes operator== with dissimilar types
    ASSERT_EQ(bits_and_bytes::Bits{int64_t{16}}, bits_and_bytes::Bits{int32_t{16}});
}

TEST_F(Bits, WillGroupByNibbleCorrectlyWhenLeadingZeroesAreOn) {
    ASSERT_STREQ("0000 0000", bits_and_bytes::Bits{uint8_t{0}});
    ASSERT_STREQ("0000 0000 0000 0010", bits_and_bytes::Bits{int16_t{2}});
    ASSERT_STREQ("0000 0000 0000 0000 0000 0000 0000 1111", bits_and_bytes::Bits{int32_t{15}});
    ASSERT_STREQ("0001 0000", bits_and_bytes::Bits{int8_t{16}});
}

TEST_F(Bits, WillProduceCorrectHexWhenLeadingZeroesAreOff) {
    enableHexaDecimalOutput();
    disableLeadingZeroes();
    ASSERT_STREQ("0x0", bits_and_bytes::Bits{uint8_t{0}});
    ASSERT_STREQ("0x2", bits_and_bytes::Bits{int16_t{2}});
    ASSERT_STREQ("0xF", bits_and_bytes::Bits{int32_t{15}});
    ASSERT_STREQ("0x10", bits_and_bytes::Bits{int32_t{16}});
}

TEST_F(Bits, WillProduceCorrectHexWhenLeadingZeroesAreOn) {
    enableHexaDecimalOutput();
    ASSERT_STREQ("0x00", bits_and_bytes::Bits{uint8_t{0}});
    ASSERT_STREQ("0x0002", bits_and_bytes::Bits{int16_t{2}});
    ASSERT_STREQ("0x0000000F", bits_and_bytes::Bits{int32_t{15}});
    ASSERT_STREQ("0x00000010", bits_and_bytes::Bits{int32_t{16}});
}