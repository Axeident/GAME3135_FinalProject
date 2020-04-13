//
//  World.cpp
//  GLProject1
//
//  Created by Nevin Flanagan on 1/17/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include "Mesh.hpp"
#include "MeshFile.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using glm::vec3; using glm::vec4; using glm::mat4;

using namespace std;

vector<GLushort> loadShape(const string& filename, gl::Vertex::Array& vertexArray)
{
    mesh::OBJ source {filename};

    gl::Buffer vbo, ebo;
    vbo.Load(GL_ARRAY_BUFFER, GL_STATIC_DRAW, source.vData());
    vertexArray.Bind(vbo);

    vertexArray.Activate();
    ebo.Load(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, source.fData());
    vertexArray.Deactivate();
    
	ebo.Release();
	vbo.Release();

    return source.faceSegments();
}

namespace gl {
    Mesh::Mesh(const std::string& filename)
    :   faces {loadShape(filename, data)}
    {}

	Mesh::Mesh(gl::Buffer& vbo, gl::Buffer& ebo, const std::vector<GLushort>& face_list)
		:	faces {face_list}
	{
		data.Bind(vbo);

		data.Activate();
		ebo.Activate(GL_ELEMENT_ARRAY_BUFFER);
		data.Deactivate();
		ebo.Deactivate(GL_ELEMENT_ARRAY_BUFFER);
	}
    
    void Mesh::Render() const
    {
        data.Activate();
        GLushort position = 0;
        for (GLushort range: faces) {
            glDrawElements(GL_TRIANGLE_FAN, range, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(position * sizeof(GLshort)));
            position += range;
        }
        data.Deactivate();
    }
    
    Object::Object(const Mesh*&& m, gl::Program& prog)
    : mesh {m}, program{prog}
    {}
    
    Object::Object(shared_ptr<const Mesh> m, gl::Program& prog)
		: mesh{ m }, program{ prog }
    {}
    
    void Object::Render( ) const
    {
        program.Activate();
        program.Uniform<vec4>("color") = color;
		program.Uniform<GLfloat>("shininess") = shininess;
		program.Uniform<vec3>("specular_color") = highlight;
        program.Uniform<mat4>("transform") = transform;
		program.Uniform<GLint>("surface") = surface ? surface->Activate() : Texture::Deactivate();
        mesh->Render();
    }
    
    void Object::Rotate(float angle, vec3 axis)
    {
        transform = glm::rotate(transform, angle, axis);
    }
    
    void Object::Translate(vec3 distance)
    {
        transform = glm::translate(transform, distance);
    }
}
