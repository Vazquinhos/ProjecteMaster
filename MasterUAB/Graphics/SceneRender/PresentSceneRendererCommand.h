#ifndef PRESENT_SCENE_RENDERER_COMMAND_H
#define PRESENT_SCENE_RENDERER_COMMAND_H

#include "SceneRendererCommand.h"

class CRenderManager;

class CPresentSceneRendererCommand : public CSceneRendererCommand
{
public:
	CPresentSceneRendererCommand(CXMLTreeNode &TreeNode);
	virtual ~CPresentSceneRendererCommand();
	void Execute(CRenderManager &RenderManager);
};

#endif //PRESENT_SCENE_RENDERER_COMMAND_H