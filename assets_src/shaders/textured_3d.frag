#version 450

layout(location = 0) out vec4 outColor;

layout(location = 3) in vec2 current_texcoords;
layout(set = 0, binding = 1) uniform sampler2D textures[8];


void main() {
 outColor = texture(textures[0], current_texcoords);
 if(outColor.a <= 0.4f)
  discard;
}


