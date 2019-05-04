#pragma once
#include "GlobalDefs.h"

enum class EAxis : uint8
{
	MOVEUP,
	MOVERIGHT,
	MOVEFORWARD,
	TURN,
	LOOKUP,
	NUM
};

enum class ECommand : uint8
{
	NONE,
	QUIT,
	COUNT
};

enum class EMouseKey : uint8
{
	Left = 0,
	Right = 1,
	Middle = 2
};

enum class EKeyboardKey : uint16
{
	/* Printable keys */
	SPACE = 32,
	APOSTROPHE = 39,/* ' */
	COMMA = 44, /* , */
	MINUS = 45,  /* - */
	PERIOD = 46,/* . */
	SLASH = 47, /* / */
	SEMICOLON = 59,/* ; */
	EQUAL = 61, /* = */
	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,
	LEFT_BRACKET = 91,  /* [ */
	BACKSLASH = 92,  /* \ */
	RIGHT_BRACKET = 93, /* ] */
	GRAVE_ACCENT = 96,  /* ` */
	WORLD_1 = 161,/* non-US #1 */
	WORLD_2 = 162, /* non-US #2 */

	ESCAPE = 256,

};

enum class EKeyState : uint8
{
	Release = 0,
	Press = 1,
	Hold = 2
};

class Input
{
public:
	inline float GetAxis(EAxis axis) { return mAixValues[(int)axis]; }
	ECommand FitchCommand();
	inline ECommand GetCommand(){return mCommand;}
	void Update(const float p_deltaTime);
	void OnKeyboardAction(EKeyboardKey key, EKeyState state);
	void OnMouseAction(EMouseKey key, EKeyState state);
	void OnMouseWheel(int32 yoffset);
	bool GetKey(EKeyboardKey key);
	static void SetMousePos(uint32 x, uint32 y);
	static void GetCursorPosition(double* xpos, double* ypos);
	double MouseX, MouseY;
protected:
	float mAixValues[EAxis::NUM];
	ECommand mCommand;

	bool bFirstUpdate = true;
};

