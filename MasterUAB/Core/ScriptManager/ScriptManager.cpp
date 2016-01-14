#include "ScriptManager.h"
#include <assert.h>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/operator.hpp>

#include "AnimatedModels\AnimatedCoreModel.h"
#include "AnimatedModels\AnimatedInstanceModel.h"
#include "AnimatedModels\AnimatedModelsManager.h"

#include "Camera\Camera.h"
#include "Camera\CameraController.h"
#include "Camera\CameraControllerManager.h"
#include "Camera\CameraInfo.h"
#include "Camera\CameraKey.h"
#include "Camera\CameraKeyController.h"
#include "Camera\FPSCameraController.h"
#include "Camera\Frustum.h"
#include "Camera\SphericalCameraController.h"

#include "Cinematics\Cinematic.h"
#include "Cinematics\CinematicObject.h"
#include "Cinematics\CinematicObjectKeyFrame.h"
#include "Cinematics\CinematicPlayer.h"

#include "ContextManager\ContextManager.h"

#include "Effects\AnimatedModelEffectParameters.h"
#include "Effects\Effect.h"
#include "Effects\EffectManager.h"
#include "Effects\EffectParameters.h"
#include "Effects\EffectShader.h"
#include "Effects\EffectTechnique.h"
#include "Effects\LightEffectParameters.h"
#include "Effects\SceneEffectParameters.h"

#include "Lights\DirectionalLight.h"
#include "Lights\Light.h"
#include "Lights\LightManager.h"
#include "Lights\OmniLight.h"
#include "Lights\SpotLight.h"

#include "Materials\Material.h"
#include "Materials\MaterialManager.h"

#include "RenderableObjects\RenderableObject.h"
#include "RenderableObjects\RenderableObjectManager.h"
#include "RenderableObjects\RenderableVertexs.h"
#include "RenderableObjects\TemplatedRenderableIndexedVertexs.h"
#include "RenderableObjects\TemplatedRenderableVertexs.h"
#include "RenderableObjects\VertexTypes.h"

#include "RenderManager\RenderManager.h"

#include "StaticMesh\InstanceMesh.h"
#include "StaticMesh\StaticMesh.h"
#include "StaticMesh\StaticMeshManager.h"

#include "Texture\Texture.h"
#include "Texture\TextureManager.h"

#include "DebugRender.h"

#include "XML\XMLTreeNode.h"

using namespace luabind;

CScriptManager g_ScriptManager;

#define LUA_STATE CUABEngine::GetInstance().GetScriptManager()->GetLuaState()
#define REGISTER_LUA_FUNCTION(FunctionName,AddrFunction) {luabind::module(LUA_STATE) [ luabind::def(FunctionName,AddrFunction) ];}

CScriptManager::CScriptManager()
: m_LS(NULL)
{
}

CScriptManager::~CScriptManager()
{
	Destroy();
}

//C�digo de la funci�n Alert que se llamar� al generarse alg�n error de LUA
int Alert(lua_State * State)
{
	std::string l_Text;
	int n = lua_gettop(State);
	int i;
	lua_getglobal(State, "tostring");
	for (i=1; i<=n; i++) {
		const char *s;
		lua_pushvalue(State, -1);
		lua_pushvalue(State, i);
		lua_call(State, 1, 1);
		s = lua_tostring(State, -1);
		if (s == NULL)
			return luaL_error(State, "`tostring' must return a string to `print'");
		if (i>1) l_Text += '\t';
		l_Text += s;
		lua_pop(State, 1);
	}
	l_Text += '\n';
	//Info( l_Text.c_str() );
	assert(!"must be log");
	return true;
}

//Para inicializar el motor de LUA
void CScriptManager::Initialize()
{
	m_LS=lua_open();
	luaopen_base(m_LS);
	luaopen_string(m_LS);
	luaopen_table(m_LS);
	luaopen_math(m_LS);
	luaopen_debug(m_LS);
	//Sobreescribimos la funci�n _ALERT de LUA cuando se genere alg�n error al ejecutar c�digo LUA 
	lua_register(m_LS,"_ALERT",Alert);

	luabind::open(m_LS);

	RegisterLUAFunctions();
}

