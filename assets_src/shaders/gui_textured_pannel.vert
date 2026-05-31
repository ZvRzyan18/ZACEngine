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


layout(push_constant) uniform PushConstants {
 mat4 transform;
 vec4 color;
 bool is_unorm;
} push_constants;


layout(location = 0) out vec2 current_texcoords;

void main() {
 int current_index = indices[gl_VertexIndex];
 current_texcoords = texcoords[current_index];
 vec2 current_vertex = positions[current_index];

 gl_Position = push_constants.transform * vec4(current_vertex, 0.0, 1.0);
}
