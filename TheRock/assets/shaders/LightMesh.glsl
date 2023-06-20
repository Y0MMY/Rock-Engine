#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 430 core

out vec4 fragColor;

// Функция для выполнения размытия Гаусса
float gaussianBlur(vec2 uv)
{
    // Здесь вы можете настроить параметры размытия
    const float blurRadius = 0.02;
    const int numSamples = 10;
    
    float result = 0.0;
    float totalWeight = 0.0;
    
    for (int i = 0; i < numSamples; ++i)
    {
        float offset = float(i) / float(numSamples - 1) - 0.5;
        float weight = exp(-offset * offset / (2.0 * blurRadius * blurRadius));
        
        vec2 samplePos = uv + vec2(offset * blurRadius);
        vec4 color = vec4(1.f); // Здесь подставьте вашу текстуру, если она есть
        
        result += color.x * weight; // Применяем размытие только к одной компоненте цвета
        totalWeight += weight;
    }
    
    result /= totalWeight;
    
    return result;
}

void main()
{
    // Яркий цвет объекта
    vec4 objectColor = vec4(1.0, 0.9, 0.5, 1.0); // Здесь можно настроить яркий цвет
    
    // Применяем размытие Гаусса к яркому цвету
    vec2 uv = vec2(gl_FragCoord.xy) / vec2(1920, 1080); // Замените screenWidth и screenHeight на соответствующие значения
    float blurredColor = gaussianBlur(uv);
    
    fragColor = vec4(blurredColor * objectColor.rgb, objectColor.a);
}