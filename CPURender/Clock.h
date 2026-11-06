#pragma once
#include <chrono>

class Clock {
public:
	Clock();
	float GetDt();
	float GetTotalTime();
	float UpdateDt();
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_previousTime;
	float m_lastDt = 0.f;
	float m_totalTime = 0.f;
};