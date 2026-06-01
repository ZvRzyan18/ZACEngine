#version 450

vec2 positions[4] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0,  1.0)
);

vec2 texcoords[4] = vec2[](
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0),
    vec2(0.0, 0.0)
);

int indices[6] = int[](
 0, 1, 2, 2, 3, 0
);


layout(location = 2) out vec2 current_texcoords;

layout(location = 0) in vec4 transform;
layout(location = 1) in vec4 uv_transform;

layout(push_constant) uniform PushConstants {
 mat4 transform;
 vec4 color;
 bool is_unorm;
} push_constants;


void main() {
 int current_index = indices[gl_VertexIndex];
 current_texcoords = texcoords[current_index] * uv_transform.zw + uv_transform.xy;

 vec2 current_vertex = positions[current_index];
 vec2 transformed_position = positions[current_index] * transform.zw + transform.xy;

 gl_Position = push_constants.transform * vec4(transformed_position, 0.0, 1.0);
}
