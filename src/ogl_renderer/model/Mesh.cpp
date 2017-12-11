//
// Created by Jip Boesenkool on 26/10/2017.
//
#include "Mesh.h"

//Public Functions
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	m_vertices = vertices;
	m_indices = indices;
	m_textures = textures;

	SetupMesh();
	//Normalize();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

void Mesh::Draw( Shader *shader )
{
	shader->Use();

	BindTextures(shader);

	// draw mesh
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
	// vertex texCoords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_texCoords));

	glBindVertexArray(0);
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void Mesh::Normalize()
{
	glm::vec3 min = glm::vec3(99.0f);
	glm::vec3 max = glm::vec3(-99.0f);
	glm::vec3 total = glm::vec3(0.0f);
	for( int i = 0; i < m_vertices.size(); ++i )
	{
		//min
		if(m_vertices[i].m_position.x < min.x) { min.x = m_vertices[i].m_position.x; }
		if(m_vertices[i].m_position.y < min.y) { min.y = m_vertices[i].m_position.y; }
		if(m_vertices[i].m_position.z < min.z) { min.z = m_vertices[i].m_position.z; }
		//max
		if(m_vertices[i].m_position.x > max.x) { max.x = m_vertices[i].m_position.x; }
		if(m_vertices[i].m_position.y > max.y) { max.y = m_vertices[i].m_position.y; }
		if(m_vertices[i].m_position.z > max.z) { max.z = m_vertices[i].m_position.z; }
		//to get midpoint
		total += m_vertices[i].m_position;
	}
	glm::vec3 div = glm::vec3( (float)m_vertices.size() );
	total = total/div;

	//resize
	glm::vec3 boundingBox = max - min;
	//get biggest size
	float maxBound = boundingBox.x;
	if(boundingBox.y > maxBound){ maxBound = boundingBox.y; };
	if(boundingBox.z > maxBound){ maxBound = boundingBox.z; };
	float boundSize = 2.0f;
	float newscale = boundSize/maxBound;

	glm::vec3 newposition = glm::vec3(0.0f) - total;
	for( int i = 0; i < m_vertices.size(); ++i ) {
		m_vertices[i].m_position += newposition;
		m_vertices[i].m_position *= newscale;
	}
}

//Private functions
void Mesh::BindTextures( Shader *shader )
{
	if( m_textures.empty() )
	{
		return;
	}

	shader->Use();
	unsigned int diffuseNr  = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr   = 1;
	unsigned int heightNr   = 1;
	for(unsigned int i = 0; i < m_textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = m_textures[i].type;
		if(name == "texture_diffuse")
		{
			number = std::to_string(diffuseNr++);
		}
		else if(name == "texture_specular")
		{
			number = std::to_string(specularNr++);
		}
		else if(name == "texture_normal")
		{
			number = std::to_string(normalNr++);
		}
		else if(name == "texture_height")
		{
			number = std::to_string(heightNr++);
		}

		shader->SetInt((name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
	}
}
