struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

cbuffer ObjectBuffer : register(b0)
{
    float4x4 transformationMatrix;
};

cbuffer CameraBuffer : register(b1)
{
    float4x4 viewProjection;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    
    float4 worldPos = mul(input.pos, transformationMatrix);
    output.pos = mul(worldPos, viewProjection);
    output.col = input.col;

    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float4 col = input.col;
    return col;
}
