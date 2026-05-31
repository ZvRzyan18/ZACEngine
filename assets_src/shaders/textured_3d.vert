#version 450


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(location = 3) out vec2 current_texcoords;

layout(set = 0, binding = 0) uniform UniformBuffer
{
 mat4 mvp_m;
 mat3 normal_m;
} ubo;




void main() {
 current_texcoords = texcoord;
 gl_Position = ubo.mvp_m * vec4(position, 1.0f);
}

