#include "Model.h"

//Public Functions
Model::Model()
{
	//Empty constructor
}

Model::Model(const char *filepath, Shader* shader, bool gamma)
{
	m_filePath = filepath;
	m_shader = shader;
	//TODO: choice out of parsers
	m_gammaCorrection = gamma;

	ObjLoader loader;
	loader.LoadModel(filepath, this);

	//NOTE: this went wrong!
	for(unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].Normalize();
		m_meshes[i].SetupMesh();
	}
}

Model::Model(const char *filepath, Shader* shader, IModelLoader* loader, bool gamma)
{
	m_filePath = filepath;
	m_shader = shader;
	m_gammaCorrection = gamma;

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
	/*
	m_shader->Use();
	m_shader->SetMat4("model", m);
	m_material.SetUniform(m_shader->ID);

	// Now Draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(m_VAO);
	// Tell OpenGL to Draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, m_mesh.m_indices.size(), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally Draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
	 */
}

//Private Functions
/*
void Model::SetupMesh()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_mesh.m_vertices.size() * sizeof(Vertex), m_mesh.m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh.m_indices.size() * sizeof(GLuint), m_mesh.m_indices.data(), GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)0);

	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)offsetof(Vertex, m_normal));

	// Vertex texCoords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)offsetof(Vertex, m_texCoords));

	glBindVertexArray(0);
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}
 */