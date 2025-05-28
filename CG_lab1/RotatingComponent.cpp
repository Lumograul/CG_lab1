#include "RotatingComponent.h"

RotatingComponent::RotatingComponent() = default;

RotatingComponent::~RotatingComponent()
{
    ReleaseResources();
}

void RotatingComponent::ReleaseResources()
{
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
    if (m_constantBuffer) m_constantBuffer->Release();

    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_constantBuffer = nullptr;
}

HRESULT RotatingComponent::Initialize(
    ID3D11Device* device,
    std::vector<DirectX::XMFLOAT4> vertices,
    std::vector<int> indices,
    float rotationSpeed)
{
    m_rotationSpeed = rotationSpeed;

    // Vertex buffer
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vInitData = {};
    vInitData.pSysMem = vertices.data();
    HRESULT hr = device->CreateBuffer(&vbd, &vInitData, &m_vertexBuffer);
    if (FAILED(hr)) return hr;

    // Index buffer
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = static_cast<UINT>(sizeof(WORD) * indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA iInitData = {};
    iInitData.pSysMem = indices.data();
    hr = device->CreateBuffer(&ibd, &iInitData, &m_indexBuffer);
    if (FAILED(hr)) return hr;

    m_indexCount = static_cast<UINT>(indices.size());

    // Constant buffer
    D3D11_BUFFER_DESC cbd = {};
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    return device->CreateBuffer(&cbd, nullptr, &m_constantBuffer);
}

void RotatingComponent::Update(float deltaTime, ID3D11DeviceContext* context)
{
    m_rotationAngle += m_rotationSpeed * deltaTime;
    if (m_rotationAngle > XM_2PI) m_rotationAngle -= XM_2PI;

    XMMATRIX world = XMMatrixRotationY(m_rotationAngle);

    D3D11_MAPPED_SUBRESOURCE mapped;
    context->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    ConstantBuffer* data = static_cast<ConstantBuffer*>(mapped.pData);
    data->transformationMatrix = XMMatrixTranspose(world);
    context->Unmap(m_constantBuffer, 0);
}

void RotatingComponent::Render(ID3D11DeviceContext* context)
{
    // Set vertex buffers
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set index buffer
    context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set constant buffer
    context->VSSetConstantBuffers(0, 1, &m_constantBuffer);

    // Draw call
    context->DrawIndexed(m_indexCount, 0, 0);
}