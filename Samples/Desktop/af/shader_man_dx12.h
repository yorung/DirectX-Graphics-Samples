/*
class ShaderManDX12
{
public:
	typedef unsigned int SMID;
	static const SMID INVALID_SMID = 0;
private:
	struct Effect
	{
		int numElements;
		BlendMode blendMode;
		DepthStencilMode depthStencilMode;
		CullMode cullMode;
		Effect() { memset(this, 0, sizeof(*this)); }
	};
	std::map<std::string, SMID> m_nameToId;
	std::vector<Effect> m_effects;
public:
	ShaderManDX12();
	~ShaderManDX12();
	SMID Create(const char *name, const D3D11_INPUT_ELEMENT_DESC elements[], int numElements, BlendMode blendMode, DepthStencilMode depthStencilMode, CullMode cullMode);
	void Destroy();
	void Reload();
	void Apply(SMID id);
};
extern ShaderManDX12 shaderMan;
typedef ShaderManDX12 ShaderMan;
*/

ComPtr<ID3DBlob> CompileShader(const char* name, const char* entryPoint, const char* target);
