//
//  Buffer.cpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/23/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include "Buffer.hpp"

namespace gl {
    template<>
    Name<Buffer>::Name()
    {
        glGenBuffers(1, &name);
    }
    
    template<>
    Name<Buffer>::~Name()
    {
        if (name) { glDeleteBuffers(1, &name); }
    }
    
    Buffer::Buffer():
        Name()
    {}

    Buffer::Buffer(GLuint n):
        Name(n)
    {}
    
    void Buffer::Activate(GLenum target) const
    {
        if (!name) { throw std::runtime_error("Attempted to activate invalid OpenGL buffer"); }
        glBindBuffer(target, name);
    }
    
    void Buffer::Deactivate(GLenum target)
    {
        glBindBuffer(target, 0);
    }

	void Buffer::Release()
    {
		name = 0;
	}
}
