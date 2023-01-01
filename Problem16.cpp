///////////////////////////
// Problem 16 - Proboscidea Volcanium

#include "AdventOfCode2022.h"

class Problem16 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 16; }

    virtual void Run() override
    {
        //TestPermutationIterator(15);
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

    class IndexIterator
    {
    public:
        IndexIterator(BigInt firstIndex, BigInt lastIndex)
            : m_firstIndex(firstIndex), m_lastIndex(lastIndex), m_fullPathLength(lastIndex - firstIndex + 1)
        {
            assert(m_firstIndex >= 0);
            assert(m_lastIndex >= m_firstIndex);
            m_visitedList.resize(m_lastIndex + 1, false);
            m_currPath.reserve(m_fullPathLength);
            m_currPath.push_back(m_firstIndex - 1);
        }

        BigInt GetCurrCandidateIndexForThisStep() const { return m_currPath.back(); }
        bool GetIndexForPrevStep(BigInt& index) const
        {
            if (m_currPath.size() <= 1)
                return false;

            index = m_currPath[m_currPath.size() - 2];

            return true;
        }

        bool FindNextCandidateIndexForThisStep()
        {
            BigInt& index = m_currPath.back();
            if (index > m_lastIndex)
                return false;

            for (;;)
            {
                ++index;
                if (index > m_lastIndex)
                    return false;
                if (!m_visitedList[index])
                    break;
            }

            return true;
        }

        bool ProgressToNextStep()
        {
            if ((BigInt)m_currPath.size() >= m_fullPathLength)
                return false;

            m_visitedList[m_currPath.back()] = true;
            m_currPath.push_back(m_firstIndex - 1);

            return true;
        }

        bool BackUpOneStep()
        {
            if ((BigInt)m_currPath.size() <= 1)
                return false;

            m_currPath.pop_back();
            m_visitedList[m_currPath.back()] = false;

            return true;
        }

        const BigIntList& GetCurrPath() const { return m_currPath; }

    private:
        BigInt m_firstIndex = 0;
        BigInt m_lastIndex = 0;
        BigInt m_fullPathLength = 0;

        BoolList m_visitedList;
        BigIntList m_currPath;
    };

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        BigInt startIndex = -1;
        BuildNodeList(lines, startIndex, verbose);

        BuildSignifToSignifDistGrid(verbose);

        const BigInt mostPressure = CalcMostPressureCanRelease(30, 1, true);
        printf("The most pressure that can be released in part one = %lld\n", mostPressure);

        const BigInt mostPressurePartTwo = CalcMostPressureCanRelease(26, 2, verbose);
        printf("The most pressure that can be released in part two = %lld\n\n", mostPressurePartTwo);
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

    BigInt CalcMostPressureCanRelease(BigInt minutesAvailable, BigInt numSearchers, bool verbose) const
    {
#if 1

        struct Searcher
        {
            Searcher(BigInt minutesAvailable) : minutesRemaining(minutesAvailable) {}

            BigInt currSigIndex = 0;
            BigInt minutesRemaining = 0;
        };

        struct Step
        {
            BigInt searcherIndex = -1;
            BigInt fromSigIndex = 0;
            BigInt toSigIndex = 0;
            BigInt timeTaken = 0;
            BigInt pressureContributed = 0;
        };

        std::vector<Searcher> searcherList;
        searcherList.resize(numSearchers, Searcher(minutesAvailable));

        std::vector<Step> stepStack;
        stepStack.reserve(m_significantNodeIndexList.size());

        BigInt mostPressure = 0;
        std::vector<Step> bestPath;

        BigInt currPressure = 0;
        IndexIterator ii(1, m_significantNodeIndexList.size() - 1);

        for (;;)
        {
            bool allDone = false;
            for (;;)
            {
                if (ii.FindNextCandidateIndexForThisStep())
                    break;

                if (!ii.BackUpOneStep())
                {
                    allDone = true;
                    break;
                }

                const Step& stepTop = stepStack.back();
                Searcher& restoreSearcher = searcherList[stepTop.searcherIndex];
                restoreSearcher.currSigIndex = stepTop.fromSigIndex;
                restoreSearcher.minutesRemaining += stepTop.timeTaken;

                currPressure -= stepTop.pressureContributed;

                stepStack.pop_back();
            }

            if (allDone)
                break;

            const BigInt nextSignifIndex = ii.GetCurrCandidateIndexForThisStep();

            // find the searcher who is going to take the next step
            BigInt mostMinutesRemaining = -1;
            BigInt chosenSearcherIndex = -1;
            BigInt chosenTimeTaken = -1;
            BigInt chosenNextMinutesRemaining = -1;
            for (BigInt s = 0; s < numSearchers; ++s)
            {
                Searcher& thisSearcher = searcherList[s];
                const BigInt currSignifIndex = thisSearcher.currSigIndex;
                const BigInt dist = m_signifToSignifDistGrid[currSignifIndex][nextSignifIndex];
                const BigInt timeToOpenValve = dist + 1;

                const BigInt nextMinutesRemaining = thisSearcher.minutesRemaining - timeToOpenValve;
                if ((nextMinutesRemaining >= 0)
                    && ((mostMinutesRemaining < 0) || (thisSearcher.minutesRemaining > mostMinutesRemaining)))
                {
                    mostMinutesRemaining = thisSearcher.minutesRemaining;
                    chosenSearcherIndex = s;
                    chosenTimeTaken = timeToOpenValve;
                    chosenNextMinutesRemaining = nextMinutesRemaining;
                }
            }
            
            if (chosenSearcherIndex < 0)
                continue;

            Searcher& searcher = searcherList[chosenSearcherIndex];

            const BigInt pressureToContribute =
                chosenNextMinutesRemaining * m_nodeList[m_significantNodeIndexList[nextSignifIndex]].flowRate;

            Step newStep;
            newStep.searcherIndex = chosenSearcherIndex;
            newStep.fromSigIndex = searcher.currSigIndex;
            newStep.toSigIndex = nextSignifIndex;
            newStep.timeTaken = chosenTimeTaken;
            newStep.pressureContributed = pressureToContribute;
            stepStack.push_back(newStep);

            searcher.minutesRemaining = chosenNextMinutesRemaining;
            searcher.currSigIndex = nextSignifIndex;
            currPressure += pressureToContribute;

            if (currPressure > mostPressure)
            {
                mostPressure = currPressure;
                bestPath = stepStack;
            }

            if (!ii.ProgressToNextStep())
            {
                currPressure -= pressureToContribute;
                searcher.currSigIndex = stepStack.back().fromSigIndex;
                searcher.minutesRemaining += chosenTimeTaken;
                stepStack.pop_back();
                continue;
            }
        }

        //if (verbose)
        {
            printf("Best path:\n");
            BigInt totalPressure = 0;
            for (const Step& step: bestPath)
            {
                totalPressure += step.pressureContributed;
                printf(
                    "  Searcher %lld went from %lld to %lld, taking time %lld, contributing pressure %lld to total pressure %lld\n",
                    step.searcherIndex,
                    step.fromSigIndex,
                    step.toSigIndex,
                    step.timeTaken,
                    step.pressureContributed,
                    totalPressure);
            }
            printf("\n");
        }

        return mostPressure;
        

#elif 1

        BigInt mostPressure = 0;
        BigIntList bestPath;

        IndexIterator ii(1, m_significantNodeIndexList.size() - 1);

        BigInt currPressure = 0;
        BigIntList currPressureStack;
        BigInt minutesRemaining = minutesAvailable;
        BigIntList minutesRemainingStack;

        for (;;)
        {
            bool allDone = false;
            for (;;)
            {
                if (ii.FindNextCandidateIndexForThisStep())
                    break;

                if (!ii.BackUpOneStep())
                {
                    allDone = true;
                    break;
                }

                currPressure = currPressureStack.back();
                currPressureStack.pop_back();

                minutesRemaining = minutesRemainingStack.back();
                minutesRemainingStack.pop_back();
            }

            if (allDone)
                break;

            BigInt currSignifIndex = 0;
            ii.GetIndexForPrevStep(currSignifIndex);
            const BigInt nextSignifIndex = ii.GetCurrCandidateIndexForThisStep();
            const BigInt dist = m_signifToSignifDistGrid[currSignifIndex][nextSignifIndex];
            const BigInt timeToOpenNextValve = dist + 1;
            if (timeToOpenNextValve > minutesRemaining)
                continue;

            minutesRemaining -= timeToOpenNextValve;
            const BigInt pressureToContribute =
                minutesRemaining * m_nodeList[m_significantNodeIndexList[nextSignifIndex]].flowRate;
            currPressure += pressureToContribute;

            if (currPressure > mostPressure)
            {
                mostPressure = currPressure;
                bestPath = ii.GetCurrPath();
            }

            if (!ii.ProgressToNextStep())
            {
                currPressure -= pressureToContribute;
                minutesRemaining += timeToOpenNextValve;
                continue;
            }

            currPressureStack.push_back(currPressure);
            minutesRemainingStack.push_back(minutesRemaining);
        }

        if (verbose)
        {
            BigInt total = 0;
            PrintBestPath("My", minutesAvailable, bestPath, total);
        }

        return mostPressure;


#elif 1
        BigIntListPermutationIterator pathIter(1, m_significantNodeIndexList.size() - 1);

        BigInt mostTotalPressure = 0;
        //bestPath.clear();

        while (pathIter.HaveMorePermutations())
        {
            BigInt currTotalPressure = 0;
            BigInt minutesRemaining = minutesAvailable;

            const BigIntList& currPath = pathIter.GetCurrentPermutation();
            BigInt currSignifIndex = 0;
            for (BigInt nextSignifIndex: currPath)
            {
                const BigInt dist = m_signifToSignifDistGrid[currSignifIndex][nextSignifIndex];
                const BigInt timeToOpenNextValve = dist + 1;
                if (timeToOpenNextValve > minutesRemaining)
                    break;

                minutesRemaining -= timeToOpenNextValve;
                currTotalPressure += minutesRemaining * m_nodeList[m_significantNodeIndexList[nextSignifIndex]].flowRate;

                currSignifIndex = nextSignifIndex;
            }

            if (currTotalPressure > mostTotalPressure)
            {
                mostTotalPressure = currTotalPressure;
                //bestPath = currPath;
            }

            pathIter.Step();
        }

        return mostTotalPressure;

#else

        BoolList visitedList;
        visitedList.resize(m_significantNodeIndexList.size(), false);

        BigInt mostTotalPressure = 0;
        RecursiveCalcPressureCanRelease(visitedList, 0, 30, 0, mostTotalPressure);

        return mostTotalPressure;

#endif
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
                    visitedList, nextSignifIndex, nextMinutesRemaining, nextValveTotalPressure, mostTotalPressure);
        }

        visitedList[currSignifIndex] = false;
    }

    BigInt CalcMostPressureCanReleasePartTwo(bool verbose) const
    {
        BoolList visitedList;
        visitedList.resize(m_significantNodeIndexList.size(), false);

        BigInt mostTotalPressure = 0;
        bool nextNodeFound = false;
        BigIntList myPath;
        myPath.push_back(0);
        BigIntList elephPath;
        elephPath.push_back(0);
        BigIntList myBestPath;
        BigIntList elephBestPath;
        RecursiveCalcPressureCanReleasePartTwo(
            visitedList, 0, 0, 26, 26, 0, mostTotalPressure, nextNodeFound, myPath, elephPath, myBestPath, elephBestPath);

        if (verbose)
        {
            BigInt total = 0;
            PrintBestPath("My", 26, myBestPath, total);
            PrintBestPath("Elephant's", 26, elephBestPath, total);
        }

        return mostTotalPressure;
    }

    void RecursiveCalcPressureCanReleasePartTwo(
        BoolList& visitedList,
        BigInt myCurrSignifIndex,
        BigInt elephCurrSignifIndex,
        BigInt myMinutesRemaining,
        BigInt elephMinutesRemaining,
        BigInt currTotalPressure,
        BigInt& mostTotalPressure,
        bool& nextNodeFound,
        BigIntList& myPath,
        BigIntList& elephPath,
        BigIntList& myBestPath,
        BigIntList& elephBestPath) const
    {
        nextNodeFound = false;

        for (BigInt nextSignifIndex = 1; nextSignifIndex < (BigInt)m_significantNodeIndexList.size(); ++nextSignifIndex)
        {
            if (visitedList[nextSignifIndex])
                continue;

            const BigInt nextNodeFlowRate = m_nodeList[m_significantNodeIndexList[nextSignifIndex]].flowRate;

            BigInt iCanReleaseThisMuchPressure = 0;
            BigInt iWillNeedThisMuchTime = 0;
            if (nextSignifIndex != myCurrSignifIndex)
            {
                const BigInt dist = m_signifToSignifDistGrid[myCurrSignifIndex][nextSignifIndex];
                iWillNeedThisMuchTime = dist + 1;
                if (iWillNeedThisMuchTime <= myMinutesRemaining)
                    iCanReleaseThisMuchPressure = myMinutesRemaining * nextNodeFlowRate;
            }

            BigInt elephCanReleaseThisMuchPressure = 0;
            BigInt elephWillNeedThisMuchTime = 0;
            if (nextSignifIndex != elephCurrSignifIndex)
            {
                const BigInt dist = m_signifToSignifDistGrid[elephCurrSignifIndex][nextSignifIndex];
                elephWillNeedThisMuchTime = dist + 1;
                if (elephWillNeedThisMuchTime <= elephMinutesRemaining)
                    elephCanReleaseThisMuchPressure = elephMinutesRemaining * nextNodeFlowRate;
            }

            if ((iCanReleaseThisMuchPressure <= 0) && (elephCanReleaseThisMuchPressure <= 0))
                continue;

            if (iCanReleaseThisMuchPressure > 0)
            {
                const BigInt myNextSignifIndex = nextSignifIndex;
                myPath.push_back(myNextSignifIndex);

                const BigInt myNextMinutesRemaining = myMinutesRemaining - iWillNeedThisMuchTime;

                const BigInt nextValveTotalPressure = currTotalPressure + iCanReleaseThisMuchPressure;
                if (nextValveTotalPressure > mostTotalPressure)
                {
                    mostTotalPressure = nextValveTotalPressure;
                    myBestPath = myPath;
                    elephBestPath = elephPath;
                }

                visitedList[nextSignifIndex] = true;

                nextNodeFound = true;
                bool nextNextNodeFound = false;
                RecursiveCalcPressureCanReleasePartTwo(
                    visitedList,
                    myNextSignifIndex,
                    elephCurrSignifIndex,
                    myNextMinutesRemaining,
                    elephMinutesRemaining,
                    nextValveTotalPressure,
                    mostTotalPressure,
                    nextNextNodeFound,
                    myPath,
                    elephPath,
                    myBestPath,
                    elephBestPath);

                myPath.pop_back();

                visitedList[nextSignifIndex] = false;
            }

            if (elephCanReleaseThisMuchPressure > 0)
            {
                const BigInt elephNextSignifIndex = nextSignifIndex;
                elephPath.push_back(elephNextSignifIndex);

                const BigInt elephNextMinutesRemaining = elephMinutesRemaining - elephWillNeedThisMuchTime;

                const BigInt nextValveTotalPressure = currTotalPressure + elephCanReleaseThisMuchPressure;
                if (nextValveTotalPressure > mostTotalPressure)
                {
                    mostTotalPressure = nextValveTotalPressure;
                    myBestPath = myPath;
                    elephBestPath = elephPath;
                }

                visitedList[nextSignifIndex] = true;

                nextNodeFound = true;
                bool nextNextNodeFound = false;
                RecursiveCalcPressureCanReleasePartTwo(
                    visitedList,
                    myCurrSignifIndex,
                    elephNextSignifIndex,
                    myMinutesRemaining,
                    elephNextMinutesRemaining,
                    nextValveTotalPressure,
                    mostTotalPressure,
                    nextNextNodeFound,
                    myPath,
                    elephPath,
                    myBestPath,
                    elephBestPath);

                elephPath.pop_back();

                visitedList[nextSignifIndex] = false;
            }
        }
    }

    void PrintBestPath(const char* who, BigInt minutesAvailable, const BigIntList& path, BigInt& total) const
    {
        printf("%s best path:\n  0\n", who);

        BigInt prevSigNodeIndex = 0;
        //-1;
        for (BigInt sigNodeIndex: path)
        {
            const BigInt dist = m_signifToSignifDistGrid[prevSigNodeIndex][sigNodeIndex];
            const BigInt timeNeeded = dist + 1;
            minutesAvailable -= timeNeeded;
            const BigInt contributes = minutesAvailable * m_nodeList[m_significantNodeIndexList[sigNodeIndex]].flowRate;
            total += contributes;

            printf(
                "  %lld (d = %lld, tn = %lld, fr = %lld, mr = %lld, c = %lld, tp = %lld)\n",
                sigNodeIndex,
                dist,
                timeNeeded,
                m_nodeList[m_significantNodeIndexList[sigNodeIndex]].flowRate,
                minutesAvailable,
                contributes,
                total);

            prevSigNodeIndex = sigNodeIndex;
        }
        printf("Total = %lld\n\n", total);
    }
};

Problem16 problem16;
