///////////////////////////
// Problem 11 - Monkey in the Middle

#include "AdventOfCode2022.h"

class Problem11 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 11; }

    virtual void Run() override
    {
        RunOnData("Day11Example.txt", true);
        RunOnData("Day11Input.txt", false);
    }

private:
    class Monkey
    {
    public:
        Monkey(const StringList& lines, BigInt startIndex)
        {
            // monkey index

            m_monkeyIndex = (BigInt)lines[startIndex + 0][7] - '0';

            StringList tokens;

            // starting items

            Tokenize(lines[startIndex + 1], tokens, ' ');
            for (BigInt tokenIndex = 2; tokenIndex < (BigInt)tokens.size(); ++tokenIndex)
            {
                std::string& entry = tokens[tokenIndex];
                if (entry[entry.length() - 1] == ',')
                    entry.pop_back();
                m_itemList.push_back(atoll(entry.c_str()));
            }

            // operation

            Tokenize(lines[startIndex + 2], tokens, ' ');
            if (tokens[3] == "old")
                m_operationArg1IsOld = true;
            else
                m_operationArg1 = atoll(tokens[3].c_str());
            m_operationIsAdd = (tokens[4] == "+");
            if (tokens[5] == "old")
                m_operationArg2IsOld = true;
            else
                m_operationArg2 = atoll(tokens[5].c_str());

            // test

            Tokenize(lines[startIndex + 3], tokens, ' ');
            m_testDivisor = atoll(tokens[3].c_str());

            Tokenize(lines[startIndex + 4], tokens, ' ');
            m_throwToMonkeyIfTrue = atoll(tokens[5].c_str());

            Tokenize(lines[startIndex + 5], tokens, ' ');
            m_throwToMonkeyIfFalse = atoll(tokens[5].c_str());
        }

        BigInt GetMonkeyIndex() const { return m_monkeyIndex; }

        void GiveItem(BigInt newItem) { m_itemList.push_back(newItem); }

        bool HasItemsToInspect() const { return !m_itemList.empty(); }
        void InspectOneItem(BigInt& monkeyToThrowTo, BigInt& itemToThrow, bool verbose)
        {
            if (!HasItemsToInspect())
                return;

            BigInt currItem = m_itemList.front();
            m_itemList.pop_front();

            if (verbose)
                printf("  Inspecting item %lld\n", currItem);

            // operation

            currItem = DoOperation(currItem, verbose);

            // decay

            if (verbose)
                printf("    Worry level decays from %lld to %lld\n", currItem, currItem / 3);
            currItem /= 3;

            // test

            const bool testResult = ((currItem % m_testDivisor) == 0);
            if (verbose)
                printf("    Testing is %lld divisible by %lld:  %s\n", currItem, m_testDivisor, testResult ? "TRUE" : "FALSE");

            // throwing

            monkeyToThrowTo = testResult ? m_throwToMonkeyIfTrue : m_throwToMonkeyIfFalse;
            itemToThrow = currItem;
            if (verbose)
                printf("    Throwing value %lld to monkey %lld\n", itemToThrow, monkeyToThrowTo);

            ++m_numTimesInspected;
        }

        BigInt GetNumTimesInspected() const { return m_numTimesInspected; }

    private:
        BigInt m_monkeyIndex = 0;

        bool m_operationIsAdd = false;
        bool m_operationArg1IsOld = false;
        BigInt m_operationArg1 = 0;
        bool m_operationArg2IsOld = false;
        BigInt m_operationArg2 = 0;

        BigInt m_testDivisor = 1;
        BigInt m_throwToMonkeyIfTrue = 0;
        BigInt m_throwToMonkeyIfFalse = 0;

        BigIntDeque m_itemList;

        BigInt m_numTimesInspected = 0;

        BigInt DoOperation(BigInt oldValue, bool verbose) const
        {
            const BigInt arg1 = m_operationArg1IsOld ? oldValue : m_operationArg1;
            const BigInt arg2 = m_operationArg2IsOld ? oldValue : m_operationArg2;

            const BigInt result = m_operationIsAdd ? (arg1 + arg2) : (arg1 * arg2);

            if (verbose)
                printf(
                    "    Doing operation:  %lld%s %s %lld%s = %lld\n",
                    arg1,
                    m_operationArg1IsOld ? " (old value)" : "",
                    m_operationIsAdd ? "+" : "*",
                    arg2,
                    m_operationArg2IsOld ? " (old value)" : "",
                    result);

            return result;
        }
    };

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        std::vector<Monkey> monkeyList;
        for (BigInt monkeyStartIndex = 0; monkeyStartIndex < (BigInt)lines.size(); monkeyStartIndex += 7)
            monkeyList.emplace_back(lines, monkeyStartIndex);

        for (BigInt roundIndex = 0; roundIndex < 20; ++roundIndex)
        {
            if (verbose)
                printf("***** Starting Round %lld *****\n\n", roundIndex + 1);

            for (Monkey& monkey: monkeyList)
            {
                if (verbose)
                    printf("Monkey %lld:\n", monkey.GetMonkeyIndex());

                if (!monkey.HasItemsToInspect())
                {
                    if (verbose)
                        printf("  Monkey has no items to inspect\n");
                    continue;
                }

                do
                {
                    BigInt monkeyToThrowTo = 0;
                    BigInt itemToThrow = 0;
                    monkey.InspectOneItem(monkeyToThrowTo, itemToThrow, verbose);

                    monkeyList[monkeyToThrowTo].GiveItem(itemToThrow);
                } while (monkey.HasItemsToInspect());
            }
        }

        if (verbose)
            printf("\n\nMonkey inspection counts:\n");
        BigIntSet numInspectionsSet;
        for (const Monkey& monkey: monkeyList)
        {
            const BigInt numInspected = monkey.GetNumTimesInspected();
            if (verbose)
                printf("  Monkey %lld inspected %lld times\n", monkey.GetMonkeyIndex(), numInspected);
            numInspectionsSet.insert(numInspected);
        }

        auto setReverseIter = numInspectionsSet.crbegin();
        const BigInt largestNumInspections = *setReverseIter;
        ++setReverseIter;
        const BigInt nextLargestNumInspections = *setReverseIter;
        const BigInt numInspectionsProduct = largestNumInspections * nextLargestNumInspections;

        printf(
            "\n\nProduct of two largest num inspections = %lld * %lld = %lld\n",
            largestNumInspections,
            nextLargestNumInspections,
            numInspectionsProduct);
    }
};

Problem11 problem11;
