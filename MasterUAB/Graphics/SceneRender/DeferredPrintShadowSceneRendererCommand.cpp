#include "DeferredPrintShadowSceneRendererCommand.h"
#include "RenderManager\RenderManager.h"
#include "ContextManager\ContextManager.h"
#include "RenderableObjects\RenderableObjectTechniqueManager.h"
#include "Engine\UABEngine.h"
#include "Lights\LightManager.h"
#include "Effects\EffectManager.h"
#include "Texture\DynamicTexture.h"
#include "Materials\MaterialManager.h"
#include "RenderableObjects\RenderableObjectTechnique.h"

#include <d3d11.h>

CDeferredPrintShadowSceneRendererCommand::CDeferredPrintShadowSceneRendererCommand(CXMLTreeNode &TreeNode) :CStagedTexturedSceneRendererCommand(TreeNode)
{
	m_RenderableObjectTechnique = UABEngine.GetRenderableObjectTechniqueManager()->GetResource("MV_POSITION4_NORMAL_TEXTURE_VERTEX");
}


CDeferredPrintShadowSceneRendererCommand::~CDeferredPrintShadowSceneRendererCommand()
{
}

void CDeferredPrintShadowSceneRendererCommand::Execute(CRenderManager &_RenderManager)
{
	m_EnabledAlphaBlendState = _RenderManager.GetContextManager()->GetBlendState(CContextManager::BLEND_DEFERRED);
	ID3D11RenderTargetView* l_ID3D11_RenderTargetView;
 	if (m_EnabledAlphaBlendState == nullptr)
		return;
	
	_RenderManager.GetDeviceContext()->OMSetBlendState(m_EnabledAlphaBlendState, NULL, 0xffffffff);

	for (size_t i = 0; i < m_StagedTextures.size(); ++i)
		m_StagedTextures[i].Activate();

	for (size_t j = 0; j < UABEngine.GetLightManager()->GetResourcesVector().size(); ++j)
	{
		CLight *l_Light = UABEngine.GetLightManager()->GetResourceById(j);
		if (!l_Light->GetEnabled() || !l_Light->GetGenerateShadowMap()) {
			continue;
		}
		UABEngine.GetEffectManager()->SetLightConstants(0, l_Light);
		//l_VectorShadowMaps.push_back(l_Light->GetShadowMapPrint()->GetRenderTargetView());
		
		l_ID3D11_RenderTargetView = l_Light->GetShadowMapPrint()->GetRenderTargetView();

		CEffectTechnique* l_EffectTechnique = m_RenderableObjectTechnique->GetEffectTechnique();
		ID3D11Buffer *l_LightConstantBufferVS = l_EffectTechnique->GetVertexShader()->GetConstantBuffer(LIGHT_CONSTANT_BUFFER_ID);
		ID3D11Buffer *l_LightConstantBufferPS = l_EffectTechnique->GetPixelShader()->GetConstantBuffer(LIGHT_CONSTANT_BUFFER_ID);


		_RenderManager.GetDeviceContext()->UpdateSubresource(l_LightConstantBufferVS, 0, NULL, &(CEffectManager::m_LightParameters), 0, 0);
		_RenderManager.GetDeviceContext()->UpdateSubresource(l_LightConstantBufferPS, 0, NULL, &(CEffectManager::m_LightParameters), 0, 0);

		_RenderManager.GetContextManager()->SetRenderTargets(1, &l_ID3D11_RenderTargetView, nullptr);
		_RenderManager.Clear(true, true);
		_RenderManager.DrawScreenQuad(l_EffectTechnique, NULL, 0, 0, 1, 1, CColor(1.f, 1.f, 1.f, 1.f));
		_RenderManager.GetContextManager()->UnsetRenderTargets();


		CContextManager *l_ContextManager = _RenderManager.GetContextManager();
		ID3D11DepthStencilView *l_DepthStencilView = m_DynamicTextures.empty() ? NULL : (l_Light->GetShadowMapBlur()->GetDepthStencilView());

		D3D11_VIEWPORT *l_CurrentViewport = _RenderManager.GetContextManager()->getViewPort();

		D3D11_VIEWPORT l_Viewport;
		l_Viewport.TopLeftX = 0.0f;
		l_Viewport.TopLeftY = 0.0f;
		l_Viewport.MinDepth = 0.0f;
		l_Viewport.MaxDepth = 1.0f;
		l_Viewport.Width = (FLOAT)l_Light->GetShadowMapBlur()->GetWidth();
		l_Viewport.Height = (FLOAT)l_Light->GetShadowMapBlur()->GetHeight();
		_RenderManager.GetContextManager()->setViewPort(&l_Viewport);
		UABEngine.GetEffectManager()->SetLightConstants(0, l_Light);
		
		l_ID3D11_RenderTargetView = l_Light->GetShadowMapBlur()->GetRenderTargetView();
		UABEngine.GetMaterialManager()->GetResource("BlurShadowMaterial")->Apply();
		l_ContextManager->SetRenderTargets(1, &l_ID3D11_RenderTargetView, nullptr);
		_RenderManager.Clear(true, true);
		_RenderManager.DrawScreenQuad(UABEngine.GetEffectManager()->GetResource("blur_technique"),
		l_Light->GetShadowMapPrint(), 0, 0, 1, 1, CColor(1.f, 1.f, 1.f, 1.f));
		_RenderManager.GetContextManager()->UnsetRenderTargets();
		
		_RenderManager.GetContextManager()->setViewPort(l_CurrentViewport);

	}
	_RenderManager.GetContextManager()->GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xffffffff);
}	
