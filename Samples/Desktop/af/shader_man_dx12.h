class ShaderManDX12
{
public:
	typedef unsigned int SMID;
	static const SMID INVALID_SMID = 0;
private:
	class Effect {
	public:
		Effect() {}
		Effect(const Effect& r) { *this = r; }
		const Effect& operator=(const Effect& r) {
			pso = r.pso;
			rso = r.rso;
			return *this;
		}
		ComPtr<ID3D12PipelineState> pso;
		ComPtr<ID3D12RootSignature> rso;
	};
	std::map<std::string, SMID> m_nameToId;
	std::vector<Effect> m_effects;
public:
	ShaderManDX12();
	~ShaderManDX12();
	SMID Create(const char *name, const D3D12_INPUT_ELEMENT_DESC elements[], int numElements, BlendMode blendMode, DepthStencilMode depthStencilMode, CullMode cullMode);
	void Destroy();
	void Apply(SMID id, ComPtr<ID3D12GraphicsCommandList>& list);
};
extern ShaderManDX12 shaderMan;
typedef ShaderManDX12 ShaderMan;


ComPtr<ID3DBlob> CompileShader(const char* name, const char* entryPoint, const char* target);
