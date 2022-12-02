///////////////////////////
// Problem # - Name

#include "AdventOfCode2022.h"

class Problem2 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 2; }

    virtual void Run() override
    {
        RunOnData("Day2Example.txt");
        RunOnData("Day2Input.txt");
    }

    void RunOnData(const char* filename)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        BigInt score = 0;
        BigInt partTwoScore = 0;

        for (const auto& line: lines)
        {
            // Part One

            BigInt thisLineScore = 0;

            const BigInt scoreForWhatIPlayed = ScoreForWhatIPlayed(line[2]);
            thisLineScore += scoreForWhatIPlayed;
            printf("Scored %lld for what I played, ", scoreForWhatIPlayed);

            const BigInt scoreForDidIWin = ScoreForDidIWin(line[0], line[2]);
            thisLineScore += scoreForDidIWin;
            printf("scored %lld for did I win;  total = %lld\n", scoreForDidIWin, thisLineScore);

            score += thisLineScore;

            // Part Two

            BigInt thisLinePartTwoScore = 0;
            
            const BigInt partTwoDesiredResultNumber = PartTwoGetDesiredResultNumber(line[2]);
            printf("Part Two:  desiredResultNumber = %lld, ", partTwoDesiredResultNumber);
            const BigInt partTwoChoiceScore = PartTwoWhatIsMyChoiceScore(line[0], partTwoDesiredResultNumber);
            thisLinePartTwoScore += partTwoChoiceScore;
            printf("scored %lld for what I played, ", partTwoChoiceScore);

            const BigInt partTwoDesiredResultScore = PartTwoDesiredResultScore(partTwoDesiredResultNumber);
            thisLinePartTwoScore += partTwoDesiredResultScore;
            printf("scored %lld for the desired result;  total = %lld\n", partTwoDesiredResultScore, thisLinePartTwoScore);

            partTwoScore += thisLinePartTwoScore;
        }

        printf("Got total score of %lld for Part One, and %lld for Part Two\n\n", score, partTwoScore);
    }

    static BigInt ScoreForWhatIPlayed(char played) { return ((BigInt)(played - 'X')) + 1; }
    static BigInt ScoreForDidIWin(char theyPlayed, char iPlayed)
    {
        const BigInt theirNumber = (BigInt)(theyPlayed - 'A');
        const BigInt myNumber = (BigInt)(iPlayed - 'X');
        const BigInt relative = QuickMod3(myNumber + 3 - theirNumber);
        static const BigInt s_scoreForRelative[3] = { 3, 6, 0 };
        printf("(%lld %lld %lld %lld), ", theirNumber, myNumber, relative, s_scoreForRelative[relative]);
        return s_scoreForRelative[relative];
    }

    static BigInt PartTwoGetDesiredResultNumber(char desiredResult)
    {
        return (BigInt)(desiredResult - 'X');   // 0 = loss, 1 = draw, 2 = win
    }
    static BigInt PartTwoWhatIsMyChoiceScore(char theyPlayed, BigInt desiredResultNumber)
    {
        const BigInt theirNumber = (BigInt)(theyPlayed - 'A');
        const BigInt relativeValue = QuickMod3(desiredResultNumber + 2);   // 0 = loss -> 2, 1 = draw -> 0, 2 = win -> 1
        const BigInt myNumber = QuickMod3(theirNumber + relativeValue);

        return myNumber + 1;
    }
    static BigInt PartTwoDesiredResultScore(BigInt desiredResultNumber) { return desiredResultNumber * 3; }

    static BigInt QuickMod3(BigInt input)
    {
        // assumes positive num and no more than one wrap-around
        static const BigInt s_wrapper[6] = { 0, 1, 2, 0, 1, 2 };
        return s_wrapper[input];
    }
};

Problem2 problem2;
