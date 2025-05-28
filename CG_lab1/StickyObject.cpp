#include "StickyObject.h"
#include <d3dcompiler.h>
#include <vector>
#include <cmath>
#include <random>

using namespace DirectX;
using namespace DirectX::SimpleMath;

Vector3 RotateVector(const Vector3& originalVec, const Vector3& axis, float angleRadians) {

    // Нормализация оси вращения
    Vector3 normalizedAxis = axis;
    normalizedAxis.Normalize();

    // Создание матрицы вращения
    Matrix rotationMatrix = Matrix::CreateFromAxisAngle(normalizedAxis, angleRadians);

    // Применение вращения к вектору
    return Vector3::Transform(originalVec, rotationMatrix);
}

float AngleBetweenAxes(const Vector3& axis1, const Vector3& axis2, const Vector3& referenceUp = Vector3::Up) {
    Vector3 a = axis1;
    Vector3 b = axis2;
    a.Normalize();
    b.Normalize();

    float dot = a.Dot(b);
    float angle = acosf(dot);

    Vector3 cross = a.Cross(b);
    float sign = cross.Dot(referenceUp);
    if (sign < 0) {
        angle = 2 * XM_PI - angle;
    }
    return max(angle, 0.0001f);
}

Vector3 RandomPointOnUnitSphere() {
    // Генератор случайных чисел
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    // Сферические координаты
    float theta = 2.0f * XM_PI * dist(gen);  // Азимутальный угол [0, 2π]
    float phi = std::acos(2.0f * dist(gen) - 1.0f);  // Зенитный угол [0, π]

    // Преобразование в декартовы координаты
    float sinPhi = std::sin(phi);
    return Vector3(
        sinPhi * std::cos(theta),
        sinPhi * std::sin(theta),
        std::cos(phi)
    );
}

Vector3 ProjectVectorOntoPlane(const Vector3& vector, const Vector3& planeNormal) {
    // 1. Нормализуем нормаль плоскости (если она ещё не нормализована)
    Vector3 normalizedNormal = planeNormal;
    normalizedNormal.Normalize();

    // 2. Находим проекцию вектора на нормаль плоскости
    float dotProduct = vector.Dot(normalizedNormal);
    Vector3 projectionOntoNormal = normalizedNormal * dotProduct;

    // 3. Вычитаем проекцию на нормаль из исходного вектора
    //    Это даёт проекцию вектора на плоскость
    Vector3 projectionOntoPlane = vector - projectionOntoNormal;

    return projectionOntoPlane;
}

StickyObject::StickyObject(ID3D11Device* device, ID3D11DeviceContext* context, Vector3 startCoords, InputDevice* inpDevice, const std::string& path, const std::string& texturePath, const DirectX::XMFLOAT4& color)
    : device(device), context(context), startCoords(startCoords), inpDevice(inpDevice)
{
    Initialize(path, texturePath, color);
}

StickyObject::~StickyObject() {
    if (vertexBuffer) vertexBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
    if (vertexShader) vertexShader->Release();
    if (pixelShader) pixelShader->Release();
    if (inputLayout) inputLayout->Release();
}

void StickyObject::Initialize(const std::string& path, const std::string& texturePath, const DirectX::XMFLOAT4& color) {
    if (!texturePath.empty()) {
        DirectX::CreateWICTextureFromFile(
            device,
            std::wstring(texturePath.begin(), texturePath.end()).c_str(),
            nullptr, &textureView
        );
        useTexture = true;

        // Создаем сэмплер
        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        device->CreateSamplerState(&samplerDesc, &samplerState);
    }
    
    if (path != "") {
        MeshData mesh = ModelLoader::LoadModel(path, color);
        vertices = mesh.vertices;
        indices = mesh.indices;
        printf("%f, %f, %f\n", vertices[0].position.x, vertices[0].position.y, vertices[0].position.z);
    }
    else {
        float s = 0.25f; // половина длины стороны (0.5 / 2)
        std::vector<Vertex> v = {
            {{-s, +s, -s, 1.0f}, {1, 0, 0, 1}},
            {{+s, +s, -s, 1.0f}, {0, 1, 0, 1}},
            {{+s, -s, -s, 1.0f}, {0, 0, 1, 1}},
            {{-s, -s, -s, 1.0f}, {1, 1, 0, 1}},
            {{-s, +s, +s, 1.0f}, {1, 0, 1, 1}},
            {{+s, +s, +s, 1.0f}, {0, 1, 1, 1}},
            {{+s, -s, +s, 1.0f}, {1, 1, 1, 1}},
            {{-s, -s, +s, 1.0f}, {0, 0, 0, 1}},
        };
        vertices = v;

        indices = {
             0,1,2, 0,2,3,
             4,6,5, 4,7,6,
             4,5,1, 4,1,0,
             3,2,6, 3,6,7,
             1,5,6, 1,6,2,
             4,0,3, 4,3,7
        };
    }

    // Вершинный буфер
    D3D11_BUFFER_DESC vertexBufDesc = {};
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufDesc.CPUAccessFlags = 0;
    vertexBufDesc.MiscFlags = 0;
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(Vertex) * vertices.size();

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    device->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);

    // Индексный буфер
    D3D11_BUFFER_DESC indexBufDesc = {};
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.CPUAccessFlags = 0;
    indexBufDesc.MiscFlags = 0;
    indexBufDesc.StructureByteStride = 0;
    indexBufDesc.ByteWidth = sizeof(int) * std::size(indices);

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);

    // Шейдеры
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* err = nullptr;

    D3DCompileFromFile(L"./Shaders/PlanetShader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vsBlob, &err);
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);

    D3DCompileFromFile(L"./Shaders/PlanetShader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &psBlob, &err);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);

    // Input Layout
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    device->CreateInputLayout(layoutDesc, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);

    D3D11_BUFFER_DESC constBufDesc = {};
    constBufDesc.Usage = D3D11_USAGE_DYNAMIC;
    constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constBufDesc.MiscFlags = 0;
    constBufDesc.StructureByteStride = 0;
    constBufDesc.ByteWidth = sizeof(Matrix);

    device->CreateBuffer(&constBufDesc, nullptr, &transformBuffer);

    vsBlob->Release();
    psBlob->Release();
}

