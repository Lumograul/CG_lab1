#pragma once
#include <d3d.h>
#include <d3d11.h>
#include <vector>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "GameComponent.h"
#include <directxtk/SimpleMath.h>
#include <limits>
#include <algorithm>
#include<iostream>
#include "InputDevice.h"
#include <vector>
#include "NewComponent.h"

enum class CameraMode {
    Orbit,
    FPS,
};

class Camera {
private:
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    DirectX::SimpleMath::Matrix proj;
    DirectX::SimpleMath::Matrix look;
    bool tabPressedLastFrame{ false };

public:
    DirectX::SimpleMath::Matrix viewProjection;

    ID3D11InputLayout* layout;
   
    ID3D11Buffer* cb;
    CameraMode mode = CameraMode::Orbit;

    // FPS камера
    DirectX::SimpleMath::Vector3 position { 0.0f, 0.0f, 0.0f };

    float yaw{ 0.0f };
    float pitch{ 0.0f };
    // Орбитальная камера
    float radiusCamera{ 10.0f };
    float teta{ 0 };
    float phi{ 1.3f };
    float deltax{ 0 };
    float deltay{ 0 };
    InputDevice* inpDevice;
    DirectX::SimpleMath::Vector3 lookAt{ DirectX::SimpleMath::Vector3::Zero };
    DirectX::SimpleMath::Vector3 targetCenter{ DirectX::SimpleMath::Vector3::Zero };
    DirectX::SimpleMath::Matrix view;

    Camera(
        ID3D11Device* dev,
        ID3D11DeviceContext* ctx)
        :
        device(dev),
        context(ctx),
        cb(nullptr),
        layout(nullptr)
    {
    }

    void Initialize(InputDevice* inputDevice);
    void Draw();
    void Update();
    //void UpdateFromMouse(DirectX::SimpleMath::Vector2 offset);
    void DestroyResources();
    void GoToStandart();
    DirectX::SimpleMath::Vector3 GetForwardDirection() const {
        return targetCenter;
    }
    DirectX::SimpleMath::Vector3 GetCamPos() const {
        return position;
    }

    DirectX::SimpleMath::Matrix GetWorldView() const {
        auto viewProj = view * proj;
        viewProj.Transpose();
        return viewProj;
    }
};
    