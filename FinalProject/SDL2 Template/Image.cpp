//
//  Image.cpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/16/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

#include "Image.hpp"
using namespace std;


#define LE_SWAP(a, b) ( static_cast<uint16_t>(a) + (static_cast<uint16_t>(b) << 8) )

image::TGA::TGA(const string& filename) {
    ifstream source{filename, ios::in | ios::binary};
    if (!source) { throw invalid_argument("Error: could not open file " + filename + "."); }
    
    unique_ptr<char unsigned[]> header{ new char unsigned[18]{} };
    if (!source.read(reinterpret_cast<char*>(header.get()), 18)) { throw invalid_argument("Error: TGA header incomplete."); }
    
    if (header[16] != 24 && header[16] != 32) { throw invalid_argument("Error: invalid bit depth."); }
    
    id_length = header[0], color_map_type = header[1], data_type_code = header[2];
    color_map_origin = LE_SWAP(header[3], header[4]), color_map_length = LE_SWAP(header[5], header[6]);
    color_map_depth = header[7];
    x_origin = LE_SWAP(header[8], header[9]), y_origin = LE_SWAP(header[10], header[11]);
    width = LE_SWAP(header[12], header[13]); height = LE_SWAP(header[14], header[15]);
    bits_per_pixel = header[16], image_descriptor = header[17];
    
    int size  = width * ((bits_per_pixel + 7) / 8) * height;
    
    switch (data_type_code) {
        case 0x02:
            if (source.ignore(id_length).eof()) {
                throw invalid_argument("Error: incomplete id string in " + filename);
            }
            
            if (source.ignore(color_map_length * (color_map_depth/8)).eof()) {
                throw invalid_argument("Error: incomplete color map in " + filename);
            }

            pixels = unique_ptr<char[]>(new char[size]());
            // source.read(pixels.get(), size);
            // if (source.gcount() < size) // also fine!
            if (source.read(pixels.get(), size).gcount() < size) {
                throw invalid_argument("Error: incomplete image data in " + filename);
            }
            break;
            
        default:
            throw invalid_argument("Error: unrecognized TGA format.");
            break;
    }
}
