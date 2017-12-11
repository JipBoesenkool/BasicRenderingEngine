//
// Created by Jip Boesenkool on 10/12/2017.
//
#ifndef ASSIMPLOADER_H
#define ASSIMPLOADER_H

#include "../../../include/renderer/IModelLoader.h"
#include "../model/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <SFML/Graphics/Image.hpp>

class AssimpLoader : public IModelLoader
{
//---------------------------------------------------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	AssimpLoader()
	{
		//Empty constructor
	};
//---------------------------------------------------------------------------------------------------------------------
// Interface Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	void LoadModel(const char* filepath, Model* model) override;
private:
	void ProcessNode(aiNode *node, const aiScene *scene, Model* model);
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene, Model* model);
	std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, Model* model);
	unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
};


#endif //ASSIMPLOADER_H
