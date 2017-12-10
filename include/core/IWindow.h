//
// Created by Jip Boesenkool on 09/12/2017.
//
#ifndef IWINDOW_H
#define IWINDOW_H
//NOTE: technically not a interface but a abstract class.
class IWindow
{
//---------------------------------------------------------------------------------------------------------------------
// Members
//---------------------------------------------------------------------------------------------------------------------
public:
	char* 	m_title;
	int 	m_width;
	int 	m_height;
//---------------------------------------------------------------------------------------------------------------------
// Getters & Setters
//---------------------------------------------------------------------------------------------------------------------
public:
	/**
	// Getter for the global window. This is the window handle that is used by the majority of the engine.
	*/
	static IWindow* Get(void);

//---------------------------------------------------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	explicit IWindow(void);
	virtual ~IWindow(void);

//---------------------------------------------------------------------------------------------------------------------
// Interface Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	virtual void CreateWindow(int width, int height, char* title) = 0;
	virtual void DestroyWindow() = 0;

	virtual void Resize(int width, int height) = 0;
	virtual void SwapBuffers() = 0;
	virtual bool IsClosed() = 0;

	// virtual void KeyCallback(int key, int scancode, int action, int mods) = 0;
	// virtual void MouseCallback(int xPos, int yPos) = 0;
	// virtual void ScrollCallback(int xOffset, int yOffset) = 0;

	//NOTE: also not window related but framework related!
	//virtual double GetDeltaTime() = 0;

	//TODO: get native handle?
};

#endif //IWINDOW_H
