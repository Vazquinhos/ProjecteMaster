#include "RenderManager\RenderManager.h"

#include "Materials\MaterialManager.h"
#include "RenderableObjects\RenderableObject.h"

#include "Engine\UABEngine.h"
#include "DebugHelper.h"

#include "Effects\EffectTechnique.h"
#include "Texture\Texture.h";

#include "DebugRender.h"

#define SCREEN_DEPTH 20.f

CRenderManager::CRenderManager()
	: m_UseDebugCamera(false)
	, m_CurrentRenderlistLength(0),
	m_DebugRender(nullptr)
	/*m_RenderTargetView(nullptr),
	m_DepthStencilView(nullptr)	*/
{
}

CRenderManager::~CRenderManager()
{

}

void CRenderManager::Init()
{
	m_DebugRender = new CDebugRender(m_ContextManager->GetDevice());
}

void CRenderManager::SetCurrentCamera(const CCamera& _CurrentCamera)
{
	m_CurrentCamera = _CurrentCamera;
	Mat44f Proj = m_CurrentCamera.GetProjection();
	// Calculate the minimum Z distance in the frustum.
	float zMinimum = -Proj.m32 / Proj.m22;
	float r = SCREEN_DEPTH / (SCREEN_DEPTH - zMinimum);
	Proj.m22 = r;
	Proj.m32 = -r * zMinimum;
	Mat44f viewProj = m_CurrentCamera.GetView() * Proj;
	m_CullFrustum.Update(viewProj);
}

//bool CRenderManager::AddRenderableObjectToRenderList(const CRenderableObject* _RenderableObject)
//{
//	// Alguien se atrebe a arreglar el frustum?
//
//	//if (m_CullFrustum.SphereVisible(_RenderableObject->GetTransform().Position, _RenderableObject->GetBoundingRadius()))
//	if (m_CullFrustum.BoxVisible(_RenderableObject->GetBoundingMax()+_RenderableObject->GetTransform().Position, _RenderableObject->GetBoundingMin()+_RenderableObject->GetTransform().Position))
//	{
//		if (m_CurrentRenderlistLength == m_RenderableObjects.size())
//		{
//			m_RenderableObjects.push_back(_RenderableObject);
//		}
//		else
//		{
//			m_RenderableObjects[m_CurrentRenderlistLength]=_RenderableObject;
//		}
//		++m_CurrentRenderlistLength;
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
struct BlendedSubmesh
{
	const CRenderableVertexs* vertices;
	Mat44f world;
	const CMaterial* material;
	Vect3f position;
};

void CRenderManager::Render()
{
	m_ContextManager->BeginRender();

	/*if (m_UseDebugCamera)
	{
		m_ContextManager->SetCamera(m_DebugCamera);
		UABEngine.GetEffectManager()->m_SceneParameters.m_CameraPosition=m_DebugCamera.GetPosition();
		UABEngine.GetEffectManager()->m_SceneParameters.m_CameraUpVector=m_DebugCamera.GetUp();
		UABEngine.GetEffectManager()->m_SceneParameters.m_CameraRightVector=Vect4f(1,1,1,1);
	}
	else
	{
		m_ContextManager->SetCamera(m_CurrentCamera);
		UABEngine.GetEffectManager()->m_SceneParameters.m_CameraPosition=m_CurrentCamera.GetPosition();
		UABEngine.GetEffectManager()->m_SceneParameters.m_CameraUpVector=m_CurrentCamera.GetUp();
		UABEngine.GetEffectManager()->m_SceneParameters.m_CameraRightVector=Vect4f(1,1,1,1);
	}
//	UABEngine.GetRenderableObjectTechniqueManager()->GetPoolRenderableObjectTechniques().GetResource("forward_shading")->Apply();

	UABEngine.GetEffectManager()->SetLightsConstants(MAX_LIGHTS_BY_SHADER);

	std::vector<BlendedSubmesh> l_SubmeshesWithBlend;

	UABEngine.GetLayerManager()->Render(this);

	m_CurrentRenderlistLength = 0;
	CDebugHelper::GetDebugHelper()->Render();*/
		
	UABEngine.GetSceneRendererCommandManager()->Execute(this);
	m_ContextManager->EndRender();
}

