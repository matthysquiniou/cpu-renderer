#include "Clock.h"

Clock::Clock() {
	m_previousTime = std::chrono::high_resolution_clock::now();
}

float Clock::GetDt() {
	return m_lastDt;
}

float Clock::GetTotalTime()
{
	return m_totalTime;
}

float Clock::UpdateDt()
{
	std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = now - m_previousTime;
	m_previousTime = now;
	m_lastDt = duration.count();
	m_totalTime += m_lastDt;
	return m_lastDt;
}
