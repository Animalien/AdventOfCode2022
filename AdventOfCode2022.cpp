// Hi, this is my AdventOfCode 2022 stuff

#include <assert.h>
#include <deque>
#include <limits.h>
#include <map>
#include <set>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


////////////////////////////
////////////////////////////
// Shared tools

////////////////////////////
// Basic Types

typedef long long BigInt;
typedef std::initializer_list<BigInt> BigIntInitList;
typedef std::vector<BigInt> BigIntList;
typedef std::vector<BigIntList> BigIntListList;
typedef std::map<BigInt, BigInt> BigIntMap;
typedef std::set<BigInt> BigIntSet;
typedef std::deque<BigInt> BigIntDeque;

typedef unsigned long long BigUInt;

const BigInt MAX_BIG_INT = LLONG_MAX;
const BigInt MIN_BIG_INT = LLONG_MIN;
const BigInt MAX_BIG_UINT = ULLONG_MAX;

typedef std::vector<bool> BoolList;


////////////////////////////
// Factorization

class Factorization : public BigIntMap
{
public:
    Factorization() : BigIntMap() {}

    bool IsPrime() const { return ((size() == 1) && (begin()->second == 1)); }

    void Absorb(const Factorization& other)
    {
        for (auto iter = other.begin(); iter != other.end(); ++iter)
        {
            Absorb(iter->first, iter->second);
        }
    }

    BigInt CalcProduct() const
    {
        BigInt product = 1;
        for (auto iter = begin(); iter != end(); ++iter)
        {
            for (BigInt i = 0; i < iter->second; ++i)
            {
                product *= iter->first;
            }
        }
        return product;
    }

    void PrintFactors() const
    {
        for (auto iter = begin(); iter != end(); ++iter)
        {
            printf("(%lldn of %lld) ", iter->second, iter->first);
        }
    }

    BigInt CalcNumDivisors() const
    {
        if (IsPrime())
        {
            // if prime, then number of divisors is simply:  1, and itself
            return 2;
        }

        BigInt numDivisors = 1;
        // the number of divisors will be the numbers of combinations of prime factors.
        // in a given divisor, each prime factor can be included from 0 to N times, where
        // N is the number of times that prime factor exists in the original number.
        // (the divisor with ZERO of any prime factors included, is the divisor 1, which every number has.)
        for (auto iter = begin(); iter != end(); ++iter)
        {
            numDivisors *= (iter->second + 1);
        }
        // add 1 more for the original number, being one of its own divisors
        numDivisors += 1;

        return numDivisors;
    }

private:
    void Absorb(BigInt number, BigInt numFactors)
    {
        auto iter = find(number);
        if (iter != end())
        {
            iter->second = std::max(iter->second, numFactors);
        }
        else
        {
            insert(value_type(number, numFactors));
        }
    }
};

class FactorizationCache : public std::map<BigInt, Factorization>
{
public:
    FactorizationCache() : std::map<BigInt, Factorization>() {}

    void PopulateCache(BigInt num) { Factorize(num * 2); }

    const Factorization& Factorize(BigInt num)
    {
        iterator fiter = find(num);
        if (fiter == end())
        {
            fiter = NewFactorize(num);
        }

        return fiter->second;
    }

private:
    iterator NewFactorize(BigInt num)
    {
        auto newValue = insert(value_type(num, Factorization()));
        iterator newIter = newValue.first;
        Factorization& newFactorization = newIter->second;

        const BigInt halfNum = num / 2;
        BigInt prodRemaining = num;
        for (BigInt i = 2; i <= halfNum; ++i)
        {
            const Factorization& f = Factorize(i);
            if (f.IsPrime())
            {
                // i is prime, so see how many i factors fit into this number
                BigInt numFactors = 0;
                for (;;)
                {
                    const lldiv_t divRem = lldiv(prodRemaining, i);
                    if (divRem.rem != 0)
                    {
                        break;
                    }
                    ++numFactors;
                    prodRemaining = divRem.quot;
                }
                if (numFactors > 0)
                {
                    newFactorization.emplace(i, numFactors);
                }
            }

            if (prodRemaining == 1)
            {
                break;
            }
        }
        if (newFactorization.empty())
        {
            newFactorization.emplace(num, 1);
        }

        return newIter;
    }
};

