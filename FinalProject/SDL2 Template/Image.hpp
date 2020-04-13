//
//  Image.hpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/16/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#include <memory>
#include <string>

namespace image {
    struct TGA {
    public:
        TGA(const std::string& filename);
        
        char  id_length;
        char  color_map_type;
        char  data_type_code;
        unsigned short  color_map_origin;
        unsigned short  color_map_length;
        char  color_map_depth;
        unsigned short  x_origin;
        unsigned short  y_origin;
        unsigned short  width;
        unsigned short  height;
        char  bits_per_pixel;
        char  image_descriptor;
        std::unique_ptr<char[]> pixels;
    };
}
