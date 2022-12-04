///////////////////////////
// Problem 4 - Camp Cleanup

#include "AdventOfCode2022.h"

class Problem4 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 4; }

    virtual void Run() override
    {
        RunOnData("Day4Example.txt", true);
        RunOnData("Day4Input.txt", false);
    }

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        BigInt numFullContainers = 0;
        BigInt numOverlappers = 0;
        for (const std::string& line: lines)
        {
            BigInt range1Start, range1End, range2Start, range2End;
            GetLineRanges(line, range1Start, range1End, range2Start, range2End);

            const BigInt startTester = range1Start - range2Start;
            const BigInt endTester = range2End - range1End;
            const BigInt containTester = startTester * endTester;
            const bool isContainer = (containTester >= 0);

            const BigInt startToEndTester = range1Start - range2End;
            const BigInt endToStartTester = range1End - range2Start;
            const BigInt overlapTester = startToEndTester * endToStartTester;
            const bool isOverlapper = (overlapTester <= 0);

            if (verbose)
                printf(
                    "For line %s, we got %lld * %lld = %lld (isContainer = %s), %lld * %lld = %lld (isOverlapper = %s)\n",
                    line.c_str(),
                    startTester,
                    endTester,
                    containTester,
                    isContainer ? "YES" : "no",
                    startToEndTester,
                    endToStartTester,
                    overlapTester,
                    isOverlapper ? "YES" : "no");

            if (isContainer)
                ++numFullContainers;

            if (isOverlapper)
                ++numOverlappers;
        }

        printf(
            "Found %lld pairs wherein one fully contained the other, and %lld pairs that overlapped at all\n\n",
            numFullContainers,
            numOverlappers);
    }

    void GetLineRanges(const std::string& line, BigInt& range1Start, BigInt& range1End, BigInt& range2Start, BigInt& range2End)
    {
        static std::string token;

        BigInt charIndex = 0;

        token.clear();
        while (line[charIndex] != '-')
        {
            token += line[charIndex];
            ++charIndex;
        }
        range1Start = atoll(token.c_str());
        ++charIndex;

        token.clear();
        while (line[charIndex] != ',')
        {
            token += line[charIndex];
            ++charIndex;
        }
        range1End = atoll(token.c_str());
        ++charIndex;

        token.clear();
        while (line[charIndex] != '-')
        {
            token += line[charIndex];
            ++charIndex;
        }
        range2Start = atoll(token.c_str());
        ++charIndex;
  
        token.clear();
        while (charIndex < (BigInt)line.size())
        {
            token += line[charIndex];
            ++charIndex;
        }
        range2End = atoll(token.c_str());
    }


};

Problem4 problem4;
