#include "Shadow.h"

Shadow::Shadow()
{
	renderTargetTexture = nullptr;
	depthStencilTexture = nullptr;
	rtv = nullptr;
	shadowMapSRV = nullptr;
	shadowMapDSV = nullptr;
}

Shadow::~Shadow()
{
	if (renderTargetTexture) renderTargetTexture->Release();
	if (depthStencilTexture) depthStencilTexture->Release();
	if (rtv) rtv->Release();
	if (shadowMapSRV) shadowMapSRV->Release();
	if (shadowMapDSV) shadowMapDSV->Release();
}

void Shadow::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, UINT width, UINT height)
{
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	device->CreateTexture2D(&texDesc, 0, &renderTargetTexture);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = texDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	device->CreateRenderTargetView(renderTargetTexture, &renderTargetViewDesc, &rtv);

	//
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	device->CreateShaderResourceView(renderTargetTexture, &srvDesc, &shadowMapSRV);

	//
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	device->CreateTexture2D(&depthBufferDesc, 0, &depthStencilTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = depthBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(depthStencilTexture, &depthStencilViewDesc, &shadowMapDSV);

}

ID3D11ShaderResourceView* Shadow::GetShadowMapDSV()
{
	return shadowMapSRV;
}

void Shadow::SetRenderTarget(ID3D11DeviceContext* context)
{
	context->OMSetRenderTargets(1, &rtv, shadowMapDSV);
	context->RSSetViewports(1, &viewport);
}

void Shadow::ClearRenderTarget(ID3D11DeviceContext* context, DirectX::SimpleMath::Vector4 color)
{
	float colorAsArray[] = { color.x, color.y, color.z, color.w };
	context->ClearRenderTargetView(rtv, colorAsArray);
	context->ClearDepthStencilView(shadowMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
