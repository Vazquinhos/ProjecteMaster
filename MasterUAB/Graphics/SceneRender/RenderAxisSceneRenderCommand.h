#ifndef RENDER_AXIS_SCENE_RENDERER_COMMAND_H
#define RENDER_AXIS_SCENE_RENDERER_COMMAND_H

#include "SceneRendererCommand.h"
class CRenderAxisSceneRenderCommand :
	public CSceneRendererCommand
{
public:
	CRenderAxisSceneRenderCommand(CXMLTreeNode &TreeNode);
	virtual ~CRenderAxisSceneRenderCommand();

	void Execute(CRenderManager &RenderManager);
};

#endif //RENDER_AXIS_SCENE_RENDERER_COMMAND_H