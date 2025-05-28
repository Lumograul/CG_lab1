#include <d3dcompiler.h>
#include <directxmath.h>
#include <iostream>

#include "RectangleComponent.h"


void RectangleComponent::Initialize() {
    ID3DBlob* vertexBC = nullptr;
    ID3DBlob* errorVertexCode = nullptr;

    auto res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
        nullptr /*macros*/,
        nullptr /*include*/,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &vertexBC,
        &errorVertexCode);

    if (FAILED(res)) {
        // If the shader failed to compile it should have written something to the error message.
        if (errorVertexCode) {
            char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

            std::cout << compileErrors << std::endl;
        }
    }

    D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

    ID3DBlob* pixelBC;
    ID3DBlob* errorPixelCode;
    res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl", Shader_Macros /*macros*/, nullptr /*include*/, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);

    device->CreateVertexShader(
        vertexBC->GetBufferPointer(),
        vertexBC->GetBufferSize(),
        nullptr, &vertexShader);

    device->CreatePixelShader(
        pixelBC->GetBufferPointer(),
        pixelBC->GetBufferSize(),
        nullptr, &pixelShader);

    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
        D3D11_INPUT_ELEMENT_DESC {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0}
    };

    rect = GetRectangleProperties(points);

    device->CreateInputLayout(
        inputElements,
        2,
        vertexBC->GetBufferPointer(),
        vertexBC->GetBufferSize(),
        &layout);

    D3D11_BUFFER_DESC vertexBufDesc = {};
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufDesc.CPUAccessFlags = 0;
    vertexBufDesc.MiscFlags = 0;
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * points.size();

    D3D11_BUFFER_DESC constBufDesc = {};
    constBufDesc.Usage = D3D11_USAGE_DYNAMIC;
    constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constBufDesc.MiscFlags = 0;
    constBufDesc.StructureByteStride = 0;
    constBufDesc.ByteWidth = sizeof(offset);

    device->CreateBuffer(&constBufDesc, nullptr, &cb);

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = points.data();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);

    D3D11_BUFFER_DESC indexBufDesc = {};
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.CPUAccessFlags = 0;
    indexBufDesc.MiscFlags = 0;
    indexBufDesc.StructureByteStride = 0;
    indexBufDesc.ByteWidth = sizeof(int) * std::size(indeces);

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indeces.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    device->CreateBuffer(&indexBufDesc, &indexData, &ib);
}

void RectangleComponent::Draw() {
    UINT strides[] = { 32 };
    UINT offsets[] = { 0 };
    context->VSSetConstantBuffers(0, 1, &cb);
    context->IASetInputLayout(layout);
    context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
    context->IASetVertexBuffers(0, 1, &vb, strides, offsets);
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);
    context->DrawIndexed(indeces.size(), 0, 0);
}

void RectangleComponent::Update() {
    std::vector<DirectX::XMFLOAT4> transformedPoints = points;

    for (auto& p : transformedPoints) {
        p.x += offset.x;
        p.y += offset.y;
    }

    rect = GetRectangleProperties(transformedPoints);

    D3D11_MAPPED_SUBRESOURCE res = {};
    context->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

    auto dataPtr = reinterpret_cast<float*>(res.pData);
    memcpy(dataPtr, &offset, sizeof(offset));

    context->Unmap(cb, 0);
}

void RectangleComponent::DestroyResources() {
    if (vb) vb->Release();
    if (ib) ib->Release();
    if (vertexShader) vertexShader->Release();
    if (pixelShader) pixelShader->Release();
    if (layout) layout->Release();
}

void RectangleComponent::BallMovement() {
    offset.x += v.x * ballVelocity;
    offset.y += v.y * ballVelocity;
}

void RectangleComponent::UpdVelocity() {
    ballVelocity *= 1.1;
}

RectF RectangleComponent::GetRectangleProperties(const std::vector<DirectX::XMFLOAT4>& points) {
    float minX = points[0].x;
    float maxX = points[0].x;
    float minY = points[0].y;
    float maxY = points[0].y;
    
    for (size_t i = 2; i < points.size(); i += 2) { // Пропускаем цветовые компоненты
        const auto& p = points[i];
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
    }
    
    return {minX, minY, maxX - minX, maxY - minY};
}

void RectangleComponent::ChangeBallDirection(DirectX::SimpleMath::Vector2 normal) {
    normal.Normalize();

    // Вычисляем скалярное произведение v ⋅ normal
    float dotProduct = v.Dot(normal);

    // Отражаем вектор v относительно normal
    v = v - 2.0f * dotProduct * normal;

    // Убедимся, что после отражения длина вектора скорости не изменилась
    v.Normalize();
    UpdVelocity();
}

void RectangleComponent::recalculateRect() {
    rect = GetRectangleProperties(points);
    auto a = rect.x + rect.w - rect.w / 4;
    auto b = rect.x + rect.w / 4;
    auto c = rect.y + rect.h / 4;
    auto d = rect.y + rect.h - rect.h / 4;
    std::vector<DirectX::XMFLOAT4> newPoints = {
    DirectX::XMFLOAT4(a, c, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    DirectX::XMFLOAT4(b, d, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    DirectX::XMFLOAT4(a, d, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    DirectX::XMFLOAT4(b, c, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
    };

    points = newPoints;

    D3D11_BUFFER_DESC vertexBufDesc = {};
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufDesc.CPUAccessFlags = 0;
    vertexBufDesc.MiscFlags = 0;
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * points.size();

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = points.data();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);

    D3D11_BUFFER_DESC indexBufDesc = {};
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.CPUAccessFlags = 0;
    indexBufDesc.MiscFlags = 0;
    indexBufDesc.StructureByteStride = 0;
    indexBufDesc.ByteWidth = sizeof(int) * std::size(indeces);

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indeces.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    device->CreateBuffer(&indexBufDesc, &indexData, &ib);
}