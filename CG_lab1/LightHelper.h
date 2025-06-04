#pragma once

#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>

#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

struct DirectionalLight {
    Vector4 ambient;
    Vector4 diffuse;
    Vector4 specular;
    Vector4 direction;
};

struct Material {
    Vector4 ambient;
    Vector4 diffuse;
    Vector4 specular;
};

struct LightData {
    Material material;
    DirectionalLight directional;
    Vector4 camera;
};