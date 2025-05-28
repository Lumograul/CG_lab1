#include <d3dcompiler.h>
#include <directxmath.h>
#include <iostream>

#include "CentralComponent.h"

using namespace DirectX::SimpleMath;


void CentralComponent::Initialize() {
    float a = 0.0f;
    int stacks = 20;
    int slices = 20;
    int radius = 1;
    for (int i = 0; i <= stacks; ++i) {
        float phi = DirectX::XM_PI * i / stacks;
        for (int j = 0; j <= slices; ++j) {
            float theta = DirectX::XM_2PI * j / slices;

            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);

            points.push_back({ x, y, z, 1.0f });
            points.push_back({ 0.0f, a, 0.0f, 1.0f });
        }
        a += 1.0f / stacks;
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = i * (slices + 1) + j;
            int second = first + slices + 1;

            indeces.push_back(first);
            indeces.push_back(second);
            indeces.push_back(first + 1);

            indeces.push_back(second);
            indeces.push_back(second + 1);
            indeces.push_back(first + 1);
        }
    }

    ID3DBlob* vertexBC = nullptr;
    ID3DBlob* errorVertexCode = nullptr;

    auto res = D3DCompileFromFile(L"./Shaders/PlanetShader.hlsl",
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
    res = D3DCompileFromFile(L"./Shaders/PlanetShader.hlsl", Shader_Macros /*macros*/, nullptr /*include*/, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);

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
    constBufDesc.ByteWidth = sizeof(transformationMatrix);

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

void CentralComponent::Draw() {
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

void CentralComponent::Update() {
    D3D11_MAPPED_SUBRESOURCE res = {};
    if (inputDevice->IsKeyDown(Keys::Up)) {
        // Получаем направление взгляда камеры и проецируем на XZ
        Vector3 cameraForward = camera->GetForwardDirection();
        cameraForward.y = 0.0f;

        if (cameraForward.LengthSquared() > 0.001f) {
            cameraForward.Normalize();
            transl += cameraForward * 0.1f; // Скорость движения
        }
    }
    angle += DirectX::XMConvertToRadians(spinVelocity);
    //transl.z = orbitRadius * sinf(angle);
    //transl.x = orbitRadius * cosf(angle);
    //transl += centerCoords;
    rotationAxis.Normalize();
    Matrix scaleM = Matrix::CreateScale(scale);
    Matrix rotatM = Matrix::CreateFromAxisAngle(rotationAxis, angle);
    Matrix translM = Matrix::CreateTranslation(transl);

    transformationMatrix = (scaleM * rotatM * translM).Transpose();


    context->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
    auto dataPtr = reinterpret_cast<Matrix*>(res.pData);
    memcpy(dataPtr, &transformationMatrix, sizeof(transformationMatrix));
    context->Unmap(cb, 0);
}

void CentralComponent::DestroyResources() {
    if (vb) vb->Release();
    if (ib) ib->Release();
    if (vertexShader) vertexShader->Release();
    if (pixelShader) pixelShader->Release();
    if (layout) layout->Release();
}