#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "NewComponent.h"

using namespace DirectX;

class Terrain {
private:
    struct Vertex {
        XMFLOAT3 pos;
        XMFLOAT4 col;
        XMFLOAT2 texCoord;
        XMFLOAT3 normal;
    };
public:
    float GetHeightAt(float x, float z) const;

    

    Terrain(ID3D11Device* device, ID3D11DeviceContext* context, float width, float depth, int gridSize);
    void Initialize();
    void Render(DirectX::SimpleMath::Vector3 cameraPos);

    ID3D11Device* device;
    ID3D11DeviceContext* context;
    LightData lightData;

    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* transformBuffer;
    ID3D11Buffer* lightBuffer;

    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;

    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

    DirectionalLight dirLight = {
        DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f),
        DirectX::SimpleMath::Vector4(1.0f, 1.1f, 1.1f, 1.0f),
        DirectX::SimpleMath::Vector4(0.1f, 0.1f, 0.1f, 1.0f),
        DirectX::SimpleMath::Vector4(0.0f, -1.0f, 1.0f, 1.0f)
    };

    Material material = {
        DirectX::SimpleMath::Vector4(0.2f, 0.2f, 0.2f, 1.0f),
        DirectX::SimpleMath::Vector4(0.2f, 0.2f, 0.2f, 1.0f),
        DirectX::SimpleMath::Vector4(0.55f, 0.55f, 0.55f, 1.00f)
    };

    float width;
    float depth;
    int gridSize;

    void CreateShaders();
    void CreateBuffers();


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