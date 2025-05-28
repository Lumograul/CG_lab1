#pragma once
#include <d3d.h>
#include <d3d11.h>

class GameComponent {
protected:
    ID3D11Device* device;
    ID3D11DeviceContext* context;
public:
    GameComponent(ID3D11Device* dev, ID3D11DeviceContext* ctx) : device(dev), context(ctx) {}
    virtual void Initialize() {}
    virtual void Update(float deltaTime) {}
    virtual void Update() {}
    virtual void Draw() {}
    virtual void DestroyResources() {}
};