#pragma once

#include <Windows.h>
namespace NBE
{
    typedef unsigned long long clocks;

    class NBETimer
    {
    private:
        double desireFramePerSecond;
        double desireSecondsPerFrame;
        clocks desireClocksPerFrame;

    public:
        NBETimer::NBETimer(const double frameRate = 60 ):desireFramePerSecond(frameRate),
            desireSecondsPerFrame(1.0/desireFramePerSecond),
            desireClocksPerFrame(secondsToClocks(desireSecondsPerFrame))
        {}
        inline clocks NBETimer::GetSystemClocks()
        {
            LARGE_INTEGER time;
            QueryPerformanceCounter(&time);
            return time.QuadPart;
        }
        inline clocks NBETimer::GetClockFrequency()
        {
            static LARGE_INTEGER frequency = {0};
            if ( frequency.QuadPart == 0)
            {
                QueryPerformanceFrequency(&frequency);
            }
            return frequency.QuadPart;
        }
        inline double NBETimer::GetClockFrequencyDouble()
        {
            return static_cast<double>(GetClockFrequency());
        }
        inline double NBETimer::clocktoSeconds(clocks c)
        {
            return c / GetClockFrequencyDouble();
        }
        inline clocks NBETimer::secondsToClocks(double seconds)
        {
            return static_cast<clocks>(seconds * GetClockFrequencyDouble());
        }
        inline double NBETimer::getsystemTimeSeconds()
        {
            return clocktoSeconds(GetSystemClocks());	
        }
        inline clocks NBETimer::PreciseWaitUntill(clocks stopWaitTime)
        {
            //TODO use sleep to make this less expensive for cases where the delay is > 16ms
            clocks now;
            while((now = GetSystemClocks()) < stopWaitTime );
            return now;
        }

        inline void NBETimer::resetFPSLimit(const double frameRate)
        {
            desireFramePerSecond = frameRate;
            desireSecondsPerFrame = 1.0/desireFramePerSecond;
            desireClocksPerFrame = secondsToClocks(desireSecondsPerFrame);
        }

        inline clocks NBETimer::getDesireColocsPerFrame()
        {
            return desireClocksPerFrame;
        }
    };


}
