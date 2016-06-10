#ifndef DEFFERRED_PRINT_SCENE_RENDER_H
#define DEFFERRED_PRINT_SCENE_RENDER_H

#include "StagedTexturedSceneRendererCommand.h"

class CRenderableObjectTechnique;
class ID3D11BlendState;

class CDeferredPrintShadowSceneRendererCommand : public
	CStagedTexturedSceneRendererCommand
{
private:
	CRenderableObjectTechnique *m_RenderableObjectTechnique;
	ID3D11BlendState *m_EnabledAlphaBlendState;
public:
	CDeferredPrintShadowSceneRendererCommand(CXMLTreeNode &TreeNode);
	virtual ~CDeferredPrintShadowSceneRendererCommand();
	void Execute(CRenderManager &RenderManager);
};

#endif //DEFFERRED_PRINT_SCENE_RENDER_H