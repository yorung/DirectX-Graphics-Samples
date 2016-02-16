#include "stdafx.h"

DeviceManDX12 deviceMan;

DeviceManDX12::~DeviceManDX12()
{
	assert(!factory);
	assert(!device);
}

void DeviceManDX12::Destroy()
{
	factory.Reset();
	device.Reset();
}

void DeviceManDX12::Create()
{
	Destroy();
#ifndef NDEBUG
	ComPtr<ID3D12Debug> debug;
	if (D3D12GetDebugInterface(IID_PPV_ARGS(&debug)) == S_OK)
	{
		debug->EnableDebugLayer();
	}
#endif
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	if (hr != S_OK) {
		return;
	}
	ComPtr<IDXGIAdapter1> adapter;
	for (int i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &adapter); i++) {
		hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
		if (hr == S_OK) {
			return;
		}
	}
}
