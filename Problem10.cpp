///////////////////////////
// Problem 10 - Cathode-Ray Tube

#include "AdventOfCode2022.h"

class Problem10 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 10; }

    virtual void Run() override
    {
        RunOnData("Day10Example.txt", false, true);
        RunOnData("Day10Example2.txt", true, false);
        RunOnData("Day10Input.txt", true, false);
    }

private:
    class CPU
    {
    public:
        CPU(const char* filename, bool verbose) : m_verbose(verbose)
        {
            ReadFileLines(filename, m_instructions);
            FetchNextInstruction(true /*init*/);
        }

        BigInt GetCycleNumber() const { return m_cycleNumber; }
        BigInt GetRegisterX() const { return m_registerX; }
        BigInt GetSignalStrength() const { return (m_cycleNumber * m_registerX); }

        bool StepOneCycle()
        {
            if (m_verbose)
                printf("Stepping cycle number %lld\n", m_cycleNumber);

            bool shouldContinue = true;
            if (!HaveCurrInstruction())
            {
                if (m_verbose)
                    printf("  No more instructions remaining!\n");
                shouldContinue = false;
            }
            else if (CurrInstructionReadyToFinish())
            {
                if (m_verbose)
                    printf("  Curr instruction ready to finish\n");

                FinishCurrInstruction();
                if (!FetchNextInstruction())
                    shouldContinue = false;
            }

            if (shouldContinue)
            {
                ++m_cycleNumber;
                if (m_verbose)
                    printf("  Cycle number now %lld\n", m_cycleNumber);
            }

            return shouldContinue;
        }

    private:
        enum class Instruction
        {
            None,
            Noop,
            Add,
        };

        StringList m_instructions;
        BigInt m_nextInstructionIndex = 0;
        bool m_verbose = false;
        BigInt m_cycleNumber = 1;

        BigInt m_registerX = 1;

        Instruction m_currInstruction = Instruction::None;
        BigInt m_currInstructionArg = 0;
        BigInt m_currInstructionFinishCycle = -1;

        bool HaveCurrInstruction() const { return (m_currInstruction != Instruction::None); }
        bool CurrInstructionReadyToFinish() const { return (m_cycleNumber >= m_currInstructionFinishCycle); }
        void FinishCurrInstruction()
        {
            switch (m_currInstruction)
            {
                case Instruction::Noop:
                    if (m_verbose)
                        printf("  Executing Noop\n");
                    break;
                case Instruction::Add:
                    m_registerX += m_currInstructionArg;
                    if (m_verbose)
                        printf(
                            "  Executing Add:  %lld(X) += %lld = %lld\n",
                            m_registerX - m_currInstructionArg,
                            m_currInstructionArg,
                            m_registerX);
                    break;
                default:
                    break;
            }
            m_currInstruction = Instruction::None;
        }
        bool FetchNextInstruction(bool init = false)
        {
            if (m_nextInstructionIndex >= (BigInt)m_instructions.size())
                return false;

            StringList tokens;
            Tokenize(m_instructions[m_nextInstructionIndex], tokens, ' ');
            if (tokens[0] == "noop")
            {
                m_currInstruction = Instruction::Noop;
                m_currInstructionArg = 0;
                m_currInstructionFinishCycle = m_cycleNumber;
            }
            else
            {
                m_currInstruction = Instruction::Add;
                m_currInstructionArg = atoll(tokens[1].c_str());
                m_currInstructionFinishCycle = m_cycleNumber + 1;
            }

            if (!init)
                ++m_currInstructionFinishCycle;

            ++m_nextInstructionIndex;

            if (m_verbose)
                printf(
                    "  Next instruction = %s, arg = %lld, finish cycle = %lld\n",
                    tokens[0].c_str(),
                    m_currInstructionArg,
                    m_currInstructionFinishCycle);

            return true;
        }
    };

    class CRT
    {
    public:
        CRT()
        {
            m_display.resize(NUM_ROWS);
            for (std::string& row: m_display)
                row.resize(ROW_LENGTH, '.');
        }

        BigInt GetNumRows() const { return NUM_ROWS; }
        BigInt GetRowLength() const { return ROW_LENGTH; }

        void PlotPixel(BigInt x, BigInt y) { m_display[y][x] = '#'; }

        void Print(const char* indent) const
        {
            for (const std::string& row: m_display)
                printf("%s%s\n", indent, row.c_str());
        }
        void PrintRow(const char* indent, BigInt y) { printf("%s%s\n", indent, m_display[y].c_str()); }

    private:
        static const BigInt NUM_ROWS = 6;
        static const BigInt ROW_LENGTH = 40;
        StringList m_display;
    };

    void RunOnData(const char* filename, bool showCRT, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        CPU cpu(filename, verbose);
        CRT crt;

        BigInt sumSignalValues = 0;
        do
        {
            const BigInt cycleNum = cpu.GetCycleNumber();
            if ((cycleNum >= 20) && (((cycleNum - 20) % 40) == 0) && (cycleNum <= 220))
            {
                const BigInt signalVal = cpu.GetSignalStrength();
                sumSignalValues += signalVal;

                if (verbose)
                    printf(
                        "At cycle num %lld, signal strength is %lld, running total is %lld\n",
                        cycleNum,
                        signalVal,
                        sumSignalValues);
            }

            if (showCRT)
            {
                const BigInt spriteX = cpu.GetRegisterX();
                const BigInt cycleIndex = cycleNum - 1;
                const BigInt pixelX = cycleIndex % crt.GetRowLength();
                const BigInt pixelY = cycleIndex / crt.GetRowLength();
                if ((pixelX >= 0) && (pixelX < crt.GetRowLength()) && (pixelY >= 0) && (pixelY < crt.GetNumRows()))
                {
                    if (std::abs(spriteX - pixelX) <= 1)
                        crt.PlotPixel(pixelX, pixelY);

                    if (verbose)
                        crt.PrintRow("  ", pixelY);
                }
            }
        } while (cpu.StepOneCycle());

        printf("Sum of signal values = %lld\n\n", sumSignalValues);

        if (showCRT)
        {
            printf("CRT after run:\n");
            crt.Print("  ");
            printf("\n");
        }
    }
};

Problem10 problem10;
