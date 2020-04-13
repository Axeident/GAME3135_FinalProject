#version 150

uniform mat4 transform, camera, projection;

in vec3 position, normal;
in vec2 uv;
in int group;

out mat4 modelview;

out vec3 frag_position, frag_normal;
out vec2 frag_uv;

void main() {
    modelview = camera * transform;
    vec4 eye_position = modelview * vec4(position, 1.0);
    gl_Position = projection * eye_position;
    frag_position = eye_position.xyz;
    frag_normal   = normalize(transpose(inverse(mat3(modelview))) * normal);
    frag_uv = uv;
}