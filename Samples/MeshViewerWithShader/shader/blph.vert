#version 450 core


in vec3 pos;
in vec2 uv;
in vec3 normal;
uniform mat4 trans;
uniform mat4 ntrans;
uniform mat4 projec;

out V
{
    vec4 pos;
    vec2 uv;
    vec3 normal;
} f;

void main(void) {
    vec4 p = projec * trans * vec4(pos, 1.);
    f.uv = uv;
    gl_Position = p;
    f.pos = trans * vec4(pos, 1.);
    f.normal = normalize((ntrans * vec4(normal, 0.)).xyz);
}