static FactorizationCache s_factorizationCache;


////////////////////////////
// Strings

typedef std::vector<std::string> StringList;
typedef std::vector<StringList> StringListList;
typedef std::set<std::string> StringSet;
typedef std::map<std::string, std::string> StringMap;
typedef std::unordered_set<std::string> UnorderedStringSet;

static const char* fileNameBase = "..\\Input\\";

void ReadFileLines(const char* fileName, StringList& lines)
{
    lines.clear();

    std::string fullFileName = fileNameBase;
    fullFileName += fileName;
    FILE* pFile = fopen(fullFileName.c_str(), "rt");
    assert(pFile);

    char string[1024];
    for (;;)
    {
        const char* fgetsRet = fgets(string, sizeof(string), pFile);
        if (!fgetsRet)
            break;

        const BigInt len = strlen(string);
        if ((len > 0) && (string[len - 1] == '\n'))
            string[len - 1] = 0;
        lines.push_back(string);

        if (feof(pFile))
            break;
    }

    fclose(pFile);
}

void Tokenize(const std::string& st, StringList& tokens, char delim)
{
    std::stringstream stream(st);

    tokens.clear();
    std::string token;
    while (std::getline(stream, token, delim))
    {
        tokens.push_back(token);
    }
}

void ParseBigIntList(const std::string& st, BigIntList& intList, char delim)
{
    StringList tokens;
    Tokenize(st, tokens, delim);

    intList.clear();
    for (const auto& intString: tokens)
    {
        intList.push_back(atoll(intString.c_str()));
    }
}

bool StringHasDigits(const std::string& st, BigInt start = 0, BigInt end = -1)
{
    if (end <= 0)
        end = st.length() - 1;

    for (BigInt i = start; i <= end; ++i)
    {
        const char ch = st[i];
        if (ch < '0')
            return false;
        if (ch > '9')
            return false;
    }

    return true;
}

bool StringHasLowerCaseAlphaNumeric(const std::string& st, BigInt start = 0, BigInt end = -1)
{
    if (end <= 0)
        end = st.length() - 1;

    for (BigInt i = start; i <= end; ++i)
    {
        const char ch = st[i];
        if ((ch >= '0') && (ch <= '9'))
            continue;
        if ((ch >= 'a') && (ch <= 'z'))
            continue;
        return false;
    }

    return true;
}

bool StringIsIntWithinRange(const std::string& st, BigInt min, BigInt max, BigInt skipLastChars = 0)
{
    if (st.empty())
        return false;
    if (!StringHasDigits(st, 0, st.length() - skipLastChars - 1))
        return false;

    const BigInt value = atoi(st.c_str());
    return ((value >= min) && (value <= max));
}

bool StringIsIntWithinRangeAndSuffix(const std::string& st, BigInt min, BigInt max, const char* suffix, BigInt suffixLen)
{
    if ((BigInt)st.length() <= suffixLen)
        return false;
    if (st.compare(st.length() - suffixLen, suffixLen, suffix) != 0)
        return false;

    return StringIsIntWithinRange(st, min, max, suffixLen);
}

void MultiplyStringLists(StringList& origList, const StringList& rhsList)
{
    if (origList.empty())
    {
        origList = rhsList;
        return;
    }

    StringList newList;
    for (const auto& lhs: origList)
    {
        for (const auto& rhs: rhsList)
        {
            newList.push_back(lhs + rhs);
        }
    }

    origList.swap(newList);
}


////////////////////////////
// Sets

template<typename T>
void IntersectSet(std::set<T>& lhs, const std::set<T>& rhs)
{
    std::set<T> newSet;

    auto iterLeft = lhs.cbegin();
    const auto iterLeftEnd = lhs.cend();

    auto iterRight = rhs.cbegin();
    const auto iterRightEnd = rhs.cend();

    for (;;)
    {
        if (iterLeft == iterLeftEnd)
            break;
        if (iterRight == iterRightEnd)
            break;

        const T& leftValue = *iterLeft;
        const T& rightValue = *iterRight;

        if (leftValue == rightValue)
        {
            newSet.insert(*iterLeft);
            ++iterLeft;
            ++iterRight;
        }
        else if (leftValue < rightValue)
        {
            ++iterLeft;
        }
        else
        {
            ++iterRight;
        }
    }

    lhs.swap(newSet);
}


