//
// Created by Jip Boesenkool on 10/12/2017.
//
#ifndef IMODELLOADER_H
#define IMODELLOADER_H

//TODO: this should not be here
class Model;
class IModelLoader
{
//---------------------------------------------------------------------------------------------------------------------
// Interface Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	virtual void LoadModel(const char* filepath, Model* model) = 0;
};

#endif //IMODELLOADER_H
