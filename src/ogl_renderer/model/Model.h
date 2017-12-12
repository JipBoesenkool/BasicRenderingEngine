#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include "Mesh.h"
#include "Material.h"
#include "Transform.h"

#include "../../../include/renderer/IModelLoader.h"
#include "../modelLoaders/ObjLoader.h"

class Model
{
//Members
public:
	/*  Model Data */
	std::vector<Texture> m_loadedTextures;
	std::vector<Mesh> m_meshes;
	Shader* m_shader;
	std::string m_filePath;

//Functions
public:
	Model();
	Model(const char* filepath, Shader* shader);
	Model(const char* filepath, Shader* shader, IModelLoader* loader);
	~Model();

	virtual void Update( glm::mat4 m );
	virtual void Draw( glm::mat4 m, Shader* shader = nullptr );

private:
	//void SetupMesh();
};

#endif