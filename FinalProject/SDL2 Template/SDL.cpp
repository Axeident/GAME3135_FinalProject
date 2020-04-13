//
//  SDLApplication.cpp
//  GLTest
//
//  Created by Nevin Flanagan on 12/28/15.
//  Copyright © 2015 PlaySmith. All rights reserved.
//

#include "SDL.hpp"

#include <string>
#include <SDL2/SDL.h>

using std::runtime_error;

sdl::Library::Library(Uint32 flags)
{
    if (SDL_Init(flags) < 0) throw sdl::Exception("Video initialization failed: ");
}

sdl::Library::~Library()
{
    SDL_Quit();
}

sdl::Exception::Exception(std::string const& message)
:   runtime_error {message + SDL_GetError()}
{}
