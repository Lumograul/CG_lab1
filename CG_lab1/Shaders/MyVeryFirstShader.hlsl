struct VS_IN
{
    float4 pos : POSITION0;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
};

cbuffer ConstBuf : register(b0)
{
    matrix transformations;
    matrix viewProj;
    float4 color;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    float4 worldPosition = mul(input.pos, transformations);
    output.pos = mul(worldPosition, viewProj);
    return output;
};

float4 PSMain(PS_IN input) : SV_Target
{
    float depthValue = input.pos.z / input.pos.w;
    return float4(depthValue, depthValue, depthValue, 1.0f);
};