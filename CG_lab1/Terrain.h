#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "NewComponent.h"

using namespace DirectX;

class Terrain {
public:
    float GetHeightAt(float x, float z) const;

    struct Vertex {
        XMFLOAT3 pos;
        XMFLOAT4 col;
        XMFLOAT2 texCoord;
    };

    Terrain(ID3D11Device* device, ID3D11DeviceContext* context, float width, float depth, int gridSize);
    void Initialize();
    void Render();

private:
    ID3D11Device* device;
    ID3D11DeviceContext* context;

    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* transformBuffer;

    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;

    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

    float width;
    float depth;
    int gridSize;

    void CreateShaders();
    void CreateBuffers();
};