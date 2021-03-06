#include "stdafx.h"

DeviceManDX12 deviceMan;

DeviceManDX12::~DeviceManDX12()
{
	assert(!factory);
	assert(!device);
	assert(!commandQueue);
	assert(!commandAllocator);
	assert(!commandList);
	assert(!fence);
	assert(!swapChain);
	for (int i = 0; i < numFrameBuffers; i++) {
		assert(!renderTargets[i]);
	}
	assert(!rtvHeap);
}

void DeviceManDX12::Destroy()
{
	if (fenceEvent != INVALID_HANDLE_VALUE) {
		WaitForPreviousFrame();
		CloseHandle(fenceEvent);
		fenceEvent = INVALID_HANDLE_VALUE;
	}
	commandList.Reset();
	commandAllocator.Reset();
	commandQueue.Reset();
	swapChain.Reset();
	for (int i = 0; i < numFrameBuffers; i++) {
		renderTargets[i].Reset();
	}
	rtvHeap.Reset();
	factory.Reset();
	fence.Reset();
	fenceValue = 1;
	frameIndex = 0;
	device.Reset();
}

void DeviceManDX12::WaitForPreviousFrame()
{
	const UINT64 prevFenceValue = fenceValue;
	deviceMan.GetCommandQueue()->Signal(fence.Get(), prevFenceValue);
	fenceValue++;

	if (fence->GetCompletedValue() < prevFenceValue)
	{
		fence->SetEventOnCompletion(prevFenceValue, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void DeviceManDX12::SetRenderTarget()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += frameIndex * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void DeviceManDX12::BeginScene()
{
	WaitForPreviousFrame();
	commandAllocator->Reset();
	commandList->Reset(commandAllocator.Get(), nullptr);
	frameIndex = deviceMan.GetSwapChain()->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrier = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE,{ deviceMan.GetRenderTarget().Get(), 0, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET } };
	commandList->ResourceBarrier(1, &barrier);

	DXGI_SWAP_CHAIN_DESC desc;
	deviceMan.GetSwapChain()->GetDesc(&desc);

	D3D12_VIEWPORT vp = {0.f, 0.f, (float)desc.BufferDesc.Width, (float)desc.BufferDesc.Height, 0.f, 1.f};
	D3D12_RECT rc = {0, 0, (LONG)desc.BufferDesc.Width, (LONG)desc.BufferDesc.Height};
	commandList->RSSetViewports(1, &vp);
	commandList->RSSetScissorRects(1, &rc);

	SetRenderTarget();
}

void DeviceManDX12::EndScene()
{
	D3D12_RESOURCE_BARRIER barrier = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE,{ deviceMan.GetRenderTarget().Get(), 0, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT } };
	commandList->ResourceBarrier(1, &barrier);

	commandList->Close();
	ID3D12CommandList* lists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(lists), lists);
}

void DeviceManDX12::Flush()
{
	commandList->Close();
	ID3D12CommandList* lists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(lists), lists);
	WaitForPreviousFrame();
	commandAllocator->Reset();
	commandList->Reset(commandAllocator.Get(), nullptr);
}

void DeviceManDX12::Present()
{
	EndScene();
	swapChain->Present(1, 0);
	BeginScene();
}

void DeviceManDX12::Create(HWND hWnd)
{
	Destroy();
#ifndef NDEBUG
	ComPtr<ID3D12Debug> debug;
	if (S_OK == D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))
	{
		debug->EnableDebugLayer();
	}
#endif
	if (S_OK != CreateDXGIFactory1(IID_PPV_ARGS(&factory))) {
		Destroy();
		return;
	}
	ComPtr<IDXGIAdapter1> adapter;
	for (int i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &adapter); i++) {
		if (S_OK == D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))) {
			break;
		}
	}
	if (!device) {
		Destroy();
		return;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = 0;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;
	if (S_OK != device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))) {
		Destroy();
		return;
	}

	RECT rc;
	GetClientRect(hWnd, &rc);

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = numFrameBuffers;
	sd.BufferDesc.Width = rc.right - rc.left;
	sd.BufferDesc.Height = rc.bottom - rc.top;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;

	ComPtr<IDXGISwapChain> sc;
	if (S_OK != factory->CreateSwapChain(deviceMan.GetCommandQueue().Get(), &sd, &sc)) {
		Destroy();
		return;
	}
	if (S_OK != sc.As(&swapChain)) {
		Destroy();
		return;
	}

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = numFrameBuffers;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	for (int i = 0; i < numFrameBuffers; i++) {
		if (S_OK != deviceMan.GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]))) {
			Destroy();
			return;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
	}

	factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
	commandList->Close();

	if (S_OK != device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))) {
		Destroy();
		return;
	}
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	assert (fenceEvent);

	BeginScene();
}
