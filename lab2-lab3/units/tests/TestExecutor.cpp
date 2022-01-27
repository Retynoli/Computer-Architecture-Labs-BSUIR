#include <gtest/gtest.h>

#include <Instruction.cpp>
#include <Instruction.h>
#include <BaseTypes.h>
#include <Executor.h>

namespace units
{
    static const Word DEFAULT_ADDRESS = 0b1000;
    static const Word DEFAULT_SRC1VAL = 0;
    static const Word DEFAULT_IP = 0b0100;
    static const Word DEFAULT_SRC1 = 0;
    static const Word DEFAULT_IMM = 0b00000010100010101000;


    class InstructionFixture: public ::testing::Test
    {
    public:

        InstructionFixture()
        {
            instruction = std::make_unique<Instruction>();
            instruction->_type = IType::Alu;
            instruction->_aluFunc = AluFunc::Add;
            instruction->_addr = DEFAULT_ADDRESS;
            instruction->_src1 = DEFAULT_SRC1;
            instruction->_src1Val = DEFAULT_SRC1VAL;
            instruction->_imm = DEFAULT_IMM;
        }

        void SetUp() override
        {
            Executor::Execute(instruction, DEFAULT_IP);
        }

        void SetUp(Word imm, Word ip, Word addr)
        {
            instruction = std::make_unique<Instruction>();
            instruction->_type = IType::Alu;
            instruction->_aluFunc = AluFunc::Add;
            instruction->_addr = addr;
            instruction->_src1 = DEFAULT_SRC1;
            instruction->_src1Val = DEFAULT_SRC1VAL;
            instruction->_imm = imm;

            Executor::Execute(instruction, DEFAULT_IP);
        }

        void TearDown() override
        {
            instruction.reset();
        }

        InstructionPtr instruction;
    };


    TEST_F(InstructionFixture, TestExecutorLUI1)
    {
        ASSERT_EQ(instruction->_data, DEFAULT_IMM);
        ASSERT_EQ(instruction->_nextIp, DEFAULT_IP + 4);
        ASSERT_EQ(instruction->_addr, DEFAULT_ADDRESS);
    }

    TEST_F(InstructionFixture, TestExecutorLUI2)
    {
        Word ip = 0b0001, addr = 0b1001, imm = 0b00000000000000000000;

        SetUp(imm, ip, addr);

        ASSERT_EQ(instruction->_data, imm);
        ASSERT_EQ(instruction->_nextIp, ip + 4);
        ASSERT_EQ(instruction->_addr, addr);
    }
}
