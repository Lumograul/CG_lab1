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
#include "StickyObject.h"

struct VertexData {
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT2 texCoord;
};

inline ID3D11Buffer* CreateFlagBuffer(ID3D11Device* device, const void* data, size_t size) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = static_cast<UINT>(size);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = { data };
    ID3D11Buffer* buffer;
    device->CreateBuffer(&desc, &initData, &buffer);
    return buffer;
}

class NewComponent {
private:
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    std::vector<int> indeces;

public:
    float angle = 0;

    std::vector<VertexData> points; 
    ID3D11InputLayout* layout;
    ID3D11Buffer* vb;
    ID3D11Buffer* ib;
    ID3D11Buffer* cb;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    DirectX::SimpleMath::Matrix transformationMatrix;
    DirectX::SimpleMath::Vector3 scale{ 1.0f, 1.0f, 1.0f };
    DirectX::SimpleMath::Vector3 rotationAxis{ 0.0f, 1.0f, 0.0f };
    DirectX::SimpleMath::Vector3 transl{ 0.0f, 1.0f, 0.0f };
    DirectX::SimpleMath::Vector3 centerCoords;
    float spinVelocity;
    InputDevice* inpDevice;
    //std::vector<StickyObject> stickers;
    bool PPressedLastFrame{ false };

    NewComponent(
        ID3D11Device* dev,
        ID3D11DeviceContext* ctx,
        InputDevice* inpDevice,
        DirectX::SimpleMath::Vector3 rotationAxis,
        DirectX::SimpleMath::Vector3 centerCoords,
        float spinVelocity)
        :
        device(dev),
        context(ctx),
        vb(nullptr),
        ib(nullptr),
        cb(nullptr),
        vertexShader(nullptr),
        pixelShader(nullptr),
        layout(nullptr),
        inpDevice(inpDevice),
        rotationAxis(rotationAxis),
        centerCoords(centerCoords),
        spinVelocity(spinVelocity)
    {
    }
    void Initialize() ;
    void Draw() ;
    void Update(DirectX::SimpleMath::Vector3 cameraForward, DirectX::SimpleMath::Vector3 cameraPosition, float surfaceHeight) ;
    void DestroyResources() ;

    DirectX::BoundingSphere GetBoundingSphere() const;
};
