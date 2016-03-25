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
#include "D3D12HelloTexture.h"

D3D12HelloTexture::D3D12HelloTexture(UINT width, UINT height, std::wstring name) :
	DXSample(width, height, name)
{
}

void D3D12HelloTexture::OnInit()
{
	LoadPipeline();
	LoadAssets();
}

// Load the rendering pipeline dependencies.
void D3D12HelloTexture::LoadPipeline()
{
	deviceMan.Create(Win32Application::GetHwnd());
}

// Load the sample assets.
void D3D12HelloTexture::LoadAssets()
{
	Descriptor descriptors[] = {
		CDescriptorSRV(0),
	};
	Sampler samplers[] = {
		CSampler(0, SF_LINEAR, SW_REPEAT),
	};
	m_rootSignature = afCreateRootSignature(_countof(descriptors), descriptors, _countof(samplers), samplers);
	static InputElement inputElementDescs[] =
	{
		CInputElement("POSITION", SF_R32G32B32_FLOAT, 0),
		CInputElement("TEXCOORD", SF_R32G32_FLOAT, 12),
	};
	m_pipelineState = afCreatePSO("shaders", inputElementDescs, _countof(inputElementDescs), BM_NONE, DSM_DISABLE, CM_CCW, m_rootSignature);

	// Create the vertex buffer.
	{
		// Define the geometry for a triangle.
		Vertex triangleVertices[] =
		{
			{ { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 0.5f, 0.0f } },
			{ { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 1.0f, 1.0f } },
			{ { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f } }
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		m_vertexBuffer = afCreateVertexBuffer(vertexBufferSize, triangleVertices);
	}

	// Create the texture.
	{
		IVec2 size(TextureWidth, TextureHeight);
		std::vector<UINT8> texture = GenerateTextureData();
		m_texture = afCreateTexture2D(DXGI_FORMAT_R8G8B8A8_UNORM, size, &texture[0]);
	}

	SRVID srvs[] = {
		m_texture,
	};
	m_srvHeap = afCreateDescriptorHeap(_countof(srvs), srvs);
}

// Generate a simple black and white checkerboard texture.
std::vector<UINT8> D3D12HelloTexture::GenerateTextureData()
{
	const UINT rowPitch = TextureWidth * TexturePixelSize;
	const UINT cellPitch = rowPitch >> 3;		// The width of a cell in the checkboard texture.
	const UINT cellHeight = TextureWidth >> 3;	// The height of a cell in the checkerboard texture.
	const UINT textureSize = rowPitch * TextureHeight;

	std::vector<UINT8> data(textureSize);
	UINT8* pData = &data[0];

	for (UINT n = 0; n < textureSize; n += TexturePixelSize)
	{
		UINT x = n % rowPitch;
		UINT y = n / rowPitch;
		UINT i = x / cellPitch;
		UINT j = y / cellHeight;

		if (i % 2 == j % 2)
		{
			pData[n] = 0x00;		// R
			pData[n + 1] = 0x00;	// G
			pData[n + 2] = 0x00;	// B
			pData[n + 3] = 0xff;	// A
		}
		else
		{
			pData[n] = 0xff;		// R
			pData[n + 1] = 0xff;	// G
			pData[n + 2] = 0xff;	// B
			pData[n + 3] = 0xff;	// A
		}
	}

	return data;
}

// Update frame-based values.
void D3D12HelloTexture::OnUpdate()
{
}

// Render the scene.
void D3D12HelloTexture::OnRender()
{
	afSetPipeline(m_pipelineState, m_rootSignature);
	afSetHeap(m_srvHeap);
	afSetVertexBuffer(m_vertexBuffer, sizeof(Vertex));
	afDraw(PT_TRIANGLELIST, 3);

	deviceMan.Present();
}

void D3D12HelloTexture::OnDestroy()
{
	deviceMan.Destroy();
}
