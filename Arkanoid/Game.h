//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"

#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"

#define BRICKOFFSETWIDTH 93
#define BRICKOFFSETHEIGHT 60

#define BRICKSROW 4
#define BRICKCOLUMN 7

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

	Paddle player;

	Ball ball;

	vector<Brick> bricks;

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

	// Load Textures
	void LoadPaddleTexture(ID3D11Device1*);
	void LoadBallTexture(ID3D11Device1*);
	void LoadBrickTexture(ID3D11Device1*);

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

	// Handle the input
	void InputHandler();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texturePaddle;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureBall;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureBrick;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	DirectX::SimpleMath::Vector2 m_screenPos;
	DirectX::SimpleMath::Vector2 m_origin;

	std::unique_ptr<DirectX::Keyboard> m_keyboard;
};
