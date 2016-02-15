#include "stdafx.h"

void afWriteBuffer(const IBOID id, const void* buf, int size)
{
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
