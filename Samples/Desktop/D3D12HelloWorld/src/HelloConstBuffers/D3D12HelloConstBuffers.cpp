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
#include "D3D12HelloConstBuffers.h"

D3D12HelloConstBuffers::D3D12HelloConstBuffers(UINT width, UINT height, std::wstring name) :
	DXSample(width, height, name)
{
}

void D3D12HelloConstBuffers::OnInit()
{
	LoadPipeline();
	LoadAssets();
}

// Load the rendering pipeline dependencies.
void D3D12HelloConstBuffers::LoadPipeline()
{
	deviceMan.Create(Win32Application::GetHwnd());
}

// Load the sample assets.
void D3D12HelloConstBuffers::LoadAssets()
{
	static Descriptor descriptors[] = {
		CDescriptorCBV(0),
	};
	static InputElement inputElementDescs[] =
	{
		CInputElement("POSITION", SF_R32G32B32_FLOAT, 0),
		CInputElement("COLOR", SF_R32G32B32A32_FLOAT, 12),
	};
	m_rootSignature = afCreateRootSignature(_countof(descriptors), descriptors, 0, nullptr);
	m_pipelineState = afCreatePSO("shaders", inputElementDescs, _countof(inputElementDescs), BM_NONE, DSM_DISABLE, CM_DISABLE, m_rootSignature);

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
		m_vertexBuffer = afCreateVertexBuffer(vertexBufferSize, triangleVertices);
	}

	// Create the constant buffer.
	{
		m_constantBuffer = afCreateUBO(sizeof(ConstantBuffer));
		SRVID srvs[] = {
			m_constantBuffer,
		};
		m_cbvHeap = afCreateDescriptorHeap(_countof(srvs), srvs);

		// Initialize and map the constant buffers. We don't unmap this until the
		// app closes. Keeping things mapped for the lifetime of the resource is okay.
		ZeroMemory(&m_constantBufferData, sizeof(m_constantBufferData));
		afWriteBuffer(m_constantBuffer, &m_constantBufferData, sizeof(m_constantBufferData));
	}
}

// Update frame-based values.
void D3D12HelloConstBuffers::OnUpdate()
{
	const float translationSpeed = 0.005f;
	const float offsetBounds = 1.25f;

	m_constantBufferData.offset.x += translationSpeed;
	if (m_constantBufferData.offset.x > offsetBounds)
	{
		m_constantBufferData.offset.x = -offsetBounds;
	}
	afWriteBuffer(m_constantBuffer, &m_constantBufferData, sizeof(m_constantBufferData));
}

// Render the scene.
void D3D12HelloConstBuffers::OnRender()
{
	afSetPipeline(m_pipelineState, m_rootSignature);
	afSetDescriptorHeap(m_cbvHeap);
	afSetVertexBuffer(m_vertexBuffer, sizeof(Vertex));
	afDraw(PT_TRIANGLELIST, 3);

	deviceMan.Present();
}

void D3D12HelloConstBuffers::OnDestroy()
{
	deviceMan.Destroy();
}
