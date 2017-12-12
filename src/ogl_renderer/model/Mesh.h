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
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<Texture>m_textures;
	//Optional
	std::vector<Tangex> m_tangents;
private:
	/*  Render data  */
	unsigned int m_VAO, m_VBO, m_EBO;
//Functions
public:
	Mesh(){};
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, std::vector<Tangex> tangents);
	~Mesh();

	void Draw( Shader* shader );

	void SetupMesh();
	void Normalize();

private:
	void BindTextures(Shader* shader);
};


#endif //MESH_H
