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

    typedef std::vector<bool> TreeVisGridRow;
    typedef std::vector<TreeVisGridRow> TreeVisGrid;

    typedef std::vector<BigInt> TreeScoreGridRow;
    typedef std::vector<TreeScoreGridRow> TreeScoreGrid;

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        const BigInt gridSizeX = (BigInt)lines[0].length();
        const BigInt gridSizeY = (BigInt)lines.size();

        // initialize tree vis and score grids

        TreeVisGrid treeVisGrid;
        treeVisGrid.resize(gridSizeY);
        for (TreeVisGridRow& row: treeVisGrid)
        {
            row.resize(gridSizeX, false);
        }

        TreeScoreGrid treeScoreGrid;
        treeScoreGrid.resize(gridSizeY);
        for (TreeScoreGridRow& row: treeScoreGrid)
        {
            row.resize(gridSizeX, 0);
        }

        // recurse through trees, tracing visibility and taking advantage of previous visibility calculations where possible to shortcut checks

        BigInt totalNumVisibleTrees = 0;
        BigInt largestTreeScore = 0;

        for (BigInt treeY = 0; treeY < gridSizeY; ++treeY)
        {
            for (BigInt treeX = 0; treeX < gridSizeX; ++treeX)
            {
                if (verbose)
                    printf("Looking at tree <%lld,%lld>\n", treeX, treeY);

                const char sourceHeight = lines[treeY][treeX];

                bool isVisible = false;
                BigInt treeScore = 1;
                BigInt treeScoreDir = 0;

                isVisible |= TraceFromTree(gridSizeX, gridSizeY, lines, treeX, treeY, WEST, sourceHeight, treeScoreDir, verbose);
                treeScore *= treeScoreDir;
                treeScoreDir = 0;
                if (verbose)
                    printf("  Score in WEST direction = %lld\n", treeScoreDir);

                isVisible |= TraceFromTree(gridSizeX, gridSizeY, lines, treeX, treeY, NORTH, sourceHeight, treeScoreDir, verbose);
                treeScore *= treeScoreDir;
                treeScoreDir = 0;
                if (verbose)
                    printf("  Score in NORTH direction = %lld\n", treeScoreDir);

                isVisible |= TraceFromTree(gridSizeX, gridSizeY, lines, treeX, treeY, EAST, sourceHeight, treeScoreDir, verbose);
                treeScore *= treeScoreDir;
                treeScoreDir = 0;
                if (verbose)
                    printf("  Score in EAST direction = %lld\n", treeScoreDir);

                isVisible |= TraceFromTree(gridSizeX, gridSizeY, lines, treeX, treeY, SOUTH, sourceHeight, treeScoreDir, verbose);
                treeScore *= treeScoreDir;
                treeScoreDir = 0;
                if (verbose)
                    printf("  Score in SOUTH direction = %lld\n", treeScoreDir);

                if (isVisible)
                {
                    ++totalNumVisibleTrees;
                    treeVisGrid[treeY][treeX] = true;

                    if (verbose)
                        printf("  Tree is VISIBLE.  total num visible = %lld\n", totalNumVisibleTrees);
                }
                else
                {
                    if (verbose)
                        printf("  Tree is NOT visible.  total num visible = %lld\n", totalNumVisibleTrees);
                }

                if (verbose)
                    printf("  Tree score = %lld\n", treeScore);

                treeScoreGrid[treeY][treeX] = treeScore;
                if (treeScore > largestTreeScore)
                {
                    largestTreeScore = treeScore;
                    if (verbose)
                        printf("  This is the largest tree score so far!\n");
                }
            }
        }

        printf("Total number of visible trees = %lld\n", totalNumVisibleTrees);
        printf("Largest tree score = %lld\n\n", largestTreeScore);

        if (verbose)
        {
            printf("Tree visibilities:\n");
            for (BigInt treeY = 0; treeY < gridSizeY; ++treeY)
            {
                printf("  ");
                for (BigInt treeX = 0; treeX < gridSizeX; ++treeX)
                {
                    printf("%c", treeVisGrid[treeY][treeX] ? (int)'1' : (int)'0');
                }
                printf("\n");
            }
            printf("\n");

            printf("Tree scores:\n");
            for (BigInt treeY = 0; treeY < gridSizeY; ++treeY)
            {
                printf("  ");
                for (BigInt treeX = 0; treeX < gridSizeX; ++treeX)
                {
                    printf("%lld ", treeScoreGrid[treeY][treeX]);
                }
                printf("\n");
            }
            printf("\n\n");
        }
    }

    bool TraceFromTree(
        BigInt gridSizeX,
        BigInt gridSizeY,
        const StringList& lines,
        BigInt currX,
        BigInt currY,
        Direction stepDir,
        char sourceHeight,
        BigInt& treeScore,
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
        BigInt steppedCellScore = 0;

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
            steppedCellScore = 1;
            if (verbose)
                printf(
                    "  reached a tree which has height %c >= source height %c, so visibility is FALSE!\n",
                    (int)lines[currY][currX],
                    (int)sourceHeight);
        }
        else
        {
            steppedCellIsVisible =
                TraceFromTree(gridSizeX, gridSizeY, lines, currX, currY, stepDir, sourceHeight, steppedCellScore, verbose);
            
            // don't forget to include this tree
            ++steppedCellScore;
        }

        treeScore += steppedCellScore;

        return steppedCellIsVisible;
    }
};

Problem8 problem8;
