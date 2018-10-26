#include <RasterScene.h>

#include <Application.h>
#include <CommandQueue.h>
#include <Helpers.h>
#include <Window.h>
#include <Geometry.h>
#include <UploadBuffer.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3dx12.h>
#include <d3dcompiler.h>

#include <algorithm> // For std::min and std::max.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif
#define PI 3.14159265

// Define To swap rendered geometry to Cube for easier debugging
//#define CUBE

using namespace DirectX;

// Clamp a value between a min and max range.
template<typename T>
constexpr const T& clamp(const T& val, const T& min, const T& max)
{
	return val < min ? min : val > max ? max : val;
}

struct RootParameters {
	XMMATRIX MVP;
	XMMATRIX ModelView;
	XMMATRIX NormalMat;
};

struct LightParams {
	float LightPosView[4];
	float SpecularColor[4];
	float AmbientColor[4];
	float SpecularPower;
	int DiffuseShadingMode;
};

float defaultFOV = 62;
bool dynamicLighting = false;

// Define Object to be drawn
#if defined(CUBE)
Cube* environment;
#else
SceneEnvironment* scene;
Sphere* sphere1;
Sphere* sphere2;
Sphere* sphere3;
CombinedGeometry* environment;
#endif

// Define Light Source
XMFLOAT4 lightSource;

RasterScene::RasterScene(const std::wstring& name, int width, int height, bool vSync)
	: super(name, width, height, vSync)
	, m_ScissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX))
	, m_Viewport(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)))
	, m_FoV(45.0)
	, m_ContentLoaded(false)
{
}

void RasterScene::UpdateBufferResource(
	ComPtr<ID3D12GraphicsCommandList2> commandList,
	ID3D12Resource** pDestinationResource,
	ID3D12Resource** pIntermediateResource,
	size_t numElements, size_t elementSize, const void* bufferData,
	D3D12_RESOURCE_FLAGS flags)
{
	auto device = Application::Get().GetDevice();

	size_t bufferSize = numElements * elementSize;

	char buffer[256];
	sprintf_s(buffer, "******** numElements: %d\n", numElements);
	OutputDebugStringA(buffer);
	sprintf_s(buffer, "******** elementSize: %d\n", elementSize);
	OutputDebugStringA(buffer);
	sprintf_s(buffer, "******** bufferSize: %d\n", bufferSize);
	OutputDebugStringA(buffer);

	// Create a committed resource for the GPU resource in a default heap.
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(pDestinationResource)));

	// Create an committed resource for the upload.
	if (bufferData)
	{
		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(pIntermediateResource)));

		D3D12_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pData = bufferData;
		subresourceData.RowPitch = bufferSize;
		subresourceData.SlicePitch = subresourceData.RowPitch;

		UpdateSubresources(commandList.Get(),
			*pDestinationResource, *pIntermediateResource,
			0, 0, 1, &subresourceData);
	}
}


