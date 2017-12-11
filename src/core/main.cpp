#include "main.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "../../include/events/IEventManager.h"
#include "../eventManager/EventManager.h"

#include "../../include/core/IWindow.h"
#include "GLFWindow.h"

#include "../../include/renderer/IRenderer.h"
#include "../ogl_renderer/OGL_Renderer.h"

#include "../ogl_renderer/Camera.h"
#include "../ogl_renderer/model/Model.h"
#include "../ogl_renderer/modelLoaders/AssimpLoader.h"
#include "../ogl_renderer/Shader.h"

GLFWwindow* window;
OGLRenderer* oglRenderer;
Camera camera( glm::vec3(0.0f, 0.0f, 20.0f) );
std::vector<glm::vec3> objectPositions;
Model* nanosuit;
Model* cube;
Shader* shaderProgram;
Shader* modelShaderProgram;
Shader* normalShaderProgram;


class Input
{
private:
	Camera* m_pCam;
public:
	Input(Camera* camera)
	{
		m_pCam = camera;
	}

	void HandleKeyInput(IEventPtr pEvent)
	{
		std::shared_ptr<KeyEvent> e = std::static_pointer_cast<KeyEvent>(pEvent);
		if(e->m_action == GLFW_PRESS)
		{
			if(e->m_key == GLFW_KEY_W)
			{
				camera.ProcessKeyboard(FORWARD, 1.0f);
			}
			if(e->m_key == GLFW_KEY_S)
			{
				camera.ProcessKeyboard(BACKWARD, 1.0f);
			}
			if(e->m_key == GLFW_KEY_A)
			{
				camera.ProcessKeyboard(LEFT, 1.0f);
			}
			if(e->m_key == GLFW_KEY_D)
			{
				camera.ProcessKeyboard(RIGHT, 1.0f);
			}
		}
	}
};
Input input(&camera);

//TODO: move to good place
void RenderTest()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(camera.Zoom, (float)oglRenderer->m_width / (float)oglRenderer->m_height, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();

	glm::mat4 model(1.0f);
	normalShaderProgram->Use();
	normalShaderProgram->SetMat4("projection", projection);
	normalShaderProgram->SetMat4("view", view);
	for (unsigned int i = 0; i < objectPositions.size(); i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, objectPositions[i]);
		model = glm::scale(model, glm::vec3(0.25f));
		nanosuit->Draw(model, normalShaderProgram);
	}

	/*
	normalShaderProgram->Use();
	normalShaderProgram->SetMat4("projection", projection);
	normalShaderProgram->SetMat4("view", view);
 	m = glm::translate(m, glm::vec3(5.0f, 0.0f, 0.0f) );
	normalShaderProgram->SetMat4("model", m);
	cube->m_meshes[0].Draw(normalShaderProgram);
	 */
}

void RenderGbuffer()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(camera.Zoom, (float)oglRenderer->m_width / (float)oglRenderer->m_height, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();

	// -----------------------------------------------------------------
	// 1. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, oglRenderer->m_gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1.0f);
		modelShaderProgram->Use();
		modelShaderProgram->SetMat4("projection", projection);
		modelShaderProgram->SetMat4("view", view);
		for (unsigned int i = 0; i < objectPositions.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, objectPositions[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			nanosuit->Draw(model, modelShaderProgram);
		}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Draw all the framebuffers
	oglRenderer->Render();
}

int main(void)
{
	new EventManager(true);
	new GLFWindow(1024, 720, "Basic Game Engine");
	oglRenderer = new OGLRenderer(1024, 720);

	shaderProgram = new Shader(
			"resources/Shaders/shader.vert",
			"resources/Shaders/shader.frag"
	);
	normalShaderProgram = new Shader(
			"resources/Shaders/shaderNormal.vert",
			"resources/Shaders/shaderNormal.frag"
	);
	modelShaderProgram = new Shader(
			"resources/Shaders/modelLoading.vert",
			"resources/Shaders/modelLoading.frag"
	);

	// load models
	// -----------
	AssimpLoader loader;
	nanosuit = new Model("resources/models/nanosuit/nanosuit.obj", oglRenderer->m_shaderGeometryPass, &loader);
	cube = new Model("resources/models/cube.obj", shaderProgram);
	objectPositions.push_back(glm::vec3(-3.0,  -3.0, -3.0));
	objectPositions.push_back(glm::vec3( 0.0,  -3.0, -3.0));
	objectPositions.push_back(glm::vec3( 3.0,  -3.0, -3.0));
	objectPositions.push_back(glm::vec3(-3.0,  -3.0,  0.0));
	objectPositions.push_back(glm::vec3( 0.0,  -3.0,  0.0));
	objectPositions.push_back(glm::vec3( 3.0,  -3.0,  0.0));
	objectPositions.push_back(glm::vec3(-3.0,  -3.0,  3.0));
	objectPositions.push_back(glm::vec3( 0.0,  -3.0,  3.0));
	objectPositions.push_back(glm::vec3( 3.0,  -3.0,  3.0));

	//Input
	// key
	EventListenerDelegate delegateFunc = fastdelegate::MakeDelegate(&input, &Input::HandleKeyInput);
	IEventManager::Get()->AddListener("KeyEvent", delegateFunc);

	// Loop while GLFW window should stay open
	while ( !IWindow::Get()->IsClosed() )
	{
		//Update
		IEventManager::Get()->Update(); //Input

		//Render
		RenderTest();

		//IRenderer::Get()->Render();
		IWindow::Get()->SwapBuffers();
	}

	IWindow::Get()->DestroyWindow();
	exit(EXIT_SUCCESS);
}