#include "nwol_timer.h"
#include <chrono>

#if defined(__WINDOWS__)
#include <windows.h>
#endif

void										nwol::STimer::Reset								()									{
#if defined(__WINDOWS__)
	QueryPerformanceFrequency( (LARGE_INTEGER*)&CountsPerSecond );
#endif
	SecondsPerCount											= (1.0 / (CountsPerSecond ? CountsPerSecond : 1));
	CountsPerMicroSecond									= CountsPerSecond/1000000.0;
	if( 0 == CountsPerMicroSecond )
		CountsPerMicroSecond									= 1;
#if defined(__WINDOWS__)
	QueryPerformanceCounter( (LARGE_INTEGER*)&PrevTimeStamp );		
#else
	PrevTimeStamp											= ::std::chrono::duration_cast<::std::chrono::microseconds>(::std::chrono::high_resolution_clock::now().time_since_epoch()).count();
#endif
	LastTimeMicroSeconds									= 0;
	LastTimeSeconds											= 0;

	FramesLastSecond										= 0;
}

void										nwol::STimer::Frame								()									{
#if defined(__WINDOWS__)
	QueryPerformanceCounter( (LARGE_INTEGER*)&CurrentTimeStamp );
#else
	CurrentTimeStamp										= ::std::chrono::duration_cast<::std::chrono::microseconds>(::std::chrono::high_resolution_clock::now().time_since_epoch()).count();
#endif
	int64_t														timeDifference						= (CurrentTimeStamp - PrevTimeStamp);
	LastTimeMicroSeconds									= (int64_t)(timeDifference/CountsPerMicroSecond);
	LastTimeSeconds											= timeDifference * SecondsPerCount;//LastTimeMicroSeconds*1000000;//(CurrentTimeStamp - PrevTimeStamp) * SecondsPerCount;
	PrevTimeStamp											= CurrentTimeStamp;

	FramesThisSecond++;
	FrameCounterSeconds										+= LastTimeSeconds;

	int32_t														totalFrames							= int32_t(FramesThisSecond	/ FrameCounterSeconds);
	int32_t														framesPerSecond						= int32_t(totalFrames		/ FrameCounterSeconds);
	while(FrameCounterSeconds >= 1.0) {
		FramesLastSecond										= framesPerSecond;
		FrameCounterSeconds										-= 1.0f;
		FramesThisSecond										= 0;
	}
}
