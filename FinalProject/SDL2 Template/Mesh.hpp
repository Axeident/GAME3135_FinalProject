//
//  World.hpp
//  GLProject1
//
//  Created by Nevin Flanagan on 1/17/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "OpenGL.hpp"
#include "Buffer.hpp"
#include "Vertex.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

#include "glm/glm.hpp"

namespace gl {
    class Mesh {
    public:
        Mesh(const std::string& filename);
		Mesh(gl::Buffer& vbo, gl::Buffer& ebo, const std::vector<GLushort>& faces);
        void Render() const;
    private:
        gl::Vertex::Array data;
        std::vector<GLushort> faces;
    };
    class Object {
    public:
        Object(const gl::Mesh*&& m, gl::Program& program);
        Object(std::shared_ptr<const gl::Mesh> m, gl::Program& program);
        
        void Render ( ) const;
        
        void Rotate(float angle, glm::vec3 axis);
        void Translate(glm::vec3 distance);
        
		glm::vec4 color{ 1.0f };
		gl::Texture* surface{ nullptr };
		GLfloat shininess = 0.0f;
		glm::vec3 highlight{ 0.0f };
    protected:
        std::shared_ptr<const gl::Mesh> mesh;
        gl::Program& program;
        glm::mat4 transform;
    };
    class StellarObject : public Object {
        using Object::Object; //Makes the StellarObject explicitly inherit Object's constructor
        glm::vec3 offset;
        glm::quat revolutionSpeed;
        
    };
}
