#include "Model.h"

//Public Functions
Model::Model()
{
	//Empty constructor
}

Model::Model(const char *filepath, Shader* shader)
{
	m_filePath = filepath;
	m_shader = shader;

	ObjLoader loader;
	loader.LoadModel(filepath, this);

	//NOTE: this went wrong!
	for(unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].Normalize();
		m_meshes[i].SetupMesh();
	}
}

Model::Model(const char *filepath, Shader* shader, IModelLoader* loader)
{
	m_filePath = filepath;
	m_shader = shader;

	loader->LoadModel(filepath, this);

	//NOTE: this went wrong!
	for(unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].SetupMesh();
	}
}

Model::~Model()
{

}

void Model::Update( glm::mat4 m )
{

	for(unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].Update( m );
	}
}

void Model::Draw( glm::mat4 m, Shader* shader )
{
	if(shader == nullptr)
	{
		shader = m_shader;
	}

	shader->Use();
	shader->SetMat4("model", m);

	for(unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].Draw(shader);
	}
}