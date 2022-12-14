///////////////////////////
// Problem 14 - Regolith Reservoir

#include "AdventOfCode2022.h"

class Problem14 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 14; }

    virtual void Run() override
    {
        RunOnData("Day14Example.txt", true);
        RunOnData("Day14Input.txt", false);
    }

private:
    static const BigInt MAX_GRID_SIZE = 1024;
    typedef std::vector<std::string> Board;
    Board m_board;
    BigInt m_boardMinX = -1;
    BigInt m_boardMaxX = -1;
    BigInt m_boardMinY = -1;
    BigInt m_boardMaxY = -1;

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        InitBoardFromLines(lines, verbose);

        BigInt numSandSettled = 0;
        bool fellToMaxY = false;
        for (;;)
        {
            DropOneSand(fellToMaxY, false, verbose);
            if (fellToMaxY)
                break;

            ++numSandSettled;
        }

        DropOneSand(fellToMaxY, true, verbose);

        printf("Num grains of sand settled = %lld\n\n", numSandSettled);

        PrintBoard();
    }

    static const BigInt SAND_SOURCE_X = 500;
    static const BigInt SAND_SOURCE_Y = 0;

    void InitBoardFromLines(const StringList& lines, bool verbose)
    {
        m_board.clear();
        m_board.resize(MAX_GRID_SIZE);
        for (std::string& row: m_board)
            row.resize(MAX_GRID_SIZE, '.');

        SetBoardChar(SAND_SOURCE_X, SAND_SOURCE_Y, '+');

        for (const std::string& line: lines)
        {
            const char* st = line.c_str();

            BigInt x = 0;
            BigInt y = 0;
            ParseNextBigInt(st, x);
            ParseNextBigInt(st, y);

            SetBoardChar(x, y, '#');

            BigInt destX = 0;
            BigInt destY = 0;
            while (ParseNextBigInt(st, destX) && ParseNextBigInt(st, destY))
            {
                BigInt stepX = 0;
                if (destX > x)
                    stepX = +1;
                else if (destX < x)
                    stepX = -1;

                BigInt stepY = 0;
                if (destY > y)
                    stepY = +1;
                else if (destY < y)
                    stepY = -1;
                
                do
                {
                    x += stepX;
                    y += stepY;
                    SetBoardChar(x, y, '#');
                } while ((x != destX) || (y != destY));
            }
        }

        PuffMinMaxPos();

        if (verbose)
            PrintBoard();
    }

    void DropOneSand(bool& fellToMaxY, bool drawPath, bool verbose)
    {
        BigInt x = SAND_SOURCE_X;
        BigInt y = SAND_SOURCE_Y;

        fellToMaxY = false;
        for (;;)
        {
            bool didMove = false;
            TryMoveSandOneStep(x, y, 0, +1, drawPath, didMove, fellToMaxY);
            if (!didMove)
            {
                TryMoveSandOneStep(x, y, -1, +1, drawPath, didMove, fellToMaxY);
                if (!didMove)
                    TryMoveSandOneStep(x, y, +1, +1, drawPath, didMove, fellToMaxY);
            }

            if (!didMove || fellToMaxY)
                break;
        }

        if (!fellToMaxY)
            SetBoardChar(x, y, 'o');

        if (verbose)
            PrintBoard();
    }

    void TryMoveSandOneStep(BigInt& x, BigInt& y, BigInt stepX, BigInt stepY, bool drawPath, bool& didMove, bool& fellToMaxY)
    {
        didMove = false;
        fellToMaxY = false;

        const BigInt newX = x + stepX;
        const BigInt newY = y + stepY;

        if (m_board[newY][newX] != '.')
            return;

        x += stepX;
        y += stepY;
        didMove = true;

        if (drawPath)
            SetBoardChar(x, y, '~');

        if (y >= m_boardMaxY)
            fellToMaxY = true;
    }

    void SetBoardChar(BigInt x, BigInt y, char ch)
    {
        m_board[y][x] = ch;
        AbsorbMinMaxPos(x, y);
    }

    void AbsorbMinMaxPos(BigInt x, BigInt y)
    {
        if ((m_boardMinX < 0) || (x < m_boardMinX))
            m_boardMinX = x;
        if ((m_boardMaxX < 0) || (x > m_boardMaxX))
            m_boardMaxX = x;
        if ((m_boardMinY < 0) || (y < m_boardMinY))
            m_boardMinY = y;
        if ((m_boardMaxY < 0) || (y > m_boardMaxY))
            m_boardMaxY = y;
    }

    void PuffMinMaxPos()
    {
        --m_boardMinX;
        ++m_boardMaxX;
        // don't puff minY because everything simply falls downward and we want the sand source to be at the top line
        ++m_boardMaxY;
    }

    void PrintBoard()
    {
        printf("Board:\n");
        for (BigInt y = m_boardMinY; y <= m_boardMaxY; ++y)
        {
            printf("  ");
            for (BigInt x = m_boardMinX; x <= m_boardMaxX; ++x)
                printf("%c", (int)m_board[y][x]);
            printf("\n");
        }
        printf("\n\n");
    }
};

Problem14 problem14;
