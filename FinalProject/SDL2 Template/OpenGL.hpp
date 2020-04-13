//
//  OpenGL.h
//  GLhomework1
//
//  Created by Nevin Flanagan on 1/29/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <string>
#include <stdexcept>

#include "glm/glm.hpp"

namespace gl {
    template <typename T>
    class Name {
    public:
        operator bool() const { return name != 0; }
        Name& operator= (Name && source) { name = source.name; source.name = 0; return *this; }
    protected:
        Name();
        Name(Name && source) { name = source.name; source.name = 0; }
        Name(GLuint existing): name(existing) {}
        ~Name();
        
        GLuint name;
    };
    
    using Point3 = glm::vec3;
    using Vector2 = glm::vec2;
    using Vector3 = glm::vec3;
}

namespace shader {
    extern std::string vFlat;
    extern std::string fFlat;
}

#define TRAPGL(a) { auto e = glGetError(); if (e) throw std::runtime_error(std::string(a) + std::to_string(e)); }