void CRenderManager::EngableAlphaBlendState()
{
	ID3D11BlendState* l_AlphaBlendState = m_ContextManager->GetBlendState(CContextManager::BLEND_ALPHA);
	m_ContextManager->GetDeviceContext()->OMSetBlendState(l_AlphaBlendState,NULL,0xffffffff);
}
void CRenderManager::DisableAlphaBlendState()
{
	m_ContextManager->GetDeviceContext()->OMSetBlendState(NULL,NULL,0xffffffff);
}

void CRenderManager::Clear(bool renderTarget, bool depthStencil)
{
	m_ContextManager->Clear(renderTarget, depthStencil);
}

void CRenderManager::Present()
{
	m_ContextManager->Present();
}

void CRenderManager::SetMatrixViewProjection()
{
	if (m_UseDebugCamera)
	{
		m_ContextManager->SetCamera(m_DebugCamera);
		UABEngine.GetEffectManager()->m_SceneParameters.m_CameraPosition = m_DebugCamera.GetPosition();
		UABEngine.GetEffectManager()->m_SceneParameters.m_CameraUpVector = m_DebugCamera.GetUp();
		UABEngine.GetEffectManager()->m_SceneParameters.m_CameraRightVector = Vect4f(1, 1, 1, 1);
	}
	else
	{
		m_ContextManager->SetCamera(m_CurrentCamera);
		UABEngine.GetEffectManager()->m_SceneParameters.m_InverseView = UABEngine.GetEffectManager()->m_SceneParameters.m_View.Invert();
		UABEngine.GetEffectManager()->m_SceneParameters.m_InverseProjection = UABEngine.GetEffectManager()->m_SceneParameters.m_Projection.Invert();
		UABEngine.GetEffectManager()->m_SceneParameters.m_CameraPosition = m_CurrentCamera.GetPosition();
		UABEngine.GetEffectManager()->m_SceneParameters.m_CameraUpVector = m_CurrentCamera.GetUp();
		UABEngine.GetEffectManager()->m_SceneParameters.m_CameraRightVector = Vect4f(1, 1, 1, 1);
	}
}

void CRenderManager::SetAntTweakBar()
{
	CDebugHelper::GetDebugHelper()->Render();
}

//Funcones Draw
void CRenderManager::DrawScreenQuad(CEffectTechnique *_EffectTechnique, CTexture *_Texture, float x, float y, float _Width, float _Height, const CColor &Color)
{
	CEffectManager::m_SceneParameters.m_BaseColor=Color;
	if(_Texture!=NULL)
		_Texture->Activate(0);
	D3D11_VIEWPORT l_Viewport;
	l_Viewport.Width = _Width*m_ContextManager->getViewPort()->Width;
	l_Viewport.Height = _Height*(m_ContextManager->getViewPort()->Height);
	l_Viewport.MinDepth = 0.0f;
	l_Viewport.MaxDepth = 1.0f;
	l_Viewport.TopLeftX = x*m_ContextManager->getViewPort()->Width;
	l_Viewport.TopLeftY = y*m_ContextManager->getViewPort()->Height;
	m_ContextManager->GetDeviceContext()->RSSetViewports(1, &l_Viewport);
	m_DebugRender->GetQuadRV()->Render(this, _EffectTechnique,	&CEffectManager::m_SceneParameters);
	m_ContextManager->GetDeviceContext()->RSSetViewports(1, m_ContextManager->getViewPort());
}


/*void CRenderManager::SetRenderTargets(int _NumViews, ID3D11RenderTargetView *const *_RenderTargetViews,
	ID3D11DepthStencilView *_DepthStencilView)
{
	m_ContextManager->SetRenderTargets(_NumViews, _RenderTargetViews, _DepthStencilView);
}

void CRenderManager::UnsetRenderTargets()
{
	GetContextManager()->Unset();
	m_ContextManager->GetDeviceContext()->RSSetViewports(1, m_ContextManager->getViewPort());
}
*/
