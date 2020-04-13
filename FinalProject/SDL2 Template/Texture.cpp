//
//  Texture.cpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/16/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include "Texture.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace gl {
    template<>
    Name<Texture>::Name() 
	{
        glGenTextures(1, &name);
    }
    
    template<>
    Name<Texture>::~Name() 
	{
        if (name) { glDeleteTextures(1, &name); }
    }
    
    Texture::Texture():
        Name()
    {}
    
    GLint Texture::Activate(GLint index) const 
	{
        glActiveTexture(index + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, name);
        return index;
    }
    
    GLint Texture::Deactivate(GLint index) 
	{
        glActiveTexture(index + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        return index;
    }

	void Texture::init()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, glm::value_ptr(glm::vec4{ 1.0f }));
	}
}
