#include "stdafx.h"

void afSetVertexBuffer(ID3D12GraphicsCommandList* list, VBOID id, int stride)
{
	D3D12_RESOURCE_DESC desc = id->GetDesc();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = { id->GetGPUVirtualAddress(), (UINT)desc.Width, (UINT)stride };
	list->IASetVertexBuffers(0, 1, &vertexBufferView);
}

void afWriteBuffer(const IBOID id, const void* buf, int size)
{
#ifdef _DEBUG
	D3D12_RESOURCE_DESC desc = id->GetDesc();
	if (size > (int)desc.Width) {
		return;
	}
#endif
	void* p;
	id->Map(0, nullptr, &p);
	memcpy(p, buf, size);
	id->Unmap(0, nullptr);
}

VBOID afCreateVertexBuffer(int size, const void* buf)
{
	VBOID v;
	deviceMan.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(size), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&v));
	if (buf) {
		afWriteBuffer(v, buf, size);
	}
	return v;
}

IBOID afCreateIndexBuffer(const AFIndex* indi, int numIndi)
{
	return afCreateVertexBuffer(numIndi * sizeof(AFIndex), indi);
}

SRVID afCreateTexture2D(AFDTFormat format, const IVec2& size, void *image)
{
	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.Width = size.x;
	textureDesc.Height = size.y;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	SRVID id;
	HRESULT hr = deviceMan.GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&id));

	D3D12_BOX box = { 0, 0, 0, (UINT)size.x, (UINT)size.y, 1 };
	id->WriteToSubresource(0, &box, image, size.x * 4, size.x * size.y * 4);
	return id;
}

SRVID afCreateTexture2D(AFDTFormat format, const struct TexDesc& desc, int mipCount, const AFTexSubresourceData datas[])
{
	// TODO:
	return SRVID();
}

void afDrawIndexed(ID3D12GraphicsCommandList* list, PrimitiveTopology pt, int numIndices, int start, int instanceCount)
{
	list->IASetPrimitiveTopology(pt);
	list->DrawIndexedInstanced(numIndices, instanceCount, start, 0, 0);
}

void afDraw(ID3D12GraphicsCommandList* list, PrimitiveTopology pt, int numVertices, int start, int instanceCount)
{
	list->IASetPrimitiveTopology(pt);
	list->DrawInstanced(numVertices, instanceCount, start, 0);
}

ComPtr<ID3D12PipelineState> afCreatePSO(const char *shaderName, const InputElement elements[], int numElements, BlendMode blendMode, DepthStencilMode depthStencilMode, CullMode cullMode, ComPtr<ID3D12RootSignature> rootSignature)
{
	ComPtr<ID3DBlob> vertexShader = CompileShader("shaders", "VSMain", "vs_5_0");
	ComPtr<ID3DBlob> pixelShader = CompileShader("shaders", "PSMain", "ps_5_0");

	// Define the vertex input layout.
	InputElement inputElementDescs[] =
	{
		CInputElement("POSITION", SF_R32G32B32_FLOAT, 0),
		CInputElement("TEXCOORD", SF_R32G32_FLOAT, 12),
	};

	D3D12_BLEND_DESC bd = { FALSE, FALSE,{
		FALSE,FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL },
	};

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { elements, (UINT)numElements };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
	psoDesc.RasterizerState = { D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK };
	psoDesc.BlendState = bd;
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	ComPtr<ID3D12PipelineState> pso;
	HRESULT hr = deviceMan.GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));
	assert(hr == S_OK);
	return pso;
}
