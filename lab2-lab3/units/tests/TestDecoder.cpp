
#include <gtest/gtest.h>

#include <Decoder.h>
#include <BaseTypes.h>

namespace units
{
    static uint SIZE_IMM = 20u;
    static uint SIZE_RD  = 5u;
    static uint SIZE_OP  = 7u;

    static Word DEFAULT_IMM  = 0b00000010100010101000;
    static Word DEFAULT_SRC1 = 0;
    static Word DEFAULT_RD   = 0b11111;
    static Word OP           = 0b0110111;


    bool checkLUIInstruction(Word imm, Word rd, Word rs1, const Instruction& inst)
    {
        return inst._dst == rd &&
               inst._imm.value() == (imm << 12u) &&
               inst._src1 == rs1 &&
               inst._type == IType::Alu &&
               inst._aluFunc == AluFunc::Add;
    }

    class DataFixture: public ::testing::Test
    {
    public:

        DataFixture()
        {
            uint offset = 32u - SIZE_IMM;
            _data = (DEFAULT_IMM << (offset));

            offset -= SIZE_RD;
            _data |= (DEFAULT_RD << (offset));

            offset -= SIZE_OP;
            _data |= (OP << (offset));

            _decoder = Decoder();
            instr = Instruction();
        }

        void SetUp() override
        {
            instr = *_decoder.Decode(_data);
        }

        void SetUp(Word rd, Word imm)
        {
            uint offset = 32u - SIZE_IMM;
            _data = (imm << (offset));

            offset -= SIZE_RD;
            _data |= (rd << (offset));

            offset -= SIZE_OP;
            _data |= (OP << (offset));

            SetUp();
        }
        void TearDown() override {}
        Instruction instr;

    private:

        Word _data;
        Decoder _decoder;
    };

    TEST_F(DataFixture, TestDecoderLUI1)
    {
        ASSERT_EQ(instr._dst, DEFAULT_RD);
        ASSERT_EQ(instr._aluFunc, AluFunc::Add);
        ASSERT_EQ(instr._imm.value(), (DEFAULT_IMM << 12u));
        ASSERT_EQ(instr._src1, DEFAULT_SRC1);
        ASSERT_EQ(instr._type, IType::Alu);
    }

    TEST_F(DataFixture, TestDecoderLUI2)
    {
        ASSERT_TRUE(checkLUIInstruction(DEFAULT_IMM, DEFAULT_RD, DEFAULT_SRC1, instr));
    }

    TEST_F(DataFixture, TestDecoderLUI3)
    {
        Word rd = 0b00111, imm = 0b00000000000000000001;
        SetUp(rd, imm);
        ASSERT_TRUE(checkLUIInstruction(imm, rd, DEFAULT_SRC1, instr));
    }

    TEST_F(DataFixture, TestDecoderLUI4)
    {
        Word rd = 0b00001, imm = 0b11111111111111111111;
        SetUp(rd, imm);
        ASSERT_TRUE(checkLUIInstruction(imm, rd, DEFAULT_SRC1, instr));
    }

    TEST_F(DataFixture, TestDecoderLUI5)
    {
        Word rd = 0b00001, imm = 0b00000000000000000000;
        SetUp(rd, imm);
        ASSERT_TRUE(checkLUIInstruction(imm, rd, DEFAULT_SRC1, instr));
    }
}
