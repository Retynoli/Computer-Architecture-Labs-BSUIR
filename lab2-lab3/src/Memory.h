
#ifndef RISCV_SIM_DATAMEMORY_H
#define RISCV_SIM_DATAMEMORY_H

#include "Instruction.h"
#include <iostream>
#include <fstream>
#include <elf.h>
#include <cstring>
#include <vector>
#include <cassert>
#include <map>
#include <queue>
#include <algorithm>


//static constexpr size_t memSize = 4*1024*1024; // memory size in 4-byte words
static constexpr size_t memSize = 1024*1024; // memory size in 4-byte words

static constexpr size_t memoryLatency = 152;
static constexpr size_t cacheMemoryLatency = 3;

static constexpr size_t lineSizeBytes = 128;
static constexpr size_t lineSizeWords = lineSizeBytes / sizeof(Word);

static constexpr size_t codeCacheSizeBytes = 512;
static constexpr size_t codeCacheSizeLines = codeCacheSizeBytes / lineSizeBytes;

static constexpr size_t dataCacheSizeBytes = 1024;
static constexpr size_t dataCacheSizeLines = dataCacheSizeBytes / lineSizeBytes;

using Line = std::array<Word, lineSizeWords>;

static Word ToWordAddr(Word addr) { return addr >> 2u; }
static Word ToLineAddr(Word addr) { return addr & ~(lineSizeBytes - 1); }
static Word ToLineOffset(Word addr) { return ToWordAddr(addr) & (lineSizeWords - 1); }


class MemoryStorage {
public:

    MemoryStorage()
    {
        _mem.resize(memSize);
    }

    bool LoadElf(const std::string &elf_filename) {
        std::ifstream elffile;
        elffile.open(elf_filename, std::ios::in | std::ios::binary);

        if (!elffile.is_open()) {
            std::cerr << "ERROR: load_elf: failed opening file \"" << elf_filename << "\"" << std::endl;
            return false;
        }

        elffile.seekg(0, elffile.end);
        size_t buf_sz = elffile.tellg();
        elffile.seekg(0, elffile.beg);

        // Read the entire file. If it doesn't fit in host memory, it won't fit in the risc-v processor
        std::vector<char> buf(buf_sz);
        elffile.read(buf.data(), buf_sz);

        if (!elffile) {
            std::cerr << "ERROR: load_elf: failed reading elf header" << std::endl;
            return false;
        }

        if (buf_sz < sizeof(Elf32_Ehdr)) {
            std::cerr << "ERROR: load_elf: file too small to be a valid elf file" << std::endl;
            return false;
        }

        // make sure the header matches elf32 or elf64
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *) buf.data();
        unsigned char* e_ident = ehdr->e_ident;
        if (e_ident[EI_MAG0] != ELFMAG0
            || e_ident[EI_MAG1] != ELFMAG1
            || e_ident[EI_MAG2] != ELFMAG2
            || e_ident[EI_MAG3] != ELFMAG3) {
            std::cerr << "ERROR: load_elf: file is not an elf file" << std::endl;
            return false;
        }

        if (e_ident[EI_CLASS] == ELFCLASS32) {
            // 32-bit ELF
            return this->LoadElfSpecific<Elf32_Ehdr, Elf32_Phdr>(buf.data(), buf_sz);
        } else if (e_ident[EI_CLASS] == ELFCLASS64) {
            // 64-bit ELF
            return this->LoadElfSpecific<Elf64_Ehdr, Elf64_Phdr>(buf.data(), buf_sz);
        } else {
            std::cerr << "ERROR: load_elf: file is neither 32-bit nor 64-bit" << std::endl;
            return false;
        }
    }

    Word Read(Word ip)
    {
        return _mem[ToWordAddr(ip)];
    }

    void Write(Word ip, Word data)
    {
        _mem[ToWordAddr(ip)] = data;
    }

private:
    template <typename Elf_Ehdr, typename Elf_Phdr>
    bool LoadElfSpecific(char *buf, size_t buf_sz) {
        // 64-bit ELF
        Elf_Ehdr *ehdr = (Elf_Ehdr*) buf;
        Elf_Phdr *phdr = (Elf_Phdr*) (buf + ehdr->e_phoff);
        if (buf_sz < ehdr->e_phoff + ehdr->e_phnum * sizeof(Elf_Phdr)) {
            std::cerr << "ERROR: load_elf: file too small for expected number of program header tables" << std::endl;
            return false;
        }
        auto memptr = reinterpret_cast<char*>(_mem.data());
        // loop through program header tables
        for (int i = 0 ; i < ehdr->e_phnum ; i++) {
            if ((phdr[i].p_type == PT_LOAD) && (phdr[i].p_memsz > 0)) {
                if (phdr[i].p_memsz < phdr[i].p_filesz) {
                    std::cerr << "ERROR: load_elf: file size is larger than memory size" << std::endl;
                    return false;
                }
                if (phdr[i].p_filesz > 0) {
                    if (phdr[i].p_offset + phdr[i].p_filesz > buf_sz) {
                        std::cerr << "ERROR: load_elf: file section overflow" << std::endl;
                        return false;
                    }

                    // start of file section: buf + phdr[i].p_offset
                    // end of file section: buf + phdr[i].p_offset + phdr[i].p_filesz
                    // start of memory: phdr[i].p_paddr
                    std::memcpy(memptr + phdr[i].p_paddr, buf + phdr[i].p_offset, phdr[i].p_filesz);
                }
                if (phdr[i].p_memsz > phdr[i].p_filesz) {
                    // copy 0's to fill up remaining memory
                    size_t zeros_sz = phdr[i].p_memsz - phdr[i].p_filesz;
                    std::memset(memptr + phdr[i].p_paddr + phdr[i].p_filesz, 0, zeros_sz);
                }
            }
        }
        return true;
    }

    std::vector<Word> _mem;
};


