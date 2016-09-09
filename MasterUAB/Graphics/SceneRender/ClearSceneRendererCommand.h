#ifndef CLEAR_SCENE_RENDERER_COMMAND_H
#define CLEAR_SCENE_RENDERER_COMMAND_H

#include "SceneRendererCommand.h"

class CRenderManager;

class CClearSceneRendererCommand : public CSceneRendererCommand
{
private:
	bool m_RenderTarget;
	bool m_DepthStencil;
public:
	CClearSceneRendererCommand(tinyxml2::XMLElement* TreeNode);
	virtual ~CClearSceneRendererCommand();
	virtual void Execute(CRenderManager &RenderManager);
};

#endif //CLEAR_SCENE_RENDERER_COMMAND_H