////////////////////////////
// Circular Buffer

template<typename T, BigInt NUM_CAPACITY_BITS>
class CircularBuffer
{
public:
    CircularBuffer() : m_buffer(), m_numInBuffer(0), m_readIndex(0), m_writeIndex(0) {}
    CircularBuffer(const CircularBuffer& other) : m_buffer(), m_numInBuffer(0), m_readIndex(0), m_writeIndex(0) { *this = other; }

    BigInt GetCapacity() const { return CAPACITY; }
    BigInt GetSize() const { return m_numInBuffer; }
    bool IsEmpty() const { return m_numInBuffer == 0; }

    void Clear() { m_numInBuffer = m_readIndex = m_writeIndex = 0; }

    CircularBuffer& operator=(const CircularBuffer& rhs)
    {
        Clear();
        for (const T& item: rhs)
            Write(item);
        return *this;
    }

    const T& PeekRead() const
    {
        assert(!IsEmpty());
        return m_buffer[m_readIndex];
    }
    T Read()
    {
        const T retVal = PeekRead();
        PopRead();
        return retVal;
    }
    void PopRead()
    {
        assert(!IsEmpty());
        m_readIndex = IncrementIndex(m_readIndex);
        --m_numInBuffer;
    }
    void PopWrite()
    {
        assert(!IsEmpty());
        m_writeIndex = DecrementIndex(m_writeIndex);
        --m_numInBuffer;
    }

    void Write(const T& newValue)
    {
        m_buffer[m_writeIndex] = newValue;
        m_writeIndex = IncrementIndex(m_writeIndex);
        ++m_numInBuffer;
    }

    struct ConstIterator
    {
        const CircularBuffer& buffer;
        BigInt index;

        ConstIterator(const CircularBuffer& b, BigInt i) : buffer(b), index(i) {}
        ConstIterator(const ConstIterator& other) : buffer(other.buffer), index(other.index) {}

        bool operator==(const ConstIterator& rhs) const { return ((&buffer == &rhs.buffer) && (index == rhs.index)); }
        bool operator!=(const ConstIterator& rhs) const { return ((&buffer != &rhs.buffer) || (index != rhs.index)); }

        const T& operator*() const { return buffer.m_buffer[index]; }

        void operator++() { index = StepIndex(index, +1); }
    };
    struct Iterator
    {
        CircularBuffer& buffer;
        BigInt index;

        Iterator(CircularBuffer& b, BigInt i) : buffer(b), index(i) {}
        Iterator(const Iterator& other) : buffer(other.buffer), index(other.index) {}

        bool operator==(const Iterator& rhs) const { return ((&buffer == &rhs.buffer) && (index == rhs.index)); }
        bool operator!=(const Iterator& rhs) const { return ((&buffer != &rhs.buffer) || (index != rhs.index)); }

        T& operator*() const { return buffer.m_buffer[index]; }

        void operator++() { index = StepIndex(index, +1); }
    };

    bool operator==(const CircularBuffer& rhs) const
    {
        if (GetSize() != rhs.GetSize())
            return false;
        ConstIterator iter = cbegin();
        const ConstIterator iterEnd = cend();
        ConstIterator rhsIter = rhs.cbegin();
        for (; iter != iterEnd; ++iter, ++rhsIter)
        {
            if (*iter != *rhsIter)
                return false;
        }

        return true;
    }
    bool operator!=(const CircularBuffer& rhs) const { return !(*this == rhs); }

    // STL