//Para desinicializar el motor de LUA
void CScriptManager::Destroy()
{
	if(m_LS)
	{
		lua_close(m_LS);
	}
}

//Para ejecutar un fragmento de c�digo LUA
void CScriptManager::RunCode(const std::string &Code) const
{
	if(luaL_dostring(m_LS,Code.c_str()))
	{
		const char *l_Str=lua_tostring(m_LS, -1);
		//Info("%s",l_Str);
		assert(!"must be log");
	}
}

//Para ejecutar un fichero de c�digo LUA
void CScriptManager::RunFile(const std::string &FileName) const
{
	if(luaL_dofile(m_LS, FileName.c_str()))
	{
		const char *l_Str=lua_tostring(m_LS, -1);
		//Info("%s",l_Str);
		assert(!"must be log");
	}
}

void CScriptManager::RegisterLUAFunctions()
{
	//lua_register(m_LS, "set_speed_player", SetSpeedPlayer);
	/*lua_register(m_LS, "set_speed_player", SetSpeedPlayer);
	lua_register(m_LS, "get_speed_player", GetSpeedPlayer);*/
	//REGISTER_LUA_FUNCTION("set_speed_player", SetSpeedPlayer);
	//REGISTER_LUA_FUNCTION("get_speed_player", GetSpeedPlayer);

	// GRAPHICS-----------------------------------------------------------------------------------------
	module(m_LS)[
		class_<CDebugRender>("CDebugRender")
			.def(constructor<ID3D11Device*>())
			.def("get_simple_triangle", &CDebugRender::GetSimpleTriangle)
			.def("get_classic_blend_triangle", &CDebugRender::GetClassicBlendTriangle)
			.def("get_premult_blend_triangle", &CDebugRender::GetPremultBlendTriangle)
			.def("get_simple_cube", &CDebugRender::GetSimpleCube)
			.def("get_axis", &CDebugRender::GetAxis)
			.def("get_simple_triangle_bs_radi", &CDebugRender::GetSimpleTriangleBSRadi)
			.def("get_premult_blend_triangle_bs_radi", &CDebugRender::GetClassicBlendTriangleBSRadi)
			.def("get_premult_blend_triangle_bs_radi", &CDebugRender::GetPremultBlendTriangleBSRadi)
			.def("get_simple_cube_bs_radi", &CDebugRender::GetSimpleCubeBSRadi)
			.def("get_axis_bs_radi", &CDebugRender::GetAxisBSRadi)
			.def("get_simple_triangle_bb_min", &CDebugRender::GetSimpleTriangleBBMin)
			.def("get_classic_blend_triangle_bb_min", &CDebugRender::GetClassicBlendTriangleBBMin)
			.def("get_premult_blend_triangle_bb_min", &CDebugRender::GetPremultBlendTriangleBBMin)
			.def("get_simple_cube_bb_min", &CDebugRender::GetSimpleCubeBBMin)
			.def("get_axis_bb_min", &CDebugRender::GetAxisBBMin)
			.def("get_simple_triangle_bb_max", &CDebugRender::GetSimpleTriangleBBMax)
			.def("get_classic_blend_triangle_bb_max", &CDebugRender::GetClassicBlendTriangleBBMax)
			.def("get_premult_blend_triangle_bb_max", &CDebugRender::GetPremultBlendTriangleBBMax)
			.def("get_simple_cube_bb_max", &CDebugRender::GetSimpleCubeBBMax)
			.def("get_axis_bb_max", &CDebugRender::GetAxisBBMax)
	];

	// AnimatedModels----------------------------------------------------------------------------------
	module(m_LS) [
		class_<CAnimatedCoreModel>("CAnimatedCoreModel")
			.def(constructor<>())
			.def("get_name", &CAnimatedCoreModel::GetName)
			.def("set_name", &CAnimatedCoreModel::SetName)
			.def("get_calCoreModel", &CAnimatedCoreModel::GetCalCoreModel)
			.def("load",&CAnimatedCoreModel::Load)
	];

	module(m_LS) [
		class_<CAnimatedInstanceModel>("CAnimatedInstanceModel")
			.def(constructor<CXMLTreeNode&>())
			.def("get_name", &CAnimatedInstanceModel::GetName)
			.def("set_name", &CAnimatedInstanceModel::SetName)
			.def("initialize",&CAnimatedInstanceModel::Initialize)
			.def("render",&CAnimatedInstanceModel::Render)
			.def("update",&CAnimatedInstanceModel::Update)
			.def("destroy",&CAnimatedInstanceModel::Destroy)
			.def("execute_action",&CAnimatedInstanceModel::ExecuteAction)
			.def("blend_cycle", &CAnimatedInstanceModel::BlendCycle)
			.def("clera_cycle",&CAnimatedInstanceModel::ClearCycle)
			.def("is_cycle_animation_active",&CAnimatedInstanceModel::IsCycleAnimationActive)
			.def("is_action_animation_active",&CAnimatedInstanceModel::IsActionAnimationActive)
	];

	module(m_LS) [
		class_<CAnimatedModelsManager>("CAnimatedModelsManager")
			.def(constructor<>())
			.def("load",&CAnimatedModelsManager::Load)
			.def("reload", &CAnimatedModelsManager::Reload)
			.def("get_resource", &CAnimatedModelsManager::GetResource)
	];	

	// Camera----------------------------------------------------------------------------------------
	module(m_LS) [
		class_<CCamera>("CCamera")
			.def(constructor<>())
			.def("get_aspect_ratio",&CCamera::GetAspectRatio)
			.def("set_aspect_ratio",&CCamera::SetAspectRatio)
			.def("get_fov", &CCamera::GetFOV)
			.def("set_fov", &CCamera::SetFOV)
			.def("get_look_at", &CCamera::GetLookAt)
			.def("set_look_at", &CCamera::SetLookAt)
			.def("get_position", &CCamera::GetPosition)
			.def("set_position", &CCamera::SetPosition)
			.def("get_projection", &CCamera::GetProjection)
			.def("get_up", &CCamera::GetUp)
			.def("set_up", &CCamera::SetUp)
			.def("get_view", &CCamera::GetView)
			.def("get_z_far", &CCamera::GetZFar)
			.def("set_z_far", &CCamera::SetZFar)
			.def("get_z_near", &CCamera::GetZNear)
			.def("set_z_near", &CCamera::GetZNear)
			.def("set_matrixs", &CCamera::SetMatrixs)
	];

	module(m_LS)[
		class_<CCameraController>("CCameraController")
	//		.def(constructor<>())
			.def("add_pitch", &CCameraController::AddPitch)
			.def("add_yaw", &CCameraController::AddYaw)
			.def("get_pitch", &CCameraController::GetPitch)
			.def("get_position", &CCameraController::GetPosition)
			.def("get_right", &CCameraController::GetRight)
			.def("get_up", &CCameraController::GetUp)
			.def("get_yaw", &CCameraController::GetYaw)
			.def("set_camera", &CCameraController::SetCamera)
			.def("set_pitch", &CCameraController::SetPitch)
			.def("set_position", &CCameraController::SetPosition)
			.def("set_yaw", &CCameraController::SetYaw)
			.def("update", &CCameraController::Update)
	];

	module(m_LS)[
		class_<CCameraControllerManager>("CCameraControllerManager")
			.def(constructor<>())
			.def("choose_debug_camera", &CCameraControllerManager::ChooseDebugCamera)
			.def("choose_main_camera", &CCameraControllerManager::ChooseMainCamera)
			.def("destroy", &CCameraControllerManager::Destroy)
			.def("get_debug_camera", &CCameraControllerManager::GetDebugCamera)
			.def("get_main_camera", &CCameraControllerManager::GetMainCamera)
			.def("get_resource", &CCameraControllerManager::GetResource)
			.def("load", &CCameraControllerManager::Load)
			.def("reload", &CCameraControllerManager::Reload)
			.def("update", &CCameraControllerManager::Update)
			.def("update_debug_camera", &CCameraControllerManager::UpdateDebugCamera)
			.def("update_main_camera", &CCameraControllerManager::UpdateMainCamera)
	];

	module(m_LS)[
		class_<CCameraInfo>("CCameraInfo")
			.def(constructor<>())
			.def(constructor<Vect3f, Vect3f, Vect3f, float, float, float>())
			.def(constructor<CXMLTreeNode&>())
	];

	module(m_LS)[
		class_<CCameraKey>("CCameraKey")
			.def(constructor<CCameraInfo&,float>())
	];

	module(m_LS)[
		class_<CCameraKeyController>("CCameraKeyController")
			.def(constructor<CXMLTreeNode&>())
			.def("add_pitch",&CCameraKeyController::AddPitch)
			.def("add_yaw", &CCameraKeyController::AddYaw)
			.def("get_pitch", &CCameraKeyController::GetPitch)
			.def("get_position", &CCameraKeyController::GetPosition)
			.def("get_right", &CCameraKeyController::GetRight)
			.def("get_total_time", &CCameraKeyController::GetTotalTime)
			.def("get_up", &CCameraKeyController::GetUp)
			.def("get_yaw", &CCameraKeyController::GetYaw)
			.def("is_cycle", &CCameraKeyController::IsCycle)
			.def("is_reverse", &CCameraKeyController::IsReverse)
			.def("reset_time", &CCameraKeyController::ResetTime)
			.def("set_camera", &CCameraKeyController::SetCamera)
			.def("set_current_time", &CCameraKeyController::SetCurrentTime)
			.def("set_cycle", &CCameraKeyController::SetCycle)
			.def("set_pitch", &CCameraKeyController::SetPitch)
			.def("set_position", &CCameraKeyController::SetPosition)
			.def("set_reverse", &CCameraKeyController::SetReverse)
			.def("set_yaw", &CCameraKeyController::SetYaw)
			.def("update", &CCameraKeyController::Update)
	];

	module(m_LS) [
		class_<CFPSCameraController>("CFPSCameraController")
			.def(constructor<>())
			.def("add_pitch",&CFPSCameraController::AddPitch)
			.def("add_yaw", &CFPSCameraController::AddYaw)
			.def("get_direction", &CFPSCameraController::GetDirection)
			.def("get_pitch", &CFPSCameraController::GetPitch)
			.def("set_pitch", &CFPSCameraController::SetPitch)
			.def("get_right", &CFPSCameraController::GetRight)
			.def("get_up", &CFPSCameraController::GetUp)
			.def("get_yaw", &CFPSCameraController::GetYaw)
			.def("set_yaw", &CFPSCameraController::SetYaw)
			.def("move", &CFPSCameraController::Move)
			.def("set_camera", &CFPSCameraController::SetCamera)
			.def("set_position", &CFPSCameraController::SetPosition)
			.def("update", &CFPSCameraController::Update)
	];

	module(m_LS) [
		class_<CFrustum>("CFrustum")
			.def(constructor<>())
			.def("box_visible",&CFrustum::BoxVisible)
			.def("sphere_visible", &CFrustum::SphereVisible)
			.def("update", &CFrustum::Update)
	];

	module(m_LS) [
		class_<CSphericalCameraController>("CSphericalCameraController")
			.def(constructor<>())
			.def("add_pitch",&CSphericalCameraController::AddPitch)
			.def("add_yaw", &CSphericalCameraController::AddYaw)
			.def("add_zoom", &CSphericalCameraController::AddZoom)
			.def("get_direction", &CSphericalCameraController::GetDirection)
			.def("set_camera", &CSphericalCameraController::SetCamera)
			.def("get_pitch", &CSphericalCameraController::GetPitch)
			.def("set_pitch", &CSphericalCameraController::SetPitch)
			.def("get_right", &CSphericalCameraController::GetRight)
			.def("get_up", &CSphericalCameraController::GetUp)
			.def("get_yaw", &CSphericalCameraController::GetYaw)
			.def("set_yaw", &CSphericalCameraController::SetYaw)
			.def("set_position", &CSphericalCameraController::SetPosition)
			.def("set_zoom", &CSphericalCameraController::SetZoom)
			.def("update", &CSphericalCameraController::Update)
	];

	// Cinematics -----------------------------------------------------------------------------------
	module(m_LS)[
		class_<CCinematic>("CCinematic")
			.def(constructor<>())
			.def("load_xml", &CCinematic::LoadXML)
			.def("add_cinematic_object", &CCinematic::AddCinematicObject)
			.def("update", &CCinematic::Update)
			.def("render", &CCinematic::Render)
			.def("get_duration",&CCinematic::GetDuration)
			.def("get_name",&CCinematic::GetName)
			.def("get_pitch",&CCinematic::GetPitch)
			.def("get_position",&CCinematic::GetPosition)
			.def("get_prev_position",&CCinematic::GetPrevPosition)
			.def("get_roll",&CCinematic::GetRoll)
			.def("get_scale",&CCinematic::GetScale)
			.def("get_tick_count",&CCinematic::GetTickCount)
			.def("get_transform",&CCinematic::GetTransform)
			.def("get_visible",&CCinematic::GetVisible)
			.def("get_yaw",&CCinematic::GetYaw)
			.def("init",&CCinematic::Init)
			.def("is_finished",&CCinematic::IsFinished)
			.def("on_restart_cycle",&CCinematic::OnRestartCycle)
			.def("pause",&CCinematic::Pause)
			.def("play",&CCinematic::Play)
			.def("set_name",&CCinematic::SetName)
			.def("set_pitch",&CCinematic::SetPitch)
			.def("set_position",&CCinematic::SetPosition)
			.def("set_roll",&CCinematic::SetRoll)
			.def("set_scale",&CCinematic::SetScale)
			.def("set_visible",&CCinematic::SetVisible)
			.def("set_yaw",&CCinematic::SetYaw)
			.def("set_yaw_pitch_roll",&CCinematic::SetYawPitchRoll)
			.def("stop",&CCinematic::Stop)
	];

	module(m_LS)[
		class_<CCinematicObject>("CCinematicObject")
			.def(constructor<CXMLTreeNode&>())
			.def("is_ok", &CCinematicObject::IsOk)
			.def("add_cinematic_object_key_frame", &CCinematicObject::AddCinematicObjectKeyFrame)
			.def("update", &CCinematicObject::Update)
			.def("on_restart_cycle", &CCinematicObject::OnRestartCycle)
			.def("get_current_key", &CCinematicObject::GetCurrentKey)
			.def("get_duration",&CCinematicObject::GetDuration)
			.def("get_tick_count",&CCinematicObject::GetTickCount)
			.def("init",&CCinematicObject::Init)
			.def("is_finished",&CCinematicObject::IsFinished)
			.def("is_ok",&CCinematicObject::IsOk)
			.def("pause",&CCinematicObject::Pause)
			.def("play",&CCinematicObject::Play)
			.def("stop",&CCinematicObject::Stop)
	];

	module(m_LS)[
		class_<CCinematicObjectKeyFrame>("CCinematicObjectKeyFrame")
			.def(constructor<CXMLTreeNode&>())
			.def("get_key_frame_time",&CCinematicObjectKeyFrame::GetKeyFrameTime)
			.def("get_pitch",&CCinematicObjectKeyFrame::GetPitch)
			.def("get_position",&CCinematicObjectKeyFrame::GetPosition)
			.def("get_prev_position",&CCinematicObjectKeyFrame::GetPrevPosition)
			.def("get_roll",&CCinematicObjectKeyFrame::GetRoll)
			.def("get_scale",&CCinematicObjectKeyFrame::GetScale)
			.def("get_transform",&CCinematicObjectKeyFrame::GetTransform)
			.def("get_visible",&CCinematicObjectKeyFrame::GetVisible)
			.def("get_yaw",&CCinematicObjectKeyFrame::GetYaw)
			.def("render",&CCinematicObjectKeyFrame::Render)
			.def("set_key_frame_time",&CCinematicObjectKeyFrame::SetKeyFrameTime)
			.def("set_pitch",&CCinematicObjectKeyFrame::SetPitch)
			.def("set_position",&CCinematicObjectKeyFrame::SetPosition)
			.def("set_roll",&CCinematicObjectKeyFrame::SetRoll)
			.def("set_scale",&CCinematicObjectKeyFrame::SetScale)
			.def("set_visible",&CCinematicObjectKeyFrame::SetVisible)
			.def("set_yaw",&CCinematicObjectKeyFrame::SetYaw)
			.def("set_yaw_pitch_roll",&CCinematicObjectKeyFrame::SetYawPitchRoll)
	];

	module(m_LS)[
		class_<CCinematicPlayer>("CCinematicPlayer")
			.def(constructor<>())
			.def("get_duration", &CCinematicPlayer::GetDuration)
			.def("get_tick_count", &CCinematicPlayer::GetTickCount)
			.def("init", &CCinematicPlayer::Init)
			.def("is_finished", &CCinematicPlayer::IsFinished)
			.def("on_restart_cycle", &CCinematicPlayer::OnRestartCycle)
			.def("pause", &CCinematicPlayer::Pause)
			.def("play", &CCinematicPlayer::Play)
			.def("stop", &CCinematicPlayer::Stop)
			.def("update", &CCinematicPlayer::Update)
	];

	// ContextManager----------------------------------------------------------------------------------
	module(m_LS)[
		class_<CContextManager>("CContextManager")
			.def(constructor<>())
			.def("dispose", &CContextManager::Dispose)
			.def("resize", &CContextManager::Resize)
			.def("create_context", &CContextManager::CreateContext)
			.def("create_back_buffer", &CContextManager::CreateBackBuffer)
			.def("init_states", &CContextManager::InitStates)
			.def("get_aspect_ratio", &CContextManager::GetAspectRatio)
			.def("begin_render", &CContextManager::BeginRender)
			.def("end_render", &CContextManager::EndRender)
			.def("draw", &CContextManager::Draw)
			.def("get_device", &CContextManager::GetDevice)
			.def("get_device_context", &CContextManager::GetDeviceContext)
			//.def("set_base_color", &CContextManager::SetBaseColor)
			.def("set_world_matrix", &CContextManager::SetWorldMatrix)
			//.def("set_camera", &CContextManager::SetCamera) cosas
			//.def("set_debug_size", &CContextManager::SetDebugSize)
	];

	// Effects----------------------------------------------------------------------------------------
	module(m_LS)[
		class_<CAnimatedModelEffectParameters>("CAnimatedModelEffectParameters")
			.def(constructor<>())
	];

	module(m_LS)[
		class_<CEffect>("CEffect")
			.def(constructor<>())
			.def("update_parameters", &CEffect::UpdateParameters)
			.def("set_active", &CEffect::SetActive)
	];

	module(m_LS)[
		class_<CEffectManager>("CEffectManager")
			.def(constructor<>())
			.def("reload_file", &CEffectManager::ReloadFile)
			.def("reload", &CEffectManager::Reload)
			.def("load", &CEffectManager::Load)
			.def("get_vertex_shader", &CEffectManager::GetVertexShader)
			.def("get_pixel_shader", &CEffectManager::GetPixelShader)
			.def("set_scene_constants", &CEffectManager::SetSceneConstants)
			.def("set_light_constants", &CEffectManager::SetLightConstants)
	];

	module(m_LS)[
		class_<CEffectParameters>("CEffectParameters")
			.def(constructor<>())
	];

	module(m_LS)[
		class_<CEffectShader>("CEffectShader")
			//.def(constructor<>())
			.def("load", &CEffectShader::Load)
			.def("reload", &CEffectShader::Reload)
			.def("set_constant_buffer", &CEffectShader::SetConstantBuffer)
	];

	module(m_LS)[
		class_<CEffectVertexShader>("CEffectVertexShader")
			.def(constructor<const CXMLTreeNode &>())
			.def("load", &CEffectVertexShader::Load)
			.def("set_constant_buffer", &CEffectVertexShader::SetConstantBuffer)
			.def("get_vertex_shader", &CEffectVertexShader::GetVertexShader)
			.def("get_vertex_layout", &CEffectVertexShader::GetVertexLayout)
			//.def("get_constant_buffer", &CEffectVertexShader::GetConstantBuffer)
	];

	module(m_LS)[
		class_<CEffectPixelShader>("CEffectPixelShader")
			.def(constructor<const CXMLTreeNode &>())
			.def("load", &CEffectPixelShader::Load)
			.def("set_constant_buffer", &CEffectPixelShader::SetConstantBuffer)
			.def("get_pixel_shader", &CEffectPixelShader::GetPixelShader)
			//.def("get_constant_buffer", &CEffectPixelShader::GetConstantBuffer)
	];

	module(m_LS)[
		class_<CEffectTechnique>("CEffectTechnique")
			.def(constructor<CXMLTreeNode&>())
			.def("get_vertex_shader", &CEffectTechnique::GetVertexShader)
			.def("get_pixel_shader", &CEffectTechnique::GetPixelShader)
			.def("refresh", &CEffectTechnique::Refresh)
			.def("set_constant_buffer", &CEffectTechnique::SetConstantBuffer)
	];

	module(m_LS)[
		class_<CLightEffectParameters>("CLightEffectParameters")
			.def(constructor<>())
	];

	module(m_LS)[
		class_<CSceneEffectParameters>("CSceneEffectParameters")
			.def(constructor<>())
	];

	// Lights-----------------------------------------------------------------------------------------
	module(m_LS)[
		class_<CDirectionalLight>("CDirectionalLight")
			.def(constructor<>())
			.def(constructor<CXMLTreeNode&>())
			.def("get_direction", &CDirectionalLight::GetDirection)
			.def("set_direction", &CDirectionalLight::SetDirection)
			.def("render", &CDirectionalLight::Render)
	];

	module(m_LS)[
		class_<CLight>("CLight")
			.def(constructor<>())
			.def(constructor<CXMLTreeNode&>())
			.def("get_position", &CLight::GetPosition)
			.def("set_position", &CLight::SetPosition)
			.def("get_color", &CLight::GetColor)
			.def("set_color", &CLight::SetColor)
			.def("get_intensity", &CLight::GetIntensity)
			.def("set_intensity", &CLight::SetIntensity)
			.def("get_start_range_attenuation", &CLight::GetStartRangeAttenuation)
			.def("set_start_range_attenuation", &CLight::SetStartRangeAttenuation)
			.def("get_end_range_attenuation", &CLight::GetEndRangeAttenuation)
			.def("set_end_range_attenuation", &CLight::SetEndRangeAttenuation)
			.def("get_enabled", &CLight::GetEnabled)
			.def("set_enabled", &CLight::SetEnabled)
			.def("get_type", &CLight::GetType)
			.def("set_type", &CLight::SetType)
			.def("render", &CLight::Render)
			.def("get_light_type_by_name", &CLight::GetLightTypeByName)
	];

	module(m_LS)[
		class_<CLightManager>("CLightManager")
			.def(constructor<>())
			.def("load", &CLightManager::Load)
			.def("render", &CLightManager::Render)
			.def("reload", &CLightManager::Reload)
			.def("get_ambient_light", &CLightManager::GetAmbientLight)
	];

	module(m_LS)[
		class_<COmniLight>("COmniLight")
			.def(constructor<>())
			.def(constructor<CXMLTreeNode&>())
	];

	module(m_LS)[
		class_<CSpotLight>("CSpotLight")
			.def(constructor<>())
			.def(constructor<CXMLTreeNode&>())
			.def("get_angle", &CSpotLight::GetAngle)
			.def("set_angle", &CSpotLight::SetAngle)
			.def("get_fall_off", &CSpotLight::GetFallOff)
			.def("set_fall_off", &CSpotLight::SetFallOff)
	];

	// Materials--------------------------------------------------------------------------------------
	module(m_LS)[
		class_<CMaterial>("CMaterial")
			.def(constructor<CXMLTreeNode&>())
			.def("apply", &CMaterial::Apply)
			.def("get_effect_technique", &CMaterial::GetEffectTechnique)
	];

	module(m_LS)[
		class_<CMaterialManager>("CMaterialManager")
			.def(constructor<>())
			.def("load", &CMaterialManager::Load)
			.def("reload", &CMaterialManager::Reload)
	];

	// RenderableObjects------------------------------------------------------------------------------
	module(m_LS)[
		class_<CRenderableObject>("CRenderableObject")
			//.def(constructor<>())
			//.def(constructor<CXMLTreeNode&>())
			.def("update", &CRenderableObject::Update)
			.def("render", &CRenderableObject::Render)
	];

	module(m_LS)[
		class_<CRenderableObjectsManager>("CRenderableObjectsManager")
			.def(constructor<>())
			.def("update", &CRenderableObjectsManager::Update)
			.def("render", &CRenderableObjectsManager::Render)
			//.def("add_mesh_instance", &CRenderableObjectsManager::AddMeshInstance)  //cosas
			//.def("add_animated_instance_model", &CRenderableObjectsManager::AddAnimatedInstanceModel) //cosas
			//.def("clean_up", &CRenderableObjectsManager::CleanUp)
			.def("reload", &CRenderableObjectsManager::Reload)
			.def("load", &CRenderableObjectsManager::Load)
			//.def("get_instance", &CRenderableObjectsManager::GetInstance)
	];

	module(m_LS)[
		class_<CRenderableVertexs>("CRenderableVertexs")
			//.def(constructor<>())
			.def("render", &CRenderableVertexs::Render)
			.def("render_indexed", &CRenderableVertexs::RenderIndexed)
	];

			// Falta TemplatedRenderableIndexedVertexs, TemplatedRenderableVertexs y VertexTypes

	// RenderManager----------------------------------------------------------------------------------
	module(m_LS)[
		class_<CRenderManager>("CRenderManager")
			.def(constructor<>())
			.def("set_current_camera", &CRenderManager::SetCurrentCamera)
			.def("set_debug_camera", &CRenderManager::SetDebugCamera)
			.def("set_use_debug_camera", &CRenderManager::SetUseDebugCamera)
			//.def("add_renderable_object_to_render_list", &CRenderManager::AddRenderableObjectToRenderList)
			.def("render", &CRenderManager::Render)
	];

	// StaticMesh-------------------------------------------------------------------------------------
	module(m_LS)[
		class_<CInstanceMesh>("CInstanceMesh")
			.def(constructor<std::string&, std::string&>())
			.def(constructor<CXMLTreeNode&>())
			.def("render", &CInstanceMesh::Render)
	];

	module(m_LS)[
		class_<CStaticMesh>("CStaticMesh")
			.def(constructor<>())
			.def("load", &CStaticMesh::Load)
			.def("reload", &CStaticMesh::Reload)
			.def("render", &CStaticMesh::Render)
	];

	module(m_LS)[
		class_<CStaticMeshManager>("CStaticMeshManager")
			.def(constructor<>())
			.def("load", &CStaticMeshManager::Load)
			.def("reload", &CStaticMeshManager::Reload)
	];

	// Texture----------------------------------------------------------------------------------------
	module(m_LS)[
		class_<CTexture>("CTexture")
			.def(constructor<>())
			.def("load", &CTexture::Load)
			.def("activate", &CTexture::Activate)
			.def("reload", &CTexture::Reload)
	];

	module(m_LS)[
		class_<CTextureManager>("CTextureManager")
			.def(constructor<>())
			.def("get_texture", &CTextureManager::GetTexture)
			.def("reload", &CTextureManager::Reload)
	];

	//RunFile("./data/scripting/init.lua");

	RunCode("Init()");
	RunCode("local light=COmniLight();light.set_intensity(1.1);");
	//RunCode("local value=3;set_speed_player(value); value=get_speed_player()+2; set_speed_player(value)");
	//RunCode("local animatedcoremodel=CAnimatedCoreModel(); animatedcoremodel:set_name(\"Nombre\"); local name=animatedcoremodel:get_name()");
}

/*void OnEnterEvent(CEvent *Event)
{
	RunCode("on_enter_player_on_event_"+Event->GetName()+"()");
	RunCode("on_enter_player_on_event_event_1()");
}*/

