//
// Created by Jip Boesenkool on 09/12/2017.
//
#include "IRenderer.h"
//---------------------------------------------------------------------------------------------------------------------
// Global
//---------------------------------------------------------------------------------------------------------------------
static IRenderer* g_pRenderer = nullptr;

//---------------------------------------------------------------------------------------------------------------------
// Getters & Setters
//---------------------------------------------------------------------------------------------------------------------
IRenderer* IRenderer::Get(void)
{
	return g_pRenderer;
}

//---------------------------------------------------------------------------------------------------------------------
// Public Functions
//---------------------------------------------------------------------------------------------------------------------
IRenderer::IRenderer(void)
{
	if (g_pRenderer)
	{
		delete g_pRenderer;
	}
	g_pRenderer = this;
}

IRenderer::~IRenderer(void)
{
	if (g_pRenderer == this)
	{
		g_pRenderer = nullptr;
	}
}
