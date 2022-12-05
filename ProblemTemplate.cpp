///////////////////////////
// Problem # - Name

#include "AdventOfCode2022.h"

class Problem# : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return #; }

    virtual void Run() override
    {
        RunOnData("Day#Example.txt", true);
        //RunOnData("Day#Input.txt", false);
    }

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);
    }
};

Problem# problem#;
