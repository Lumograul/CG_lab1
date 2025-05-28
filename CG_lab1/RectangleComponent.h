#pragma once
#include <d3d.h>
#include <d3d11.h>
#include <vector>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "GameComponent.h"
#include <directxtk/SimpleMath.h>
#include <limits>

struct RectF {
    float x;
    float y;
    float w;
    float h;
    DirectX::SimpleMath::Rectangle toRectangle() {
        return DirectX::SimpleMath::Rectangle(static_cast<int>((x+1)/2*800), static_cast<int>((y + 1) / 2 *800), static_cast<int>((w ) / 2 *800), static_cast<int>((h) / 2 *800));
    };
};

class RectangleComponent : public GameComponent {
private:
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    std::vector<DirectX::XMFLOAT4> points;
    std::vector<int> indeces;
    DirectX::SimpleMath::Vector2 v;
    float ballVelocity;
public:
    ID3D11InputLayout* layout;
    ID3D11Buffer* vb;
    ID3D11Buffer* ib;
    ID3D11Buffer* cb;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    DirectX::XMFLOAT4 offset;
    RectF rect;
    RectangleComponent(ID3D11Device* dev, ID3D11DeviceContext* ctx, const std::vector<DirectX::XMFLOAT4> points, std::vector<int> indeces, DirectX::SimpleMath::Vector2 startVector) :
        device(dev), 
        context(ctx), 
        GameComponent(dev, ctx), 
        vb(nullptr), 
        ib(nullptr), 
        cb(nullptr),
        vertexShader(nullptr),
        pixelShader(nullptr), 
        layout(nullptr),
        points(points),
        indeces(indeces),
        v(startVector),
        ballVelocity(0.01f),
        rect({ 0,0,0,0 })
    {}
    void Initialize() override;
    void Draw() override;
    void Update() override;
    void DestroyResources() override;
    void BallMovement();
    void UpdVelocity();
    RectF GetRectangleProperties(const std::vector<DirectX::XMFLOAT4>& points);
    void ChangeBallDirection(DirectX::SimpleMath::Vector2 normal);
    void recalculateRect();
};
