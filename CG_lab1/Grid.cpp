#include "Grid.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>

using namespace DirectX;

Grid::Grid(ID3D11Device* device, ID3D11DeviceContext* context, float gridSize, int gridLines)
    : device(device), context(context), gridSize(gridSize), gridLines(gridLines), vertexBuffer(nullptr), indexBuffer(nullptr) {
}

Grid::~Grid() {
    if (vertexBuffer) vertexBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
    if (vertexShader) vertexShader->Release();
    if (pixelShader) pixelShader->Release();
    if (inputLayout) inputLayout->Release();
}

void Grid::Initialize() {
    // Сначала создаем вершины сетки
    float step = gridSize / (float)gridLines;
    for (int i = 0; i <= gridLines; ++i) {
        float position = -gridSize / 2 + i * step;
        vertices.push_back({ position, 0.0f, -gridSize / 2, 1.0f });  // Линии по оси X
        vertices.push_back({ position, 0.0f, gridSize / 2, 1.0f });

        vertices.push_back({-gridSize / 2, 0.0f, position, 1.0f });  // Линии по оси Z
        vertices.push_back({ gridSize / 2, 0.0f, position, 1.0f });
    }

    // Теперь создадим индексы для этих линий
    for (int i = 0; i < vertices.size(); i += 2) {
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // Создание буфера для вершин
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.ByteWidth = sizeof(XMFLOAT4) * vertices.size();

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();

    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
    if (FAILED(hr)) {
        // Обработка ошибки создания буфера
        std::cout << "Failed to create vertex buffer!" << std::endl;
    }

    // Создание буфера для индексов
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.ByteWidth = sizeof(int) * indices.size();

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices.data();

    hr = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
    if (FAILED(hr)) {
        // Обработка ошибки создания буфера
        std::cout << "Failed to create index buffer!" << std::endl;
    }

    // Загрузка шейдеров (предположим, что у вас уже есть компиляция шейдеров)
    ID3DBlob* vertexBC = nullptr;
    ID3DBlob* errorVertexCode = nullptr;
    hr = D3DCompileFromFile(L"./Shaders/GridShader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexBC, &errorVertexCode);
    if (FAILED(hr)) {
        if (errorVertexCode) {
            char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
            std::cout << compileErrors << std::endl;
        }
    }

    ID3DBlob* pixelBC = nullptr;
    ID3DBlob* errorPixelCode = nullptr;
    hr = D3DCompileFromFile(L"./Shaders/GridShader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);
    if (FAILED(hr)) {
        if (errorPixelCode) {
            char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());
            std::cout << compileErrors << std::endl;
        }
    }

    // Создание шейдеров
    hr = device->CreateVertexShader(vertexBC->GetBufferPointer(), vertexBC->GetBufferSize(), nullptr, &vertexShader);
    hr = device->CreatePixelShader(pixelBC->GetBufferPointer(), pixelBC->GetBufferSize(), nullptr, &pixelShader);

    // Создание input layout
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    hr = device->CreateInputLayout(inputElements, ARRAYSIZE(inputElements), vertexBC->GetBufferPointer(), vertexBC->GetBufferSize(), &inputLayout);
}

void Grid::Render() {
    // Устанавливаем шейдеры
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    // Устанавливаем буфер вершин
    UINT strides[] = { sizeof(DirectX::XMFLOAT4) };
    UINT offsets[] = { 0 };
    context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);

    // Устанавливаем индексный буфер
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Устанавливаем топологию (линии)
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    // Рисуем сетку
    context->DrawIndexed(indices.size(), 0, 0);
}

void Grid::Update() {
    // Здесь можно обновить состояние сетки, если это необходимо
}
