#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec4 aTangent;

layout (location = 4) in vec4 aModel0;
layout (location = 5) in vec4 aModel1;
layout (location = 6) in vec4 aModel2;
layout (location = 7) in vec4 aModel3;

layout (std140, binding=1) uniform CameraUBO {
    mat4 uView;
    mat4 uProj;
    vec4 uPos;
};

layout (std140, binding=2) uniform NodeUBO{
    mat4 globalTransform;
};

out vec3 fragPos;
out vec2 fuv;
out vec3 fNormal;
out vec4 fTangent;

void main(){
    mat4 aModel = mat4(aModel0,aModel1,aModel2,aModel3);
    mat4 finalModel = globalTransform * aModel;
    vec4 worldPos = finalModel * vec4(aPos,1.0);
    fragPos = worldPos.xyz;
    fuv = aUV;
    fNormal = normalize(mat3(transpose(inverse(finalModel))) * aNormal);
    fTangent = aTangent;

    gl_Position = uProj * uView * worldPos;
}
