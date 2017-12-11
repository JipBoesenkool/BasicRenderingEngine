//
// Created by Jip Boesenkool on 10/12/2017.
//

#include "AssimpLoader.h"
#include "../model/Model.h"

//Public functions
void AssimpLoader::LoadModel(const char* filepath, Model* model)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	ProcessNode(scene->mRootNode, scene, model);
}

//Private functions
void AssimpLoader::ProcessNode(aiNode *node, const aiScene *scene, Model* model)
{
	// process all the node's meshes (if any)
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		model->m_meshes.push_back( ProcessMesh(mesh, scene, model) );
	}
	// then do the same for each of its children
	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, model);
	}
}

Mesh AssimpLoader::ProcessMesh(aiMesh *mesh, const aiScene *scene, Model* model)
{
	// data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// Walk through each of the mesh's vertices
	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.m_position = vector;
		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.m_normal = vector;
		// texture coordinates
		if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.m_texCoords = vec;
		}
		else
		{
			vertex.m_texCoords = glm::vec2(0.0f, 0.0f);
		}
		/*
		// tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.Tangent = vector;
		// bitangent
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		vertex.Bitangent = vector;
		 */
		vertices.push_back(vertex);
	}

	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for(unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for(unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", model);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", model);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", model);
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", model);
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Texture> AssimpLoader::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, Model* model)
{
	std::vector<Texture> textures;
	for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for(unsigned int j = 0; j < model->m_loadedTextures.size(); j++)
		{
			if( std::strcmp(model->m_loadedTextures[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(model->m_loadedTextures[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if(!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			std::string dir = model->m_filePath.substr(0, model->m_filePath.find_last_of('/'));
			texture.id = TextureFromFile(str.C_Str(), dir);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			// store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			model->m_loadedTextures.push_back(texture);
		}
	}
	return textures;
}

//TODO: move to renderer
unsigned int AssimpLoader::TextureFromFile(const char *fileName, const std::string &directory, bool gamma)
{
	std::string file(directory + "/" + fileName);

	unsigned int textureID;
	glGenTextures(1, &textureID);

	sf::Image img;
	if ( img.loadFromFile( file ) )
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				img.getSize().x, img.getSize().y,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				img.getPixelsPtr()
		);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << file << std::endl;
	}

	return textureID;
}