#ifndef RISCV_SIM_CPU_H
#define RISCV_SIM_CPU_H

#include "Memory.h"
#include "Decoder.h"
#include "RegisterFile.h"
#include "CsrFile.h"
#include "Executor.h"

class Cpu
{
public:

    Cpu(IMem& mem)
            : _mem(mem)
    {

    }

    void Clock()
    {
        _csrf.Clock();

        if(continueRequestForRead){
            continueFromInstructionRequest();
            return;
        }
        if(continueRequestForWriteBack){
            continueFromWritingBackRequest();
            return;
        }

        // Fetch
        _mem.Request(_ip);
        _requestedWord = _mem.Response();

        if (!_requestedWord.has_value())
        {
            continueRequestForRead = true;
            return;
        }

        processInstruction();
    }

    void Reset(Word ip)
    {
        _csrf.Reset();
        _ip = ip;
    }

    std::optional<CpuToHostData> GetMessage()
    {
        return _csrf.GetMessage();
    }

private:
    Reg32 _ip;              // the same as PC and IAR
    Word _word;
    Decoder _decoder;
    RegisterFile _rf;
    CsrFile _csrf;          // used as storage devices for information about instructions received from machines
    IMem& _mem;
    std::optional <Word> _requestedWord;

    bool continueRequestForRead = false;
    bool continueRequestForWriteBack = false;
    InstructionPtr _instruction;

    void continueFromInstructionRequest()
    {
        _requestedWord = _mem.Response();

        if (!_requestedWord.has_value())
            return;

        continueRequestForRead = false;

        processInstruction();
    }

    void continueFromWritingBackRequest()
    {
        if (!_mem.Response(_instruction))
            return;

        continueRequestForWriteBack = false;

        _rf.Write(_instruction);
        _csrf.Write(_instruction);

        // Advance PC
        _csrf.InstructionExecuted();
        _ip = _instruction->_nextIp;
    }

    void processInstruction()
    {
        // After fetch
        _word = _requestedWord.value();

        // Decode
        _instruction = _decoder.Decode(_word);

        // Read
        _csrf.Read(_instruction);
        _rf.Read(_instruction);

        // Execute
        Executor::Execute(_instruction, _ip);

        // Write back
        _mem.Request(_instruction);

        if(!_mem.Response(_instruction))
        {
            continueRequestForWriteBack = true;
            return;
        }

        _rf.Write(_instruction);
        _csrf.Write(_instruction);

        // Advance PC
        _csrf.InstructionExecuted();
        _ip = _instruction->_nextIp;
    }

};


#endif //RISCV_SIM_CPU_H