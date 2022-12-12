///////////////////////////
// Problem 12 - Hill Climbing Algorithm

#include "AdventOfCode2022.h"

class Problem12 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 12; }

    virtual void Run() override
    {
        RunOnData("Day12Example.txt", true);
        RunOnData("Day12Input.txt", false);
    }

private:
    enum Direction
    {
        WEST,
        NORTH,
        EAST,
        SOUTH,
    };

    struct Node
    {
        BigInt elevation = 0;
        bool visited = false;
        bool isGoal = false;
        BigInt shortestPathEnteredFromDir = -1;
        BigInt shortestPathToHere = -1;
        BigInt shortestPathExitingDir = -1;
    };

    typedef std::vector<Node> BoardRow;
    typedef std::vector<BoardRow> Board;

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        Board board;
        BigInt startX = 0;
        BigInt startY = 0;
        BigInt endX = 0;
        BigInt endY = 0;
        BuildBoardFromLines(lines, board, startX, startY, endX, endY);

        BigInt shortestPath = 0;
        RecursiveExplore(board, startX, startY, -1, 0, shortestPath, verbose);

        printf("Found shortest path = %lld\n\n", shortestPath);

        if (verbose)
        {
            // first trace back through the shortest path, marking the way

            BigInt x = endX;
            BigInt y = endY;
            do
            {
                const BigInt dirBack = board[y][x].shortestPathEnteredFromDir;
                assert(dirBack >= 0);

                BigInt stepX = 0;
                BigInt stepY = 0;
                GetDirSteps(dirBack, stepX, stepY);
                assert((stepX != 0) || (stepY != 0));

                x += stepX;
                y += stepY;
                board[y][x].shortestPathExitingDir = GetOppositeDir(dirBack);
            } while ((x != startX) || (y != startY));

            // now show the way

            printf("Board showing path:\n\n");

            for (BigInt y = 0; y < (BigInt)board.size(); ++y)
            {
                printf("  ");
                const BoardRow& boardRow = board[y];
                for (BigInt x = 0; x < (BigInt)boardRow.size(); ++x)
                {
                    const Node& node = boardRow[x];
                    if (node.isGoal)
                    {
                        printf("E");
                    }
                    else if (node.shortestPathExitingDir >= 0)
                    {
                        switch (node.shortestPathExitingDir)
                        {
                            case WEST:
                                printf("<");
                                break;
                            case NORTH:
                                printf("^");
                                break;
                            case EAST:
                                printf(">");
                                break;
                            case SOUTH:
                                printf("V");
                                break;
                            default:
                                printf("?");
                                break;
                        }
                    }
                    else
                    {
                        printf(".");
                    }
                }
                printf("\n");
            }
            printf("\n");
        }
    }

    void BuildBoardFromLines(const StringList& lines, Board& board, BigInt& startX, BigInt& startY, BigInt& endX, BigInt& endY)
    {
        board.clear();
        board.resize(lines.size());

        for (BigInt y = 0; y < (BigInt)lines.size(); ++y)
        {
            const std::string& line = lines[y];
            BoardRow& boardRow = board[y];
            boardRow.clear();
            boardRow.resize(line.length());

            for (BigInt x = 0; x < (BigInt)line.length(); ++x)
            {
                const char ch = line[x];

                Node& node = boardRow[x];
                if (ch == 'S')
                {
                    startX = x;
                    startY = y;
                }
                else if (ch == 'E')
                {
                    endX = x;
                    endY = y;
                    node.elevation = (BigInt)'z' - 'a';
                    node.isGoal = true;
                }
                else
                {
                    node.elevation = (BigInt)ch - 'a';
                }
            }
        }
    }

    void RecursiveExplore(
        Board& board,
        BigInt x,
        BigInt y,
        BigInt dirEntered,
        BigInt pathLengthSoFar,
        BigInt& shortestPath,
        bool verbose)
    {
        if (verbose)
            printf("  Exploring node <%lld,%lld>\n", x, y);

        Node& currNode = board[y][x];
        if (dirEntered >= 0)
        {
            // check and update path length
            if ((currNode.shortestPathToHere < 0) || (pathLengthSoFar < currNode.shortestPathToHere))
            {
                currNode.shortestPathToHere = pathLengthSoFar;
                currNode.shortestPathEnteredFromDir = dirEntered;

                if (currNode.isGoal)
                {
                    // we found one way to the goal
                    shortestPath = pathLengthSoFar;
                    return;
                }
            }
            else
            {
                // this path is already not the shortest, so just back up
                return;
            }
        }

        const BigInt startDirInt = dirEntered + 1;
        const BigInt endDirInt = (dirEntered >= 0) ? (startDirInt + 2) : (startDirInt + 3);

        for (BigInt dirInt = startDirInt; dirInt <= endDirInt; ++dirInt)
        {
            const BigInt dir = QuickMod4(dirInt);

            BigInt stepX = 0;
            BigInt stepY = 0;
            GetDirSteps(dir, stepX, stepY);
            assert((stepX != 0) || (stepY != 0));

            const BigInt nextX = x + stepX;
            const BigInt nextY = y + stepY;

            if (verbose)
                printf("    Looking in dir %s at <%lld,%lld>\n", GetDirName(dir), nextX, nextY);

            if (!IsPosValid(board, nextX, nextY))
            {
                if (verbose)
                    printf("      Position is invalid, skipping...\n");
                continue;
            }

            const BigInt nextElevation = board[nextY][nextX].elevation;
            const BigInt nextElevationDiff = nextElevation - currNode.elevation;
            if (nextElevationDiff > 1)
            {
                if (verbose)
                    printf("      Node's elevation %lld is too high compared with current elevation %lld, skipping...\n", nextElevation, currNode.elevation);
                continue;
            }

            RecursiveExplore(
                board,
                nextX,
                nextY,
                GetOppositeDir(dir),
                pathLengthSoFar + 1,
                shortestPath,
                verbose);
        }
    }

    static BigInt QuickMod4(BigInt input) { return input & 3; }

    static bool IsPosValid(const Board& board, BigInt x, BigInt y)
    {
        if (x < 0)
            return false;
        if (x >= (BigInt)board[0].size())
            return false;
        if (y < 0)
            return false;
        if (y >= (BigInt)board.size())
            return false;

        return true;
    }

    static const char* GetDirName(BigInt dir)
    {
        switch (dir)
        {
            case WEST:
                return "WEST";
            case NORTH:
                return "NORTH";
            case EAST:
                return "EAST";
            case SOUTH:
                return "SOUTH";
            default:
                return "UNKNOWN";
        }
    }

    static BigInt GetOppositeDir(BigInt dir) { return QuickMod4(dir + 2); }

    static void GetDirSteps(BigInt dir, BigInt& stepX, BigInt& stepY)
    {
        if (dir < 0)
        {
            stepX = 0;
            stepY = 0;
            return;
        }

        static const BigInt s_stepXList[] = { -1, 0, +1, 0 };
        static const BigInt s_stepYList[] = { 0, -1, 0, +1 };

        stepX = s_stepXList[dir];
        stepY = s_stepYList[dir];
    }
};

Problem12 problem12;
