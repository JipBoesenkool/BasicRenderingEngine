#include "main.h"
#include "../../include/events/IEventManager.h"
#include "../eventManager/EventManager.h"

#include "../../include/core/IWindow.h"
#include "GLFWindow.h"

#include "../../include/renderer/IRenderer.h"
#include "../ogl_renderer/OGL_Renderer.h"

#include "../ogl_renderer/Camera.h"

GLFWwindow* window;
OGLRenderer* oglRenderer;
Camera camera( glm::vec3(0.0f, 1.0f, 20.0f) );
std::vector<glm::vec3> objectPositions;

//TODO: move to good place
void RenderGbuffer()
{
	// -----------------------------------------------------------------
	// 1. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, oglRenderer->m_gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)oglRenderer->m_width / (float)oglRenderer->m_height, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model;
	oglRenderer->m_shaderGeometryPass->Use();
	oglRenderer->m_shaderGeometryPass->SetMat4("projection", projection);
	oglRenderer->m_shaderGeometryPass->SetMat4("view", view);
	for (unsigned int i = 0; i < objectPositions.size(); i++)
	{
		model = glm::mat4();
		model = glm::translate(model, objectPositions[i]);
		model = glm::scale(model, glm::vec3(0.25f));
		oglRenderer->m_shaderGeometryPass->SetMat4("model", model);
		//nanosuit.Draw(shaderGeometryPass);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Draw all the framebuffers
}

int main(void)
{
	new EventManager(true);
	new GLFWindow(1920, 1080, "Basic Game Engine");
	oglRenderer = new OGLRenderer(1920, 1080);

	objectPositions.push_back(glm::vec3(-3.0,  -3.0, -3.0));
	objectPositions.push_back(glm::vec3( 0.0,  -3.0, -3.0));
	objectPositions.push_back(glm::vec3( 3.0,  -3.0, -3.0));
	objectPositions.push_back(glm::vec3(-3.0,  -3.0,  0.0));
	objectPositions.push_back(glm::vec3( 0.0,  -3.0,  0.0));
	objectPositions.push_back(glm::vec3( 3.0,  -3.0,  0.0));
	objectPositions.push_back(glm::vec3(-3.0,  -3.0,  3.0));
	objectPositions.push_back(glm::vec3( 0.0,  -3.0,  3.0));
	objectPositions.push_back(glm::vec3( 3.0,  -3.0,  3.0));

	// Loop while GLFW window should stay open
	while ( !IWindow::Get()->IsClosed() )
	{
		//Update
		IEventManager::Get()->Update(); //Input

		//Render
		//RenderGbuffer();

		//IRenderer::Get()->Render();
		IWindow::Get()->SwapBuffers();
	}

	IWindow::Get()->DestroyWindow();
	exit(EXIT_SUCCESS);
}