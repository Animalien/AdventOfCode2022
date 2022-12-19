///////////////////////////
// Problem 16 - Proboscidea Volcanium

#include "AdventOfCode2022.h"

class Problem16 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 16; }

    virtual void Run() override
    {
        RunOnData("Day16Example.txt", true);
        RunOnData("Day16Input.txt", false);
    }

private:
    struct Node
    {
        char ch1 = '\0';
        char ch2 = '\0';

        BigInt flowRate = 0;

        BigIntList adjacentNodeIndices;

        BigInt significantIndex = -1;
    };

    BigIntUnorderedMap m_charHashToNodeIndexMap;
    BigInt m_nextNodeIndex = 0;
    std::vector<Node> m_nodeList;
    BigIntList m_significantNodeIndexList;
    BigIntListList m_signifToSignifDistGrid;

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        BigInt startIndex = -1;
        BuildNodeList(lines, startIndex, verbose);

        BuildSignifToSignifDistGrid(verbose);

        const BigInt mostPressure = CalcMostPressureCanRelease();
        printf("The most pressure that can be released = %lld\n\n", mostPressure);
    }

    void BuildNodeList(const StringList& lines, BigInt& startIndex, bool verbose)
    {
        if (verbose)
            printf("Building node list\n");

        m_nodeList.clear();
        m_nextNodeIndex = 0;
        m_nodeList.resize(lines.size());
        m_significantNodeIndexList.clear();
        m_significantNodeIndexList.reserve(lines.size());
        m_significantNodeIndexList.push_back(-1);

        for (const std::string& line: lines)
        {
            const char* st = line.c_str();
            st += 6;   // "Valve "

            const char ch1 = *st++;
            const char ch2 = *st++;

            if (verbose)
                printf("  Creating node '%c%c'\n", (int)ch1, (int)ch2);

            const BigInt nodeIndex = CharCharToNodeIndex(ch1, ch2);
            Node& newNode = m_nodeList[nodeIndex];
            newNode.ch1 = ch1;
            newNode.ch2 = ch2;

            if (verbose)
                printf("    Gets node index %lld\n", nodeIndex);

            bool isStartNode = false;
            if ((ch1 == 'A') && (ch2 == 'A'))
            {
                startIndex = nodeIndex;
                isStartNode = true;

                if (verbose)
                    printf("    Is START node!\n");
            }

            st += 15;   // " has flow rate="

            ParseNextBigInt(st, newNode.flowRate);

            if (verbose)
                printf("    Has flow rate %lld\n", newNode.flowRate);

            if (isStartNode)
            {
                newNode.significantIndex = 0;
                m_significantNodeIndexList[0] = nodeIndex;
            }
            else if (newNode.flowRate > 0)
            {
                newNode.significantIndex = (BigInt)m_significantNodeIndexList.size();
                m_significantNodeIndexList.push_back(nodeIndex);
            }

            if (verbose)
                printf("    Gets significant index %lld\n", newNode.significantIndex);

            st += 23;   // "; tunnels lead to valve"

            if (*st == 's')
                ++st;
            ++st;   // skip ' '

            if (verbose)
                printf("    Has adjacent nodes:  ");

            BigInt charIndex = st - line.c_str();
            while (charIndex < (BigInt)line.length())
            {
                const char adjacentCh1 = line[charIndex];
                const char adjacentCh2 = line[charIndex + 1];
                const BigInt adjacentNodeIndex = CharCharToNodeIndex(adjacentCh1, adjacentCh2);
                newNode.adjacentNodeIndices.push_back(adjacentNodeIndex);

                if (verbose)
                    printf("%lld('%c%c') ", adjacentNodeIndex, (int)adjacentCh1, (int)adjacentCh2);

                charIndex += 4;   // "XX, "
            }

            if (verbose)
                printf("\n");
        }

        if (verbose)
            printf("\nNumber of significant nodes = %lld\n\n", (BigInt)m_significantNodeIndexList.size());
    }

    void BuildSignifToSignifDistGrid(bool verbose)
    {
        const BigInt numSignificantNodes = m_significantNodeIndexList.size();

        m_signifToSignifDistGrid.clear();
        m_signifToSignifDistGrid.resize(numSignificantNodes);
        for (BigIntList& gridRow: m_signifToSignifDistGrid)
            gridRow.resize(numSignificantNodes, -1);

        if (verbose)
            printf("Filling up significant to significant distance grid:\n");

        for (BigInt sourceSignifIndex = 0; sourceSignifIndex < numSignificantNodes; ++sourceSignifIndex)
        {
            for (BigInt destSignifIndex = sourceSignifIndex + 1; destSignifIndex < numSignificantNodes; ++destSignifIndex)
            {
                const BigInt dist = CalcDistFromNodeToNode(
                    m_significantNodeIndexList[sourceSignifIndex], m_significantNodeIndexList[destSignifIndex]);
                SetSignifToSignifDistInGrid(sourceSignifIndex, destSignifIndex, dist);

                if (verbose)
                    printf("  Distance from signif %lld to signif %lld = %lld\n", sourceSignifIndex, destSignifIndex, dist);
            }
        }
    }

    BigInt CalcDistFromNodeToNode(BigInt sourceNodeIndex, BigInt destNodeIndex) const
    {
        BoolList visitedList;
        visitedList.resize(m_nodeList.size(), false);
        BigInt shortestTotalDistance = -1;
        RecursiveDistFromNodeToNode(visitedList, sourceNodeIndex, destNodeIndex, 0, shortestTotalDistance);
        return shortestTotalDistance;
    }

    void RecursiveDistFromNodeToNode(
        BoolList& visitedList,
        BigInt sourceNodeIndex,
        BigInt destNodeIndex,
        BigInt distSoFar,
        BigInt& shortestTotalDistance) const
    {
        // check to see if next-dist-so-far is already going to be larger than the current shortest; if so, exit

        const BigInt nextDistSoFar = distSoFar + 1;
        if ((shortestTotalDistance > 0) && (nextDistSoFar > shortestTotalDistance))
            return;

        // check to see if the destination is next door

        const Node& currNode = m_nodeList[sourceNodeIndex];
        for (BigInt adjacentIndex: currNode.adjacentNodeIndices)
        {
            if (adjacentIndex == destNodeIndex)
            {
                if ((shortestTotalDistance < 0) || (nextDistSoFar < shortestTotalDistance))
                    shortestTotalDistance = nextDistSoFar;
                return;
            }
        }

        // visit non-visited adjacent nodes to recursively check for shortest distance

        visitedList[sourceNodeIndex] = true;
        for (BigInt adjacentIndex: currNode.adjacentNodeIndices)
        {
            if (visitedList[adjacentIndex])
                continue;

            RecursiveDistFromNodeToNode(visitedList, adjacentIndex, destNodeIndex, nextDistSoFar, shortestTotalDistance);
        }
        visitedList[sourceNodeIndex] = false;
    }

    void SetSignifToSignifDistInGrid(BigInt sourceSignificantIndex, BigInt destSignificantIndex, BigInt dist)
    {
        m_signifToSignifDistGrid[sourceSignificantIndex][destSignificantIndex] =
            m_signifToSignifDistGrid[destSignificantIndex][sourceSignificantIndex] = dist;
    }

    BigInt CharCharToNodeIndex(char ch1, char ch2)
    {
        const BigInt hash = CalcCharHash(ch1, ch2);
        const auto find_iter = m_charHashToNodeIndexMap.find(hash);
        if (find_iter != m_charHashToNodeIndexMap.end())
            return find_iter->second;

        const BigInt index = m_nextNodeIndex++;
        m_charHashToNodeIndexMap[hash] = index;
        return index;
    }

    static BigInt CalcCharHash(char ch1, char ch2) { return (((BigInt)ch1) << 8) | (BigInt)ch2; }

    BigInt CalcMostPressureCanRelease() const
    {
        BoolList visitedList;
        visitedList.resize(m_significantNodeIndexList.size(), false);

        BigInt mostTotalPressure = 0;
        RecursiveCalcPressureCanRelease(visitedList, 0, 30, 0, mostTotalPressure);

        return mostTotalPressure;
    }

    void RecursiveCalcPressureCanRelease(
        BoolList& visitedList,
        BigInt currSignifIndex,
        BigInt minutesRemaining,
        BigInt currTotalPressure,
        BigInt& mostTotalPressure) const
    {
        visitedList[currSignifIndex] = true;

        for (BigInt nextSignifIndex = 0; nextSignifIndex < (BigInt)m_significantNodeIndexList.size(); ++nextSignifIndex)
        {
            if ((nextSignifIndex == currSignifIndex) || visitedList[nextSignifIndex])
                continue;

            const BigInt dist = m_signifToSignifDistGrid[currSignifIndex][nextSignifIndex];
            const BigInt timeToOpenNextValve = dist + 1;

            if (timeToOpenNextValve > minutesRemaining)
                continue;

            const BigInt nextMinutesRemaining = minutesRemaining - timeToOpenNextValve;
            const BigInt nextValvePressure =
                nextMinutesRemaining * m_nodeList[m_significantNodeIndexList[nextSignifIndex]].flowRate;
            const BigInt nextValveTotalPressure = currTotalPressure + nextValvePressure;

            if (nextValveTotalPressure > mostTotalPressure)
                mostTotalPressure = nextValveTotalPressure;

            if (nextMinutesRemaining > 0)
                RecursiveCalcPressureCanRelease(
                    visitedList,
                    nextSignifIndex,
                    nextMinutesRemaining,
                    nextValveTotalPressure,
                    mostTotalPressure);
        }

        visitedList[currSignifIndex] = false;
    }
};

Problem16 problem16;
