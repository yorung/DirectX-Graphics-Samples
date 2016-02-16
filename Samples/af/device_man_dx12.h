class DeviceManDX12
{
	ComPtr<IDXGIFactory4> factory;
	ComPtr<ID3D12Device> device;
public:
	~DeviceManDX12();
	void Create();
	void Destroy();
	ComPtr<ID3D12Device> GetDevice() { return device; }
	ComPtr<IDXGIFactory4> GetFactory() { return factory; }
};

extern DeviceManDX12 deviceMan;
