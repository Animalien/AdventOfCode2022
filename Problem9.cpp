///////////////////////////
// Problem 9 - Rope Bridge

#include "AdventOfCode2022.h"

class Problem9 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 9; }

    virtual void Run() override
    {
        RunOnData("Day9Example.txt", 2, true);

        RunOnData("Day9Example.txt", 10, true);
        RunOnData("Day9Example2.txt", 10, true);

        RunOnData("Day9Input.txt", 2, false);
        RunOnData("Day9Input.txt", 10, false);
    }

private:
    class Rope
    {
    public:
        Rope(BigInt numKnots, BigInt startX, BigInt startY)
        {
            m_xList.resize(numKnots, startX);
            m_yList.resize(numKnots, startY);
        }

        BigInt GetNumKnots() const { return (BigInt)m_xList.size(); }

        void GetKnotPos(BigInt knotIndex, BigInt& x, BigInt& y) const
        {
            x = m_xList[knotIndex];
            y = m_yList[knotIndex];
        }

        void StepHead(BigInt stepX, BigInt stepY)
        {
            m_xList[0] += stepX;
            m_yList[0] += stepY;

            for (BigInt knotIndex = 1; knotIndex < (BigInt)m_xList.size(); ++knotIndex)
                StepKnot(knotIndex);
        }

        bool GetCharForLocation(BigInt x, BigInt y, char& ch) const
        {
            for (BigInt knotIndex = 0; knotIndex < (BigInt)m_xList.size(); ++knotIndex)
            {
                if ((x == m_xList[knotIndex]) && (y == m_yList[knotIndex]))
                {
                    if (knotIndex == 0)
                        ch = 'H';
                    else if (knotIndex == ((BigInt)m_xList.size() - 1))
                        ch = 'T';
                    else
                        ch = (char)('0' + knotIndex);
                    return true;
                }
            }

            return false;
        }

    private:
        void StepKnot(BigInt knotIndex)
        {
            const BigInt diffX = m_xList[knotIndex - 1] - m_xList[knotIndex];
            const BigInt diffY = m_yList[knotIndex - 1] - m_yList[knotIndex];
            if (diffX < -1)
            {
                --m_xList[knotIndex];
                if (diffY < 0)
                    --m_yList[knotIndex];
                else if (diffY > 0)
                    ++m_yList[knotIndex];
            }
            else if (diffX > +1)
            {
                ++m_xList[knotIndex];
                if (diffY < 0)
                    --m_yList[knotIndex];
                else if (diffY > 0)
                    ++m_yList[knotIndex];
            }
            else if (diffY < -1)
            {
                --m_yList[knotIndex];
                if (diffX < 0)
                    --m_xList[knotIndex];
                else if (diffX > 0)
                    ++m_xList[knotIndex];
            }
            else if (diffY > +1)
            {
                ++m_yList[knotIndex];
                if (diffX < 0)
                    --m_xList[knotIndex];
                else if (diffX > 0)
                    ++m_xList[knotIndex];
            }
        }

        std::vector<BigInt> m_xList;
        std::vector<BigInt> m_yList;
    };

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

        bool MarkVisited(const Rope& rope)
        {
            BigInt knotIndex = 0;
            BigInt x = 0;
            BigInt y = 0;
            const BigInt penultimateKnotIndex = rope.GetNumKnots() - 2;
            for (; knotIndex <= penultimateKnotIndex; ++knotIndex)
            {
                rope.GetKnotPos(knotIndex, x, y);
                UpdateMinMax(x, y);
            }

            rope.GetKnotPos(knotIndex, x, y);
            UpdateMinMax(x, y);
            const bool prevVisited = m_currBoard[y][x];
            m_currBoard[y][x] = true;

            return prevVisited;
        }

        void Print(const Rope& rope)
        {
            printf("Curr board:\n");
            for (BigInt y = m_minVisitedY; y <= m_maxVisitedY; ++y)
            {
                printf("  ");
                for (BigInt x = m_minVisitedX; x <= m_maxVisitedX; ++x)
                {
                    char ch;
                    if (rope.GetCharForLocation(x, y, ch))
                        printf("%c", (int)ch);
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

    void RunOnData(const char* filename, BigInt numKnots, bool verbose)
    {
        printf("For file '%s', with rope with %lld knots...\n", filename, numKnots);

        StringList lines;
        ReadFileLines(filename, lines);

        BigInt startX = 0;
        BigInt startY = 0;
        Board board;
        board.GetStartPos(startX, startY);
        BigInt numVisitedLocations = 1;

        Rope rope(numKnots, startX, startY);
        board.MarkVisited(rope);

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
                rope.StepHead(stepX, stepY);

                const bool wasPreviouslyVisited = board.MarkVisited(rope);
                if (!wasPreviouslyVisited)
                {
                    ++numVisitedLocations;
                    if (verbose)
                        printf("  Tail visiting new location\n");
                }

                if (verbose)
                {
                    printf("\n");
                    board.Print(rope);
                }
            }
        }

        printf("Num visited locations = %lld\n\n", numVisitedLocations);
    }

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
};

Problem9 problem9;
