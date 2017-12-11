//
// Created by Jip Boesenkool on 09/12/2017.
//
#ifndef OGL_Renderer_H
#define OGL_Renderer_H

#include "../../include/renderer/IRenderer.h"
#include "../../include/events/IEventManager.h"

#include <glad/glad.h>
//TODO: remove glfw
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

//typedef
typedef std::map<std::string, Shader* > OglShaderMap;

class OGLRenderer : public IRenderer
{
//---------------------------------------------------------------------------------------------------------------------
// Members
//---------------------------------------------------------------------------------------------------------------------
public:
	int m_width;
	int m_height;
	OglShaderMap m_shaderMap;
	unsigned int m_gBuffer, m_gPosition, m_gNormal, m_gAlbedoSpec, m_rboDepth;
	Shader* m_shaderGeometryPass;
	Shader* m_defaultScreenShader;

private:

	GLuint quadVAO = 0;
	GLuint quadVBO;

	bool NDEBUG = false;
//---------------------------------------------------------------------------------------------------------------------
// Public Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	OGLRenderer(void);
	OGLRenderer(int width, int height);
	~OGLRenderer(void) override;

//---------------------------------------------------------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------------------------------------------------------
private:
	void PrintVersions();
	void SetupOpenglSettings();

	void SetupEventListeners();
	void SetupBuffers();
	void SetupShaders();

//---------------------------------------------------------------------------------------------------------------------
// Interface Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	//Render all render target and swap buffers.
	void Render() override;
	void RenderTargetToScreen( unsigned int gbufferID, float x = 0.0f, float y = 0.0f, float width = 1.0f, float height = 1.0f ) override;

	void BindFrameBuffer() override;
	//void BindFrameBuffer() override;
	void UnbindFrameBuffer() override;

	void HandleKeyInput(IEventPtr pEvent) override;
	void Resize(int width, int height) override;
	void ResizeCallback(IEventPtr pEvent) override;

	unsigned int LoadShader(const char * vertexPath, const char * fragmentPath, const char *shaderName) override;

//Primitives
	void RenderQuad() override;
};


#endif //OGL_Renderer_H
