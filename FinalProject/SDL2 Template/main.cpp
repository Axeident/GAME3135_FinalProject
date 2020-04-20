//
//  GL1.cpp
//  GLProject1
//
//  Created by Nevin Flanagan on 1/9/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <functional>

#include "SDL.hpp"

#include "Vertex.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

#include "Image.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"

using glm::vec3; using glm::vec4; using glm::mat4;

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_keycode.h>

using namespace std;

string from_file(const string& filename) 
{
	ifstream in{ filename, ios::in | ios::binary };
	if (in) {
		ostringstream contents;
		contents << in.rdbuf();
		return contents.str();
	}
	throw (errno);
}

class Transform {
public:
    glm::vec3 position;
    glm::quat rotation;
    
	operator glm::mat4() const { return glm::mat4_cast(rotation) * glm::translate(mat4{}, -position); }
};

class Display {
public:
    Display(GLsizei width, GLsizei height);
    void Swap();
private:
    SDL_Window* window;
};

Display::Display (GLsizei width, GLsizei height)
:   window {nullptr}
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
    window = SDL_CreateWindow("", 20, 20, width, height, SDL_WINDOW_OPENGL);
    if (!window) { throw sdl::Exception( "Failed to create window" ); }
    if (!SDL_GL_CreateContext(window)) { throw sdl::Exception("Failed to create context"); }
    
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();
#endif
    
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    
    /* Setup our viewport. */
    glViewport( 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height) );
}

void Display::Swap()
{
    SDL_GL_SwapWindow(window);
}

class Project {
public:
    Project(GLsizei width, GLsizei height);
    int operator()(const vector<string>& args);
private:
    sdl::Library SDL2;
    Display display;
    bool running = true;
    list<gl::StellarObject*> objects;
	list<function<bool(float, float)>> updates;
    gl::Program plain;
    Transform world_camera;
    
    void setup_camera();
    void build_world();
    
    void processKey(const SDL_Keysym& keysym);
    void events();
    
    void apply_time(float seconds, float lifetime);
    void render();
};

Project::Project(GLsizei width, GLsizei height)
:   SDL2 {SDL_INIT_VIDEO}, display {width, height},
    plain {
        gl::Shader {GL_VERTEX_SHADER, from_file("layer_vertex.glsl")}, gl::Shader {GL_FRAGMENT_SHADER, from_file("layer_fragment.glsl")},
        map<string, GLint> {
            { "position", gl::Vertex::Array::position },
            { "normal", gl::Vertex::Array::normal },
            { "uv", gl::Vertex::Array::uv },
			{ "group", gl::Vertex::Array::group },
        }
    },
	world_camera{ vec3{0.0f, 6.0f, -20.0f} , glm::angleAxis(atan2(-6.0f, 20.0f), vec3{ 1.0f, 0.0f, 0.0f }) }
{
	gl::Texture::init();
    setup_camera();
    
    build_world();
}

void Project::setup_camera()
{
	plain.Activate();
    
    mat4 camera = world_camera;
	plain.Uniform<mat4>("camera") = camera;
    plain.Uniform<mat4>("projection") = glm::perspectiveLH(1.0f, 4.0f/3.0f, 1.0f, 50.0f);
    plain.Uniform<mat4>("transform") = glm::mat4{};
}

void Project::build_world()
{
    auto& w = objects;
    
    shared_ptr<const gl::Mesh> ball {new gl::Mesh{"sphere.obj"}};
    shared_ptr<const gl::Mesh> planetEarth{ new gl::Mesh{"Earth.obj"} };

	gl::StellarObject* a = new gl::StellarObject{ planetEarth, plain };
    a->SetStats(vec3{ 6.0f, 0.0f, 0.0f }, 1.0f, 1.0f);
    a->Translate(a->orbitDistance);
	//a->color = vec4{ 0.0f, 1.0f, 1.0f, 1.0f };
	a->shininess = 1;
	a->highlight = vec3{ 0.1f };
    a->surface = new gl::Texture(image::TGA{ "Earth_TEXTURE_CM.tga" });
	w.push_back(a);

	/*updates.emplace_back([a](float seconds, float lifetime)->bool {a->shininess = 20 * sin(lifetime) + 20; return true; });
	updates.emplace_back(
		[this](float seconds, float lifetime)->bool {
		plain.Uniform<vec3>("light_position") = vec3{ glm::rotate(mat4{}, seconds, vec3{0, 1, 0}) * vec4 { static_cast<vec3>(plain.Uniform<vec3>("light_position")), 1.0f } };
			return true; 
		}
	);*/
//	updates.emplace_back([a](float seconds, float lifetime)->bool {a->Rotate(seconds, vec3{ 0.0f, 1.0f, 0.0f }); return true; });
}

void Project::apply_time(float seconds, float lifetime) {
	bool remove = false;

	//for (auto which = updates.begin(); which != updates.end(); remove? (which = updates.erase(which)): ++which) {
	//	remove = !(*which)(seconds, lifetime);
	//}

    //For each object, rotate around the 'up' axis by 'degPerSec' times 'seconds since last frame' every frame
    for (gl::StellarObject* st : objects) {
        st->Update(seconds);
    }
}

void Project::processKey(const SDL_Keysym& keysym) {
    switch( keysym.sym ) {
        case SDLK_ESCAPE:
            running = false;
            break;
        default:
            break;
    }
}

void Project::events() {
    /* Our SDL event placeholder. */
    SDL_Event event;
    
    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {
        
        switch( event.type ) {
            case SDL_KEYDOWN:
                /* Handle key presses. */
                processKey( event.key.keysym );
                break;
            case SDL_QUIT:
                /* Handle quit requests (like Ctrl-c). */
                running = false;
                break;
        }
    }
}

void Project::render() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    for (gl::Object* object: objects) {
        object->Render();
    }
    
    display.Swap();
}

int Project::operator()(const vector<string>& args) {
    Uint32 lastCall = 0;
    while (running) {
        events();
        Uint32 time = SDL_GetTicks();
        apply_time(static_cast<float>(time - lastCall) / 1000.0f, static_cast<float>(time) / 1000.0f);
        lastCall = time;
        render();
    }
    return 0;
}

int main(int argc, char* argv[]) {
    try {
        return Project {640, 480}(vector<string>{argv, argv + argc});
    } catch (std::exception& e) {
        cerr << e.what() <<endl;
        return 1;
    }
}
