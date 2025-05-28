#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <vector>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class RotatingComponent
{
public:
    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

    RotatingComponent();
    ~RotatingComponent();

    HRESULT Initialize(
        ID3D11Device* device,
        std::vector<DirectX::XMFLOAT4> vertices,
        std::vector<int> indices,
        float rotationSpeed
    );

    void Update(float deltaTime, ID3D11DeviceContext* context);
    void Render(ID3D11DeviceContext* context);
    void ReleaseResources();

private:
    struct ConstantBuffer
    {
        XMMATRIX transformationMatrix;
    };

    ID3D11Buffer* m_vertexBuffer = nullptr;
    ID3D11Buffer* m_indexBuffer = nullptr;
    ID3D11Buffer* m_constantBuffer = nullptr;
    UINT m_indexCount = 0;
    float m_rotationAngle = 0.0f;
    float m_rotationSpeed = 0.0f;
};