bool RasterScene::LoadContent()
{
	auto device = Application::Get().GetDevice();
	auto commandQueue = Application::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	auto commandList = commandQueue->GetCommandList();
	m_UploadBuffer = std::make_unique<UploadBuffer>();

	//Create a Cube
#if defined(CUBE)
	environment = new Cube(XMFLOAT3(0.0, 0.0, 0.0), XMFLOAT3(1.0, 1.0, 1.0));
#else
	//Individual Objects
	scene = new SceneEnvironment(XMFLOAT3(0.0, 0.0, 0.0), XMFLOAT3(1.0, 1.0, 1.0));
	sphere1 = new Sphere(XMFLOAT3(0.0, 0.0, -16.0), 4, XMFLOAT4(1.0, 1.0, 1.0, 1.0));
	sphere2 = new Sphere(XMFLOAT3(-3.0, -1.0, -14.0), 2, XMFLOAT4(1.0, 1.0, 1.0, 1.0));
	sphere3 = new Sphere(XMFLOAT3(3.0, -1.0, -14.0), 2, XMFLOAT4(1.0, 0.0, 0.0, 1.0));

	//Comnbined Object
	environment = new CombinedGeometry();
	environment->AddGeometry(scene);
	environment->AddGeometry(sphere1);
	environment->AddGeometry(sphere2);
	environment->AddGeometry(sphere3);
#endif

	m_FoV = defaultFOV;

	// Upload vertex buffer data.
	ComPtr<ID3D12Resource> intermediateVertexBuffer;
	UpdateBufferResource(commandList,
		&m_VertexBuffer, &intermediateVertexBuffer,
		environment->vertexCount, sizeof(Geometry::VertexPosColorNorm), environment->GetVertices());

	// Create the vertex buffer view.
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.SizeInBytes = sizeof(Geometry::VertexPosColorNorm) * environment->vertexCount;
	m_VertexBufferView.StrideInBytes = sizeof(Geometry::VertexPosColorNorm);

	// Upload index buffer data.
	ComPtr<ID3D12Resource> intermediateIndexBuffer;
	UpdateBufferResource(commandList,
		&m_IndexBuffer, &intermediateIndexBuffer,
		environment->indexCount, sizeof(WORD), environment->GetIndicies());

	// Create index buffer view.
	m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_IndexBufferView.SizeInBytes = sizeof(WORD) * environment->indexCount;

	// Create the descriptor heap for the depth-stencil view.
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));

	// Load the vertex shader.
	ComPtr<ID3DBlob> vertexShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(L"VertexShader3.cso", &vertexShaderBlob));

	// Load the pixel shader.
	ComPtr<ID3DBlob> pixelShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(L"PixelShader3.cso", &pixelShaderBlob));

	// Create the vertex input layout
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// Create a root signature.
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	// Allow input layout and deny unnecessary access to certain pipeline stages.
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	// A single 32-bit constant root parameter that is used by the vertex shader.
	CD3DX12_ROOT_PARAMETER1 rootParameters[2];
	// 0 - Matrices
	rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
	// 1 - Lighting
	rootParameters[1].InitAsConstantBufferView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

	// Serialize the root signature.
	ComPtr<ID3DBlob> rootSignatureBlob;
	ComPtr<ID3DBlob> errorBlob;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
		featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
	// Create the root signature.
	ThrowIfFailed(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));

	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	} pipelineStateStream;

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	pipelineStateStream.pRootSignature = m_RootSignature.Get();
	pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.RTVFormats = rtvFormats;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
	sizeof(PipelineStateStream), &pipelineStateStream
	};
	ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));

	auto fenceValue = commandQueue->ExecuteCommandList(commandList);
	commandQueue->WaitForFenceValue(fenceValue);

	// Initialize Camera Position/Location
#if defined(CUBE)
	m_eyePosition = XMVectorSet(0, 0, -10, 1);
	m_focusPoint = XMVectorSet(0, 0, 0, 1);
	m_upDirection = XMVectorSet(0, 1, 0, 0);
	m_camRotX = 0;
#else
	m_eyePosition = XMVectorSet(0, 0, 0, 1);
	m_focusPoint = XMVectorSet(0, 0, -1, 1);
	m_upDirection = XMVectorSet(0, 1, 0, 0);
	m_camRotX = 180;
#endif

	// Initialize Light Position
	lightSource = XMFLOAT4(10, 10, -10, 1);

	m_ContentLoaded = true;

	// Resize/Create the depth buffer.
	ResizeDepthBuffer(GetClientWidth(), GetClientHeight());

	return true;
}

void RasterScene::ResizeDepthBuffer(int width, int height)
{
	if (m_ContentLoaded)
	{
		// Flush any GPU commands that might be referencing the depth buffer.
		Application::Get().Flush();

		width = std::max(1, width);
		height = std::max(1, height);

		auto device = Application::Get().GetDevice();

		// Resize screen dependent resources.
		// Create a depth buffer.
		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height,
				1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(&m_DepthBuffer)
		));

		// Update the depth-stencil view.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
		dsv.Format = DXGI_FORMAT_D32_FLOAT;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Texture2D.MipSlice = 0;
		dsv.Flags = D3D12_DSV_FLAG_NONE;

		device->CreateDepthStencilView(m_DepthBuffer.Get(), &dsv,
			m_DSVHeap->GetCPUDescriptorHandleForHeapStart());
	}
}

