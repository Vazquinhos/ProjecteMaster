#include "SetRenderTargetSceneRendererCommand.h"

#include "RenderManager\RenderManager.h"
#include "Texture\DynamicTexture.h" 

CSetRenderTargetSceneRendererCommand::CSetRenderTargetSceneRendererCommand(CXMLTreeNode &TreeNode) :CStagedTexturedSceneRendererCommand(TreeNode)
{

}


CSetRenderTargetSceneRendererCommand::~CSetRenderTargetSceneRendererCommand()
{
}


void CSetRenderTargetSceneRendererCommand::Execute(CRenderManager &_RenderManager)
{
	ID3D11DepthStencilView *l_DepthStencilView = m_DynamicTextures.empty() ? NULL : 
		(m_DynamicTextures[0]->GetDepthStencilView());
	_RenderManager.SetRenderTargets((UINT)m_DynamicTextures.size(),&m_RenderTargetViews[0],l_DepthStencilView==NULL?
		_RenderManager.GetCurrentDepthStencilView():l_DepthStencilView);
}