class IMem
{
public:
    IMem() = default;
    virtual ~IMem() = default;
    IMem(const IMem &) = delete;
    IMem(IMem &&) = delete;

    IMem& operator=(const IMem&) = delete;
    IMem& operator=(IMem&&) = delete;

    virtual void Request(Word ip) = 0;
    virtual std::optional<Word> Response() = 0;
    virtual void Request(const InstructionPtr &instr) = 0;
    virtual bool Response(const InstructionPtr &instr) = 0;
    virtual void Clock() = 0;
};


class CashMemoryStorage
{
public:
    explicit CashMemoryStorage(MemoryStorage& amem) : _mem(amem) {

    }

    struct CashUnit{
        CashUnit(){
            tag = (~((1u) << (31u))) | (1u << 31u) ;
        }

        CashUnit(Word t, Line l):
                tag(t), line(l){

        }

        Word tag{};
        Line line{};
        bool vb = true;

        bool operator == (Word addressTag){
            return tag == addressTag;
        }
        bool operator != (Word addressTag){
            return tag != addressTag;
        }
    };


    std::pair <Word, bool> ReadInstruction(Word ip)
    {
        Word cacheAddress = ToLineAddr(ip);
        Word offset = ToLineOffset(ip);

        auto findUnit = std::find(cacheCode.begin(), cacheCode.end(), cacheAddress);

        if(findUnit != cacheCode.end())
            return std::make_pair(findUnit->line[offset], false);
        else
        {
            Line readLine = ReadLineFromMemory(cacheAddress);
            CashUnit newUnit = CashUnit(cacheAddress, readLine);

            size_t newIndex;

            if (codeTimeQueue.size() == codeCacheSizeLines){
                size_t deletedIndex = codeTimeQueue.front();
                codeTimeQueue.pop();
                CashUnit deletedUnit = cacheCode[deletedIndex];

                if (!deletedUnit.vb)
                    WriteLineInMemory(deletedUnit.tag, deletedUnit.line);

                newIndex = deletedIndex;
            }
            else
                newIndex = codeTimeQueue.size();

            cacheCode[newIndex] = newUnit;
            codeTimeQueue.push(newIndex);

            return std::make_pair(readLine[offset], true);
        }
    }

    std::pair <Word, bool> LoadInstruction(Word ip)
    {
        Word cacheAddress = ToLineAddr(ip);
        Word offset = ToLineOffset(ip);
        auto findUnit = std::find(cacheData.begin(), cacheData.end(), cacheAddress);

        if (findUnit != cacheData.end())
            return std::make_pair(findUnit->line[offset], false);
        else
        {
            Line readLine = ReadLineFromMemory(cacheAddress);
            CashUnit newUnit = CashUnit(cacheAddress, readLine);

            size_t newIndex;

            if (dataTimeQueue.size() == dataCacheSizeLines){
                size_t deletedIndex = dataTimeQueue.front();
                dataTimeQueue.pop();
                CashUnit deletedUnit = cacheData[deletedIndex];

                if (!deletedUnit.vb)
                    WriteLineInMemory(deletedUnit.tag, deletedUnit.line);

                newIndex = deletedIndex;
            }
            else
                newIndex = dataTimeQueue.size();

            cacheData[newIndex] = newUnit;
            dataTimeQueue.push(newIndex);

            return std::make_pair(readLine[offset], true);
        }
    }

