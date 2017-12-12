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
std::vector<glm::vec3> objectPositions;
std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;
Model* nanosuit;
Model* cube;

Shader* shaderProgram;
Shader* modelShaderProgram;
Shader* normalShaderProgram;
Shader* shaderLightingPass;
Shader* shaderLightBox;


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
				m_pCam->ProcessKeyboard(FORWARD, 1.0f);
			}
			if(e->m_key == GLFW_KEY_S)
			{
				m_pCam->ProcessKeyboard(BACKWARD, 1.0f);
			}
			if(e->m_key == GLFW_KEY_A)
			{
				m_pCam->ProcessKeyboard(LEFT, 1.0f);
			}
			if(e->m_key == GLFW_KEY_D)
			{
				m_pCam->ProcessKeyboard(RIGHT, 1.0f);
			}
		}
	}
};
Input input(&IRenderer::camera);

//TODO: move to good place
void RenderTest()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(IRenderer::camera.Zoom, (float)oglRenderer->m_width / (float)oglRenderer->m_height, 0.1f, 1000.0f);
	glm::mat4 view = IRenderer::camera.GetViewMatrix();

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

void RenderModels()
{
	glm::mat4 projection = glm::perspective(IRenderer::camera.Zoom, (float)oglRenderer->m_width / (float)oglRenderer->m_height, 0.1f, 1000.0f);
	glm::mat4 view = IRenderer::camera.GetViewMatrix();

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
}

void RenderLights()
{
	glm::mat4 projection = glm::perspective(IRenderer::camera.Zoom, (float)oglRenderer->m_width / (float)oglRenderer->m_height, 0.1f, 1000.0f);
	glm::mat4 view = IRenderer::camera.GetViewMatrix();
	glm::mat4 model(1.0f);

	shaderLightBox->Use();
	shaderLightBox->SetMat4("projection", projection);
	shaderLightBox->SetMat4("view", view);
	for (unsigned int i = 0; i < lightPositions.size(); i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPositions[i]);
		model = glm::scale(model, glm::vec3(0.125f));
		shaderLightBox->SetMat4("model", model);
		shaderLightBox->SetVec3("lightColor", lightColors[i]);
		oglRenderer->RenderCube();
	}
}

void RenderQueueTest()
{
	glm::mat4 model;
	for (unsigned int i = 0; i < objectPositions.size(); i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, objectPositions[i]);
		model = glm::scale(model, glm::vec3(0.25f));
		nanosuit->Update( model );
	}

	IRenderer::Get()->Render();
}

void RenderGbuffer()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glm::mat4 projection = glm::perspective(IRenderer::camera.Zoom, (float)oglRenderer->m_width / (float)oglRenderer->m_height, 0.1f, 1000.0f);
	glm::mat4 view = IRenderer::camera.GetViewMatrix();

	// -----------------------------------------------------------------
	// 1. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, oglRenderer->m_gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::mat4 model(1.0f);
		oglRenderer->m_activeShaderGeometry->Use();
		oglRenderer->m_activeShaderGeometry->SetMat4("projection", projection);
		oglRenderer->m_activeShaderGeometry->SetMat4("view", view);
		for (unsigned int i = 0; i < objectPositions.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, objectPositions[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			nanosuit->Draw(model, oglRenderer->m_activeShaderGeometry);
		}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
	// -----------------------------------------------------------------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, oglRenderer->m_hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//TODO: remove hack, render geometry for depth buffer since depth buffer copy doesn't work
		RenderModels();
		glClear(GL_COLOR_BUFFER_BIT);

		shaderLightingPass->Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, oglRenderer->m_gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, oglRenderer->m_gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, oglRenderer->m_gAlbedoSpec);
		// send light relevant uniforms
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			shaderLightingPass->SetVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			shaderLightingPass->SetVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
			// update attenuation parameters and calculate radius
			const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
			const float linear = 0.7f;
			const float quadratic = 1.8f;
			shaderLightingPass->SetFloat("lights[" + std::to_string(i) + "].Linear", linear);
			shaderLightingPass->SetFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
		}

		shaderLightingPass->Use();
		shaderLightingPass->SetVec3("viewPos", IRenderer::camera.Position);

		glDisable(GL_DEPTH_TEST);
		oglRenderer->RenderQuad();
		glEnable(GL_DEPTH_TEST);

		// 3.1. render lights as bright cubes
		// --------------------------------
		shaderLightBox->Use();
		shaderLightBox->SetMat4("projection", projection);
		shaderLightBox->SetMat4("view", view);
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPositions[i]);
			model = glm::scale(model, glm::vec3(0.125f));
			shaderLightBox->SetMat4("model", model);
			shaderLightBox->SetVec3("lightColor", lightColors[i]);
			oglRenderer->RenderCube();
		}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 3. Blur bright fragments w/ two-pass Gaussian Blur
	bool horizontal = true, first_iteration = true;
	unsigned int amount = 10;
	oglRenderer->m_shaderBlur->Use();
	for (int i = 0; i < amount; i++)
	{
	glBindFramebuffer(GL_FRAMEBUFFER, oglRenderer->m_pingpongFBO[horizontal]);
		oglRenderer->m_shaderBlur->SetInt("horizontal", horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? oglRenderer->m_colorBuffers[1] : oglRenderer->m_pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		oglRenderer->RenderQuad();
		glBindTexture(GL_TEXTURE_2D, 0);
		horizontal = !horizontal;
		if (first_iteration)
		{
			first_iteration = false;
		}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

//Final render pass
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 3. Now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
	oglRenderer->m_shaderBloomFinal->Use();
	oglRenderer->m_shaderBloomFinal->SetInt("scene", 0);
	oglRenderer->m_shaderBloomFinal->SetInt("bloomBlur", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, oglRenderer->m_colorBuffers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, oglRenderer->m_pingpongColorbuffers[!horizontal]);
	oglRenderer->RenderQuad();
	glBindTexture(GL_TEXTURE_2D, 0);

	//3.2 Render sky box

	//Draw all the framebuffers
	oglRenderer->Render();
//End final render pass
}

int main(void)
{
	new EventManager(true);
	new GLFWindow(1920, 1080, "Basic Game Engine");
	oglRenderer = new OGLRenderer(IWindow::Get()->m_width, IWindow::Get()->m_height);

	// load shaders
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
	shaderLightingPass = new Shader(
			"resources/Shaders/deferredLighting.vert",
			"resources/Shaders/deferredLighting.frag"
	);
	shaderLightBox = new Shader(
			"resources/Shaders/lightBox.vert",
			"resources/Shaders/lightBox.frag"
	);

	// load models
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

	// lighting info
	const unsigned int NR_LIGHTS = 32;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5 * 2.0f; // between 1.0 and 2.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5 * 2.0f; // between 1.0 and 2.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5 * 2.0f; // between 1.0 and 2.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	//Shader configuration
	shaderLightingPass->Use();
	shaderLightingPass->SetInt("gPosition", 0);
	shaderLightingPass->SetInt("gNormal", 1);
	shaderLightingPass->SetInt("gAlbedoSpec", 2);

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
		RenderGbuffer();

		//IRenderer::Get()->Render();
		IWindow::Get()->SwapBuffers();
	}

	IWindow::Get()->DestroyWindow();
	exit(EXIT_SUCCESS);
}