// Добавляем текстуру и сэмплер
Texture2D objTexture : register(t0);
SamplerState objSampler : register(s0);

Texture2D shadowMap : register(t1);
SamplerComparisonState shadowSamp : register(s1);

struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 tex : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float4 worldPos : TEXCOORD3;
};


struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
};

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float pad;
};

cbuffer cbPerFrame : register(b3)
{
    Material material;
    DirectionalLight gDirLight;
    float4 gEyePosW;
    Matrix lightSpace;
}

void ComputeDirectionalLight(Material mat, DirectionalLight L,
    float3 normal, float3 toEye,
    out float4 ambient,
    out float4 diffuse,
    out float4 spec)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    
    ambient = mat.Ambient * L.Ambient;
    
    //diffusion
    float3 vector2light = -L.Direction.xyz;
    float diffuseFactor = max(dot(vector2light, normal), 0.0f);
    diffuse = diffuseFactor * mat.Diffuse;
    
    //specular
    float3 reflectedLight = reflect(-vector2light, normal);
    float specularFactor = pow(max(dot(reflectedLight, toEye), 0.0f), mat.Specular.w);
    spec = specularFactor * mat.Specular;
}

float CalculateShadow(float4 lightSpacePos)
{
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = 1.0 - projCoords.y;
    
    float currentDepth = projCoords.z;
    
    if (currentDepth > 1.0 || projCoords.x < 0 || projCoords.x > 1 ||
        projCoords.y < 0 || projCoords.y > 1)
    {
        return 1.0;
    }
    
    float shadow = 0.0;
    float2 texelSize = 1.0 / float2(128, 128);
    
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            shadow += shadowMap.SampleCmpLevelZero(shadowSamp, (projCoords.xy + float2(x, y) * texelSize), currentDepth);
        }
    }
    
    shadow /= 9.0;
    
    return shadow;
}


cbuffer ObjectBuffer : register(b0)
{
    float4x4 transformationMatrix;
};

cbuffer CameraBuffer : register(b1)
{
    float4x4 viewProjection;
};

cbuffer TextureFlags : register(b2)
{
    bool useTexture;
    float3 padding; // Для выравнивания
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    
    output.worldPos = mul(input.pos, transformationMatrix);
    output.pos = mul(output.worldPos, viewProjection);
    output.col = input.col;
    output.tex = input.tex;
    
    output.normal = normalize(mul(float4(input.normal, 0.0f), transformationMatrix));
    
    return output;
}


float4 PSMain(PS_IN input) : SV_TARGET
{
    float4 baseColor;
    if (useTexture)
    {
        baseColor = objTexture.Sample(objSampler, input.tex);
    }
    else
    {
        baseColor = input.col;
    }
    
    float4 lightSpacePos = mul(input.worldPos, lightSpace);
    float shadowFactor = CalculateShadow(lightSpacePos);
    
    float3 toEye = normalize(gEyePosW.xyz - input.worldPos.xyz);
    float4 ambient, diffuse, specular;
    
    ComputeDirectionalLight(material, gDirLight, input.normal, toEye, ambient, diffuse, specular);

    // Суммируем компоненты освещения
    float4 lighting = ambient + diffuse + specular;

    // Итоговый цвет — базовый цвет, умноженный на освещение
    float4 finalColor = baseColor * lighting;
    
    finalColor *= shadowFactor;


    // Альфа остается из базового цвета
    finalColor.a = baseColor.a;

    return finalColor;
}
