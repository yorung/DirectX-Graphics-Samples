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
	DXSample(width, height, name)
{
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

	// Create the vertex buffer.
	{
		// Define the geometry for a triangle.
		Vertex triangleVertices[] =
		{
			{ { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 1.f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -1.f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		m_vertexBuffer = afCreateVertexBuffer(vertexBufferSize, triangleVertices);
	}
}

// Update frame-based values.
void D3D12HelloTriangle::OnUpdate()
{
}

// Render the scene.
void D3D12HelloTriangle::OnRender()
{
	afSetPipeline(m_pipelineState, m_rootSignature);
	afSetVertexBuffer(m_vertexBuffer, sizeof(Vertex));
	afDraw(PT_TRIANGLELIST, 3);
	deviceMan.Present();
}

void D3D12HelloTriangle::OnDestroy()
{
	deviceMan.Destroy();
}
