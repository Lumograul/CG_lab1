#pragma once
#include <iostream>
#include <vector>
#include <d3d.h>
#include <d3d11.h>
#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>
#include "RectangleComponent.h"
#include "DisplayWin32.h"
#include "NewComponent.h"
#include "Camera.h"
#include <random>
#include "Grid.h"
#include "CentralComponent.h"
#include "StickyObject.h"
#include "Terrain.h"

class Game {
private:
    int width;
    int height;
    DisplayWin32* display;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    ID3D11DeviceContext* context;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* rtv;
    ID3D11DepthStencilView* dsv;
    bool isExitRequested;
    ID3D11RasterizerState* rastState;
    ID3D11RasterizerState* rastStateSticky;
    InputDevice* inpDevice;
    int scoreLeft;
    int scoreRight;
    Camera* camera;
    DirectX::SimpleMath::Vector2 MousePosition{ 0,0 };
    DirectX::SimpleMath::Vector2 MouseOffset{ 0,0 };
    bool tabPressedLastFrame{ false };
    Grid* grid;
    Terrain* terrain;
    std::vector<StickyObject*> stickers;
public:
    NewComponent* ball;

    Game() :
        width(800),
        height(800),
        display(nullptr),
        context(nullptr),
        swapChain(nullptr),
        rtv(nullptr),
        isExitRequested(false),
        rastState(nullptr),
        scoreLeft(0),
        scoreRight(0),
        camera(nullptr),
        grid(nullptr)
    {
    }
    void Initialize();
    void Run();
    void DestroyResources();
    std::vector<DirectX::XMFLOAT4> GenerateCirclePoints(float cx, float cy, float r);
    std::vector<int> GenerateCircleIndeces();
    DirectX::SimpleMath::Vector2 GenerateRandomBallDirection();

};