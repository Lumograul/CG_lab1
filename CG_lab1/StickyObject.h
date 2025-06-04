#pragma once
#include <DirectXTex.h>
#include <DDSTextureLoader.h>

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
    LightData lightData;

    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;
    ID3D11Buffer* lightBuffer;


    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;

    ID3D11ShaderResourceView* textureView = nullptr;
    ID3D11Resource* texture = nullptr;
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
    DirectionalLight dirLight = {
    DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f),
    DirectX::SimpleMath::Vector4(1.0f, 1.1f, 1.1f, 1.0f),
    DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 1.0f),
    DirectX::SimpleMath::Vector4(0.0f, -1.0f, 1.0f, 1.0f)
    };

    Material material = {
        DirectX::SimpleMath::Vector4(0.2f, 0.2f, 0.2f, 1.0f),
        DirectX::SimpleMath::Vector4(0.2f, 0.2f, 0.2f, 1.0f),
        DirectX::SimpleMath::Vector4(0.55f, 0.55f, 0.55f, 1.00f)
    };

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
    void Update(const DirectX::SimpleMath::Vector3& ballCenter, float ballRadius, const DirectX::SimpleMath::Vector3 rotationAxis, float angle, DirectX::SimpleMath::Vector3 camPos);
    void Attach(const DirectX::SimpleMath::Vector3& ballCenter, float ballRadius, float ballAngle, const DirectX::SimpleMath::Vector3 ballRotAxis);
    DirectX::BoundingBox GetBoundingBox() const;


    struct ShadowBuffData {
        DirectX::SimpleMath::Matrix transform;
        DirectX::SimpleMath::Matrix viewProjection;
        DirectX::SimpleMath::Vector4 color;
    };

    ShadowBuffData shadowBuffData;

    ID3D11Buffer* shadowBuff;
    ID3D11ShaderResourceView* shadowsResource;
    ID3D11SamplerState* shadowSampler;

    ID3D11RasterizerState* rastState_shadows;
    ID3D11VertexShader* vertexShader_shadows;
    ID3DBlob* vertexByteCode_shadows;

    ID3D11PixelShader* pixelShader_shadows;
    ID3DBlob* pixelByteCode_shadows;
    void CreateShadowShaders();
    void LightRender();
    void LightUpdate(DirectX::SimpleMath::Matrix lightViewProjection);
};