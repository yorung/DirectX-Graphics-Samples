class DeviceManDX12
{
	ComPtr<IDXGIFactory4> factory;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<IDXGISwapChain3> swapChain;
public:
	~DeviceManDX12();
	void Create(HWND hWnd, int bufferCount);
	void Destroy();
	ComPtr<ID3D12Device> GetDevice() { return device; }
	ComPtr<IDXGIFactory4> GetFactory() { return factory; }
	ComPtr<ID3D12CommandQueue> GetCommandQueue() { return commandQueue; }
	ComPtr<IDXGISwapChain3> GetSwapChain() { return swapChain; }
};

extern DeviceManDX12 deviceMan;
