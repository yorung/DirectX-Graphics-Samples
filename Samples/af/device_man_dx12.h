class DeviceManDX12
{
	ComPtr<ID3D12Device> device;
public:
	~DeviceManDX12() { assert(!device); }
	void Create(ComPtr<ID3D12Device> d) { device = d; }
	void Destroy() { device.Reset(); }
	ComPtr<ID3D12Device> GetDevice() { return device; }
};

extern DeviceManDX12 deviceMan;
