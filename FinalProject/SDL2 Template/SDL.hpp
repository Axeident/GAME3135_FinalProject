//
//  SDLApplication.hpp
//  GLTest
//
//  Created by Nevin Flanagan on 12/28/15.
//  Copyright © 2015 PlaySmith. All rights reserved.
//

#pragma once

#include <stdexcept>
#include <SDL2/SDL_stdinc.h>

namespace sdl {
    class Library {
    public:
        Library(Uint32 flags);
        ~Library();
    };
    
    class Exception: public std::runtime_error {
    public:
        Exception(const std::string& message);
    };
}
