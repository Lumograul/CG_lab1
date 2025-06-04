#pragma once

#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <iostream>
#include <vector>
#include <d3d.h>
#include <d3d11.h>
#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "GameComponent.h"
#include <directxtk/SimpleMath.h>
#include <limits>
#include <algorithm>


class Shadow
{
private:
	UINT width;
	UINT height;

	ID3D11Texture2D* renderTargetTexture;
	ID3D11Texture2D* depthStencilTexture;
	ID3D11RenderTargetView* rtv;

	ID3D11ShaderResourceView* shadowMapSRV;
	ID3D11DepthStencilView* shadowMapDSV;

	D3D11_VIEWPORT viewport;

public:
	Shadow();
	~Shadow();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, UINT width, UINT height);

	ID3D11ShaderResourceView* GetShadowMapDSV();

	void SetRenderTarget(ID3D11DeviceContext* context);
	void ClearRenderTarget(ID3D11DeviceContext* context, DirectX::SimpleMath::Vector4 color);
};

