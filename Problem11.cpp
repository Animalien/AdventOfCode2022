///////////////////////////
// Problem 11 - Monkey in the Middle

#include "AdventOfCode2022.h"

class Problem11 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 11; }

    virtual void Run() override
    {
        RunOnData("Day11Example.txt", 20, false, false);
        RunOnData("Day11Input.txt", 20, false, false);

        RunOnData("Day11Example.txt", 10000, true, false);
        RunOnData("Day11Input.txt", 10000, true, false);
    }

private:
    enum Primes
    {
        Two,
        Three,
        Five,
        Seven,
        Eleven,
        Thirteen,
        Seventeen,
        Nineteen,
        TwentyThree,
        NUM_PRIMES
    };

    static const BigInt MAX_PRIME = 29;

    static const BigInt s_primeNumbers[NUM_PRIMES];
    static const BigInt s_primeNumberToIndex[MAX_PRIME];

    class PrimeRelativeNumber
    {
    public:
        void InitPrime(BigInt p) { m_prime = p; }

        void SetToValue(BigInt value) { m_mod = value % m_prime; }

        bool IsDivisible() const { return (m_mod == 0); }

        void AddNumber(BigInt num)
        {
            m_mod += num;
            m_mod %= m_prime;
        }
        void AddNumber(const PrimeRelativeNumber& rhs)
        {
            m_mod += rhs.m_mod;
            m_mod %= m_prime;
        }
        void MultiplyNumber(BigInt num)
        {
            m_mod *= num;
            m_mod %= m_prime;
        }
        void MultiplyNumber(const PrimeRelativeNumber& rhs)
        {
            m_mod *= rhs.m_mod;
            m_mod %= m_prime;
        }

    private:
        BigInt m_prime = 0;
        BigInt m_mod = 0;
    };

    class ItemNumber
    {
    public:
        ItemNumber(BigInt num = 0)
        {
            for (BigInt index = Two; index < NUM_PRIMES; ++index)
            {
                PrimeRelativeNumber& prn = m_primeRels[index];
                prn.InitPrime(s_primeNumbers[index]);
                prn.SetToValue(num);
            }
        }

        ItemNumber& operator=(BigInt num)
        {
            for (BigInt index = Two; index < NUM_PRIMES; ++index)
            {
                PrimeRelativeNumber& prn = m_primeRels[index];
                prn.InitPrime(s_primeNumbers[index]);
                prn.SetToValue(num);
            }
            return *this;
        }

        bool IsDivisibleByPrime(BigInt prime) const
        {
            const BigInt primeIndex = s_primeNumberToIndex[prime];
            if (primeIndex < 0)
                return false;
            return m_primeRels[primeIndex].IsDivisible();
        }

        void AddNumber(BigInt num)
        {
            for (BigInt index = Two; index < NUM_PRIMES; ++index)
                m_primeRels[index].AddNumber(num);
        }
        void AddNumber(const ItemNumber& rhs)
        {
            for (BigInt index = Two; index < NUM_PRIMES; ++index)
                m_primeRels[index].AddNumber(rhs.m_primeRels[index]);
        }
        void MultiplyNumber(BigInt num)
        {
            for (BigInt index = Two; index < NUM_PRIMES; ++index)
                m_primeRels[index].MultiplyNumber(num);
        }
        void MultiplyNumber(const ItemNumber& rhs)
        {
            for (BigInt index = Two; index < NUM_PRIMES; ++index)
                m_primeRels[index].MultiplyNumber(rhs.m_primeRels[index]);
        }

    private:
        PrimeRelativeNumber m_primeRels[NUM_PRIMES];
    };

    class Monkey
    {
    public:
        Monkey(const StringList& lines, BigInt startIndex, bool partTwo)
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
                const BigInt num = atoll(entry.c_str());
                if (!partTwo)
                    m_itemList.push_back(num);
                else
                    m_itemList2.push_back(ItemNumber(num));
            }

            // operation

            Tokenize(lines[startIndex + 2], tokens, ' ');
            if (tokens[3] == "old")
                m_operationArg1IsOld = true;
            else if (partTwo)
                m_operationArg1PartTwo = atoll(tokens[3].c_str());
            else
                m_operationArg1 = atoll(tokens[3].c_str());
            m_operationIsAdd = (tokens[4] == "+");
            if (tokens[5] == "old")
                m_operationArg2IsOld = true;
            else if (partTwo)
                m_operationArg2PartTwo = atoll(tokens[5].c_str());
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

            // worry decay

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

        void GiveItemPartTwo(const ItemNumber& newItem) { m_itemList2.push_back(newItem); }
        bool HasItemsToInspectPartTwo() const { return !m_itemList2.empty(); }
        void InspectOneItemPartTwo(BigInt& monkeyToThrowTo, ItemNumber& itemToThrow, bool verbose)
        {
            if (!HasItemsToInspectPartTwo())
                return;

            ItemNumber currItem = m_itemList2.front();
            m_itemList2.pop_front();

            if (verbose)
                printf("  Inspecting item\n");

            // operation

            currItem = DoOperation2(currItem, verbose);

            // test

            const bool testResult = currItem.IsDivisibleByPrime(m_testDivisor);
            if (verbose)
                printf("    Testing is divisible by %lld:  %s\n", m_testDivisor, testResult ? "TRUE" : "FALSE");

            // throwing

            monkeyToThrowTo = testResult ? m_throwToMonkeyIfTrue : m_throwToMonkeyIfFalse;
            itemToThrow = currItem;
            if (verbose)
                printf("    Throwing value to monkey %lld\n", monkeyToThrowTo);

            ++m_numTimesInspected;
        }

        BigInt GetNumTimesInspected() const { return m_numTimesInspected; }

    private:
        BigInt m_monkeyIndex = 0;

        bool m_operationIsAdd = false;
        bool m_operationArg1IsOld = false;
        BigInt m_operationArg1 = 0;
        ItemNumber m_operationArg1PartTwo;
        bool m_operationArg2IsOld = false;
        BigInt m_operationArg2 = 0;
        ItemNumber m_operationArg2PartTwo;

        BigInt m_testDivisor = 1;
        BigInt m_throwToMonkeyIfTrue = 0;
        BigInt m_throwToMonkeyIfFalse = 0;

        BigIntDeque m_itemList;
        typedef std::deque<ItemNumber> ItemNumberDeque;
        ItemNumberDeque m_itemList2;

        BigInt m_numTimesInspected = 0;

        BigInt DoOperation(BigInt oldValue, bool verbose) const
        {
            const BigInt arg1 = m_operationArg1IsOld ? oldValue : m_operationArg1;
            const BigInt arg2 = m_operationArg2IsOld ? oldValue : m_operationArg2;

            assert(arg1 >= 0);
            assert(arg2 >= 0);

            const BigInt result = m_operationIsAdd ? (arg1 + arg2) : (arg1 * arg2);

            assert(result >= 0);

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

        ItemNumber DoOperation2(const ItemNumber& oldValue, bool verbose) const
        {
            const ItemNumber* arg1 = m_operationArg1IsOld ? &oldValue : &m_operationArg1PartTwo;
            const ItemNumber* arg2 = m_operationArg2IsOld ? &oldValue : &m_operationArg2PartTwo;

            ItemNumber result = *arg1;
            if (m_operationIsAdd)
                result.AddNumber(*arg2);
            else
                result.MultiplyNumber(*arg2);

            return result;
        }
    };

    void RunOnData(const char* filename, BigInt numRounds, bool partTwo, bool verbose)
    {
        printf("For file '%s', num rounds = %lld, partTwo = %s\n", filename, numRounds, partTwo ? "YES" : "NO");

        StringList lines;
        ReadFileLines(filename, lines);

        std::vector<Monkey> monkeyList;
        for (BigInt monkeyStartIndex = 0; monkeyStartIndex < (BigInt)lines.size(); monkeyStartIndex += 7)
            monkeyList.emplace_back(lines, monkeyStartIndex, partTwo);

        if (!partTwo)
        {
            for (BigInt roundIndex = 0; roundIndex < numRounds; ++roundIndex)
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
        }
        else
        {
            for (BigInt roundIndex = 0; roundIndex < numRounds; ++roundIndex)
            {
                if (verbose)
                    printf("***** Starting Round %lld *****\n\n", roundIndex + 1);

                for (Monkey& monkey: monkeyList)
                {
                    if (verbose)
                        printf("Monkey %lld:\n", monkey.GetMonkeyIndex());

                    if (!monkey.HasItemsToInspectPartTwo())
                    {
                        if (verbose)
                            printf("  Monkey has no items to inspect\n");
                        continue;
                    }

                    do
                    {
                        BigInt monkeyToThrowTo = 0;
                        ItemNumber itemToThrow = 0;
                        monkey.InspectOneItemPartTwo(monkeyToThrowTo, itemToThrow, verbose);

                        monkeyList[monkeyToThrowTo].GiveItemPartTwo(itemToThrow);
                    } while (monkey.HasItemsToInspectPartTwo());
                }
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
            "\nProduct of two largest num inspections = %lld * %lld = %lld\n\n",
            largestNumInspections,
            nextLargestNumInspections,
            numInspectionsProduct);
    }
};

const BigInt Problem11::s_primeNumbers[NUM_PRIMES] = { 2, 3, 5, 7, 11, 13, 17, 19, 23 };
const BigInt Problem11::s_primeNumberToIndex[MAX_PRIME] = { -1, -1,        Two, Three,    -1, Five,     -1, Seven,
                                                            -1, -1,        -1,  Eleven,   -1, Thirteen, -1, -1,
                                                            -1, Seventeen, -1,  Nineteen, -1, -1,       -1, TwentyThree };


Problem11 problem11;
