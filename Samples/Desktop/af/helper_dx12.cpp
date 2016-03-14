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
