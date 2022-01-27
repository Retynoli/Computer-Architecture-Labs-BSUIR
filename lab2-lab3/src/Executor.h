#ifndef RISCV_SIM_EXECUTOR_H
#define RISCV_SIM_EXECUTOR_H

#include "Instruction.h"

class Executor
{
public:
    static void Execute(InstructionPtr& instr, Word ip)
    {
        instr->_nextIp = ip + 4;    // by default

        switch (instr->_type) {
            case IType::Alu:
            {

                if (instr->_src1.has_value() && instr->_imm.has_value())
                    aluFunc(instr, instr->_imm.value());
                else if (instr->_src1.has_value() && instr->_src2.has_value())
                    aluFunc(instr, instr->_src2Val);

                return;
            }

            case IType::Br:
                if (!instr->_src1.has_value() || !instr->_src2.has_value())
                    return;

                switch (instr->_brFunc) {
                    case BrFunc::Eq:
                        if (instr->_src1Val == instr->_src2Val)
                            instr->_nextIp = ip + instr->_imm.value();
                        return;
                    case BrFunc::Neq:
                        if (instr->_src1Val != instr->_src2Val)
                            instr->_nextIp = ip + instr->_imm.value();
                        return;
                    case BrFunc::Lt:
                        if (signedLessThan(instr->_src1Val, instr->_src2Val))
                            instr->_nextIp = ip + instr->_imm.value();
                        return;
                    case BrFunc::Ltu:
                        if (instr->_src1Val < instr->_src2Val)
                            instr->_nextIp = ip + instr->_imm.value();
                        return;
                    case BrFunc::Ge:
                        if (!signedLessThan(instr->_src1Val, instr->_src2Val))
                            instr->_nextIp = ip + instr->_imm.value();
                        return;
                    case BrFunc::Geu:
                        if (instr->_src1Val >= instr->_src2Val)
                            instr->_nextIp = ip + instr->_imm.value();
                        return;
                    case BrFunc::AT:
                        instr->_nextIp = ip + instr->_imm.value();
                        return;
                    case BrFunc::NT:
                        return;
                }
            case IType::Ld:
                instr->_addr = instr->_src1Val + instr->_imm.value();
                return;
            case IType::St:
                instr->_addr = instr->_src1Val + instr->_imm.value();
                instr->_data = instr->_src2Val;
                return;
            case IType::Csrr:
                instr->_data = instr->_csrVal;
                return;
            case IType::Csrw:
                instr->_data = instr->_src1Val;
                return;
            case IType::J:
                instr->_data = ip + 4;
                instr->_nextIp = ip + instr->_imm.value();
                return;
            case IType::Jr:
                instr->_data = ip + 4;
                instr->_nextIp = instr->_src1Val + instr->_imm.value();
                return;
            case IType::Auipc:
                instr->_data = ip + instr->_imm.value();
                return;
            case IType::Unsupported:
                return;
        }
    }

private:

    static bool checkIsNegative(Word w){
        return (w & (1u << 31u)) != 0;
    }

    static Word twosComplementAbs(Word w){
        if(checkIsNegative(w)){
            return ~w + 1;
        }
        return w;
    }

    static void aluFunc(InstructionPtr& instr, Word secondOperand) {
        switch (instr->_aluFunc) {
            case AluFunc::Add:
                instr->_data = instr->_src1Val + secondOperand;
                break;
            case AluFunc::Sub:
                instr->_data = instr->_src1Val + (~secondOperand + 1);
                break;
            case AluFunc::And:
                instr->_data = instr->_src1Val & secondOperand;
                break;
            case AluFunc::Or:
                instr->_data = instr->_src1Val | secondOperand;
                break;
            case AluFunc::Xor:
                instr->_data = instr->_src1Val ^ secondOperand;
                break;
            case AluFunc::Slt:
                instr->_data = signedLessThan(instr->_src1Val, secondOperand);
                break;
            case AluFunc::Sltu:
                instr->_data = instr->_src1Val < secondOperand;
                break;
            case AluFunc::Sll:
                instr->_data = instr->_src1Val << (secondOperand % 32);
                break;
            case AluFunc::Srl:
                instr->_data = instr->_src1Val >> (secondOperand % 32);
                break;
            case AluFunc::Sra:
                if (checkIsNegative(instr->_src1Val)) {
                    Word unsignedShift = instr->_src1Val >> (secondOperand % 32);
                    Word addedBits = (1u << 31u) - (1u << (31 - secondOperand % 32));
                    instr->_data = unsignedShift | addedBits | (1u << 31u);
                } else
                    instr->_data = instr->_src1Val >> (secondOperand % 32);
                break;
        }
    }

    static bool signedLessThan(Word fi, Word sec){

        if(checkIsNegative(fi) && !checkIsNegative(sec)){
            return true;
        }
        if(!checkIsNegative(fi) && checkIsNegative(sec)){
            return false;
        }
        if(!checkIsNegative(fi) && !checkIsNegative(sec)){
            return fi < sec;
        }
        if(checkIsNegative(fi) && checkIsNegative(sec)){
            Word absFi = twosComplementAbs(fi);
            Word absSec = twosComplementAbs(sec);

            return absFi > absSec;
        }

    }
};

#endif // RISCV_SIM_EXECUTOR_H