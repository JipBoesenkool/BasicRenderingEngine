//
// Created by Jip Boesenkool on 10/12/2017.
//

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "../../../include/renderer/IModelLoader.h"


class ObjLoader : public IModelLoader
{
//---------------------------------------------------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	ObjLoader()
	{
		//Empty constructor
	};
//---------------------------------------------------------------------------------------------------------------------
// Interface Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	void LoadModel(const char* filepath, Model* model) override;
};


#endif //OBJLOADER_H
