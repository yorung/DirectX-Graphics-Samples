#define SF_R32G32_FLOAT DXGI_FORMAT_R32G32_FLOAT
#define SF_R32G32B32_FLOAT DXGI_FORMAT_R32G32B32_FLOAT
#define SF_R32G32B32A32_FLOAT DXGI_FORMAT_R32G32B32A32_FLOAT
#define SF_R8G8B8A8_UNORM DXGI_FORMAT_R8G8B8A8_UNORM

typedef D3D12_INPUT_ELEMENT_DESC InputElement;

class CInputElement : public InputElement {
public:
	CInputElement(const char* name, DXGI_FORMAT format, int offset, int inputSlot = 0) {
		SemanticName = name;
		SemanticIndex = 0;
		Format = format;
		InputSlot = inputSlot;
		AlignedByteOffset = offset;
		InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		InstanceDataStepRate = 0;
	}
};

typedef unsigned short AFIndex;

#define AFIndexTypeToDevice DXGI_FORMAT_R16_UINT
typedef ComPtr<ID3D12Resource> IBOID;
typedef ComPtr<ID3D12Resource> VBOID;

void afWriteBuffer(const IBOID id, const void* buf, int size);
VBOID afCreateVertexBuffer(int size, const void* buf);
