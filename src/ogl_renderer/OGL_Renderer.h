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
#include <queue>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "model/Mesh.h"

//typedef
struct RenderPass
{
	Shader* m_shader;
	std::queue<Mesh *> m_renderQueue;
};

typedef std::map<std::string, RenderPass > OglRenderPassMap; //shader effect, RenderPassObj
typedef std::map<std::string, OglRenderPassMap > OglPassMap; //pass, OglRenderPassMap

class OGLRenderer : public IRenderer
{
//---------------------------------------------------------------------------------------------------------------------
// Members
//---------------------------------------------------------------------------------------------------------------------
public:
	int m_width;
	int m_height;
	OglPassMap m_shaderMap;

	//gBuffer
	unsigned int m_gBuffer, m_gPosition, m_gNormal, m_gAlbedoSpec, m_gDepth, m_rboDepth;
	Shader* m_activeShaderGeometry;
	Shader* m_shaderGeometryPass;
	Shader* m_shaderGeometryNormalPass;
	//hdrBuffer
	unsigned int m_hdrFBO, m_colorBuffers[2], m_hdrRboDepth;
	Shader* m_shaderBlur;

	// Post-processing
	// Ping pong framebuffer for blurring
	unsigned m_pingpongFBO[2];
	unsigned m_pingpongColorbuffers[2];
	Shader* m_shaderBloomFinal;

private:
	// Post-processing
	// Ping pong framebuffer for blurring
	const float EXPOSURE = 1.0f;
	bool m_bloom = true;

	//Primitives
	Shader* m_defaultScreenShader;
	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;

	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;

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
	void RenderQueue();
	void PrintVersions();
	void SetupOpenglSettings();

	void SetupEventListeners();
	void SetupBuffers();
	void SetupShaders();
	void SetupGbufferShaders();

	void GeometryPass();

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

	void AddToRenderQueue(Mesh* mesh) override;

	unsigned int LoadShader(const char *renderPass ,const char *effectName, const char * vertexPath, const char* geometryPath = nullptr ) override;

//Primitives
	void RenderCube() override;
	void RenderQuad() override;
};


#endif //OGL_Renderer_H