void RasterScene::OnResize(ResizeEventArgs& e)
{
	if (e.Width != GetClientWidth() || e.Height != GetClientHeight())
	{
		super::OnResize(e);

		m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f,
			static_cast<float>(e.Width), static_cast<float>(e.Height));

		ResizeDepthBuffer(e.Width, e.Height);
	}
}

void RasterScene::UnloadContent()
{
	m_ContentLoaded = false;
}

void RasterScene::OnUpdate(UpdateEventArgs& e)
{
	static uint64_t frameCount = 0;
	static double totalTime = 0.0;

	super::OnUpdate(e);

	totalTime += e.ElapsedTime;
	frameCount++;

	if (totalTime > 1.0)
	{
		double fps = frameCount / totalTime;

		char buffer[512];
		sprintf_s(buffer, "FPS: %f\n", fps);
		OutputDebugStringA(buffer);

		frameCount = 0;
		totalTime = 0.0;
	}

	// Update Keyboard Input
	UpdateKeyPresses();

	// Update the model matrix.
	m_ModelMatrix = XMMatrixIdentity();

	static float timeCounter = 0;
	timeCounter += e.ElapsedTime;
	lightSource.x = 20 * cos(timeCounter);
	lightSource.z = 20 * sin(timeCounter);
	if (!dynamicLighting) {
		lightSource = XMFLOAT4(-3.0, 5.0, -15.0, 1.0);
	}

	// Update the view matrix.
	// HANDLE MOUSE & KEYBOARD INPUT

	float mouseSensitivity = 0.25;
	float moveSensitivity = 20;
	float boostMultiplier = 5;

	// Store Mouse Position from Prior Frames
	static float oldMouseX = 0;
	static float oldMouseY = 0;

	// Check Left Mouse Button Down
	if (m_mouseLClick) {
		m_camRotX += mouseSensitivity *(m_mouseX - oldMouseX);
		m_camRotX = constrainAngle(m_camRotX);
		m_camRotY += mouseSensitivity * (oldMouseY - m_mouseY);
		m_camRotY = clamp(m_camRotY, (float)-89, (float)89);
	}
	oldMouseX = m_mouseX;
	oldMouseY = m_mouseY;
	XMVECTOR camForward = XMVectorSet(sinf(XMConvertToRadians(m_camRotX)) * cosf(XMConvertToRadians(m_camRotY)), sinf(XMConvertToRadians(m_camRotY)), cosf(XMConvertToRadians(m_camRotX)) * cosf(XMConvertToRadians(m_camRotY)), 1);
	m_focusPoint = XMVectorAdd(m_eyePosition, camForward);
	XMVECTOR camRight = XMVector3Cross(m_upDirection, camForward);
	XMVECTOR camUp = XMVector3Cross(camForward, camRight);
	camUp = XMVector3Normalize(camUp);

	// Apply Movement
	// Forward/Backward Movement
	float moveAmount = (m_movement[0] - m_movement[1]) * moveSensitivity * e.ElapsedTime * (m_boost ? boostMultiplier : 1);
	m_eyePosition = XMVectorAdd(m_eyePosition, camForward * moveAmount);
	m_focusPoint = XMVectorAdd(m_focusPoint, camForward * moveAmount);

	// Strafing Movement
	moveAmount = (m_movement[2] - m_movement[3]) * moveSensitivity * e.ElapsedTime * (m_boost ? boostMultiplier : 1);
	m_eyePosition = XMVectorAdd(m_eyePosition, camRight * moveAmount);
	m_focusPoint = XMVectorAdd(m_focusPoint, camRight * moveAmount);

	// Vertical Movement
	moveAmount = (m_movement[4] - m_movement[5]) * moveSensitivity * e.ElapsedTime * (m_boost ? boostMultiplier : 1);
	m_eyePosition = XMVectorAdd(m_eyePosition, camUp * moveAmount);
	m_focusPoint = XMVectorAdd(m_focusPoint, camUp * moveAmount);

	// END MOUSE & KEYBOARD INPUT

	m_ViewMatrix = XMMatrixLookAtLH(m_eyePosition, m_focusPoint, m_upDirection);

	// Update the projection matrix.
	float aspectRatio = GetClientWidth() / static_cast<float>(GetClientHeight());
	m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_FoV), aspectRatio, 0.1f, 100.0f);
}

