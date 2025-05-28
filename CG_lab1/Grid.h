#ifndef GRID_H
#define GRID_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

class Grid {
public:
    Grid(ID3D11Device* device, ID3D11DeviceContext* context, float gridSize, int gridLines);
    ~Grid();

    void Initialize();
    void Render();
    void Update();

private:
    ID3D11Device* device;
    ID3D11DeviceContext* context;

    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* constBuffer;

    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;

    std::vector<DirectX::XMFLOAT4> vertices;
    std::vector<int> indices;

    float gridSize;
    int gridLines;
};

#endif // GRID_H
