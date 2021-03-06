#include "Effects\EffectShader.h"
#include "RenderManager\RenderManager.h"
#include "Effects\SceneEffectParameters.h"
#include "Effects\AnimatedModelEffectParameters.h"
#include "Effects\LightEffectParameters.h"
#include "Engine\UABEngine.h"
#include "RenderableObjects\VertexTypes.h"
#include "Utils.h"

#include <assert.h>

#include <D3Dcompiler.h>

#define USE_D3DX
#ifdef USE_D3DX
#include <D3DX11async.h>

#endif

CEffectShader::CEffectShader(const CXMLTreeNode &TreeNode):CNamed(TreeNode){
	m_Filename = TreeNode.GetPszProperty("file");
	m_ShaderModel = TreeNode.GetPszProperty("shader_model");
	m_EntryPoint = TreeNode.GetPszProperty("entry_point");
}

CEffectShader::~CEffectShader(void)
{
}

void CEffectShader::CreateShaderMacro()
{
	m_PreprocessorMacros.clear();
	if (m_Preprocessor.empty())
	{
		m_ShaderMacros = NULL;
		return;
	}
	std::vector<std::string> l_PreprocessorItems;
	SplitString(m_Preprocessor, ';', l_PreprocessorItems);
	m_ShaderMacros = new D3D10_SHADER_MACRO[l_PreprocessorItems.size() + 1];
	for (size_t i = 0; i<l_PreprocessorItems.size(); ++i)
	{
		std::vector<std::string> l_PreprocessorItem;
		SplitString(l_PreprocessorItems[i], '=', l_PreprocessorItem);
		if (l_PreprocessorItem.size() == 1)
		{
			m_PreprocessorMacros.push_back(l_PreprocessorItems[i]);
			m_PreprocessorMacros.push_back("1");
		}
		else if (l_PreprocessorItem.size() == 2)
		{
			m_PreprocessorMacros.push_back(l_PreprocessorItem[0]);
			m_PreprocessorMacros.push_back(l_PreprocessorItem[1]);
		}
		else
		{
			//Info("Error creating shader macro '%s', with wrong size on parameters", l_PreprocessorItems[i]);
			printf("Error creating shader macro '%s', with wrong size on parameters", l_PreprocessorItems[i]);
			CHECKED_DELETE_ARRAY(m_ShaderMacros);
			return;
		}
	}
	for (size_t i = 0; i<l_PreprocessorItems.size(); ++i)
	{
		m_ShaderMacros[i].Name = m_PreprocessorMacros[i * 2].c_str();
		m_ShaderMacros[i].Definition = m_PreprocessorMacros[(i * 2) + 1].c_str();
	}
	m_ShaderMacros[l_PreprocessorItems.size()].Name = NULL;
	m_ShaderMacros[l_PreprocessorItems.size()].Definition = NULL;
}

bool CEffectShader::LoadShader(const std::string &Filename, const std::string
	&EntryPoint, const std::string &ShaderModel, ID3DBlob **BlobOut)
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(Filename.c_str(), m_ShaderMacros, NULL,
		EntryPoint.c_str(), ShaderModel.c_str(), dwShaderFlags, 0, NULL, BlobOut,
		&pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob)
			pErrorBlob->Release();
		return false;
	}
	if (pErrorBlob)
		pErrorBlob->Release();
	return true;
}

bool CEffectShader::CreateConstantBuffer(int IdBuffer, unsigned int BufferSize)
{
	ID3D11Buffer *l_ConstantBuffer;
	/*CRenderManager &l_RenderManager = UABEngine.GetRenderManager();
	ID3D11Device *l_Device = l_RenderManager.GetDevice();*/
	CRenderManager* l_RenderManager = UABEngine.GetRenderManager();
	ID3D11Device *l_Device = l_RenderManager->GetDevice();
	D3D11_BUFFER_DESC l_BufferDescription;
	ZeroMemory(&l_BufferDescription, sizeof(l_BufferDescription));
	l_BufferDescription.Usage = D3D11_USAGE_DEFAULT;
	l_BufferDescription.ByteWidth = BufferSize;
	if ((BufferSize % 16) != 0)
		//Info("Constant Buffer '%d' with wrong size '%d' on shader '%s'.", IdBuffer, BufferSize, m_Filename.c_str());
		printf("Constant Buffer '%d' with wrong size '%d' on shader '%s'.", IdBuffer, BufferSize, m_Filename.c_str());

	l_BufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	l_BufferDescription.CPUAccessFlags = 0 ;
	l_BufferDescription.MiscFlags = 0;
	l_BufferDescription.StructureByteStride = 0;
	if (FAILED(l_Device->CreateBuffer(&l_BufferDescription, NULL,
		&l_ConstantBuffer)))
	{
		//Info("Constant buffer '%d' couldn't created on shader '%s'.", IdBuffer, m_Filename.c_str());
		printf("Constant buffer '%d' couldn't created on shader '%s'.", IdBuffer, m_Filename.c_str());
		m_ConstantBuffers.push_back(NULL);
		return false;
	}
	m_ConstantBuffers.push_back(l_ConstantBuffer);
	return true;
}

