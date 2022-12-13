///////////////////////////
// Problem 13 - Distress Signal

#include "AdventOfCode2022.h"

class Problem13 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 13; }

    virtual void Run() override
    {
        RunOnData("Day13Example.txt", true);
        RunOnData("Day13Input.txt", false);
    }

private:
    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        BigInt sumIndicesOfOrderedPairs = 0;

        for (BigInt lineIndex = 0; lineIndex < (BigInt)lines.size(); lineIndex += 3)
        {
            if (TwoLinesAreInOrder(lines[lineIndex], lines[lineIndex + 1], verbose))
            {
                const BigInt pairIndex = (lineIndex / 3) + 1;
                sumIndicesOfOrderedPairs += pairIndex;

                if (verbose)
                    printf("  Pair %lld is in order, so it is added to the running sum\n", pairIndex);
            }
        }

        printf("Total sum of indices of ordered pairs = %lld\n\n", sumIndicesOfOrderedPairs);
    }

    struct Node
    {
        bool isLiteralValue = false;
        BigInt literalValue = 0;
        std::vector<Node> nodeList;

        Node() = default;
        Node(BigInt value) : isLiteralValue(true), literalValue(value) {}

        bool IsLiteralValue() const { return isLiteralValue; }

        static Node WrapNode(const Node& node)
        {
            Node newNode;
            newNode.nodeList.push_back(node);
            return newNode;
        }
    };

    bool TwoLinesAreInOrder(const std::string& leftLine, const std::string& rightLine, bool verbose)
    {
        if (verbose)
            printf("Testing whether these two lines are in order:\n  %s\n  %s\n", leftLine.c_str(), rightLine.c_str());

        Node left, right;
        ConvertLineToNode(leftLine, left, verbose);
        ConvertLineToNode(rightLine, right, verbose);
        return (CompareTwoNodes(left, right, verbose) < 0);
    }

    void ConvertLineToNode(const std::string& line, Node& node, bool verbose)
    {
        if (verbose)
            printf("  Converting line %s:\n", line.c_str());

        const char* s = &line[1];
        RecursiveConvertStringToNode(s, node, verbose);
    }

    void RecursiveConvertStringToNode(const char*& s, Node& node, bool verbose)
    {
        node.nodeList.clear();
        for (;;)
        {
            if (isdigit(*s))
            {
                BigInt num = 0;
                ParseInt(s, num);
                node.nodeList.push_back(Node(num));

                if (verbose)
                    printf("    Found num %lld\n", num);

                if (*s == ',')
                    ++s;

                continue;
            }

            if (*s == '[')
            {
                if (verbose)
                    printf("    Found start of list [:\n");

                ++s;
                Node nestedNode;
                RecursiveConvertStringToNode(s, nestedNode, verbose);
                node.nodeList.push_back(nestedNode);
                assert(*s == ']');

                if (verbose)
                    printf("    Finished list ]\n");

                ++s;
                if (*s == ',')
                    ++s;

                continue;
            }

            assert(*s == ']');  // step past this in the above level
            break;
        }
    }

    void ParseInt(const char*& s, BigInt& num)
    {
        num = 0;
        while (isdigit(*s))
        {
            const BigInt digit = (BigInt)*s - '0';
            num *= 10;
            num += digit;
            ++s;
        }
    }

    BigInt CompareTwoNodes(const Node& left, const Node& right, bool verbose)
    {
        if (left.IsLiteralValue())
        {
            if (right.IsLiteralValue())
            {
                if (verbose)
                    printf(
                        "  left and right are both literals, so comparing %lld vs %lld\n",
                        left.literalValue,
                        right.literalValue);

                if (left.literalValue < right.literalValue)
                    return -1;
                if (left.literalValue > right.literalValue)
                    return +1;
                return 0;
            }
            else
            {
                if (verbose)
                    printf(
                        "  left is literal, right is list, so we have to wrap left and try again\n");

                return CompareTwoNodes(Node::WrapNode(left), right, verbose);
            }
        }
        else
        {
            if (right.IsLiteralValue())
            {
                if (verbose)
                    printf("  left is list, right is literal, so we have to wrap right and try again\n");

                return CompareTwoNodes(left, Node::WrapNode(right), verbose);
            }
            else
            {
                if (verbose)
                    printf("  comparing two lists of lengths %lld vs %lld\n", (BigInt)left.nodeList.size(), (BigInt)right.nodeList.size());

                BigInt index = 0;
                for (;;)
                {
                    if (index >= (BigInt)left.nodeList.size())
                    {
                        if (index >= (BigInt)right.nodeList.size())
                        {
                            if (verbose)
                                printf("  both lists ended, and compare as same\n");

                            return 0;
                        }
                        else
                        {
                            if (verbose)
                                printf("  left list ended, but right continues, so left is less than\n");

                            return -1;
                        }
                    }
                    else
                    {
                        if (index >= (BigInt)right.nodeList.size())
                        {
                            if (verbose)
                                printf("  left list continues, but right ended, so left is greater than\n");

                            return +1;
                        }
                        else
                        {
                            // both lists continue, so compare their next list members

                            if (verbose)
                                printf("  comparing left list member to right list member, at index %lld\n", index);

                            const BigInt compareMember = CompareTwoNodes(left.nodeList[index], right.nodeList[index], verbose);
                            if (compareMember != 0)
                            {
                                // found a member that tips the balance one way or the other
                                return compareMember;
                            }

                            // otherwise we must continue comparing list members
                            ++index;
                        }
                    }
                }
            }
        }
    }
};

Problem13 problem13;
