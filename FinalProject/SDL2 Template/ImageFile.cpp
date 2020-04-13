#include "Image.hpp"
#include "Texture.hpp"

#include <stdexcept>

using namespace std;

template<>
gl::Texture& gl::Texture::Load<image::TGA>(const image::TGA& source)
{
    if (source.bits_per_pixel != 24 && source.bits_per_pixel != 32) {
        throw std::invalid_argument("Error: TGA not direct-color uncompressed.");
    }
	bool alpha = source.bits_per_pixel == 32;
    Activate();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    switch (((source.width * source.bits_per_pixel) / 8) % 8) {
        case 0:
            glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
            break;
        case 4:
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            break;
        case 2:
        case 6:
            glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
            break;
        default:
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            break;
    }
    glTexImage2D(
                 GL_TEXTURE_2D, 0,                  /* target, level of detail */
                 alpha? GL_RGBA8: GL_RGB8,          /* internal format */
                 source.width, source.height, 0,    /* width, height, border */
                 alpha? GL_RGBA: GL_BGR, GL_UNSIGNED_BYTE,	/* external format, type */
                 source.pixels.get()                /* pixels */
                 );
    return *this;
}