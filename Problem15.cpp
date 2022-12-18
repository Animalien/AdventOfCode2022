///////////////////////////
// Problem 15 - Beacon Exclusion Zone

#include "AdventOfCode2022.h"

class Problem15 : public ProblemBase
{
public:
    virtual int GetProblemNum() const override { return 15; }

    virtual void Run() override
    {
        RunOnData("Day15Example.txt", 10, true);
        RunOnData("Day15Input.txt", 2000000, false);
    }

private:
    class Row
    {
    public:
        Row(BigInt y) : m_y(y) {}

        void ConsiderSensorAndBeacon(BigInt sensorX, BigInt sensorY, BigInt beaconX, BigInt beaconY, bool verbose)
        {
            if (verbose)
                printf(
                    "Considering sensor at <%lld,%lld> and beacon at <%lld,%lld>, against row %lld\n",
                    sensorX,
                    sensorY,
                    beaconX,
                    beaconY,
                    m_y);

            const BigInt beaconDist = std::abs(sensorX - beaconX) + std::abs(sensorY - beaconY);

            if (verbose)
                printf("  Beacon distance = %lld\n", beaconDist);

            bool haveNewSpan = false;
            Span newSpan;

            const BigInt distY = std::abs(sensorY - m_y);
            if (distY <= beaconDist)
            {
                haveNewSpan = true;
                const BigInt distX = beaconDist - distY;
                newSpan.x0 = sensorX - distX;
                newSpan.x1 = sensorX + distX;

                if (verbose)
                    printf(
                        "  The diamond indicated by the sensor+beacon pair intersects with the row (with distY = %lld), so we are considering a new span of <%lld,%lld>\n",
                        distY,
                        newSpan.x0,
                        newSpan.x1);
            }
            else
            {
                if (verbose)
                    printf(
                        "  The diamond indicated by the sensor+beacon pair does NOT intersect with our row, because the sensor is at distance %lld (> beacon distance %lld), so we do not consider any spans\n",
                        distY,
                        beaconDist);
            }

            if (haveNewSpan && (beaconY == m_y))
            {
                if (newSpan.x0 == newSpan.x1)
                    haveNewSpan = false;
                else if (beaconX == newSpan.x0)
                    ++newSpan.x0;
                else if (beaconX == newSpan.x1)
                    --newSpan.x1;

                if (verbose)
                {
                    if (haveNewSpan)
                        printf(
                            "  The beacon sits exactly on our row (at x = %lld), so the new span has been changed to <%lld,%lld>\n",
                            beaconX,
                            newSpan.x0,
                            newSpan.x1);
                    else
                        printf(
                            "  The beacon sits exactly on our row, but also sits exactly on a point of the diamond, so we do not have a span\n");
                }
            }

            if (haveNewSpan)
                MergeInNewSpan(newSpan, verbose);
        }

        BigInt CalcNumCoveredBySpans() const
        {
            BigInt numCovered = 0;
            for (const Span& span: m_spanList)
            {
                numCovered += (span.x1 - span.x0 + 1);
            }
            return numCovered;
        }

    private:
        struct Span
        {
            BigInt x0 = 0;
            BigInt x1 = 0;
        };
        std::vector<Span> m_spanList;

        BigInt m_y = 0;