bool CEffectShader::CreateConstantBuffer()
{
	CreateConstantBuffer(SCENE_CONSTANT_BUFFER_ID, sizeof(CSceneEffectParameters));
	CreateConstantBuffer(LIGHT_CONSTANT_BUFFER_ID, sizeof(CLightEffectParameters));
	CreateConstantBuffer(ANIMATED_CONSTANT_BUFFER_ID, sizeof(CAnimatedModelEffectParameters));

	
	/*CreateConstantBuffer( SCENE_CONSTANT_BUFFER_ID, 60*sizeof(float));
	CreateConstantBuffer( LIGHT_CONSTANT_BUFFER_ID, 80*sizeof(float));
	CreateConstantBuffer( ANIMATED_CONSTANT_BUFFER_ID, 640*sizeof(float));*/

	/*CRenderManager* l_RenderManager=UABEngine.GetRenderManager();
	ID3D11Device *l_Device=l_RenderManager->GetDevice();
	D3D11_BUFFER_DESC l_BufferDescription;
	ZeroMemory(&l_BufferDescription, sizeof(l_BufferDescription));
	l_BufferDescription.Usage=D3D11_USAGE_DEFAULT;
	l_BufferDescription.ByteWidth=sizeof(CEffectParameters);
	l_BufferDescription.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
	l_BufferDescription.CPUAccessFlags=0;
	if( FAILED(l_Device->CreateBuffer(&l_BufferDescription, NULL,&m_ConstantBuffer)))
		return false;*/
	return true;
}

void CEffectShader::Destroy()
{
	assert(!"this method must not be called");
}

bool CEffectShader::Reload()
{
	assert(!"this method must not be called");
	return false;
}

ID3D11Buffer * CEffectShader::GetConstantBuffer(unsigned int IdBuffer)
{
	return m_ConstantBuffers[IdBuffer];
}

CEffectVertexShader::CEffectVertexShader(const CXMLTreeNode &TreeNode):CEffectShader(TreeNode),
	m_VertexShader(nullptr),
	m_VertexLayout(nullptr)
{
	m_VertexType = TreeNode.GetPszProperty("vertex_type");
}

void CEffectVertexShader::Destroy()
{
	//delete m_VertexLayout;
	//delete m_VertexShader;
	//delete m_ConstantBuffer;
}

bool CEffectVertexShader::Load()
{
	CreateShaderMacro();
	ID3DBlob *l_VSBlob = NULL;
	bool l_Loaded = LoadShader(m_Filename, m_EntryPoint, m_ShaderModel,
		&l_VSBlob);
	if (!l_Loaded)
		return false;
	/*CRenderManager &l_RenderManager = UABEngine.GetRenderManager();
	ID3D11Device *l_Device = l_RenderManager.GetDevice();*/
	CRenderManager* l_RenderManager = UABEngine.GetRenderManager();
	ID3D11Device *l_Device = l_RenderManager->GetDevice();
	HRESULT l_HR = l_Device->CreateVertexShader(l_VSBlob->GetBufferPointer(),
		l_VSBlob->GetBufferSize(), NULL, &m_VertexShader);
	if (FAILED(l_HR))
	{
		l_VSBlob->Release();
		return false;
	}
	if (m_VertexType == "MV_POSITION_NORMAL_TEXTURE_VERTEX")
		l_Loaded = MV_POSITION_NORMAL_TEXTURE_VERTEX::CreateInputLayout(l_RenderManager, l_VSBlob, &m_VertexLayout);
	else
		//Info("Vertex type '%s' not recognized on CEffectVertexShader::Load", m_VertexType.c_str());
		printf("Vertex type '%s' not recognized on CEffectVertexShader::Load", m_VertexType.c_str());
	l_VSBlob->Release();
	if (!l_Loaded)
		return false;
	return CreateConstantBuffer();
}

void CEffectVertexShader::SetConstantBuffer(unsigned int IdBuffer, void
	*ConstantBuffer)
{
	ID3D11DeviceContext
		*l_DeviceContext = UABEngine.GetRenderManager()->GetDeviceContext();
	ID3D11Buffer *l_ConstantBuffer = GetConstantBuffer(IdBuffer);
	if (l_ConstantBuffer != NULL)
	{
		l_DeviceContext->UpdateSubresource(l_ConstantBuffer, 0, NULL,
			ConstantBuffer, 0, 0);
		l_DeviceContext->VSSetConstantBuffers(IdBuffer, 1, &l_ConstantBuffer);
	}
}

CEffectPixelShader::CEffectPixelShader(const CXMLTreeNode &TreeNode):CEffectShader(TreeNode),
	m_PixelShader(nullptr)
{
}

void CEffectPixelShader::Destroy()
{
	//CHECKED_DELETE(m_PixelShader);
	//CHECKED_DELETE(m_ConstantBuffer);
}

bool CEffectPixelShader::Load()
{
	CreateShaderMacro();
	ID3DBlob *l_PSBlob = NULL;
	bool l_Loaded = LoadShader(m_Filename.c_str(), m_EntryPoint.c_str(),
		m_ShaderModel.c_str(), &l_PSBlob);
	if (!l_Loaded)
		return false;
	/*CRenderManager &l_RenderManager = UABEngine.GetRenderManager();
	ID3D11Device *l_Device = l_RenderManager.GetDevice();*/
	CRenderManager* l_RenderManager = UABEngine.GetRenderManager();
	ID3D11Device *l_Device = l_RenderManager->GetDevice();
	HRESULT l_HR = l_Device->CreatePixelShader(l_PSBlob->GetBufferPointer(),
		l_PSBlob->GetBufferSize(), NULL, &m_PixelShader);
	l_PSBlob->Release();
	return CreateConstantBuffer();
}

void CEffectPixelShader::SetConstantBuffer(unsigned int IdBuffer, void *ConstantBuffer)
{
	ID3D11DeviceContext *l_DeviceContext = UABEngine.GetRenderManager()->GetDeviceContext();
	ID3D11Buffer *l_ConstantBuffer = GetConstantBuffer(IdBuffer);
	l_DeviceContext->UpdateSubresource(l_ConstantBuffer, 0, NULL,
		ConstantBuffer, 0, 0);
	l_DeviceContext->PSSetConstantBuffers(IdBuffer, 1, &l_ConstantBuffer);
}