void StickyObject::Draw() {
    UINT stride = sizeof(XMFLOAT4) * 2;
    UINT offset = 0;

    if (useTexture) {
        context->PSSetShaderResources(0, 1, &textureView);
        context->PSSetSamplers(0, 1, &samplerState);
    }

    context->IASetInputLayout(inputLayout);
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetConstantBuffers(0, 1, &transformBuffer);

    struct TextureFlags { int usTexture; float padding[3]; } flags = { useTexture };
    ID3D11Buffer* flagBuffer = CreateFlagBuffer(device, &flags, sizeof(flags));
    context->PSSetConstantBuffers(2, 1, &flagBuffer);
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    context->DrawIndexed(indices.size(), 0, 0);
}

void StickyObject::Update(const Vector3& ballCenter, float ballRadius, Vector3 rotationAxis, float angle) {
    if (mode == Mode::Attached) {
        Vector3 rotatedPos = RotateVector(startAttachmentCoords, startAttachmentRotationAxis, angle - startAttachmentBallAngle);
        Vector3 direction = rotationAxis.Cross(Vector3::Up);
        direction.Normalize();
        float deltaAngle = AngleBetweenAxes(startAttachmentRotationAxis, rotationAxis);
        Matrix yawRotation = Matrix::CreateRotationY(deltaAngle);
        Vector3 finalPos = Vector3::Transform(rotatedPos, yawRotation);
        translation = Matrix::CreateTranslation(ballCenter + finalPos);
        rotationA = Matrix::CreateFromAxisAngle({ 0.0f, 1.0f, 0.0f }, deltaAngle);
        rotationB = Matrix::CreateFromAxisAngle(rotationAxis, angle - startAttachmentBallAngle);

    }
    else {
        translation = Matrix::CreateTranslation(startCoords);
    }

    worldMatrix = scale * rotationA * rotationB * translation;

    Matrix transposed = worldMatrix.Transpose();

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    context->Map(transformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    auto dataPtr = reinterpret_cast<Matrix*>(mapped.pData);

    memcpy(dataPtr, &transposed, sizeof(Matrix));
    context->Unmap(transformBuffer, 0);

}

DirectX::BoundingBox StickyObject::GetBoundingBox() const {
    using namespace DirectX::SimpleMath;
    Vector3 center = startCoords;

    float minX = vertices[0].position.x, maxX = vertices[0].position.x;
    float minY = vertices[0].position.y, maxY = vertices[0].position.y;
    float minZ = vertices[0].position.z, maxZ = vertices[0].position.z;
    for (Vertex vertex : vertices) {
        minX = min(minX, vertex.position.x);
        maxX = max(maxX, vertex.position.x);
        minY = min(minY, vertex.position.y);
        maxY = max(maxY, vertex.position.y);
        minZ = min(minZ, vertex.position.z);
        maxZ = max(maxZ, vertex.position.z);
    }

    Vector3 extents = { (maxX - minX) / 2 - (maxX - minX) / 5, (maxY - minY) / 2 - (maxY - minY) / 5, (maxZ - minZ) / 2 - (maxZ - minZ) / 5 };

    return DirectX::BoundingBox(center, extents);
}

void StickyObject::Attach(const Vector3& ballCenter, float ballRadius, float ballAngle, const DirectX::SimpleMath::Vector3 ballRotAxis) {
    mode = Mode::Attached;
    startAttachmentCoords = startCoords - ballCenter;
    startAttachmentBallAngle = ballAngle;
    startAttachmentRotationAxis = ballRotAxis;
    coords = startCoords;
}