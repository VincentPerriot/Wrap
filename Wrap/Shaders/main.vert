#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout( set = 0, binding = 0 ) uniform CameraUBO
{
    mat4 view;
    mat4 proj;
} camera;

layout( set = 0, binding = 1 ) uniform ModelUBO
{
    mat4 model;
} models[];

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(push_constant) uniform PushConstants {
    uint modelIndex;
} pushConsts;

layout(location = 0) out vec3 fragColor;

void main() 
{
    gl_Position = camera.proj * camera.view * models[nonuniformEXT(pushConsts.modelIndex)].model * vec4(inPosition, 0.0, 1.0 );
    fragColor = inColor;
}