// Transition a resource
void RasterScene::TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
	Microsoft::WRL::ComPtr<ID3D12Resource> resource,
	D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(),
		beforeState, afterState);

	commandList->ResourceBarrier(1, &barrier);
}

// Clear a render target.
void RasterScene::ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
	D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor)
{
	commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void RasterScene::ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
	D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth)
{
	commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

void RasterScene::OnRender(RenderEventArgs& e)
{
	super::OnRender(e);

	auto commandQueue = Application::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	auto commandList = commandQueue->GetCommandList();

	UINT currentBackBufferIndex = m_pWindow->GetCurrentBackBufferIndex();
	auto backBuffer = m_pWindow->GetCurrentBackBuffer();
	auto rtv = m_pWindow->GetCurrentRenderTargetView();
	auto dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();

	// Clear the render targets.
	{
		TransitionResource(commandList, backBuffer,
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Set Clear Color to Black

		ClearRTV(commandList, rtv, clearColor);
		ClearDepth(commandList, dsv);
	}

	commandList->SetPipelineState(m_PipelineState.Get());
	commandList->SetGraphicsRootSignature(m_RootSignature.Get());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	commandList->IASetIndexBuffer(&m_IndexBufferView);

	commandList->RSSetViewports(1, &m_Viewport);
	commandList->RSSetScissorRects(1, &m_ScissorRect);

	commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

	// Update the MVP matrix
	XMMATRIX modelView = XMMatrixMultiply(m_ModelMatrix, m_ViewMatrix);
	// Calculate Normal Matrix
	XMMATRIX normalMat = XMMatrixTranspose(XMMatrixInverse(nullptr, modelView));
	XMMATRIX mvpMatrix = XMMatrixMultiply(modelView, m_ProjectionMatrix);

	//Multiply the Light Position by the View Matrix
	XMVECTOR lightPosition = XMVectorSet(lightSource.x, lightSource.y, lightSource.z, lightSource.w);
	lightPosition = XMVector4Transform(lightPosition, m_ViewMatrix);
	

	RootParameters rootParams = { mvpMatrix, modelView, normalMat };

	// Send Root Parameter 1 (Matrices) to the Command List
	// Constant buffers must be 256-byte aligned.
	auto heapAllocation = m_UploadBuffer->Allocate(sizeof(RootParameters), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	memcpy(heapAllocation.CPU, &rootParams, sizeof(RootParameters));

	commandList->SetGraphicsRootConstantBufferView(0, heapAllocation.GPU);

	LightParams lightParams;
	if (dynamicLighting) { //Dynamic Lighting
		lightParams = {
			{XMVectorGetX(lightPosition), XMVectorGetY(lightPosition), XMVectorGetZ(lightPosition), XMVectorGetW(lightPosition)}, // Light Position (View Coordinates)
			{0.5, 0.5, 0.5, 1},                                                                                                   // Specular Color
			{0.1, 0.08, 0.0, 1},                                                                                                  // Ambient Color
			10.0,                                                                                                                 // Specular Power
			0                                                                                                                     // True Diffuse (Allow darker diffuse shading)
		};
	}
	else { //Static Lighting
		lightParams = {
			{XMVectorGetX(lightPosition), XMVectorGetY(lightPosition), XMVectorGetZ(lightPosition), XMVectorGetW(lightPosition)}, // Light Position (View Coordinates)
			{0.0, 0.0, 0.0, 1},                                                                                                   // Specular Color
			{0.0, 0.0, 0.0, 1},                                                                                                   // Ambient Color
			10.0,                                                                                                                 // Specular Power
			1                                                                                                                     // Clamped Diffuse  >= 0.2
		};
	}

	// Send Root Parameter 2 (Light Information) to the Command List
	auto heapAllocation2 = m_UploadBuffer->Allocate(sizeof(LightParams), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	memcpy(heapAllocation2.CPU, &lightParams, sizeof(LightParams));

	commandList->SetGraphicsRootConstantBufferView(1, heapAllocation2.GPU);

	// Draw the Geometry!
	commandList->DrawIndexedInstanced(environment->indexCount, 1, 0, 0, 0);

	// Present rendered image to screen
	{
		TransitionResource(commandList, backBuffer,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		m_FenceValues[currentBackBufferIndex] = commandQueue->ExecuteCommandList(commandList);

		currentBackBufferIndex = m_pWindow->Present();

		commandQueue->WaitForFenceValue(m_FenceValues[currentBackBufferIndex]);
	}

	// Reset Upload Buffer When Command List is finished Executing on the Command Queue
	m_UploadBuffer->Reset();
}

void RasterScene::OnKeyPressed(KeyEventArgs& e)
{
	super::OnKeyPressed(e);

	switch (e.Key)
	{
	case KeyCode::Escape:
		Application::Get().Quit(0);
		break;
	case KeyCode::R:
#if defined(CUBE)
		m_eyePosition = XMVectorSet(0, 0, -10, 1);
		m_focusPoint = XMVectorSet(0, 0, 0, 1);
		m_upDirection = XMVectorSet(0, 1, 0, 0);
		m_camRotX = 0;
#else
		m_eyePosition = XMVectorSet(0, 0, 0, 1);
		m_focusPoint = XMVectorSet(0, 0, -10, 1);
		m_upDirection = XMVectorSet(0, 1, 0, 0);
		m_camRotX = 180;
#endif
		m_camRotY = 0;
		m_FoV = defaultFOV;
		break;
	case KeyCode::Enter:
		if (e.Alt)
		{
	case KeyCode::F11:
		m_pWindow->ToggleFullscreen();
		break;
		}
	case KeyCode::V:
		m_pWindow->ToggleVSync();
		break;
	case KeyCode::L:
		dynamicLighting = !dynamicLighting;
	}
}

void RasterScene::OnMouseWheel(MouseWheelEventArgs& e)
{
	m_FoV -= e.WheelDelta;
	m_FoV = clamp(m_FoV, 12.0f, 90.0f);

	char buffer[256];
	sprintf_s(buffer, "FoV: %f\n", m_FoV);
	OutputDebugStringA(buffer);
}

void RasterScene::OnMouseMoved(MouseMotionEventArgs& e)
{
	m_mouseX = e.X;
	m_mouseY = e.Y;
}

void RasterScene::UpdateKeyPresses()
{
	m_mouseLClick = (GetKeyState(KeyCode::LButton) & 0x80) != 0;
	m_mouseRClick = (GetKeyState(KeyCode::RButton) & 0x80) != 0;
	m_movement[0] = (GetKeyState(KeyCode::W) & 0x80) != 0;
	m_movement[1] = (GetKeyState(KeyCode::S) & 0x80) != 0;
	m_movement[2] = (GetKeyState(KeyCode::D) & 0x80) != 0;
	m_movement[3] = (GetKeyState(KeyCode::A) & 0x80) != 0;
	m_movement[4] = (GetKeyState(KeyCode::Space) & 0x80) != 0;
	m_movement[5] = (GetKeyState(KeyCode::ControlKey) & 0x80) != 0;
	m_boost = (GetKeyState(KeyCode::ShiftKey) & 0x80) != 0;
}