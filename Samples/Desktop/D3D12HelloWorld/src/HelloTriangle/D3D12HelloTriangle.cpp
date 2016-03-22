//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "stdafx.h"
#include "D3D12HelloTriangle.h"

D3D12HelloTriangle::D3D12HelloTriangle(UINT width, UINT height, std::wstring name) :
	DXSample(width, height, name),
	m_viewport(),
	m_scissorRect()
{
	m_viewport.Width = static_cast<float>(width);
	m_viewport.Height = static_cast<float>(height);
	m_viewport.MaxDepth = 1.0f;

	m_scissorRect.right = static_cast<LONG>(width);
	m_scissorRect.bottom = static_cast<LONG>(height);
}

void D3D12HelloTriangle::OnInit()
{
	LoadPipeline();
	LoadAssets();
}

// Load the rendering pipeline dependencies.
void D3D12HelloTriangle::LoadPipeline()
{
	deviceMan.Create(Win32Application::GetHwnd());
	m_device = deviceMan.GetDevice();
}

// Load the sample assets.
void D3D12HelloTriangle::LoadAssets()
{
	GoMyDir();

	static InputElement inputElementDescs[] =
	{
		CInputElement("POSITION", SF_R32G32B32_FLOAT, 0),
		CInputElement("COLOR", SF_R32G32B32A32_FLOAT, 12),
	};

	m_rootSignature = afCreateRootSignature(0, nullptr, 0, nullptr);
	m_pipelineState = afCreatePSO("shaders", inputElementDescs, dimof(inputElementDescs), BM_NONE, DSM_DISABLE, CM_DISABLE, m_rootSignature);

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(deviceMan.GetCommandList()->Close());

	// Create the vertex buffer.
	{
		// Define the geometry for a triangle.
		Vertex triangleVertices[] =
		{
			{ { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		m_vertexBuffer = afCreateVertexBuffer(vertexBufferSize, triangleVertices);
	}

	deviceMan.WaitForPreviousFrame();
}

// Update frame-based values.
void D3D12HelloTriangle::OnUpdate()
{
}

// Render the scene.
void D3D12HelloTriangle::OnRender()
{
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList(deviceMan.GetCommandList());

	deviceMan.Present();
}

void D3D12HelloTriangle::OnDestroy()
{
	deviceMan.Destroy();
}

void D3D12HelloTriangle::PopulateCommandList(ComPtr<ID3D12GraphicsCommandList> list)
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(deviceMan.GetCommandAllocator()->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(list->Reset(deviceMan.GetCommandAllocator(), nullptr/*m_pipelineState.Get()*/));

	list->SetPipelineState(m_pipelineState.Get());
	list->SetGraphicsRootSignature(m_rootSignature.Get());

	// Set necessary state.
	list->RSSetViewports(1, &m_viewport);
	list->RSSetScissorRects(1, &m_scissorRect);

	// Indicate that the back buffer will be used as a render target.
	list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(deviceMan.GetRenderTarget().Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	deviceMan.SetRenderTarget();

	// Record commands.
	afSetVertexBuffer(list.Get(), m_vertexBuffer, sizeof(Vertex));
	afDraw(list.Get(), PT_TRIANGLELIST, 3);

	// Indicate that the back buffer will now be used to present.
	list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(deviceMan.GetRenderTarget().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(list->Close());
}
