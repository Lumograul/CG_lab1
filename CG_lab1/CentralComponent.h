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
#include "Camera.h"


class CentralComponent : public GameComponent {
private:
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    Camera* camera;
    InputDevice* inputDevice;
    float angle;
public:
    std::vector<DirectX::XMFLOAT4> points;
    std::vector<int> indeces;
    ID3D11InputLayout* layout;
    ID3D11Buffer* vb;
    ID3D11Buffer* ib;
    ID3D11Buffer* cb;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    DirectX::SimpleMath::Matrix transformationMatrix;
    DirectX::SimpleMath::Vector3 scale{ 1.f, 1.f, 1.f };
    DirectX::SimpleMath::Vector3 rotationAxis{ 1.0f, 0.0f, 0.0f };
    DirectX::SimpleMath::Vector3 transl;
    DirectX::SimpleMath::Vector3 centerCoords{ 0.0f, 1.0f, 0.0f };
    float spinVelocity;
    //float orbitRadius;
    CentralComponent(
        ID3D11Device* dev,
        ID3D11DeviceContext* ctx,
        //float orbitRadius,
        //DirectX::SimpleMath::Vector3 rotationAxis,
        //DirectX::SimpleMath::Vector3 centerCoords,
        //float spinVelocity,
        InputDevice* inputDevice,
        Camera* camera
        )
        :
        device(dev),
        context(ctx),
        GameComponent(dev, ctx),
        vb(nullptr),
        ib(nullptr),
        cb(nullptr),
        vertexShader(nullptr),
        pixelShader(nullptr),
        layout(nullptr),
        inputDevice(inputDevice),
        camera(camera)
        //rotationAxis(rotationAxis),
        //centerCoords(centerCoords),
        //spinVelocity(spinVelocity)
        //orbitRadius(orbitRadius)
    {
    }
    void Initialize() override;
    void Draw() override;
    void Update() override;
    void DestroyResources() override;
};
