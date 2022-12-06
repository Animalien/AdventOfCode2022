///////////////////////////
// Problem 6 - Tuning Trouble

#include "AdventOfCode2022.h"

class Problem6 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 6; }

    virtual void Run() override
    {
        RunOnData("Day6Example.txt", true);
        RunOnData("Day6Input.txt", false);
    }

private:
    class Tracker
    {
    public:
        Tracker() : m_flags1(0), m_flags2(0), m_totalNumChars(0) {}

        BigInt GetTotalNumChars() const { return m_totalNumChars; }

        void AddChar(char c)
        {
            BigUInt* flagField;
            BigUInt shift;
            GetCharInfo(c, flagField, shift);
            BigUInt counter = (*flagField >> shift) & 15;
            ++counter;
            *flagField &= ~(7ULL << shift);
            *flagField |= (counter << shift);
            ++m_totalNumChars;
        }
        void RemoveChar(char c)
        {
            BigUInt* flagField;
            BigUInt shift;
            GetCharInfo(c, flagField, shift);
            BigUInt counter = (*flagField >> shift) & 15;
            --counter;
            *flagField &= ~(7ULL << shift);
            *flagField |= (counter << shift);
            --m_totalNumChars;
        }

        bool HasAnyDupes() const { return (((m_flags1 | m_flags2) & FLAG_TEST) != 0); }

    private:
        const BigUInt FLAG_TEST = (14ULL << 0) | (14ULL << 4) | (14ULL << 8) | (14ULL << 12) | (14ULL << 16) | (14ULL << 20)
                                  | (14ULL << 24) | (14ULL << 28) | (14ULL << 32) | (14ULL << 36) | (14ULL << 40) | (14ULL << 44)
                                  | (14ULL << 48);
        void GetCharInfo(char c, BigUInt*& flagField, BigUInt& shift)
        {
            if (c < 'n')
            {
                flagField = &m_flags1;
                shift = ((BigUInt)(c - 'a')) * 4;
            }
            else
            {
                flagField = &m_flags2;
                shift = ((BigUInt)(c - 'n')) * 4;
            }
        }
        BigUInt m_flags1;
        BigUInt m_flags2;
        BigInt m_totalNumChars;
    };

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        for (const std::string& line: lines)
        {
            printf("Looking at line:\n");

            if (verbose)
                printf("  %s\n", line.c_str());

            Tracker tracker1;
            Tracker tracker2;

            BigInt partOneIndex = -1;
            BigInt partTwoIndex = -1;

            BigInt charIndex = 0;
            for (;;)
            {
                if (tracker1.GetTotalNumChars() < 4)
                {
                    tracker1.AddChar(line[charIndex]);
                    if (verbose)
                        printf("  added %c to tracker1\n", (int)line[charIndex]);
                }
                else if (partOneIndex < 0)
                {
                    if (!tracker1.HasAnyDupes())
                    {
                        partOneIndex = charIndex;
                    }
                    else
                    {
                        if (verbose)
                        {
                            printf(
                                "  %c%c%c%c - has a dupe\n",
                                (int)line[charIndex - 4],
                                (int)line[charIndex - 3],
                                (int)line[charIndex - 2],
                                (int)line[charIndex - 1]);
                            printf("  removing %c and adding %c\n", (int)line[charIndex - 4], (int)line[charIndex]);
                        }

                        tracker1.RemoveChar(line[charIndex - 4]);
                        tracker1.AddChar(line[charIndex]);
                    }
                }

                if (tracker2.GetTotalNumChars() < 14)
                {
                    tracker2.AddChar(line[charIndex]);
                    if (verbose)
                        printf("  added %c to tracker2\n", (int)line[charIndex]);
                }
                else if (partTwoIndex < 0)
                {
                    if (!tracker2.HasAnyDupes())
                    {
                        partTwoIndex = charIndex;
                    }
                    else
                    {
                        if (verbose)
                        {
                            printf(
                                "  %c%c%c%c%c%c%c%c%c%c%c%c%c%c - has a dupe\n",
                                (int)line[charIndex - 14],
                                (int)line[charIndex - 13],
                                (int)line[charIndex - 12],
                                (int)line[charIndex - 11],
                                (int)line[charIndex - 10],
                                (int)line[charIndex - 9],
                                (int)line[charIndex - 8],
                                (int)line[charIndex - 7],
                                (int)line[charIndex - 6],
                                (int)line[charIndex - 5],
                                (int)line[charIndex - 4],
                                (int)line[charIndex - 3],
                                (int)line[charIndex - 2],
                                (int)line[charIndex - 1]);
                            printf("  removing %c and adding %c\n", (int)line[charIndex - 14], (int)line[charIndex]);
                        }

                        tracker2.RemoveChar(line[charIndex - 14]);
                        tracker2.AddChar(line[charIndex]);
                    }
                }

                ++charIndex;

                if ((charIndex >= (BigInt)line.length()) || ((partOneIndex >= 0) && (partTwoIndex >= 0)))
                    break;
            }

            printf(
                "Found non-duping sequence %c%c%c%c, at index %lld\n",
                (int)line[partOneIndex - 4],
                (int)line[partOneIndex - 3],
                (int)line[partOneIndex - 2],
                (int)line[partOneIndex - 1],
                partOneIndex);
            printf(
                "Found non-duping sequence %c%c%c%c%c%c%c%c%c%c%c%c%c%c, at index %lld\n",
                (int)line[partTwoIndex - 14],
                (int)line[partTwoIndex - 13],
                (int)line[partTwoIndex - 12],
                (int)line[partTwoIndex - 11],
                (int)line[partTwoIndex - 10],
                (int)line[partTwoIndex - 9],
                (int)line[partTwoIndex - 8],
                (int)line[partTwoIndex - 7],
                (int)line[partTwoIndex - 6],
                (int)line[partTwoIndex - 5],
                (int)line[partTwoIndex - 4],
                (int)line[partTwoIndex - 3],
                (int)line[partTwoIndex - 2],
                (int)line[partTwoIndex - 1],
                partTwoIndex);
        }
    }
};

Problem6 problem6;
