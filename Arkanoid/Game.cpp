//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
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

	// Init gameobjects position respect windows size
	player = new Paddle(width, height);
	ball = new Ball(Vector2(width, height));

	int heigthSpace = 0;
	for (int i = 0; i < BRICKSROW; i++) {
		int widthSpace = 0;
		for (int j = 0; j < BRICKCOLUMN; j++) {
			bricks.push_back(new Brick(Vector2(widthSpace+BRICKOFFSETWIDTH,heigthSpace+BRICKOFFSETHEIGHT)));
			widthSpace += BRICKOFFSETWIDTH;
		}
		heigthSpace += BRICKOFFSETHEIGHT;
	}

	// Init input handler
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
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;

	InputHandler();

	if (!ball->IsAttached()) {
		ball->UpdatePosition(player->GetPosition().x);
	}

	CollisionCheck();
	CheckGameOver();
	CheckWin();
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

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied());

	// Render paddle
	m_spriteBatch->Draw(texturePaddle.Get(), player->GetPosition(), nullptr,
		Colors::White, 0.f, m_origin, 0.5f);

	// Render ball
	if (ball->IsAttached()) {
		ball->SetPosition(player->GetPosition());
	}
	m_spriteBatch->Draw(textureBall.Get(), ball->GetPosition(), nullptr,
		Colors::White, 0.f, m_origin, 0.3f);

	// Render Bricks
	int countColor = 0;
	for (Brick* b : bricks)
	{
		if (!b->IsDestroyed()) {
			if (countColor % 2 == 0) {
				m_spriteBatch->Draw(textureBrick.Get(), b->GetPosition(), nullptr,
					Colors::Red, 0.f, m_origin, 0.3f);
			}
			else {
				m_spriteBatch->Draw(textureBrick.Get(), b->GetPosition(), nullptr,
					Colors::Blue, 0.f, m_origin, 0.3f);
			}
		}
		countColor++;
	}

	m_spriteBatch->End();

    context;

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::AliceBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
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
	ID3D11Device1* device = m_deviceResources->GetD3DDevice();
	
    // TODO: Initialize device dependent objects here (independent of window size).
	auto context = m_deviceResources->GetD3DDeviceContext();
	m_spriteBatch = std::make_unique<SpriteBatch>(context);

	LoadBrickTexture(device);
	LoadPaddleTexture(device); 
	LoadBallTexture(device);

	device;

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
	auto size = m_deviceResources->GetOutputSize();
	m_screenPos.x = float(size.right) / 2.f;
	m_screenPos.y = float(size.bottom) / 2.f;
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
	texturePaddle.Reset();

	m_spriteBatch.reset();
	m_states.reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}

#pragma endregion

#pragma region
void Game::InputHandler()
{
	auto kb = m_keyboard->GetState();
	if (kb.Escape)
	{
		ExitGame();
	}
	if (kb.Space) {
		ball->DetachBall();
	}

	if (kb.Right) {
		player->MovePaddle(true);
		ball->SetDirection(1);
	}
	else if (kb.Left) {
		player->MovePaddle(false);
		ball->SetDirection(-1);
	}
	else {
		ball->SetDirection(0);
	}
	
	Keyboard::ProcessMessage(0, 0, 0);
}

void Game::LoadPaddleTexture(ID3D11Device1* device)
{
	ComPtr<ID3D11Resource> resource;
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(device, L"Assets/Paddle.png",
			resource.GetAddressOf(),
			texturePaddle.ReleaseAndGetAddressOf()));

	m_states = std::make_unique<CommonStates>(device);
	ComPtr<ID3D11Texture2D> paddle;
	DX::ThrowIfFailed(resource.As(&paddle));

	CD3D11_TEXTURE2D_DESC paddledesc;
	paddle->GetDesc(&paddledesc);

	m_origin.x = float(paddledesc.Width / 2);
	m_origin.y = float(paddledesc.Height / 2);
}

void Game::LoadBallTexture(ID3D11Device1* device)
{
	ComPtr<ID3D11Resource> resource;
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(device, L"Assets/Ball.png",
			resource.GetAddressOf(),
			textureBall.ReleaseAndGetAddressOf()));

	m_states = std::make_unique<CommonStates>(device);
	ComPtr<ID3D11Texture2D> ball;
	DX::ThrowIfFailed(resource.As(&ball));

	CD3D11_TEXTURE2D_DESC balldesc;
	ball->GetDesc(&balldesc);

	m_origin.x = float(balldesc.Width / 2);
	m_origin.y = float(balldesc.Height / 2);
}

void Game::LoadBrickTexture(ID3D11Device1* device)
{
	ComPtr<ID3D11Resource> resource;
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(device, L"Assets/Brick.png",
			resource.GetAddressOf(),
			textureBrick.ReleaseAndGetAddressOf()));

	m_states = std::make_unique<CommonStates>(device);
	ComPtr<ID3D11Texture2D> brick;
	DX::ThrowIfFailed(resource.As(&brick));

	CD3D11_TEXTURE2D_DESC brickdesc;
	brick->GetDesc(&brickdesc);

	m_origin.x = float(brickdesc.Width / 2);
	m_origin.y = float(brickdesc.Height / 2);
}

void Game::CollisionCheck()
{
	Vector2 ballTempPos = ball->GetPosition();
	for (Brick* b : bricks)
	{
		Vector2 brickTempPos = b->GetPosition();
		if (!b->IsDestroyed() &&
				(brickTempPos.y+BRICKSPRITEHEIGHTOFFSET > ballTempPos.y && brickTempPos.y-BRICKSPRITEHEIGHTOFFSET/2 < ballTempPos.y &&
					brickTempPos.x+BRICKSPRITEWIDTHOFFSET > ballTempPos.x && brickTempPos.x-BRICKSPRITEWIDTHOFFSET/4 < ballTempPos.x)
			) {
			b->Destroy();
			bricksAlive--;

			if (ballTempPos.y>brickTempPos.y && ballTempPos.y < brickTempPos.y+BRICKSPRITEHEIGHTOFFSET-2) {
				ball->ChangeHorizontalDir();
			}
			else {
				ball->ChangeVerticalDir();
			}
		}
	}
}

void Game::CheckGameOver()
{
	if (ball->IsUnderPaddle()) {
		Restart();
	}
}

void Game::CheckWin()
{
	if (bricksAlive <= 0) {
		Restart();
	}
}

void Game::Restart()
{
	// Set initial player and ball position
	player->SetPosition(Vector2(player->GetScreenSize().x/2 - (SPRITEWIDTH / 1.5f), player->GetScreenSize().y-20));
	ball->SetPosition(player->GetPosition());
	ball->Attach();

	// Reset number of bricks and respawn them
	bricksAlive = BRICKSROW * BRICKCOLUMN;

	for (Brick* b : bricks)
	{
		b->Respawn();
	}
}

#pragma endregion