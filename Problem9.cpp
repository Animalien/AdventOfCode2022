///////////////////////////
// Problem 9 - Rope Bridge

#include "AdventOfCode2022.h"

class Problem9 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 9; }

    virtual void Run() override
    {
        RunOnData("Day9Example.txt", true);
        RunOnData("Day9Input.txt", false);
    }

private:
    class Board
    {
    public:
        Board()
        {
            m_currBoard.resize(MAX_SIZE);
            for (std::vector<bool>& row: m_currBoard)
                row.resize(MAX_SIZE, false);
        }

        void GetStartPos(BigInt& x, BigInt& y) const { x = y = START_POS; }

        bool MarkVisited(BigInt x, BigInt y)
        {
            const bool prevVisited = m_currBoard[y][x];
            m_currBoard[y][x] = true;
            UpdateMinMax(x, y);
            return prevVisited;
        }

        void Print(BigInt headX, BigInt headY, BigInt tailX, BigInt tailY)
        {
            printf("Curr board:\n");
            for (BigInt y = m_minVisitedY; y <= m_maxVisitedY; ++y)
            {
                printf("  ");
                for (BigInt x = m_minVisitedX; x <= m_maxVisitedX; ++x)
                {
                    if ((x == headX) && (y == headY))
                        printf("H");
                    else if ((x == tailX) && (y == tailY))
                        printf("T");
                    else if ((x == START_POS) && (y == START_POS))
                        printf("s");
                    else if (m_currBoard[y][x])
                        printf("#");
                    else
                        printf(".");
                }
                printf("\n");
            }
            printf("\n");
        }

    private:
        void UpdateMinMax(BigInt x, BigInt y)
        {
            if (x <= m_minVisitedX)
                m_minVisitedX = x - 1;
            if (x >= m_maxVisitedX)
                m_maxVisitedX = x + 1;

            if (y <= m_minVisitedY)
                m_minVisitedY = y - 1;
            if (y >= m_maxVisitedY)
                m_maxVisitedY = y + 1;
        }

        static const BigInt MAX_SIZE = 2048;
        static const BigInt START_POS = MAX_SIZE / 2;
        std::vector<std::vector<bool>> m_currBoard;

        BigInt m_minVisitedX = START_POS;
        BigInt m_maxVisitedX = START_POS;
        BigInt m_minVisitedY = START_POS;
        BigInt m_maxVisitedY = START_POS;
    };

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        Board board;

        BigInt headX = 0;
        BigInt headY = 0;
        BigInt tailX = 0;
        BigInt tailY = 0;

        board.GetStartPos(headX, headY);
        board.GetStartPos(tailX, tailY);

        board.MarkVisited(tailX, tailY);
        BigInt numVisitedLocations = 1;

        for (const std::string& line: lines)
        {
            StringList tokens;
            Tokenize(line, tokens, ' ');

            BigInt stepX = 0;
            BigInt stepY = 0;
            GetDirStep(tokens[0][0], stepX, stepY);

            const BigInt numSteps = atoll(tokens[1].c_str());

            if (verbose)
                printf("Stepping '%c' (%lld,%lld) x %lld\n", (int)tokens[0][0], stepX, stepY, numSteps);

            for (BigInt i = 0; i < numSteps; ++i)
            {
                if (verbose)
                    printf("  Head goes from <%lld,%lld> to ", headX, headY);

                headX += stepX;
                headY += stepY;

                if (verbose)
                    printf("<%lld,%lld>\n  Tail goes from <%lld,%lld> to ", headX, headY, tailX, tailY);

                const BigInt diffX = headX - tailX;
                const BigInt diffY = headY - tailY;
                if (diffX < -1)
                {
                    --tailX;
                    if (diffY < 0)
                        --tailY;
                    else if (diffY > 0)
                        ++tailY;
                }
                else if (diffX > +1)
                {
                    ++tailX;
                    if (diffY < 0)
                        --tailY;
                    else if (diffY > 0)
                        ++tailY;
                }
                else if (diffY < -1)
                {
                    --tailY;
                    if (diffX < 0)
                        --tailX;
                    else if (diffX > 0)
                        ++tailX;
                }
                else if (diffY > +1)
                {
                    ++tailY;
                    if (diffX < 0)
                        --tailX;
                    else if (diffX > 0)
                        ++tailX;
                }

                if (verbose)
                    printf("<%lld,%lld>\n", tailX, tailY);

                if ((std::abs(headX - tailX) > 1) || (std::abs(headY - tailY) > 1))
                    printf("OH MY GOD!!!!!\n");

                const bool wasPreviouslyVisited = board.MarkVisited(tailX, tailY);
                if (!wasPreviouslyVisited)
                {
                    ++numVisitedLocations;
                    if (verbose)
                        printf("  Tail visiting new location\n");
                }

                if (verbose)
                {
                    printf("\n");
                    board.Print(headX, headY, tailX, tailY);
                }
            }
        }

        printf("Num visited locations = %lld\n\n", numVisitedLocations);
    }

    static BigInt CalcCoordValue(BigInt x, BigInt y) { return (y << 32) | x; };
    static void GetDirStep(char dir, BigInt& stepX, BigInt& stepY)
    {
        switch (dir)
        {
            case 'U':
                stepX = 0;
                stepY = -1;
                break;
            case 'D':
                stepX = 0;
                stepY = +1;
                break;
            case 'L':
                stepX = -1;
                stepY = 0;
                break;
            case 'R':
                stepX = +1;
                stepY = 0;
                break;
            default:
                break;
        }
    }
    static void AdjustVisitedMinMax(BigInt& min, BigInt& max, BigInt curr)
    {
        if (curr < min)
            min = curr;
        if (curr > max)
            max = curr;
    }
};

Problem9 problem9;
