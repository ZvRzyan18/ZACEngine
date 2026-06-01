#version 450

layout(push_constant) uniform PushConstants {
 mat4 transform;
 vec4 color;
 bool is_unorm;
} pc;

layout(location = 0) out vec4 out_color;

void main() {
 out_color = pc.color;
 
 if(pc.is_unorm)
   out_color.rgb = mix(12.92f * out_color.rgb, 1.055f * pow(out_color.rgb, vec3(1.0f/2.4f)) - 0.055f, step(vec3(0.0031308), out_color.rgb));
}