///////////////////////////
// Problem 1 - Calorie Counting

#include "AdventOfCode2022.h"

class Problem1 : public ProblemBase
{
public:
	virtual int GetProblemNum() const override { return 1; }

	virtual void Run() override
	{
		printf("Test:\n\n");
		RunCalorieCountingTest({ "1000", "2000", "3000", "", "4000", "", "5000", "6000", "", "7000", "8000", "9000", "", "10000" });

		printf("\n\nInput:\n\n");
		RunCalorieCountingInputFile();
	}

	void AbsorbCalorieCountingTotal(BigInt newTotal, BigIntList& biggestTotalsList)
	{
		if (newTotal < biggestTotalsList[biggestTotalsList.size() - 1])
			return;

		auto iter = biggestTotalsList.begin();
		while ((iter != biggestTotalsList.end()) && (newTotal < *iter))
			++iter;

		biggestTotalsList.insert(iter, newTotal);
		biggestTotalsList.pop_back();
	}

	void PrintCalorieCountingBiggestTotalsList(const BigIntList& biggestTotalsList)
	{
		BigInt totalTotal = 0;
		printf("Biggest %lld totals:  ", (BigInt)biggestTotalsList.size());
		for (BigInt currTotal: biggestTotalsList)
		{
			printf("%lld ", currTotal);
			totalTotal += currTotal;
		}
		printf(" (total %lld)\n", totalTotal);
	}

	void RunCalorieCountingOnLines(const StringList& lines)
	{
		BigIntList biggestTotalsList = { 0, 0, 0 };

		BigInt currentTotal = 0;

		for (const std::string& line: lines)
		{
			if (line.empty())
			{
				printf("Found elf with total %lld\n", currentTotal);
				AbsorbCalorieCountingTotal(currentTotal, biggestTotalsList);
				PrintCalorieCountingBiggestTotalsList(biggestTotalsList);
				printf("\n");

				currentTotal = 0;
			}
			else
			{
				currentTotal += atoll(line.c_str());
			}
		}

		printf("Found elf with total %lld\n", currentTotal);
		AbsorbCalorieCountingTotal(currentTotal, biggestTotalsList);
		PrintCalorieCountingBiggestTotalsList(biggestTotalsList);
		printf("\n");
	}

	void RunCalorieCountingTest(std::initializer_list<const char*> list)
	{
		StringList lines;
		for (const char* oneLine: list)
		{
			lines.push_back(oneLine);
		}
		RunCalorieCountingOnLines(lines);
	}

	void RunCalorieCountingInputFile()
	{
		StringList lines;
		ReadFileLines("Day1Input.txt", lines);
		RunCalorieCountingOnLines(lines);
	}
};

Problem1 problem1;
