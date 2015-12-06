#include "Engine\UABEngine.h"
#include <string>

#define LEVEL "1"

CUABEngine::CUABEngine(void)
{
	m_EffectManager = new CEffectManager();
	m_MaterialManager = new CMaterialManager();
	m_TextureManager = new CTextureManager();
	m_RenderManager = new CRenderManager();
	m_StaticMeshManager = new CStaticMeshManager();
	m_RenderableObjectsManager = new CRenderableObjectsManager();
	m_AnimatedModelsManager = new CAnimatedModelsManager();
}


CUABEngine::~CUABEngine(void)
{
}

CUABEngine* CUABEngine::m_Instance = nullptr;

CUABEngine* CUABEngine::GetInstance()
{
	if(m_Instance == nullptr)
	{
		m_Instance = new  CUABEngine();
	}
	return m_Instance;
}

void CUABEngine::Init()
{
	m_EffectManager->Load("Data\\effects.xml");
	m_MaterialManager->Load("Data\\level_1\\materials.xml");
	m_StaticMeshManager->Load("Data\\level_1\\static_meshes.xml");
	m_AnimatedModelsManager->Load("Data\\level_1\\animated_models.xml");
	m_RenderableObjectsManager->Load("Data\\level_1\\renderable_objects.xml");
}