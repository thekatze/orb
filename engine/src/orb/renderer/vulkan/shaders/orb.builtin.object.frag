#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(0.8, 0.5, 0.6, 1.0);
}
