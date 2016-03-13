#include "stdafx.h"

void afSetVertexBuffer(ID3D12GraphicsCommandList* list, VBOID id, int stride)
{
	D3D12_RESOURCE_DESC desc = id->GetDesc();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = { id->GetGPUVirtualAddress(), desc.Width, stride };
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
	// TODO:
	return SRVID();
}

SRVID afCreateTexture2D(AFDTFormat format, const struct TexDesc& desc, int mipCount, const AFTexSubresourceData datas[])
{
	// TODO:
	return SRVID();
}
