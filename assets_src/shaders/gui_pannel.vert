#version 450

layout(push_constant) uniform PushConstants {
 mat4 transform;
 vec4 color;
 bool is_unorm;
} pc;

vec2 positions[4] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0,  1.0)
);

int indices[6] = int[](
 0, 1, 2, 2, 3, 0
);

void main() {
 int current_index = indices[gl_VertexIndex];

 vec2 pos = positions[current_index];
 gl_Position = pc.transform * vec4(pos, 0.0, 1.0);
}
