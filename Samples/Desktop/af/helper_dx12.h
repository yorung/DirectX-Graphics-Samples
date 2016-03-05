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
typedef ComPtr<ID3D12Resource> SRVID;

void afWriteBuffer(const IBOID id, const void* buf, int size);
VBOID afCreateVertexBuffer(int size, const void* buf);
IBOID afCreateIndexBuffer(const AFIndex* indi, int numIndi);

enum CullMode {
	CM_DISABLE,
	CM_CW,
	CM_CCW,
};
enum BlendMode {
	BM_NONE,
	BM_ALPHA,
};
enum DepthStencilMode {
	DSM_DISABLE,
	DSM_DEPTH_ENABLE,
	DSM_DEPTH_CLOSEREQUAL_READONLY,
};

typedef D3D12_SUBRESOURCE_DATA AFTexSubresourceData;
typedef DXGI_FORMAT AFDTFormat;
#define AFDT_INVALID DXGI_FORMAT_UNKNOWN
#define AFDT_R8G8B8A8_UNORM DXGI_FORMAT_R8G8B8A8_UNORM
#define AFDT_R5G6B5_UINT DXGI_FORMAT_B5G6R5_UNORM
#define AFDT_R32G32B32A32_FLOAT DXGI_FORMAT_R32G32B32A32_FLOAT
#define AFDT_R16G16B16A16_FLOAT DXGI_FORMAT_R16G16B16A16_FLOAT
#define AFDT_DEPTH DXGI_FORMAT_D24_UNORM_S8_UINT
#define AFDT_DEPTH_STENCIL DXGI_FORMAT_D24_UNORM_S8_UINT
#define AFDT_BC1_UNORM DXGI_FORMAT_BC1_UNORM
#define AFDT_BC2_UNORM DXGI_FORMAT_BC2_UNORM
#define AFDT_BC3_UNORM DXGI_FORMAT_BC3_UNORM

SRVID afCreateTexture2D(AFDTFormat format, const IVec2& size, void *image);
SRVID afCreateTexture2D(AFDTFormat format, const struct TexDesc& desc, int mipCount, const AFTexSubresourceData datas[]);