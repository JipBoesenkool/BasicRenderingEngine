//
// Created by Jip Boesenkool on 09/12/2017.
//
#include <glad/glad.h>

#include <glm/ext.hpp>
#include <iostream>

#include "OGL_Renderer.h"
#include "Shader.h"
#include "../../include/deps/FastDelegate/FastDelegate.h"

#include "../../include/events/WindowResizeEvent.h"

//---------------------------------------------------------------------------------------------------------------------
// Public Functions
//---------------------------------------------------------------------------------------------------------------------
OGLRenderer::OGLRenderer( )
		: IRenderer()
{
	PrintVersions();
}

OGLRenderer::OGLRenderer( int width, int height )
		: IRenderer()
{
	m_width	= width;
	m_height= height;

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	//Setup renderer
	PrintVersions();
	SetupOpenglSettings();
	SetupShaders();
	SetupBuffers();
	SetupEventListeners();
}

OGLRenderer::~OGLRenderer()
{
	// remove the delegates from the event manager
	EventListenerDelegate delegateFunc = fastdelegate::MakeDelegate(this, &OGLRenderer::ResizeCallback);
	IEventManager::Get()->RemoveListener("WindowResizeEvent", delegateFunc);

	// remove the delegates from the event manager
	delegateFunc = fastdelegate::MakeDelegate(this, &OGLRenderer::HandleKeyInput);
	IEventManager::Get()->RemoveListener("KeyEvent", delegateFunc);
}

void OGLRenderer::Render()
{
	// Clear the color and depth buffers
	//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Render scene?

	//Debug: Custom render targets
	if(NDEBUG)
	{
		float w = (m_width / 6.0f) / m_width;
		float h = (m_height / 6.0f) / m_height;
		RenderTargetToScreen(m_gPosition, w * 0, h * 0, w, h);
		RenderTargetToScreen(m_gNormal, w * 1, h * 0, w, h);
		RenderTargetToScreen(m_gAlbedoSpec, w * 2, h * 0, w, h);
		//RenderTargetToScreen(m_rboDepth, w * 3, h * 0, w, h); doesn't work
		RenderTargetToScreen(m_colorBuffers[0], w * 5, h * 0, w, h);

		//m_renderer.RenderTargetToScreen(m_renderer.pingpongColorbuffers[1], w * 3, h * 0, w, h);
		//m_renderer.RenderTargetToScreen(m_renderer.m_guiTexID, w * 4, h * 0, w, h);
	}
}

void OGLRenderer::RenderTargetToScreen( unsigned int gbufferID, float x, float y, float width, float height )
{
	//Default screen shader
	m_defaultScreenShader->Use();

	glm::vec3 scale(width, height, 1.0f);
	glm::mat4 model(1.0f);
	model = glm::translate( model, glm::vec3(
			x * 2 + scale.x - 1.0f, //relative to left bottom
			y * 2 + scale.y - 1.0f, //relative to left bottom
			0.0f
	));
	model = glm::scale(model, scale );
	m_defaultScreenShader->SetMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbufferID);
	RenderQuad();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OGLRenderer::BindFrameBuffer()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
}

/*
void OGLRenderer::BindFrameBuffer(std::string frameBufferName)
{
	OglShaderMap::iterator findIt = m_gbufferMap.find(frameBufferName);
	if (findIt == m_gbufferMap.end())
	{
		std::cerr << "OGLRenderer::BindFrameBuffer: gbuffer not found." << std::endl;
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, findIt->second);
}
 */

void OGLRenderer::UnbindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OGLRenderer::HandleKeyInput( IEventPtr pEvent )
{
	std::shared_ptr<KeyEvent> e = std::static_pointer_cast<KeyEvent>(pEvent);
	if(e->m_action == GLFW_PRESS)
	{
		if(e->m_key == GLFW_KEY_F10)
		{
			NDEBUG = !NDEBUG;
			std::cout << NDEBUG << std::endl;
		}
		if(e->m_key == GLFW_KEY_F9)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if(e->m_key == GLFW_KEY_F8)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

}

void OGLRenderer::Resize(int width, int height)
{
	m_width = width;
	m_height = height;

	SetupBuffers();

	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, m_width, m_height);
}

