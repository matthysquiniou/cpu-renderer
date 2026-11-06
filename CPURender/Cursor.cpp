#include "Cursor.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <SDL3/SDL_mouse.h>
#include <glm/vec2.hpp>

#include "main.h"

glm::vec2 Cursor::m_lastCursorPos = glm::vec2(0.0f);
SDL_Window* Cursor::m_window = nullptr;

void Cursor::SetCursorPositionCenter()
{
    if (m_window == nullptr)
        return;
    SDL_WarpMouseInWindow(m_window, WIDTH / 2.0f, HEIGHT / 2.0f);
}

void Cursor::SetCursorVisible(bool visible)
{
    if (visible)
    {
        while (ShowCursor(TRUE) < 0) {}
    }
    else
    {
        while (ShowCursor(FALSE) >= 0) {}
    }
}

glm::vec2 Cursor::GetCursorDelta()
{
    glm::vec2 currentPos;
    SDL_GetMouseState(&currentPos.x, &currentPos.y);

    glm::vec2 delta = currentPos - m_lastCursorPos;
    m_lastCursorPos = currentPos;
    return delta;
}

void Cursor::ResetCursorDelta()
{
    glm::vec2 currentPos;
    SDL_GetMouseState(&currentPos.x, &currentPos.y);
    m_lastCursorPos = currentPos;
}

void Cursor::SetSDLWindow(SDL_Window* window)
{
    m_window = window;
}
