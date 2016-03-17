class DeviceManDX12
{
	ComPtr<IDXGIFactory4> factory;
	ComPtr<ID3D12Device> device;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent = INVALID_HANDLE_VALUE;
	UINT64 fenceValue = 1;
public:
	~DeviceManDX12();
	void Create(HWND hWnd, int bufferCount);
	void Destroy();
	void Present();
	void WaitForPreviousFrame();
	ComPtr<ID3D12Device> GetDevice() { return device; }
	ComPtr<IDXGIFactory4> GetFactory() { return factory; }
	ComPtr<IDXGISwapChain3> GetSwapChain() { return swapChain; }
	ComPtr<ID3D12CommandQueue> GetCommandQueue() { return commandQueue; }
	ID3D12CommandAllocator* GetCommandAllocator() { return commandAllocator.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList.Get(); }
};

extern DeviceManDX12 deviceMan;
