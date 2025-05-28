// Добавляем текстуру и сэмплер
Texture2D objTexture : register(t0);
SamplerState objSampler : register(s0);

// Флаг использования текстуры
cbuffer TextureFlags : register(b2)
{
    bool useTexture;
    float3 padding; // Для выравнивания
};

struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR;
    float2 tex : TEXCOORD0; // Добавляем текстурные координаты
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 tex : TEXCOORD0; // Передаем в пиксельный шейдер
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
    output.tex = input.tex; // Передаем текстурные координаты
    return output;
}

float4 PSMain(PS_IN input) : SV_TARGET
{
    if (useTexture)
    {
        return objTexture.Sample(objSampler, input.tex);
    }
    else
    {
        return input.col;
    }
}