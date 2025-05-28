#pragma once
#include <DirectXTex.h>
#include <WICTextureLoader.h>

#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include "NewComponent.h"
#include "InputDevice.h"
#include "ModelLoader.h"


enum class Mode {
    Starting,
    Attached,
};


class StickyObject {
private:
    ID3D11Device* device;
    ID3D11DeviceContext* context;

    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;


    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;

    ID3D11ShaderResourceView* textureView = nullptr;
    ID3D11SamplerState* samplerState = nullptr;
    bool useTexture = false;


    ID3D11Buffer* transformBuffer = nullptr;

    DirectX::SimpleMath::Matrix worldMatrix;

    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

    DirectX::SimpleMath::Vector3 relativeOffset = {}; // смещение от центра шара
    DirectX::SimpleMath::Vector3 coords = startCoords;
    DirectX::SimpleMath::Matrix translation = DirectX::SimpleMath::Matrix::CreateTranslation({ 0.0f, 0.0f, 0.0f });
    DirectX::SimpleMath::Matrix rotationA = DirectX::SimpleMath::Matrix::CreateFromAxisAngle({ 0.0f, 1.0f, 0.0f }, 0.0f);
    DirectX::SimpleMath::Matrix rotationB = DirectX::SimpleMath::Matrix::CreateFromAxisAngle({ 0.0f, 1.0f, 0.0f }, 0.0f);

public:
    DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale({ 1.0f, 1.0f, 1.0f });
    float startAttachmentBallAngle = 0;
    DirectX::SimpleMath::Vector3 startAttachmentRotationAxis = { 0.0f, 0.0f, 0.0f };
    Mode mode = Mode::Starting;
    const DirectX::SimpleMath::Vector3 startCoords;
    DirectX::SimpleMath::Vector3 startAttachmentCoords = { 0.0f, 0.0f, 0.0f };
    DirectX::SimpleMath::Vector3 attachedOffset = { 0.0f, 0.f, 0.0f };
    InputDevice* inpDevice;

    StickyObject(ID3D11Device* device, ID3D11DeviceContext* context, const DirectX::SimpleMath::Vector3 startCoords, InputDevice* inputDevice, const std::string& path = "", const std::string& texturePath = "", const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
    ~StickyObject();

    void Initialize(const std::string& path = "", const std::string& texturePath = "", const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
    void Draw();
    void Update(const DirectX::SimpleMath::Vector3& ballCenter, float ballRadius, const DirectX::SimpleMath::Vector3 rotationAxis, float angle);
    void Attach(const DirectX::SimpleMath::Vector3& ballCenter, float ballRadius, float ballAngle, const DirectX::SimpleMath::Vector3 ballRotAxis);
    DirectX::BoundingBox GetBoundingBox() const;
};