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
    struct Node
    {
        BigInt sourceIndex = -1;
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

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        BigInt sumIndicesOfOrderedPairs = 0;

        const BigInt totalInputNodes = ((lines.size() + 1) / 3) * 2;
        const BigInt totalNumNodes = totalInputNodes + 2;
        std::vector<Node> masterNodeList;
        masterNodeList.resize(totalNumNodes);

        std::vector<const Node*> nodeSortList;
        nodeSortList.reserve(totalNumNodes);
        for (Node& node: masterNodeList)
            nodeSortList.push_back(&node);

        // part 1

        for (BigInt lineIndex = 0, nodeIndex = 0; lineIndex < (BigInt)lines.size(); lineIndex += 3, nodeIndex += 2)
        {
            const std::string& leftLine = lines[lineIndex];
            const std::string& rightLine = lines[lineIndex + 1];

            if (verbose)
                printf("Testing whether these two lines are in order:\n  %s\n  %s\n", leftLine.c_str(), rightLine.c_str());

            Node& leftNode = masterNodeList[nodeIndex];
            leftNode.sourceIndex = lineIndex;
            ConvertLineToNode(leftLine, leftNode, verbose);

            Node& rightNode = masterNodeList[nodeIndex + 1];
            rightNode.sourceIndex = lineIndex + 1;
            ConvertLineToNode(rightLine, rightNode, verbose);

            if (CompareTwoNodes(leftNode, rightNode, verbose) < 0)
            {
                const BigInt pairIndex = (lineIndex / 3) + 1;
                sumIndicesOfOrderedPairs += pairIndex;

                if (verbose)
                    printf("  Pair %lld is in order, so it is added to the running sum\n", pairIndex);
            }
        }

        printf("Total sum of indices of ordered pairs = %lld\n\n", sumIndicesOfOrderedPairs);

        // part 2

        // add in the two divider packets

        const char* const dividerPacket1Line = "[[2]]";
        Node& dividerPacket1 = masterNodeList[totalInputNodes];
        ConvertLineToNode(dividerPacket1Line, dividerPacket1, verbose);

        const char* const dividerPacket2Line = "[[6]]";
        Node& dividerPacket2 = masterNodeList[totalInputNodes + 1];
        ConvertLineToNode(dividerPacket2Line, dividerPacket2, verbose);

        // now sort

        SortNodes(nodeSortList);

        if (verbose)
            printf("Sorted line list:\n");

        BigInt dividerPacket1SortedIndex = -1;
        BigInt dividerPacket2SortedIndex = -1;
        for (BigInt sortedIndex = 0; sortedIndex < (BigInt)nodeSortList.size(); ++sortedIndex)
        {
            const Node* node = nodeSortList[sortedIndex];

            if (node == &dividerPacket1)
            {
                dividerPacket1SortedIndex = sortedIndex + 1;

                if (verbose)
                    printf("  %s\n", dividerPacket1Line);
            }
            else if (node == &dividerPacket2)
            {
                dividerPacket2SortedIndex = sortedIndex + 1;

                if (verbose)
                    printf("  %s\n", dividerPacket2Line);
            }
            else
            {
                if (verbose)
                    printf("  %s\n", lines[node->sourceIndex].c_str());
            }
        }

        printf(
            "Decoder key = %lld * %lld = %lld\n\n",
            dividerPacket1SortedIndex,
            dividerPacket2SortedIndex,
            dividerPacket1SortedIndex * dividerPacket2SortedIndex);
    }

    static void ConvertLineToNode(const std::string& line, Node& node, bool verbose)
    {
        ConvertLineToNode(line.c_str(), node, verbose);
    }

    static void ConvertLineToNode(const char* line, Node& node, bool verbose)
    {
        if (verbose)
            printf("  Converting line %s:\n", line);

        const char* s = line + 1;   // start after the initial [
        RecursiveConvertStringToNode(s, node, verbose);
    }

    static void RecursiveConvertStringToNode(const char*& s, Node& node, bool verbose)
    {
        node.nodeList.clear();
        for (;;)
        {
            if (isdigit(*s))
            {
                BigInt num = 0;
                ParseNextBigInt(s, num);
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

            assert(*s == ']');   // step past this in the above level
            break;
        }
    }

    static BigInt CompareTwoNodes(const Node& left, const Node& right, bool verbose)
    {
        if (left.IsLiteralValue())
        {
            if (right.IsLiteralValue())
            {
                if (verbose)
                    printf(
                        "  left and right are both literals, so comparing %lld vs %lld\n", left.literalValue, right.literalValue);

                if (left.literalValue < right.literalValue)
                    return -1;
                if (left.literalValue > right.literalValue)
                    return +1;
                return 0;
            }
            else
            {
                if (verbose)
                    printf("  left is literal, right is list, so we have to wrap left and try again\n");

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
                    printf(
                        "  comparing two lists of lengths %lld vs %lld\n",
                        (BigInt)left.nodeList.size(),
                        (BigInt)right.nodeList.size());

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

    struct NodeSorter
    {
        bool operator()(const Node* lhs, const Node* rhs) const { return (CompareTwoNodes(*lhs, *rhs, false) < 0); }
    };

    void SortNodes(std::vector<const Node*>& nodeSortList) { std::sort(nodeSortList.begin(), nodeSortList.end(), NodeSorter()); }
};

Problem13 problem13;
