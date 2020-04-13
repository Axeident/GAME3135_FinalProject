//
//  Vertex.cpp
//  GLhomework1
//
//  Created by Nevin Flanagan on 1/31/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include "Vertex.hpp"

namespace {
    template<typename block>
    class Attribute {
    public:
		Attribute(GLint i, GLint size, GLuint offset, GLenum kind = GL_FLOAT) : index{ i }
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, sizeof(block), reinterpret_cast<void*>(offset));
		}
        ~Attribute()
        {
            glDisableVertexAttribArray(index);
        }
    private:
        GLint index;
    };
}

namespace gl {
    template<>
    Name<Vertex::Array>::Name()
    {
        glGenVertexArrays(1, &name);
    }
    
    template<>
    Name<Vertex::Array>::~Name()
    {
        if (name) { glDeleteVertexArrays(1, &name); }
    }
    
    Vertex::Array::Array():
        Name()
    {}
    
    Vertex::Array& Vertex::Array::Bind(Buffer& storage)
    {
        Activate();
		storage.Activate(GL_ARRAY_BUFFER);
		Attribute<Vertex> gr{ group, 1, 0, GL_INT };
        Attribute<Vertex> pos{position, 3, sizeof(Vertex::group)};
        Attribute<Vertex> norm{normal, 3, sizeof(Vertex::group) + sizeof(Vertex::position)};
        Attribute<Vertex> surf{uv, 2, sizeof(Vertex::group) + sizeof(Vertex::position) + sizeof(Vertex::normal)};
        Deactivate();
        return *this;
    }
    
    void Vertex::Array::Activate() const
    {
        glBindVertexArray(name);
    }
    
    void Vertex::Array::Deactivate()
    {
        glBindVertexArray(0);
    }
    constexpr GLint Vertex::Array::position, Vertex::Array::normal, Vertex::Array::uv, Vertex::Array::group;
}
