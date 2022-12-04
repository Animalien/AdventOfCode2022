///////////////////////////
// Problem 3 - Rucksack reorganization

#include "AdventOfCode2022.h"

class Problem3 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 3; }

    virtual void Run() override
    {
        RunOnData("Day3Example.txt", true);
        RunOnData("Day3Input.txt", false);
    }

    class ItemSet
    {
    public:
        ItemSet()
            : m_flags(0)
        {}

        void AddItemToSet(char item)
        {
            m_flags |= (1ULL << FindItemIndex(item));
        }
        void Reset() { m_flags = 0; }

        BigInt GetPriorityIfInSet(char item) const
        {
            const BigInt itemIndex = FindItemIndex(item);
            if (!(m_flags & (1ULL << itemIndex)))
                return 0;

            // priority "happens to be" the index + 1
            return itemIndex + 1;
        }

    private:
        static const BigInt NUM_LETTERS = 26;
        BigUInt m_flags = 0;

        static BigInt FindItemIndex(char item)
        {
            if (item >= 'a')
                return (BigInt)(item - 'a');

            return (BigInt)(item - 'A') + NUM_LETTERS;
        }
    };

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        ItemSet lineItemSet;
        ItemSet groupItemSets[2];
        bool foundGroupPrio = false;

        BigInt totalPrios = 0;
        BigInt totalGroupPrios = 0;

        for (BigInt lineIndex = 0, groupMemberIndex = 0; lineIndex < (BigInt)lines.size(); ++lineIndex, ++groupMemberIndex)
        {
            if (groupMemberIndex >= 3)
            {
                groupMemberIndex = 0;
                groupItemSets[0].Reset();
                groupItemSets[1].Reset();
                foundGroupPrio = false;
            }

            const std::string& line = lines[lineIndex];

            const BigInt len = (BigInt)line.length();
            const BigInt halfLen = len / 2;

            for (BigInt i = 0; i < halfLen; ++i)
            {
                lineItemSet.AddItemToSet(line[i]);

                if (!foundGroupPrio)
                {
                    if (groupMemberIndex < 2)
                    {
                        groupItemSets[groupMemberIndex].AddItemToSet(line[i]);
                    }
                    else
                    {
                        const BigInt groupPrio = groupItemSets[0].GetPriorityIfInSet(line[i]);
                        if ((groupPrio > 0) && (groupPrio == groupItemSets[1].GetPriorityIfInSet(line[i])))
                        {
                            totalGroupPrios += groupPrio;
                            foundGroupPrio = true;

                            if (verbose)
                                printf("Found '%c' in whole group, priority = %lld\n", (int)line[i], groupPrio);
                        }
                    }
                }
            }

            bool foundDupe = false;
            for (BigInt i = halfLen; i < len; ++i)
            {
                if (!foundDupe)
                {
                    const BigInt prio = lineItemSet.GetPriorityIfInSet(line[i]);
                    if (prio > 0)
                    {
                        if (verbose)
                            printf("In line '%s', found '%c' in both sides, priority = %lld\n", line.c_str(), (int)line[i], prio);

                        totalPrios += prio;
                        foundDupe = true;

                        if (foundGroupPrio)
                            break;
                    }
                }

                if (!foundGroupPrio)
                {
                    if (groupMemberIndex < 2)
                    {
                        groupItemSets[groupMemberIndex].AddItemToSet(line[i]);
                    }
                    else
                    {
                        const BigInt groupPrio = groupItemSets[0].GetPriorityIfInSet(line[i]);
                        if ((groupPrio > 0) && (groupPrio == groupItemSets[1].GetPriorityIfInSet(line[i])))
                        {
                            totalGroupPrios += groupPrio;
                            foundGroupPrio = true;

                            if (verbose)
                                printf("Found '%c' in whole group, priority = %lld\n", (int)line[i], groupPrio);

                            if (foundDupe)
                                break;
                        }
                    }
                }
            }

            lineItemSet.Reset();
        }

        printf("Total prios of dupe items = %lld\nTotal prios of groups = %lld\n\n", totalPrios, totalGroupPrios);
    }
};

Problem3 problem3;
