#version 450 core

out vec4 color;

uniform vec3 lightDir = normalize(vec3(-1., 0.5, 0.3));
uniform vec4 lightColor = vec4(0.9, 0.8, 0.7, 1.0);

in V
{
    vec4 pos;
    vec2 uv;
    vec3 normal;
} v;

void main(void) {
    vec3 normal = normalize(v.normal);
    vec3 base = vec3(0.98, 0.27, 0.2);
    color = vec4(
        base / 3. + 
        base * vec3(1., 1., 1.) * (1. - abs(dot(normal, vec3(0., 0., -1.)))) / 3. + 
        base * lightColor.rgb * max(0., dot(lightDir, normal)) + 
        base * 4. * pow(max(0., dot(normalize((normalize(v.pos.xyz) - lightDir) / 2.), -normal)), 87.) * lightColor.rgb
        , 1.);
}