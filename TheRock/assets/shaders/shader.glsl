#type vertex
#version 430

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

out vec3 v_Position;

void main()
{
    vec4 poisition = vec4(a_Position.xy, 1.0f, 1.0f);
    gl_Position = poisition;

    v_Position = (1 * poisition).xyz;
}

#type fragment
#version 430

layout (location = 0) out vec4 finalColor;
uniform samplerCube u_Texture;

in vec3 v_Position;

void main()
{
    finalColor = textureLod(u_Texture,v_Position, 1.0f);
}