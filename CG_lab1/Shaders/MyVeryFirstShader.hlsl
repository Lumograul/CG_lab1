struct VS_IN
{
    float4 pos : POSITION0;
    float2 texCoord : TEXCOORD0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

cbuffer ObjectBuffer : register(b0)
{
    float4x4 transformationMatrix;
};

cbuffer CameraBuffer : register(b1)
{
    float4x4 viewProjection;
};

Texture2D tex : register(t0);
SamplerState sampl : register(s0);

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    float4 worldPos = mul(input.pos, transformationMatrix);
    output.pos = mul(worldPos, viewProjection);
    output.texCoord = input.texCoord;
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float4 color = tex.Sample(sampl, input.texCoord);
    return color;
}