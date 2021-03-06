#include <TFE_Input/input.h>
#include <memory.h>
#include <string.h>
#include <assert.h>

namespace TFE_Input
{
	#define BUFFERED_TEXT_LEN 64

	////////////////////////////////////////////////////////
	// Input State
	////////////////////////////////////////////////////////
	f32 s_axis[AXIS_COUNT] = { 0 };
	u8 s_buttonDown[CONTROLLER_BUTTON_COUNT] = { 0 };
	u8 s_buttonPressed[CONTROLLER_BUTTON_COUNT] = { 0 };

	u8 s_keyDown[KEY_COUNT] = { 0 };
	u8 s_keyPressed[KEY_COUNT] = { 0 };

	char s_bufferedText[BUFFERED_TEXT_LEN];
	u8 s_bufferedKey[KEY_COUNT];

	u8 s_mouseDown[MBUTTON_COUNT] = { 0 };
	u8 s_mousePressed[MBUTTON_COUNT] = { 0 };
	
	s32 s_mouseWheel[2] = { 0 };
	s32 s_mouseMove[2] = { 0 };
	s32 s_mousePos[2] = { 0 };

	bool s_relativeMode = false;

	////////////////////////////////////////////////////////
	// Implementation
	////////////////////////////////////////////////////////
	void endFrame()
	{
		for (u32 i = 0; i < CONTROLLER_BUTTON_COUNT; i++)
		{
			s_buttonPressed[i] = 0;
		}
		for (u32 i = 0; i < MBUTTON_COUNT; i++)
		{
			s_mousePressed[i] = 0;
		}
		s_mouseWheel[0] = 0;
		s_mouseWheel[1] = 0;
		memset(s_keyPressed, 0, KEY_COUNT);
		memset(s_bufferedKey, 0, KEY_COUNT);
		memset(s_bufferedText, 0, BUFFERED_TEXT_LEN);
	}

	// Set, from the OS
	void setAxis(Axis axis, f32 value)
	{
		s_axis[axis] = value;
	}

	void setButtonDown(Button button)
	{
		if (!s_buttonDown[button])
		{
			s_buttonPressed[button] = 1;
		}
		s_buttonDown[button] = 1;
	}

	void setButtonUp(Button button)
	{
		s_buttonDown[button] = 0;
	}

	void setKeyDown(KeyboardCode key)
	{
		if (!s_keyDown[key])
		{
			s_keyPressed[key] = 1;
		}
		s_keyDown[key] = 1;
	}

	void setKeyUp(KeyboardCode key)
	{
		s_keyDown[key] = 0;
	}

	void setMouseButtonDown(MouseButton button)
	{
		if (!s_mouseDown[button])
		{
			s_mousePressed[button] = 1;
		}
		s_mouseDown[button] = 1;
	}

	void setMouseButtonUp(MouseButton button)
	{
		s_mouseDown[button] = 0;
	}

	void setMouseWheel(s32 dx, s32 dy)
	{
		s_mouseWheel[0] = dx;
		s_mouseWheel[1] = dy;
	}

	void setRelativeMousePos(s32 x, s32 y)
	{
		s_mouseMove[0] = x;
		s_mouseMove[1] = y;
	}

	void setMousePos(s32 x, s32 y)
	{
		s_mousePos[0] = x;
		s_mousePos[1] = y;
	}

	void enableRelativeMode(bool enable)
	{
		s_relativeMode = enable;
	}
	
	// Buffered Input
	void setBufferedInput(const char* text)
	{
		strcpy(s_bufferedText, text);
	}

	void setBufferedKey(KeyboardCode key)
	{
		s_bufferedKey[key] = 1;
	}

	// Get
	f32 getAxis(Axis axis)
	{
		return s_axis[axis];
	}

	void getMouseWheel(s32* dx, s32* dy)
	{
		assert(dx && dy);

		*dx = s_mouseWheel[0];
		*dy = s_mouseWheel[1];
	}

	void getMouseMove(s32* x, s32* y)
	{
		assert(x && y);

		*x = s_mouseMove[0];
		*y = s_mouseMove[1];
	}

	void getMousePos(s32* x, s32* y)
	{
		assert(x && y);

		*x = s_mousePos[0];
		*y = s_mousePos[1];
	}

	bool buttonDown(Button button)
	{
		return s_buttonDown[button] != 0;
	}

	bool buttonPressed(Button button)
	{
		return s_buttonPressed[button] != 0;
	}

	bool keyDown(KeyboardCode key)
	{
		return s_keyDown[key] != 0;
	}

	bool keyPressed(KeyboardCode key)
	{
		return s_keyPressed[key] != 0;
	}

	bool mouseDown(MouseButton button)
	{
		return s_mouseDown[button] != 0;
	}

	bool mousePressed(MouseButton button)
	{
		return s_mousePressed[button] != 0;
	}

	bool relativeModeEnabled()
	{
		return s_relativeMode;
	}

	// Buffered Input
	const char* getBufferedText()
	{
		return s_bufferedText;
	}

	bool bufferedKeyDown(KeyboardCode key)
	{
		return s_bufferedKey[key];
	}
}
