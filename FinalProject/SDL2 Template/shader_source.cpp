//
//  shader.cpp
//  GLhomework1
//
//  Created by Nevin Flanagan on 1/29/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include "OpenGL.hpp"

std::string shader::vFlat =
"#version 150" "\n"

"uniform mat4 transform, camera, projection;" "\n"

"in vec3 position, normal;" "\n"
"in vec2 uv;" "\n"

"out mat4 modelview;" "\n"

"out vec3 frag_position, frag_normal;" "\n"
"out vec2 frag_uv;" "\n"

"void main() {" "\n"
    "modelview = camera * transform;" "\n"
    "vec4 eye_position = modelview * vec4(position, 1.0);" "\n"
    "gl_Position = projection * eye_position;" "\n"
    "frag_position = eye_position.xyz;" "\n"
    "frag_normal   = (modelview * vec4(normal, 0.0)).xyz;" "\n"
    "frag_uv = uv;" "\n"
"}";

std::string shader::fFlat =
"#version 150" "\n"

"in mat4 modelview;" "\n"

"in vec3 frag_position, frag_normal;" "\n"
"in vec2 frag_uv;" "\n"

"uniform vec4 color;" "\n"

"const vec3 light_direction = vec3(0.408248, -0.816497, -0.408248);" "\n"

"out vec4 fragColor;" "\n"

"void main() {" "\n"
    "vec3 normal = normalize(frag_normal);" "\n"

    "float shade = 0.5 * (-dot(normal, normalize(light_direction)) + 1.0);" "\n"
    "fragColor = vec4(color.rgb * shade, color.a);" "\n"
"}";

