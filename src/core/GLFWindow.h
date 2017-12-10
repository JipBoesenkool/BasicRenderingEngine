//
// Created by Jip Boesenkool on 09/12/2017.
//
#ifndef GLFWINDOW_H
#define GLFWINDOW_H

#include "../../include/core/IWindow.h"
#include "../../include/events/IEventManager.h"

#include <iostream>
#include <GLFW/glfw3.h>

class GLFWindow : public IWindow
{
//---------------------------------------------------------------------------------------------------------------------
// Members
//---------------------------------------------------------------------------------------------------------------------
public:
	static GLFWwindow* mp_window;
//---------------------------------------------------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	GLFWindow( );
	GLFWindow( int width, int height, char* title );

public:
	void SetupCallbacks();

	//Input handling
	static void ErrorCallback(int error, const char* description);
	static void ResizeCallback(GLFWwindow* window, int width, int height);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

//---------------------------------------------------------------------------------------------------------------------
//Interface Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	void CreateWindow(int width, int height, char* title) override;
	void DestroyWindow() override;

	void SwapBuffers() override;
	void Resize(int width, int height) override;
	bool IsClosed() override;
	// void KeyCallback(int key, int scancode, int action, int mods) override;
	// void MouseCallback(int xPos, int yPos) override;
	// void ScrollCallback(int xOffset, int yOffset) override;
};

#endif //GLFWINDOW_H
