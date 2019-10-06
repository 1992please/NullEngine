#pragma once
#include "Input.h"

class EngineBase
{
public:
	static EngineBase* Engine;
	struct GLFWwindow* Window;

	struct APPINFO
	{
		char title[128];
		int windowWidth;
		int windowHeight;
		int windowPosX;
		int windowPosY;
		int majorVersion;
		int minorVersion;
		int samples;
		union
		{
			struct
			{
				unsigned int    fullscreen : 1;
				unsigned int    vsync : 1;
				unsigned int    cursor : 1;
				unsigned int    stereo : 1;
				unsigned int    debug : 1;
				unsigned int    robust : 1;
			};
			unsigned int        all;
		} flags;
	};

	EngineBase();
	virtual void Run();
protected:
	APPINFO EngineConfigs;
	Input mInput;
	float CurrentTime;
	virtual void Init() {}
	virtual void StartUp() {}
	virtual void Update(float DeltaTime) {}
	virtual void Render() {}
	virtual void Shutdown() {}
	virtual void OnResize(uint32 Width, uint32 Height);
	inline void Quit() { bIsRunning = false; }
	inline Input* GetInput() { return &mInput; }
private:
	bool bIsRunning;
	void MainLoop();
	static void KeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* pWindow, int Button, int Action, int Mode);
	static void MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void glfw_FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
	//static void glfw_ReSizeCallback(GLFWwindow* window, int width, int height);
};