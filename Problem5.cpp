///////////////////////////
// Problem 5 - Name

#include "AdventOfCode2022.h"

class Problem5 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 5; }

    virtual void Run() override
    {
        RunOnData("Day5Example.txt", true);
        RunOnData("Day5Input.txt", false);
    }

private:
    typedef std::deque<char> Stack;
    typedef std::vector<Stack> StackList;

    void RunOnData(const char* filename, bool verbose)
    {
        StackList stackList;

        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        // build the stacks from the initial drawing

        BigInt lineIndex = 0;
        for (; lineIndex < (BigInt)lines.size(); ++lineIndex)
        {
            const std::string& line = lines[lineIndex];

            if ((line[0] == ' ') && (line[1] == '1'))
                break;

            BigInt charIndex = 0;
            BigInt stackIndex = 0;

            while (charIndex < (BigInt)line.length())
            {
                if (line[charIndex] == '[')
                {
                    if (stackIndex >= (BigInt)stackList.size())
                        stackList.resize(stackIndex + 1);

                    stackList[stackIndex].push_front(line[charIndex + 1]);
                }

                charIndex += 4;
                ++stackIndex;
            }
        }
        printf("Found stack numbers line:  %s\n", lines[lineIndex].c_str());

        ++lineIndex;   // skip past stack numbers line
        ++lineIndex;   // skip blank line

        // print them if necessary

        if (verbose)
            PrintStacks(stackList);

        // now consume the moves

        StackList partTwoStackList = stackList;

        StringList tokens;
        Stack reversedMoveStack;
        for (; lineIndex < (BigInt)lines.size(); ++lineIndex)
        {
            Tokenize(lines[lineIndex], tokens, ' ');

            const BigInt numToMove = atoll(tokens[1].c_str());
            const BigInt fromStackIndex = atoll(tokens[3].c_str()) - 1;
            const BigInt toStackIndex = atoll(tokens[5].c_str()) - 1;

            if (verbose)
                printf("Moving %lld items from stack index %lld to %lld:\n  ", numToMove, fromStackIndex, toStackIndex);

            // part one

            for (BigInt i = 0; i < numToMove; ++i)
            {
                const char item = stackList[fromStackIndex].back();

                if (verbose)
                    printf("%c ", (int)item);

                stackList[fromStackIndex].pop_back();
                stackList[toStackIndex].push_back(item);
            }
            if (verbose)
                printf("\n");

            if (verbose)
                PrintStacks(stackList);

            // part two

            if (verbose)
                printf("\n  ");

            reversedMoveStack.clear();
            for (BigInt i = 0; i < numToMove; ++i)
            {
                const char item = partTwoStackList[fromStackIndex].back();
                partTwoStackList[fromStackIndex].pop_back();
                reversedMoveStack.push_front(item);
            }

            while (!reversedMoveStack.empty())
            {
                const char item = reversedMoveStack.front();

                if (verbose)
                    printf("%c ", (int)item);

                partTwoStackList[toStackIndex].push_back(item);
                reversedMoveStack.pop_front();
            }
            if (verbose)
                printf("\n");

            if (verbose)
                PrintStacks(partTwoStackList);
        }

        // now print the results

        printf("Final stack condition, Part One:\n");
        PrintStacks(stackList);
        printf("\n");

        printf("Stack moving final result, stack tops, Part One:  ");
        for (const Stack& stack: stackList)
        {
            printf("%c", (int)stack.back());
        }
        printf("\n\n");

        printf("Final stack condition, Part Two:\n");
        PrintStacks(partTwoStackList);
        printf("\n");

        printf("Stack moving final result, stack tops, Part Two:  ");
        for (const Stack& stack: partTwoStackList)
        {
            printf("%c", (int)stack.back());
        }
        printf("\n\n");
    }

    void PrintStacks(const StackList& stackList)
    {
        printf("Stacks:\n");
        for (const Stack& stack: stackList)
        {
            printf("  Stack: ");

            for (char item: stack)
                printf("%c ", (int)item);

            printf("\n");
        }
    }
};

Problem5 problem5;
