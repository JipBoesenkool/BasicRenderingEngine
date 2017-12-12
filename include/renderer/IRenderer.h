//
// Created by Jip Boesenkool on 09/12/2017.
//

#ifndef IRENDERER_H
#define IRENDERER_H

#include "../events/IEvent.h"

class IRenderer
{
//---------------------------------------------------------------------------------------------------------------------
// Getters & Setters
//---------------------------------------------------------------------------------------------------------------------
public:
	/**
	// Getter for the global renderer. This is the renderer that is used by the majority of the engine.
	*/
	static IRenderer* Get(void);

//---------------------------------------------------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	explicit IRenderer(void);
	virtual ~IRenderer(void);

//---------------------------------------------------------------------------------------------------------------------
// Interface Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	//Render all render target and swap buffers.
	virtual void Render() = 0;
	virtual void RenderTargetToScreen( unsigned int gbufferID, float x = 0.0f, float y = 0.0f, float width = 1.0f, float height = 1.0f ) = 0;

	virtual void BindFrameBuffer() = 0;
	virtual void UnbindFrameBuffer() = 0;

	virtual void HandleKeyInput(IEventPtr pEvent) = 0;
	virtual void Resize(int width, int height) = 0;
	virtual void ResizeCallback(IEventPtr pEvent) = 0;

	//TODO: how does this work for other rendering apis?
	virtual unsigned int LoadShader(const char * vertexPath, const char * fragmentPath, const char *shaderName) = 0;

//Primitives
	virtual void RenderCube() = 0;
	virtual void RenderQuad() = 0;
};

#endif //IRENDERER_H
