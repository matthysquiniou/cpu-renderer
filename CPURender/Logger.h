#pragma once
#include <iostream>
#include <SDL3/SDL_events.h>

class Logger
{
public:
    static void HandleEvent(const SDL_Event& event);
    static void ResetKey();

    template<typename... Args>
    static void Log(const Args&... args);

private:
    static bool s_canLog;
};

#include "Logger.inl"