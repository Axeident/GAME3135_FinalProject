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
#include <vector>
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
    vector<gl::StellarObject*> objects; //needs to be indexable for the camera to grab the correct planet to track
    gl::StellarObject* cameraFocus; //needed so that the camera can update to the tracking position each frame
	list<function<bool(float, float)>> updates;
    gl::Program plain;
    Transform world_camera;
    Transform cameraDefault;
    
    void setup_camera();
    void build_world();
    
    void processKey(const SDL_Keysym& keysym);
    void events();
    
    void apply_time(float seconds, float lifetime);
    void render();
};

Project::Project(GLsizei width, GLsizei height)
:   SDL2 {SDL_INIT_VIDEO}, display {width, height}, cameraFocus{nullptr},
    plain {
        gl::Shader {GL_VERTEX_SHADER, from_file("layer_vertex.glsl")}, gl::Shader {GL_FRAGMENT_SHADER, from_file("layer_fragment.glsl")},
        map<string, GLint> {
            { "position", gl::Vertex::Array::position },
            { "normal", gl::Vertex::Array::normal },
            { "uv", gl::Vertex::Array::uv },
			{ "group", gl::Vertex::Array::group },
        }
    },
	world_camera{ vec3{0.0f, 6.0f, -20.0f} , glm::angleAxis(atan2(-6.0f, 20.0f), vec3{ 1.0f, 0.0f, 0.0f }) },
    cameraDefault{ world_camera }
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
    plain.Uniform<mat4>("projection") = glm::perspectiveLH(1.0f, 4.0f/3.0f, 1.0f, 250.0f);
    plain.Uniform<mat4>("transform") = glm::mat4{};
}

