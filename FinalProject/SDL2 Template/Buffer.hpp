//
//  Buffer.hpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/23/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#include <cstddef>
#include <vector>

#include "OpenGL.hpp"

namespace gl {
    class Buffer: public Name<Buffer> {
    public:
        Buffer();

        void Activate(GLenum buffer) const;
        
        static void Deactivate(GLenum target);

		void Release();
        
        template<typename E>
        Buffer& Load(GLenum buffer, GLenum role, E* source, size_t count)
        {
            Activate(buffer);
            glBufferData(buffer, sizeof(E) * count, source, role);
            return *this;
        }
        
        template<typename E>
        Buffer& Load(GLenum buffer, GLenum role, std::vector<E> const& source)
        {
            return Load(buffer, role, source.data(), source.size());
        }
    protected:
        Buffer(GLuint n);
    };

}