        void MergeInNewSpan(const Span& newSpan, bool verbose)
        {
            if (verbose)
            {
                printf("  Merging in new span <%lld,%lld>\n", newSpan.x0, newSpan.x1);
                PrintCurrentSpanList();
            }

            if (m_spanList.empty())
            {
                m_spanList.push_back(newSpan);
                if (verbose)
                    PrintCurrentSpanList();
                return;
            }

            // there are existing spans, so try to fit the new span to the gaps around and between the existing ones

            Span fittedSpan;

            // test against the first span

            Span& firstSpan = m_spanList[0];
            if (FitSpanToLeftInfiniteGap(newSpan, firstSpan.x0 - 1, fittedSpan))
            {
                if (fittedSpan.x1 == (firstSpan.x0 - 1))
                {
                    // fitted span butts up against first span, so simply pre-pend it to the first span

                    firstSpan.x0 = fittedSpan.x0;
                }
                else
                {
                    // fitted span completely precedes the first span, so insert it

                    m_spanList.insert(m_spanList.begin(), fittedSpan);
                }
            }

            // test against gaps between consecutive spans

            if (m_spanList.size() > 1)
            {
                BigInt prevIndex = 0;
                BigInt nextIndex = 1;

                for (;;)
                {
                    const BigInt gapX0 = m_spanList[prevIndex].x1 + 1;
                    const BigInt gapX1 = m_spanList[nextIndex].x0 - 1;
                    if (FitSpanToGap(newSpan, gapX0, gapX1, fittedSpan))
                    {
                        if (fittedSpan.x0 == gapX0)
                        {
                            if (fittedSpan.x1 == gapX1)
                            {
                                // fitted span perfectly covers the gap, so simply merge the two current spans

                                m_spanList[prevIndex].x1 = m_spanList[nextIndex].x1;
                                m_spanList.erase(m_spanList.begin() + nextIndex);

                                // we do not increment the indices because we now have to re-evaluate vs the *next* gap

                                // but test the current next index in case we are now done with the list
                                if (nextIndex >= (BigInt)m_spanList.size())
                                    break;
                            }
                            else
                            {
                                // fitted span extends the previous span

                                m_spanList[prevIndex].x1 = fittedSpan.x1;

                                // and we have now dealt with the last of the new span, so we can break out
                                break;
                            }
                        }
                        else
                        {
                            if (fittedSpan.x1 == gapX1)
                            {
                                // fitted span pre-pends to the next span

                                m_spanList[nextIndex].x0 = fittedSpan.x0;

                                // now step forward to check more gaps

                                prevIndex = nextIndex;
                                ++nextIndex;
                                if (nextIndex >= (BigInt)m_spanList.size())
                                    break;
                            }
                            else
                            {
                                // fitted span fits in between these spans, so insert it

                                m_spanList.insert(m_spanList.begin() + nextIndex, fittedSpan);

                                // and we have now dealt with the entire new span, so we can break out
                                break;
                            }
                        }
                    }
                    else
                    {
                        prevIndex = nextIndex;
                        ++nextIndex;
                        if (nextIndex >= (BigInt)m_spanList.size())
                            break;
                    }
                }
            }

            // test against the last span

            Span& lastSpan = m_spanList.back();
            if (FitSpanToRightInfiniteGap(newSpan, lastSpan.x1 + 1, fittedSpan))
            {
                if (fittedSpan.x0 == (lastSpan.x1 + 1))
                {
                    // fitted span butts up against last span, so simply append it

                    lastSpan.x1 = fittedSpan.x1;
                }
                else
                {
                    // fitted span completely succeeds the last span, so simply insert it last in the list

                    m_spanList.push_back(fittedSpan);
                }
            }

            if (verbose)
                PrintCurrentSpanList();
        }

        static bool FitSpanToLeftInfiniteGap(const Span& span, BigInt gapX1, Span& newSpan)
        {
            if (span.x0 > gapX1)
                return false;

            newSpan.x0 = span.x0;
            newSpan.x1 = std::min(gapX1, span.x1);

            return true;
        }

        static bool FitSpanToGap(const Span& span, BigInt gapX0, BigInt gapX1, Span& newSpan)
        {
            newSpan.x0 = std::max(gapX0, span.x0);
            newSpan.x1 = std::min(gapX1, span.x1);
            if (newSpan.x0 > newSpan.x1)
                return false;

            return true;
        }

        static bool FitSpanToRightInfiniteGap(const Span& span, BigInt gapX0, Span& newSpan)
        {
            if (span.x1 < gapX0)
                return false;

            newSpan.x0 = std::max(gapX0, span.x0);
            newSpan.x1 = span.x1;

            return true;
        }

        void PrintCurrentSpanList()
        {
            printf("    Current span list:\n");
            for (const Span& span: m_spanList)
                printf("      <%lld,%lld>\n", span.x0, span.x1);
        }
    };

    void RunOnData(const char* filename, BigInt rowY, bool verbose)
    {
        printf("For file '%s'...\n", filename);

        Row row(rowY);

        StringList lines;
        ReadFileLines(filename, lines);

        for (const std::string& line: lines)
        {
            BigInt sensorX = 0;
            BigInt sensorY = 0;
            BigInt beaconX = 0;
            BigInt beaconY = 0;

            const char* st = line.c_str() + 12;  // "Sensor at x="
            ParseNextBigInt(st, sensorX, true /*checkForNegation*/);
            ParseNextBigInt(st, sensorY, true /*checkForNegation*/);

            st += 25;   // ": closest beacon is at x="
            ParseNextBigInt(st, beaconX, true /*checkForNegation*/);
            ParseNextBigInt(st, beaconY, true /*checkForNegation*/);

            row.ConsiderSensorAndBeacon(sensorX, sensorY, beaconX, beaconY, verbose);
        }

        printf("In the row %lld, there are %lld spaces where a beacon cannot be!\n\n", rowY, row.CalcNumCoveredBySpans());
    }
};

Problem15 problem15;
