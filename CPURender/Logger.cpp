#include "Logger.h"

bool Logger::s_canLog = false;

void Logger::HandleEvent(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        switch (event.key.key)
        {
        case SDLK_L:
            s_canLog = true;
            break;
        default:
            break;
        }
    }
}

void Logger::ResetKey()
{
    s_canLog = false;
}
