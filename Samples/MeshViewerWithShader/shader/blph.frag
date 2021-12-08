#version 450 core

out vec4 color;

uniform vec3 lightDir = normalize(vec3(-1., 0.5, 0.3));
uniform vec4 lightColor = vec4(0.9, 0.8, 0.7, 1.0);
uniform sampler2D mainTex;
uniform sampler2D aoTex;
uniform sampler2D smoothTex;

in V
{
    vec4 pos;
    vec2 uv;
    vec3 normal;
} v;

void main(void) {
    vec3 normal = normalize(v.normal);
    vec3 base = texture(mainTex, v.uv).rgb;
    float ao = pow(texture(aoTex, v.uv).r, 2.);
    float smoo = texture(smoothTex, v.uv).r;
    // mix(vec3(0.98, 0.27, 0.2), vec3(0.23, 0.00, 0.78), fract(floor(v.uv.x * 16.) / 2. + floor(v.uv.y * 16.) / 2.));
    color = vec4(
        base / 2. * ao + 
        base * ao * vec3(1., 1., 1.) * pow((1. - abs(dot(normal, normalize(v.pos.xyz)))), 1.3) / 2. + 
        base * ao * lightColor.rgb * max(0., dot(lightDir, normal)) * 2. + 
        base * ao * 4. * smoo * pow(max(0., dot(normalize((normalize(v.pos.xyz) - lightDir) / 2.), -normal)), 87. * (0.01 + smoo)) * lightColor.rgb
        , 1.);
}