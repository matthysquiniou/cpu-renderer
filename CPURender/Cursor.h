#pragma once
#include <glm/vec2.hpp>

struct SDL_Window;

class Cursor
{
public:
	static void SetCursorPositionCenter();
	static void SetCursorVisible(bool visible);
	static glm::vec2 GetCursorDelta();
	static void ResetCursorDelta();
	static void SetSDLWindow(SDL_Window* window);
private:
	static glm::vec2 m_lastCursorPos;
	static SDL_Window* m_window;
};