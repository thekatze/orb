#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 out_color;
layout(location = 0) in vec3 in_position;

void main() {
    vec3 color = (in_position);
    out_color = vec4(color, 1.0);
}
