//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
    if (m_deviceResources)
    {
        m_deviceResources->WaitForGpu();
    }
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

	// Init paddle pos respect windows size
	player = Paddle(width, height);
	ball = Ball(player.GetPosition());
	m_keyboard = std::make_unique<Keyboard>();
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Update");

    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;

	InputHandler();

    PIXEndEvent();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	// Prepare the command list to render a new frame.
	m_deviceResources->Prepare();
	Clear();

	auto commandList = m_deviceResources->GetCommandList();
	PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Render");

	// TODO: Add your rendering code here.
	float time = float(m_timer.GetTotalSeconds());

	ID3D12DescriptorHeap* heaps[] = { m_resourceDescriptors->Heap() };
	commandList->SetDescriptorHeaps(static_cast<UINT>(std::size(heaps)), heaps);

	m_spriteBatch->Begin(commandList);

	// Render player
	m_spriteBatch->Draw(m_resourceDescriptors->GetGpuHandle(PaddleDescriptors::PaddleSprite),
		GetTextureSize(m_texture.Get()),
		player.GetPosition(), nullptr, Colors::White, 0.f, m_origin, 0.5f);
	
	// Render Ball
	/*m_spriteBatch->Draw(m_resourceDescriptors_ball->GetGpuHandle(BallDescriptors::BallSprite),
		GetTextureSize(m_texture.Get()),
		ball.GetPosition(), nullptr, Colors::White, 0.f, m_origin, 0.5f);*/
	m_spriteBatch->End();

	PIXEndEvent(commandList);

	// Show the new frame.
	PIXBeginEvent(m_deviceResources->GetCommandQueue(), PIX_COLOR_DEFAULT, L"Present");
	m_deviceResources->Present();
	PIXEndEvent(m_deviceResources->GetCommandQueue());
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    auto commandList = m_deviceResources->GetCommandList();
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Clear");

    // Clear the views.
    auto rtvDescriptor = m_deviceResources->GetRenderTargetView();
    auto dsvDescriptor = m_deviceResources->GetDepthStencilView();

    commandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
    commandList->ClearRenderTargetView(rtvDescriptor, Colors::CornflowerBlue, 0, nullptr);
    commandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Set the viewport and scissor rect.
    auto viewport = m_deviceResources->GetScreenViewport();
    auto scissorRect = m_deviceResources->GetScissorRect();
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    PIXEndEvent(commandList);
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // Check Shader Model 6 support
    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_0 };
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)))
        || (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0))
    {
#ifdef _DEBUG
        OutputDebugStringA("ERROR: Shader Model 6.0 is not supported!\n");
#endif
        throw std::runtime_error("Shader Model 6.0 is not supported!");
    }

    // If using the DirectX Tool Kit for DX12, uncomment this line:
    m_graphicsMemory = std::make_unique<GraphicsMemory>(device);

    // TODO: Initialize device dependent objects here (independent of window size).
	m_resourceDescriptors = std::make_unique<DescriptorHeap>(device, PaddleDescriptors::Count);
	m_resourceDescriptors_ball = std::make_unique<DescriptorHeap>(device, BallDescriptors::CountBall);

	// Load paddle sprite 
	ResourceUploadBatch resourceUpload(device);

	resourceUpload.Begin();

	DX::ThrowIfFailed(
		CreateWICTextureFromFile(device, resourceUpload, L"Assets/Paddle.png",
			m_texture.ReleaseAndGetAddressOf()));

	CreateShaderResourceView(device, m_texture.Get(),
		m_resourceDescriptors->GetCpuHandle(PaddleDescriptors::PaddleSprite));

	RenderTargetState rtState(m_deviceResources->GetBackBufferFormat(),
		m_deviceResources->GetDepthBufferFormat());

	SpriteBatchPipelineStateDescription pd(rtState, &CommonStates::NonPremultiplied);
	m_spriteBatch = std::make_unique<SpriteBatch>(device, resourceUpload, pd);

	XMUINT2 paddleSize = GetTextureSize(m_texture.Get());

	m_origin.x = float(paddleSize.x / 2);
	m_origin.y = float(paddleSize.y / 2);

	auto uploadResourcesFinished = resourceUpload.End(
		m_deviceResources->GetCommandQueue());

	uploadResourcesFinished.wait();

	// Load ball sprite 
	resourceUpload.Begin();

	DX::ThrowIfFailed(
		CreateWICTextureFromFile(device, resourceUpload, L"Assets/Ball.png",
			m_texture.ReleaseAndGetAddressOf()));

	CreateShaderResourceView(device, m_texture.Get(),
		m_resourceDescriptors_ball->GetCpuHandle(BallDescriptors::BallSprite));

	RenderTargetState rtStateB(m_deviceResources->GetBackBufferFormat(),
		m_deviceResources->GetDepthBufferFormat());

	SpriteBatchPipelineStateDescription pdb(rtStateB, &CommonStates::NonPremultiplied);
	m_spriteBatch = std::make_unique<SpriteBatch>(device, resourceUpload, pdb);

	XMUINT2 ballSize = GetTextureSize(m_texture.Get());

	m_origin.x = float(ballSize.x / 2);
	m_origin.y = float(ballSize.y / 2);
	
	auto uploadResourcesFinishedB = resourceUpload.End(
		m_deviceResources->GetCommandQueue());

	uploadResourcesFinished.wait();

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
	auto viewport = m_deviceResources->GetScreenViewport();
	m_spriteBatch->SetViewport(viewport);

	auto size = m_deviceResources->GetOutputSize();
	m_screenPos.x = float(size.right) / 2.f;
	m_screenPos.y = float(size.bottom) / 2.f;
}

void Game::InputHandler()
{
	auto kb = m_keyboard->GetState();
	if (kb.Escape)
	{
		ExitGame();
	}
	if (kb.Right) {
		player.MovePaddle(true);
	}
	if (kb.Left) {
		player.MovePaddle(false);
	}
	Keyboard::ProcessMessage(0, 0, 0);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    // If using the DirectX Tool Kit for DX12, uncomment this line:
    m_graphicsMemory.reset();

	m_texture.Reset();
	m_resourceDescriptors.reset();
	m_spriteBatch.reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
