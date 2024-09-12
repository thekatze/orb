#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_position;

layout(set = 0, binding = 0) uniform global_uniform_object {
    mat4 camera_projection;
} global_ubo;

void main() {
    gl_Position = global_ubo.camera_projection * vec4(in_position, 1.0);
}
