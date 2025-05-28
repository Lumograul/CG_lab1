// ��������� �������� � �������
Texture2D objTexture : register(t0);
SamplerState objSampler : register(s0);

// ���� ������������� ��������
cbuffer TextureFlags : register(b2)
{
    bool useTexture;
    float3 padding; // ��� ������������
};

struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR;
    float2 tex : TEXCOORD0; // ��������� ���������� ����������
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 tex : TEXCOORD0; // �������� � ���������� ������
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
    output.tex = input.tex; // �������� ���������� ����������
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