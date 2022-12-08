///////////////////////////
// Problem 7 - No Space Left On Device

#include "AdventOfCode2022.h"

class Problem7 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 7; }

    virtual void Run() override
    {
        RunOnData("Day7Example.txt", true);
        RunOnData("Day7Input.txt", false);
    }

    class FileSystem
    {
    public:
        FileSystem() : rootDir("/") { workingPath.push(&rootDir); }

        void ChangeDir(const std::string& dir)
        {
            if (dir[0] == '/')
            {
                while (workingPath.top()->name != "/")
                    workingPath.pop();
            }
            else if (dir == "..")
            {
                workingPath.pop();
            }
            else
            {
                Directory& newDir = workingPath.top()->dirs[dir];
                if (newDir.name.empty())
                    newDir.name = dir;
                workingPath.push(&newDir);
            }
        }

        void AddDirContent(const std::string& line)
        {
            StringList tokens;
            Tokenize(line, tokens, ' ');
            if (tokens[0] == "dir")
            {
                Directory& newDir = workingPath.top()->dirs[tokens[1]];
                if (newDir.name.empty())
                    newDir.name = tokens[1];
            }
            else
            {
                workingPath.top()->files[tokens[1]] = atoll(tokens[0].c_str());
            }
        }

        void PrintTree()
        {
            std::string indent;
            printf("Tree:\n\n/:\n");
            PrintDir(indent, rootDir);
        }

        void CalcTotalSizeDirsAtSizeLimit(BigInt sizeLimit, BigInt& totalSize, BigInt& totalAtSizeLimit, bool verbose)
        {
            totalSize = 0;
            totalAtSizeLimit = 0;
            CalcTotalSizeDirsAtSizeLimit(rootDir, sizeLimit, totalSize, totalAtSizeLimit, verbose);
        }

        BigInt CalcSizeOfSmallestDirToFreeUpSpace(BigInt spaceToFreeUp, bool verbose)
        {
            BigInt smallestDirSize = -1;
            BigInt totalSize = 0;
            CalcSizeOfSmallestDirToFreeUpSpace(rootDir, spaceToFreeUp, smallestDirSize, totalSize, verbose);
            return smallestDirSize;
        }

    private:
        struct Directory
        {
            Directory() = default;
            Directory(const char* n) : name(n) {}

            std::string name;
            std::map<std::string, BigInt> files;
            std::map<std::string, Directory> dirs;
        };

        Directory rootDir;
        std::stack<Directory*> workingPath;

        void PrintDir(std::string& indent, const Directory& dir)
        {
            indent.push_back(' ');
            indent.push_back(' ');

            for (const auto& fileNode: dir.files)
            {
                printf("%sfile '%s', %lld\n", indent.c_str(), fileNode.first.c_str(), fileNode.second);
            }

            for (const auto& dirNode: dir.dirs)
            {
                printf("%sdir '%s'\n", indent.c_str(), dirNode.first.c_str());

                PrintDir(indent, dirNode.second);
            }

            indent.pop_back();
            indent.pop_back();
        }

        void CalcTotalSizeDirsAtSizeLimit(const Directory& dir, BigInt sizeLimit, BigInt& totalSize, BigInt& totalAtSizeLimit, bool verbose)
        {
            totalSize = 0;

            for (const auto& fileNode: dir.files)
            {
                totalSize += fileNode.second;
            }

            for (const auto& dirNode: dir.dirs)
            {
                BigInt subDirTotal = 0;
                CalcTotalSizeDirsAtSizeLimit(dirNode.second, sizeLimit, subDirTotal, totalAtSizeLimit, verbose);
                totalSize += subDirTotal;
            }

            if (verbose)
                printf("dir %s has total of %lld\n", dir.name.c_str(), totalSize);
            if (totalSize <= sizeLimit)
            {
                totalAtSizeLimit += totalSize;
                if (verbose)
                    printf("  is <= %lld, so it adds to running total at size limit\n", sizeLimit);
            }
        }

        void CalcSizeOfSmallestDirToFreeUpSpace(
            const Directory& dir, BigInt spaceToFreeUp, BigInt& smallestDirSize, BigInt& totalSize, bool verbose)
        {
            totalSize = 0;

            for (const auto& fileNode: dir.files)
            {
                totalSize += fileNode.second;
            }

            for (const auto& dirNode: dir.dirs)
            {
                BigInt subDirTotal = 0;
                CalcSizeOfSmallestDirToFreeUpSpace(
                    dirNode.second, spaceToFreeUp, smallestDirSize, subDirTotal, verbose);
                totalSize += subDirTotal;
            }

            if (verbose)
                printf("dir %s has total of %lld\n", dir.name.c_str(), totalSize);
            if ((totalSize >= spaceToFreeUp) && ((smallestDirSize < 0) || (totalSize < smallestDirSize)))
            {
                if (verbose)
                    printf("  is >= %lld and < %lld, so it becomes our new selected size\n", spaceToFreeUp, smallestDirSize);
                smallestDirSize = totalSize;
            }
        }
    };

    void RunOnData(const char* filename, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        StringList lines;
        ReadFileLines(filename, lines);

        FileSystem fs;

        bool inLS = false;
        for (const std::string& line: lines)
        {
            if (inLS && (line[0] == '$'))
                inLS = false;

            if (inLS)
            {
                fs.AddDirContent(line);
            }
            else
            {
                StringList tokens;
                Tokenize(line, tokens, ' ');

                if (tokens[1] == "cd")
                {
                    fs.ChangeDir(tokens[2]);
                }
                else if (tokens[1] == "ls")
                {
                    inLS = true;
                }
            }
        }

        if (verbose)
            fs.PrintTree();

        BigInt totalSize = 0;
        BigInt totalAtSizeLimit = 0;
        fs.CalcTotalSizeDirsAtSizeLimit(100000, totalSize, totalAtSizeLimit, verbose);
        printf("Total at size limit = %lld\n\n", totalAtSizeLimit);

        const BigInt totalCapacity = 70000000LL;
        const BigInt needUnusedSpace = 30000000LL;
        const BigInt currentFreeSpace = totalCapacity - totalSize;
        const BigInt spaceToFreeUp = needUnusedSpace - currentFreeSpace;
        printf(
            "Need %lld unused space out of total capacity %lld.  Current total space taken = %lld, current free space = %lld.  Space to free up = %lld - %lld = %lld\n",
            needUnusedSpace,
            totalCapacity,
            totalSize,
            currentFreeSpace,
            needUnusedSpace,
            currentFreeSpace,
            spaceToFreeUp);

        const BigInt smallestDirSizeToFreeUp = fs.CalcSizeOfSmallestDirToFreeUpSpace(spaceToFreeUp, verbose);
        printf(
            "Found smallest directory to delete in order to free up the required space.  Dir size = %lld\n",
            smallestDirSizeToFreeUp);
    }
};

Problem7 problem7;
