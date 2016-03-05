class DeviceManDX12
{
	ComPtr<IDXGIFactory4> factory;
	ComPtr<ID3D12Device> device;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
public:
	~DeviceManDX12();
	void Create(HWND hWnd, int bufferCount);
	void Destroy();
	ComPtr<ID3D12Device> GetDevice() { return device; }
	ComPtr<IDXGIFactory4> GetFactory() { return factory; }
	ComPtr<IDXGISwapChain3> GetSwapChain() { return swapChain; }
	ComPtr<ID3D12CommandQueue> GetCommandQueue() { return commandQueue; }
	ID3D12CommandAllocator* GetCommandAllocator() { return commandAllocator.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList.Get(); }
};

extern DeviceManDX12 deviceMan;
