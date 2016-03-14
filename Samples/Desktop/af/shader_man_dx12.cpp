#include "stdafx.h"

ComPtr<ID3DBlob> CompileShader(const char* name, const char* entryPoint, const char* target)
{
	char path[MAX_PATH];
	sprintf_s(path, sizeof(path), "%s.hlsl", name);
//	sprintf_s(path, sizeof(path), "hlsl/%s.hlsl", name);
#ifdef _DEBUG
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif
	ComPtr<ID3DBlob> blob, err;
	WCHAR wname[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, path, -1, wname, dimof(wname));
	D3DCompileFromFile(wname, nullptr, nullptr, entryPoint, target, flags, 0, &blob, &err);
	if (err) {
		MessageBoxA(nullptr, (const char*)err->GetBufferPointer(), name, MB_OK | MB_ICONERROR);
	}
	return blob;
}

ShaderManDX12 shaderMan;

ShaderManDX12::ShaderManDX12()
{
	m_effects.push_back(Effect());	// make ID 0 invalid
}

ShaderManDX12::~ShaderManDX12()
{
	Destroy();
}

ShaderManDX12::SMID ShaderManDX12::Create(const char *name, const D3D12_INPUT_ELEMENT_DESC elements[], int numElements, BlendMode blendMode, DepthStencilMode depthStencilMode, CullMode cullMode)
{
	auto it = m_nameToId.find(name);
	if (it != m_nameToId.end())
	{
		return it->second;
	}

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Effect e;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = { 0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT };
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	deviceMan.GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&e.rso));

	ComPtr<ID3DBlob> vertexShader = CompileShader(name, "VSMain", "vs_5_0");
	ComPtr<ID3DBlob> pixelShader = CompileShader(name, "PSMain", "ps_5_0");

	D3D12_RASTERIZER_DESC rd = { D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK };
	D3D12_BLEND_DESC bd = { FALSE, FALSE,{
		FALSE,FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL },
	};

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { elements, (UINT)numElements };
	psoDesc.pRootSignature = e.rso.Get();
	psoDesc.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
	psoDesc.RasterizerState = rd;
	psoDesc.BlendState = bd;
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	ComPtr<ID3D12PipelineState> pso;
	HRESULT hr = deviceMan.GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&e.pso));
	assert(hr == S_OK);

	m_effects.push_back(e);
	return m_nameToId[name] = (int)m_effects.size() - 1;
}

void ShaderManDX12::Destroy()
{
	m_effects.clear();
	m_nameToId.clear();
	m_effects.push_back(Effect());	// make ID 0 invalid
}

void ShaderManDX12::Apply(SMID id, ComPtr<ID3D12GraphicsCommandList>& list)
{
	if (id >= 0 && id < (SMID)m_effects.size())
	{
		auto& e = m_effects[id];
		list->SetPipelineState(e.pso.Get());
		list->SetGraphicsRootSignature(e.rso.Get());
	}
}

