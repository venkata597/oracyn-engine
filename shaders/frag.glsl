#version 460 core

in vec3 fragPos;
in vec2 fuv;
in vec3 fNormal;
in vec4 fTangent;

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D emissive;
uniform sampler2D occlusion;
uniform sampler2D mr;

layout (std140, binding = 0) uniform MaterialUBO{
    vec4 uBaseColor;
    float uMetallicFactor;
    float uRoughnessFactor;
    float uOcclusionStrength;
    float uEmissiveStrength;
    vec3 uEmissiveFactor;
};

layout (std140, binding = 1) uniform CameraUBO{
    mat4 uView;
    mat4 uProj;
    vec4 uPos;
};

uniform vec3 ulightDir;
uniform vec3 ulightColor;

out vec4 FragColor;
const float PI = 3.14159265359;

float distrubutionGGX(vec3 N,vec3 H, float roughness){
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N,H),0.0);
    float denom = (NdotH * NdotH * (a2-1.0) + 1.0);
    return a2/(PI * denom * denom);
}

float geometrySchlickGGX(float NdotV,float roughness){
    float r = roughness + 1.0;
    float k = (r*r)/8.0;
    return NdotV / (NdotV * (1.0-k)+k);
}

float geometrySmith(vec3 N,vec3 V,vec3 L,float roughness){
    float NdotV = max(dot(N,V),0.0);
    float NdotL = max(dot(N,L),0.0);
    return geometrySchlickGGX(NdotV,roughness) * geometrySchlickGGX(NdotL,roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0-F0) * pow(clamp(1.0-cosTheta,0.0,1.0),5.0);
}

void main(){
    vec4 albedoSample = texture(albedo,fuv);
    vec4 normalSample = texture(normal,fuv);
    vec4 mrSample = texture(mr,fuv);
    vec3 emissiveSample = texture(emissive,fuv).rgb;
    float occlusionSample = texture(occlusion,fuv).r;

    vec3 baseColor = albedoSample.rgb * uBaseColor.rgb;
    float alpha = albedoSample.a * uBaseColor.a;
    float roughness = mrSample.g * uRoughnessFactor;
    float metallic = mrSample.b * uMetallicFactor;

    vec3 N = normalize(fNormal);
    vec3 T = normalize(fTangent.xyz);
    vec3 B = cross(N,T) * fTangent.w;

    mat3 TBN =  mat3(T,B,N);

    vec3 normalTex = texture(normal,fuv).rgb;
    normalTex = normalTex * 2.0 - 1.0;

    vec3 Nmapped = normalize(TBN * normalTex);

    vec3 V = normalize(uPos.xyz - fragPos);
    vec3 L = normalize(-ulightDir);
    vec3 H = normalize(V+L);
    vec3 F0 = mix(vec3(0.04),baseColor,metallic);

    float NDF = distrubutionGGX(Nmapped,H,roughness);
    float G = geometrySmith(Nmapped,V,L,roughness);
    vec3 F = fresnelSchlick(max(dot(H,V),0.0),F0);
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(Nmapped,V),0.0) * (max(dot(Nmapped,L),0.0)) + 0.0001;
    vec3 specular = numerator/denominator;

    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metallic);
    float NdotL = max(dot(Nmapped,L),0.0);
    vec3 Lo = (kD * baseColor / PI + specular) * ulightColor * NdotL;
    vec3 ambient = vec3(0.15) * baseColor * mix(1.0, occlusionSample, uOcclusionStrength);
    vec3 emissiveOut = emissiveSample * uEmissiveFactor * uEmissiveStrength;
    vec3 color = ambient + Lo + emissiveOut;
    color = color/(color + vec3(1.0));
    color = pow(color,vec3(1.0/2.2));
    FragColor = vec4(color,alpha);
}
