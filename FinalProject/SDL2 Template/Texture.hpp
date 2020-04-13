//
//  Texture.hpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/16/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#include "OpenGL.hpp"

namespace gl {
    
    class Texture: public Name<Texture> {
    public:
        Texture();
        
        template<typename T>
        Texture(const T& source): Name() { Load(source); }
        
        template <class T>
        Texture& Load(const T& source);
        
        GLint Activate(GLint index = 0) const;
        
        static GLint Deactivate(GLint index = 0);

		static void init();
    };
}
