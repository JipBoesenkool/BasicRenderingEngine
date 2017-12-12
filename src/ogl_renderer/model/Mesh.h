//
// Created by Jip Boesenkool on 26/10/2017.
//
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Vertex.h"
#include "Texture.h"
#include "../Shader.h"

class Mesh
{
//Members
public:
	glm::mat4 m_origin;
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<Texture>m_textures;
	//Optional
	glm::vec3 m_color;
	std::vector<glm::vec3> m_tangents;
	std::vector<PassShaderPair> m_shadersEffects;

private:
	/*  Render data  */
	unsigned int m_VAO, m_VBO, m_EBO;
	//Optional
	unsigned int m_TangVBO;
//Functions
public:
	Mesh(){};
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, glm::vec3 color);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, std::vector<glm::vec3> tangents);
	~Mesh();

	void Update( glm::mat4 m );
	void Draw( Shader* shader );

	void SetupMesh();
	void Normalize();

private:
	void BindTextures(Shader* shader);
};


#endif //MESH_H