void Project::build_world()
{
    auto& objs = objects;
    
    shared_ptr<const gl::Mesh> ball {new gl::Mesh{"Objects/sphere.obj"}};
    shared_ptr<const gl::Mesh> planet{ new gl::Mesh{"Objects/Earth.obj"} };

    //THIS SHIT NEEDS A FUNCTION

    // SUN
    gl::StellarObject* s = new gl::StellarObject{ planet, plain };
    s->SetStats(vec3{ 0.0f, 0.0f, 0.0f }, 100.0f, 100.0f);
    s->Scale(0.8f);
    s->color = vec4{ 1.0f, 1.0f, 0.0f, 1.0f };
    s->shininess = 1;
    s->highlight = vec3{ 0.1f };
    s->surface = new gl::Texture(image::TGA{ "Objects/2k_sun.tga" });
    objs.push_back(s);

    //MERCURY
    gl::StellarObject* a = new gl::StellarObject{ planet, plain };
    a->SetStats(vec3{ 0.39f, 0.0f, 0.0f }, 0.241f, 58.646f);
    a->Translate(a->orbitDistance);
    a->Scale(0.382f);
    a->shininess = 1;
    a->highlight = vec3{ 0.1f };
    a->surface = new gl::Texture(image::TGA{ "Objects/2k_mercury.tga" });
    objs.push_back(a);

    //VENUS
    gl::StellarObject* b = new gl::StellarObject{ planet, plain };
    b->SetStats(vec3{ 0.723f, 0.0f, 0.0f }, 0.616f, 116.75f);
    b->Translate(b->orbitDistance);
    b->Scale(0.949f);
    b->shininess = 1;
    b->highlight = vec3{ 0.1f };
    b->surface = new gl::Texture(image::TGA{ "Objects/2k_venus_atmosphere.tga" });
    objs.push_back(b);

    //EARTH
	gl::StellarObject* c = new gl::StellarObject{ planet, plain };
    c->SetStats(vec3{ 1.0f, 0.0f, 0.0f }, 1.0f, 1.0f);
    c->Translate(c->orbitDistance);
    c->Scale(1.0f);
	c->shininess = 1;
	c->highlight = vec3{ 0.1f };
    c->surface = new gl::Texture(image::TGA{ "Objects/Earth_TEXTURE_CM.tga" });
	objs.push_back(c);

    //MARS
    gl::StellarObject* d = new gl::StellarObject{ planet, plain };
    d->SetStats(vec3{ 1.524f, 0.0f, 0.0f }, 1.88f, 1.02f);
    d->Translate(d->orbitDistance);
    d->Scale(0.532f);
    d->shininess = 1;
    d->highlight = vec3{ 0.1f };
    d->surface = new gl::Texture(image::TGA{ "Objects/2k_mars.tga" });
    objs.push_back(d);

    //JUPITER
    gl::StellarObject* e = new gl::StellarObject{ planet, plain };
    e->SetStats(vec3{ 0.0f, 0.0f, 5.203f }, 12.0f, 0.415f);
    e->Translate(e->orbitDistance);
    e->Scale(11.194f);
    e->shininess = 1;
    e->highlight = vec3{ 0.1f };
    e->surface = new gl::Texture(image::TGA{ "Objects/2k_jupiter.tga" });
    objs.push_back(e);

    //SATURN
    gl::StellarObject* f = new gl::StellarObject{ planet, plain };
    f->SetStats(vec3{ 0.0f, 0.0f, 9.539f }, 29.0f, 0.445f);
    f->Translate(f->orbitDistance);
    f->Scale(9.459f);
    f->shininess = 1;
    f->highlight = vec3{ 0.1f };
    f->surface = new gl::Texture(image::TGA{ "Objects/2k_saturn.tga" });
    objs.push_back(f);

    //URANUS
    gl::StellarObject* g = new gl::StellarObject{ planet, plain };
    g->SetStats(vec3{ 0.0f, 0.0f, 19.18f }, 84.0f, 0.718f);
    g->Translate(g->orbitDistance);
    g->Scale(4.007f);
    g->shininess = 1;
    g->highlight = vec3{ 0.1f };
    g->surface = new gl::Texture(image::TGA{ "Objects/2k_uranus.tga" });
    objs.push_back(g);

    //NEPTUNE
    gl::StellarObject* h = new gl::StellarObject{ planet, plain };
    h->SetStats(vec3{ 0.0f, 0.0f, 30.06f },165.0f, 0.671f);
    h->Translate(h->orbitDistance);
    h->Scale(3.81f);
    h->shininess = 1;
    h->highlight = vec3{ 0.1f };
    h->surface = new gl::Texture(image::TGA{ "Objects/2k_neptune.tga" });
    objs.push_back(h);
}

void Project::apply_time(float seconds, float lifetime) {
	bool remove = false;

    //For each object, rotate around the 'up' axis by 'degPerSec' times 'seconds since last frame' every frame
    for (gl::StellarObject* st : objects) {
        st->Update(seconds);
    }

    //Update camera position
    if (cameraFocus == nullptr) {
        plain.Uniform<mat4>("camera") = cameraDefault;
    }
    else
    {
        //Updates location while ignoring rotation
        Transform newPos{ cameraFocus->getLocation() + world_camera.position, world_camera.rotation };
        plain.Uniform<mat4>("camera") = newPos;
    }
}

void Project::processKey(const SDL_Keysym& keysym) {
    switch( keysym.sym ) {
        case SDLK_ESCAPE:
            running = false;
            break;
        case SDLK_0: //Default view
            cameraFocus = nullptr;
            break;
        case SDLK_1: //Sun view
            cameraFocus = objects[0];
            break;
        case SDLK_2: //Mercury view
            cameraFocus = objects[1];
            break;
        case SDLK_3: //Venus view
            cameraFocus = objects[2];
            break;
        case SDLK_4: //Earth view
            cameraFocus = objects[3];
            break;
        case SDLK_5: //Mars view
            cameraFocus = objects[4];
            break;
        case SDLK_6: //Jupiter view
            cameraFocus = objects[5];
            break;
        case SDLK_7: //Saturn view
            cameraFocus = objects[6];
            break;
        case SDLK_8: //Uranus view
            cameraFocus = objects[7];
            break;
        case SDLK_9: //Neptune view
            cameraFocus = objects[8];
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
