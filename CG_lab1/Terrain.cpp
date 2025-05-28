#include "Terrain.h"
#include <d3dcompiler.h>
#include <algorithm>


Terrain::Terrain(ID3D11Device* device, ID3D11DeviceContext* context, float width, float depth, int gridSize)
    : device(device), context(context), width(width), depth(depth), gridSize(gridSize)
{
    Initialize();
}

void Terrain::Initialize() {
    vertices.clear();
    indices.clear();
    const float scaleY = 0.4f;
    const float noiseScale = 0.5f;
    const float minHeight = -0.8f; // Минимальная возможная высота
    const float maxHeight = 0.8f;  // Максимальная возможная высота

    // Цвета для градиента
    const XMFLOAT4 darkColor = { 0.2f, 0.2f, 0.2f, 1.0f };
    const XMFLOAT4 lightColor = { 0.8f, 0.8f, 0.8f, 1.0f };

    for (int z = 0; z <= gridSize; ++z) {
        for (int x = 0; x <= gridSize; ++x) {
            float xPos = (x / (float)gridSize) * width - width / 2.0f;
            float zPos = (z / (float)gridSize) * depth - depth / 2.0f;
            float yPos = scaleY * (sinf(xPos * noiseScale) + cosf(zPos * noiseScale));

            // Нормализуем высоту в диапазон [0, 1]
            float t = (yPos - minHeight) / (maxHeight - minHeight);
            t = fmaxf(fminf(t, 1.0f), 0.0f); // Ограничиваем диапазон

            // Интерполяция цвета
            XMFLOAT4 color;
            color.x = darkColor.x + t * (lightColor.x - darkColor.x);
            color.y = darkColor.y + t * (lightColor.y - darkColor.y);
            color.z = darkColor.z + t * (lightColor.z - darkColor.z);
            color.w = 1.0f;

            vertices.push_back({
                XMFLOAT3(xPos, yPos, zPos),
                color,
                { 0.0f, 0.0f }
                });
        }
    }

    // Генерация индексов
    for (int z = 0; z < gridSize; ++z) {
        for (int x = 0; x < gridSize; ++x) {
            int topLeft = z * (gridSize + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (gridSize + 1) + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    CreateShaders();
    CreateBuffers();
}

void Terrain::CreateShaders() {
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    // Компиляция вершинного шейдера
    D3DCompileFromFile(
        L"./Shaders/PlanetShader.hlsl",
        nullptr,
        nullptr,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &vsBlob,
        &errorBlob
    );

    device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        &vertexShader
    );

    // Компиляция пиксельного шейдера
    D3DCompileFromFile(
        L"./Shaders/PlanetShader.hlsl",
        nullptr,
        nullptr,
        "PSMain",
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &psBlob,
        &errorBlob
    );

    device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        &pixelShader
    );

    // Создание input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 } 
    };

    device->CreateInputLayout(
        layout,
        2,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &inputLayout
    );

    vsBlob->Release();
    psBlob->Release();
}

void Terrain::CreateBuffers() {
    // Вершинный буфер
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(Vertex) * vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vinitData = { vertices.data() };
    device->CreateBuffer(&vbd, &vinitData, &vertexBuffer);

    // Индексный буфер
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(UINT) * indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA iinitData = { indices.data() };
    device->CreateBuffer(&ibd, &iinitData, &indexBuffer);

    // Константный буфер для трансформаций
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.ByteWidth = sizeof(XMMATRIX);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device->CreateBuffer(&cbd, nullptr, &transformBuffer);
}

void Terrain::Render() {
    // Настройка константного буфера
    XMMATRIX world = XMMatrixIdentity();
    world = XMMatrixTranspose(world);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(transformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &world, sizeof(XMMATRIX));
    context->Unmap(transformBuffer, 0);

    // Настройка рендер-конвейера
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(inputLayout);

    context->VSSetShader(vertexShader, nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &transformBuffer);
    struct TextureFlags { int usTexture = 0; float padding[3]; } flags;
    ID3D11Buffer* flagBuffer = CreateFlagBuffer(device, &flags, sizeof(flags));
    context->PSSetConstantBuffers(2, 1, &flagBuffer);
    context->PSSetShader(pixelShader, nullptr, 0);

    context->DrawIndexed(indices.size(), 0, 0);
}

float Terrain::GetHeightAt(float x, float z) const {
    // Преобразуем мировые координаты в локальные координаты сетки
    float localX = x + width / 2.0f;
    float localZ = z + depth / 2.0f;

    // Вычисляем шаг сетки
    float stepX = width / gridSize;
    float stepZ = depth / gridSize;

    // Находим индексы ближайших вершин
    int x0 = static_cast<int>(localX / stepX);
    int z0 = static_cast<int>(localZ / stepZ);
    int x1 = x0 + 1;
    int z1 = z0 + 1;

    // Ограничиваем индексы вручную
    x0 = (x0 < 0) ? 0 : (x0 >= gridSize) ? gridSize - 1 : x0;
    x1 = (x1 < 0) ? 0 : (x1 > gridSize) ? gridSize : x1;
    z0 = (z0 < 0) ? 0 : (z0 >= gridSize) ? gridSize - 1 : z0;
    z1 = (z1 < 0) ? 0 : (z1 > gridSize) ? gridSize : z1;

    // Получаем высоты 4-х ближайших вершин
    float h00 = vertices[z0 * (gridSize + 1) + x0].pos.y;
    float h10 = vertices[z0 * (gridSize + 1) + x1].pos.y;
    float h01 = vertices[z1 * (gridSize + 1) + x0].pos.y;
    float h11 = vertices[z1 * (gridSize + 1) + x1].pos.y;

    // Коэффициенты интерполяции
    float tx = (localX - x0 * stepX) / stepX;
    float tz = (localZ - z0 * stepZ) / stepZ;

    // Билинейная интерполяция
    float h0 = h00 * (1 - tx) + h10 * tx;
    float h1 = h01 * (1 - tx) + h11 * tx;
    return h0 * (1 - tz) + h1 * tz;
}