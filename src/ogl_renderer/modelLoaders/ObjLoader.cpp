//
// Created by Jip Boesenkool on 10/12/2017.
//
#include "ObjLoader.h"
#include "../model/Model.h"

void ObjLoader::LoadModel(const char* filepath, Model* model)
{
	Mesh mesh;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	float vx, vy, vz;  // vertex coordinates
	float vnx, vny, vnz;  // vertex normals
	unsigned int vindex[3], nindex[3];
	float r,g,b;  // vertex color
	std::string firstToken;
	std::string vnPair;

	//Parse .obj
	std::ifstream file(filepath);
	std::string line;
	while ( std::getline(file, line) )
	{
		std::istringstream iss(line);
		iss >> firstToken;

		if (firstToken == "v")
		{
			iss >> vx >> vy >> vz;// >> vertX >> vertY >> vertZ;
			vertices.push_back( glm::vec3(vx,vy,vz) );
			//TODO: store colors
		}
		else if (firstToken == "vn")
		{
			iss >> vnx >> vny >> vnz;// normX>>normY>>normZ
			normals.push_back( glm::vec3(vnx,vny,vnz) );
		}
		else if(firstToken == "f")
		{
			//done parsing vectors, collect them into mesh
			if(mesh.m_vertices.size() == 0)
			{
				mesh.m_vertices.resize( vertices.size() );
				for( int i = 0; i < vertices.size(); ++i ) {
					mesh.m_vertices[i] = Vertex{vertices[i], normals[i], glm::vec2(0)};
				}
			}
			for( int i = 0; i < 3; ++i ) {
				iss >> vnPair;
				int delimPos = vnPair.find("//");
				int spacing = 2;
				if(delimPos < 0)
				{
					delimPos = vnPair.find("/");
					spacing = 1;
				}
				vindex[i] = std::stoi( vnPair.substr(0, delimPos) );
				nindex[i] = std::stoi( vnPair.substr(delimPos + spacing, vnPair.size()) );
				mesh.m_indices.push_back( vindex[i]-1 );
			}
		}
	}

	mesh.SetupMesh();
	mesh.Normalize();
	model->m_meshes.push_back(mesh);
}