void OGLRenderer::ResizeCallback(IEventPtr pEvent)
{
	std::shared_ptr<WindowResizeEvent> pCastEventData = std::static_pointer_cast<WindowResizeEvent>(pEvent);
	Resize(pCastEventData->m_width, pCastEventData->m_height);
}

unsigned int OGLRenderer::LoadShader( const char *vertexPath, const char *fragmentPath, const char *shaderName )
{
	Shader *shader = new Shader(vertexPath, fragmentPath);
	m_shaderMap[shaderName] = shader;
	return shader->ID;
}

//Primitives
// RenderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
void OGLRenderer::RenderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
				1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				// right face
				1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
				1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
				// bottom face
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				// top face
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
				1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		//Position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		//Normal
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		//Texcoords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
// and post-processing effects.
void OGLRenderer::RenderQuad()
{
	glDisable(GL_CULL_FACE);

	if (quadVAO == 0)
	{
		float quadVertices[] = {
				// Positions    // Texture Coords
				-1.0f,  1.0f,   0.0f, 1.0f,
				-1.0f, -1.0f,   0.0f, 0.0f,
				1.0f,  1.0f,   1.0f, 1.0f,
				1.0f, -1.0f,   1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glEnable(GL_CULL_FACE);
}

//---------------------------------------------------------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------------------------------------------------------
void OGLRenderer::PrintVersions()
{
	// Get info of GPU and supported OpenGL version
	printf("renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

	//If the shading language symbol is defined
#ifdef GL_SHADING_LANGUAGE_VERSION
	std::printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
}

void OGLRenderer::SetupOpenglSettings()
{
	// Enable depth buffering
	glEnable(GL_DEPTH_TEST);
	// Related to shaders and z value comparisons for the depth buffer
	glDepthFunc(GL_LEQUAL);
	// Set polygon drawing mode to fill front and back of each polygon
	// You can also use the paramter of GL_LINE instead of GL_FILL to see wireframes
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Disable backface culling to render both sides of polygons
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Set clear color
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void OGLRenderer::SetupEventListeners()
{

	// Window resize
	EventListenerDelegate delegateFunc = fastdelegate::MakeDelegate(this, &OGLRenderer::ResizeCallback);
	IEventManager::Get()->AddListener("WindowResizeEvent", delegateFunc);

	// key
	delegateFunc = fastdelegate::MakeDelegate(this, &OGLRenderer::HandleKeyInput);
	IEventManager::Get()->AddListener("KeyEvent", delegateFunc);
}

void OGLRenderer::SetupBuffers()
{
	// ------------------------------
	// configure g-buffer framebuffer
	// ------------------------------
	glGenFramebuffers(1, &m_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
	{
		// position color buffer
		glGenTextures(1, &m_gPosition);
		glBindTexture(GL_TEXTURE_2D, m_gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);
		// normal color buffer
		glGenTextures(1, &m_gNormal);
		glBindTexture(GL_TEXTURE_2D, m_gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);
		// color + specular color buffer
		glGenTextures(1, &m_gAlbedoSpec);
		glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedoSpec, 0);

		// Depth
		glGenRenderbuffers(1, &m_rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);
		/*
		// Depth buffer
		glGenTextures(1, &m_gDepth);
		glBindTexture(GL_TEXTURE_2D, m_gDepth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gDepth, 0);
		 */

		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "gBuffer Framebuffer not complete!" << std::endl;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ------------------------------
	// configure HDR framebuffer
	// ------------------------------
	// Set up floating point framebuffer to render scene to
	glGenFramebuffers(1, &m_hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);
	{
		// Create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
		glGenTextures(2, m_colorBuffers);
		for (GLuint i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, m_colorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorBuffers[i], 0);
		}
		// Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
		GLuint attachments2[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments2);

		// create and attach depth buffer (renderbuffer)
/*
		glGenRenderbuffers(1, &m_hdrRboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, m_hdrRboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_hdrRboDepth);
 */

		// - Finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "hdrFBO: Framebuffer not complete!" << std::endl;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OGLRenderer::SetupShaders()
{
	m_defaultScreenShader = new Shader(
			"resources/Shaders/screen.vert",
			"resources/Shaders/screen.frag"
	);

	m_shaderGeometryPass = new Shader(
			"resources/Shaders/gbuffer.vert",
			"resources/Shaders/gbuffer.frag"
	);
}