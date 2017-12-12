//
// Created by Jip Boesenkool on 09/12/2017.
//
#include "GLFWindow.h"

GLFWwindow* GLFWindow::mp_window;

//Public Functions
GLFWindow::GLFWindow( )
		: IWindow()
{
	m_width	= 1080;
	m_height= 1920;
	m_title	= "NULL";
}

GLFWindow::GLFWindow( int width, int height, char* title )
		: IWindow()
{
	m_width	= width;
	m_height= height;
	m_title	= title;

	//Create the window
	GLFWindow::CreateWindow(m_width, m_height, m_title);

	//Setup GLFW callbacks
	SetupCallbacks();
}

void GLFWindow::SetupCallbacks()
{
	// Set the error callback
	glfwSetErrorCallback(GLFWindow::ErrorCallback);
	// Set the key callback
	glfwSetKeyCallback(GLFWindow::mp_window, GLFWindow::KeyCallback);
	// Set the mouse callback
	glfwSetCursorPosCallback(GLFWindow::mp_window, GLFWindow::MouseCallback);
	glfwSetScrollCallback(GLFWindow::mp_window, GLFWindow::ScrollCallback);
	// Set the window resize callback
	glfwSetFramebufferSizeCallback(GLFWindow::mp_window, GLFWindow::ResizeCallback);
}

void GLFWindow::ErrorCallback(int error, const char* description)
{
	// Print error
	fputs(description, stderr);
}

void GLFWindow::ResizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif

	IWindow::Get()->Resize(width, height);
	// Instantiate the event. The event system tracks all events with smart
	// pointers, so you must instantiate the event using a smart pointer.
	std::shared_ptr<WindowResizeEvent> pNewEvent( new WindowResizeEvent(width, height) );
	// Trigger the event.
	IEventManager::Get()->TriggerEvent(pNewEvent);
}

void GLFWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Instantiate the event. The event system tracks all events with smart
	// pointers, so you must instantiate the event using a smart pointer.
	std::shared_ptr<KeyEvent> pNewEvent( new KeyEvent(key, scancode, action, mods) );
	// Trigger the event.
	IEventManager::Get()->QueueEvent(pNewEvent);
}

void GLFWindow::MouseCallback( GLFWwindow *window, double xpos, double ypos )
{
	// Instantiate the event. The event system tracks all events with smart
	// pointers, so you must instantiate the event using a smart pointer.
	std::shared_ptr<MouseMoveEvent> pNewEvent( new MouseMoveEvent(xpos, ypos) );
	// Trigger the event.
	IEventManager::Get()->QueueEvent(pNewEvent);
}

void GLFWindow::ScrollCallback( GLFWwindow *window, double xoffset, double yoffset )
{
	// Instantiate the event. The event system tracks all events with smart
	// pointers, so you must instantiate the event using a smart pointer.
	std::shared_ptr<MouseScrollEvent> pNewEvent( new MouseScrollEvent(xoffset, yoffset) );
	// Trigger the event.
	IEventManager::Get()->QueueEvent(pNewEvent);
}

//Interface Functions
void GLFWindow::CreateWindow( int width, int height, char* title )
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	//TODO: should not be here, opengl specific
#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif


	// Create the GLFW window
	GLFWindow::mp_window = glfwCreateWindow(m_width, m_height, m_title, NULL, NULL);

	// Check if the window could not be created
	if (!GLFWindow::mp_window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return;
	}

	// Make the context of the window
	glfwMakeContextCurrent(GLFWindow::mp_window);
	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(GLFWindow::mp_window, &m_width, &m_height);

	// Call the resize callback to make sure things get drawn immediately
	Resize(m_width, m_height);
}

void GLFWindow::DestroyWindow()
{
	// Destroy the window
	glfwDestroyWindow(GLFWindow::mp_window);

	// Terminate GLFW
	glfwTerminate();
}

void GLFWindow::SwapBuffers()
{
	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(GLFWindow::mp_window);
}

void GLFWindow::Resize(int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(GLFWindow::mp_window, &width, &height); // In case your Mac has a retina display
#endif
	m_width = width;
	m_height= height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);
}

bool GLFWindow::IsClosed()
{
	if( glfwWindowShouldClose(GLFWindow::mp_window) )
	{
		return true;
	}
	return false;
}