///////////////////////////
// Problem 8 - Treetop Tree House

#include "AdventOfCode2022.h"

class Problem8 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 8; }

    virtual void Run() override
    {
        RunOnData("Day8Example.txt", true);
        RunOnData("Day8Input.txt", false);
    }

private:
    enum Direction
    {
        WEST,
        NORTH,
        EAST,
        SOUTH,
    };

    typedef std::vector<uint8_t> TreeStatusGridRow;
    typedef std::vector<TreeStatusGridRow> TreeStatusGrid;

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        const BigInt gridSizeX = (BigInt)lines[0].length();
        const BigInt gridSizeY = (BigInt)lines.size();

        // initialize status grid

        TreeStatusGrid treeStatusGrid;
        treeStatusGrid.resize(gridSizeY);
        for (TreeStatusGridRow& row: treeStatusGrid)
        {
            row.resize(gridSizeX, 0);
        }

        // recurse through trees, tracing visibility and taking advantage of previous visibility calculations where possible to shortcut checks

        BigInt totalNumVisibleTrees = 0;

        for (BigInt treeY = 0; treeY < gridSizeY; ++treeY)
        {
            for (BigInt treeX = 0; treeX < gridSizeX; ++treeX)
            {
                if (verbose)
                    printf("Looking at tree <%lld,%lld>\n", treeX, treeY);

                const char sourceHeight = lines[treeY][treeX];

                bool isVisible = false;

                if (TraceForVisibility(gridSizeX, gridSizeY, lines, treeStatusGrid, treeX, treeY, WEST, sourceHeight, verbose))
                {
                    isVisible = true;
                    treeStatusGrid[treeY][treeX] |= (1 << WEST);
                }

                if (TraceForVisibility(gridSizeX, gridSizeY, lines, treeStatusGrid, treeX, treeY, NORTH, sourceHeight, verbose))
                {
                    isVisible = true;
                    treeStatusGrid[treeY][treeX] |= (1 << NORTH);
                }

                if (TraceForVisibility(gridSizeX, gridSizeY, lines, treeStatusGrid, treeX, treeY, EAST, sourceHeight, verbose))
                {
                    isVisible = true;
                    treeStatusGrid[treeY][treeX] |= (1 << EAST);
                }

                if (TraceForVisibility(gridSizeX, gridSizeY, lines, treeStatusGrid, treeX, treeY, SOUTH, sourceHeight, verbose))
                {
                    isVisible = true;
                    treeStatusGrid[treeY][treeX] |= (1 << SOUTH);
                }

                if (isVisible)
                {
                    ++totalNumVisibleTrees;

                    if (verbose)
                        printf("  Tree is VISIBLE.  total num visible = %lld\n", totalNumVisibleTrees);
                }
                else
                {
                    if (verbose)
                        printf("  Tree is NOT visible.  total num visible = %lld\n", totalNumVisibleTrees);
                }
            }
        }

        printf("Total number of visible trees = %lld\n", totalNumVisibleTrees);

        if (verbose)
        {
            printf("Tree visibilities:\n");
            for (BigInt treeY = 0; treeY < gridSizeY; ++treeY)
            {
                printf("  ");
                for (BigInt treeX = 0; treeX < gridSizeX; ++treeX)
                {
                    printf("%02u", treeStatusGrid[treeY][treeX]);
                }
                printf("\n");
            }
            printf("\n");
        }
    }

    bool TraceForVisibility(
        BigInt gridSizeX,
        BigInt gridSizeY,
        const StringList& lines,
        const TreeStatusGrid& treeStatusGrid,
        BigInt currX,
        BigInt currY,
        Direction stepDir,
        char sourceHeight,
        bool verbose)
    {
        static const BigInt s_stepXList[] = { -1, 0, +1, 0 };
        static const BigInt s_stepYList[] = { 0, -1, 0, +1 };

        const BigInt stepX = s_stepXList[stepDir];
        const BigInt stepY = s_stepYList[stepDir];

        currX += stepX;
        currY += stepY;

        if (verbose)
            printf("  stepping in direction <%lld,%lld> to cell <%lld,%lld>\n", stepX, stepY, currX, currY);

        bool steppedCellIsVisible = false;

        if ((currX < 0) || (currX >= gridSizeX))
        {
            steppedCellIsVisible = true;
            if (verbose)
                printf("  reached edge via X = %lld, so visibility is TRUE!\n", currX);
        }
        else if ((currY < 0) || (currY >= gridSizeY))
        {
            steppedCellIsVisible = true;
            if (verbose)
                printf("  reached edge via Y = %lld, so visibility is TRUE!\n", currY);
        }
        else if (lines[currY][currX] >= sourceHeight)
        {
            if (verbose)
                printf(
                    "  reached a tree which has height %c >= source height %c, so visibility is FALSE!\n",
                    (int)lines[currY][currX],
                    (int)sourceHeight);
        }
        else if (treeStatusGrid[currY][currX] & (1 << stepDir))
        {
            steppedCellIsVisible = true;
            if (verbose)
                printf(
                    "  reached a tree which was already determined to be visible in the step direction, so visibility is TRUE!\n");
        }
        else
        {
            steppedCellIsVisible =
                TraceForVisibility(gridSizeX, gridSizeY, lines, treeStatusGrid, currX, currY, stepDir, sourceHeight, verbose);
        }

        return steppedCellIsVisible;
    }
};

Problem8 problem8;