    bool StoreInstruction(Word ip, Word data)
    {
        Word cacheAddress = ToLineAddr(ip);
        Word offset = ToLineOffset(ip);
        auto findUnit = std::find(cacheData.begin(), cacheData.end(), cacheAddress);

        if(findUnit != cacheData.end())
        {
            findUnit->line[offset] = data;
            findUnit->vb = false;
            return false;
        }
        else
        {
            Line readLine = ReadLineFromMemory(cacheAddress);
            readLine[offset] = data;
            CashUnit newUnit = CashUnit(cacheAddress, readLine);
            newUnit.vb = false;

            size_t newIndex;

            if (dataTimeQueue.size() == dataCacheSizeLines)
            {
                size_t deletedIndex = dataTimeQueue.front();
                dataTimeQueue.pop();
                CashUnit deletedUnit = cacheData[deletedIndex];

                if (!deletedUnit.vb)
                    WriteLineInMemory(deletedUnit.tag, deletedUnit.line);

                newIndex = deletedIndex;
            }
            else
                newIndex = dataTimeQueue.size();

            cacheData[newIndex] = newUnit;
            dataTimeQueue.push(newIndex);

            return true;
        }
    }

private:
    std::array <CashUnit, codeCacheSizeLines> cacheCode = std::array <CashUnit, codeCacheSizeLines>();
    std::array <CashUnit, dataCacheSizeLines> cacheData = std::array <CashUnit, dataCacheSizeLines>();

    std::queue <size_t> dataTimeQueue = std::queue <size_t>();
    std::queue <size_t> codeTimeQueue = std::queue <size_t>();

    MemoryStorage& _mem;

    Line ReadLineFromMemory(Word address)
    {
        Line line;

        for (size_t i = 0; i < lineSizeWords; i++)
        {
            line[i] = _mem.Read(address);
            address += 1u << 2u;
        }

        return line;
    }

    void WriteLineInMemory(Word address, Line line)
    {
        for (size_t i = 0; i < lineSizeWords; i++)
        {
            _mem.Write(address, line[i]);
            address += 1u << 2u;
        }
    }
};


class CachedMem: public IMem
{
public:
    explicit CachedMem(MemoryStorage& amem):
            _mem(amem){

    }

    void Request(Word ip) override
    {
        _requestedIp = ip;
        _waitCycles = cacheMemoryLatency;
    }

    std::optional<Word> Response() override
    {
        if (_waitCycles != 0)
            return std::optional<Word>();

        if (!_isMiss)
        {
            auto loadResult = _mem.ReadInstruction(_requestedIp);
            _data = loadResult.first;
            _isMiss = loadResult.second;
            _waitCycles = _isMiss ? memoryLatency : 0;
        }

        if (_waitCycles == 0)
        {
            _isMiss = false;
            return _data;
        }
        else
            return std::optional<Word>();
    }

    void Request(const InstructionPtr &instr) override
    {
        if (instr->_type != IType::Ld && instr->_type != IType::St)
            return;

        Request(instr->_addr);
    }

    bool Response(const InstructionPtr &instr) override
    {
        if (instr->_type != IType::Ld && instr->_type != IType::St)
            return true;

        if (_waitCycles != 0)
            return false;

        if (instr->_type == IType::Ld && !_isMiss)
        {
            auto loadResult = _mem.LoadInstruction(_requestedIp);
            _data = loadResult.first;
            _isMiss = loadResult.second;
            _waitCycles = _isMiss ? memoryLatency : 0;
        }
        else if (instr->_type == IType::St && !_isMiss)
        {
            _isMiss = _mem.StoreInstruction(_requestedIp, instr->_data);
            _waitCycles = _isMiss ? memoryLatency : 0;
        }

        if (_waitCycles == 0)
        {
            if (instr->_type == IType :: Ld)
                instr->_data = _data;

            _isMiss = false;
        }

        return _waitCycles == 0;
    }

    void Clock() override
    {
        if (_waitCycles > 0)
            _waitCycles--;
    }

private:
    Word _requestedIp = 0;
    size_t _waitCycles = 0;
    Word _data;
    bool _isMiss = false;

    CashMemoryStorage _mem;
};


class UncachedMem : public IMem
{
public:
    explicit UncachedMem(MemoryStorage& amem)
            : _mem(amem)
    {

    }

    void Request(Word ip) override
    {
        _requestedIp = ip;
        _waitCycles = latency;
    }

    std::optional<Word> Response() override
    {
        if (_waitCycles > 0)
            return std::optional<Word>();

        return _mem.Read(_requestedIp);
    }

    void Request(const InstructionPtr &instr) override
    {
        if (instr->_type != IType::Ld && instr->_type != IType::St)
            return;

        Request(instr->_addr);
    }

    bool Response(const InstructionPtr &instr) override
    {
        if (instr->_type != IType::Ld && instr->_type != IType::St)
            return true;

        if (_waitCycles != 0)
            return false;

        if (instr->_type == IType::Ld)
            instr->_data = _mem.Read(instr->_addr);
        else if (instr->_type == IType::St)
            _mem.Write(instr->_addr, instr->_data);

        return true;
    }

    void Clock()
    {
        if (_waitCycles > 0)
            _waitCycles--;
    }

private:
    static constexpr size_t latency = 120;
    Word _requestedIp = 0;
    size_t _waitCycles = 0;
    MemoryStorage& _mem;
};

#endif //RISCV_SIM_DATAMEMORY_H