    bool empty() const { return IsEmpty(); }
    size_t size() const { return GetSize(); }
    void resize(size_t newSize)
    {
        const BigInt sizeDiff = (BigInt)newSize - GetSize();
        if (sizeDiff > 0)
        {
            while (sizeDiff > 0)
            {
                Write(T());
                --sizeDiff;
            }
        }
        else
        {
            while (sizeDiff < 0)
            {
                PopWrite();
                ++sizeDiff;
            }
        }
    }
    const T& front() const { return PeekRead(); }
    void pop_front() { PopRead(); }
    void push_back(const T& newValue) { Write(newValue); }
    ConstIterator cbegin() const { return ConstIterator(*this, m_readIndex); }
    ConstIterator cend() const { return ConstIterator(*this, m_writeIndex); }
    ConstIterator begin() const { return ConstIterator(*this, m_readIndex); }
    ConstIterator end() const { return ConstIterator(*this, m_writeIndex); }
    Iterator begin() { return Iterator(*this, m_readIndex); }
    Iterator end() { return Iterator(*this, m_writeIndex); }

private:
    static const BigInt CAPACITY = 1LL << NUM_CAPACITY_BITS;
    static const BigInt CAPACITY_MASK = CAPACITY - 1;

    static BigInt IncrementIndex(BigInt index) { return StepIndex(index, +1); }
    static BigInt DecrementIndex(BigInt index) { return StepIndex(index, -1); }
    static BigInt StepIndex(BigInt index, BigInt step) { return (index + step + CAPACITY) & CAPACITY_MASK; }

    T m_buffer[CAPACITY];
    BigInt m_numInBuffer;
    BigInt m_readIndex;
    BigInt m_writeIndex;
};



////////////////////////////
////////////////////////////
// Problems

////////////////////////////
// Problem 1 - Calorie Counting

void AbsorbCalorieCountingTotal(BigInt newTotal, BigIntList& biggestTotalsList)
{
    if (newTotal < biggestTotalsList[biggestTotalsList.size() - 1])
        return;

    auto iter = biggestTotalsList.begin();
    while ((iter != biggestTotalsList.end()) && (newTotal < *iter))
        ++iter;

    biggestTotalsList.insert(iter, newTotal);
    biggestTotalsList.pop_back();
}

void PrintCalorieCountingBiggestTotalsList(const BigIntList& biggestTotalsList)
{
    BigInt totalTotal = 0;
    printf("Biggest %lld totals:  ", (BigInt)biggestTotalsList.size());
    for (BigInt currTotal: biggestTotalsList)
    {
        printf("%lld ", currTotal);
        totalTotal += currTotal;
    }
    printf(" (total %lld)\n", totalTotal);
}

void RunCalorieCountingOnLines(const StringList& lines)
{
    BigIntList biggestTotalsList = { 0, 0, 0 };

    BigInt currentTotal = 0;

    for (const std::string& line: lines)
    {
        if (line.empty())
        {
            printf("Found elf with total %lld\n", currentTotal);
            AbsorbCalorieCountingTotal(currentTotal, biggestTotalsList);
            PrintCalorieCountingBiggestTotalsList(biggestTotalsList);
            printf("\n");

            currentTotal = 0;
        }
        else
        {
            currentTotal += atoll(line.c_str());
        }
    }

    printf("Found elf with total %lld\n", currentTotal);
    AbsorbCalorieCountingTotal(currentTotal, biggestTotalsList);
    PrintCalorieCountingBiggestTotalsList(biggestTotalsList);
    printf("\n");
}

void RunCalorieCountingTest(std::initializer_list<const char*> list)
{
    StringList lines;
    for (const char* oneLine: list)
    {
        lines.push_back(oneLine);
    }
    RunCalorieCountingOnLines(lines);
}

void RunCalorieCountingInputFile()
{
    StringList lines;
    ReadFileLines("Day1Input.txt", lines);
    RunCalorieCountingOnLines(lines);
}

void RunCalorieCounting()
{
    printf("Test:\n\n");
    RunCalorieCountingTest({ "1000", "2000", "3000", "", "4000", "", "5000", "6000", "", "7000", "8000", "9000", "", "10000" });

    printf("\n\nInput:\n\n");
    RunCalorieCountingInputFile();
}


////////////////////////////
////////////////////////////
// Main

int main(int argc, char** argv)
{
    if (argc <= 1)
    {
        printf(
            "Usages:\n"
            "  AdventOfCode2022 <problem#>\n");
        return 0;
    }

    const char* problemArg = argv[1];
    int problemNum = atoi(problemArg);
    printf("Solving problem #%d\n\n", problemNum);
    switch (problemNum)
    {
        case 1:
            RunCalorieCounting();
            break;
        default:
            printf("'%s' is not a valid problem number!\n\n", problemArg);
            break;
    }

    return 0;
}
