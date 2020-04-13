#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"

#include <string>
#include <memory>
using namespace std;

using glm::vec4; using glm::mat4;

namespace {
    template <typename GET, typename LOG>
    string show_log_info(GLuint object, GET glGet__iv, LOG glGet__InfoLog)
    {
        GLint log_length;
        
        glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
        unique_ptr<char[]> buffer{new char[log_length]};
        glGet__InfoLog(object, log_length, NULL, buffer.get());
        return string{buffer.get()};
    }
}

namespace gl {
    template<>
    Name<Shader>::~Name() {
        if (name) { glDeleteShader(name); }
    }
    
    Shader::Shader(GLenum type, string const& source)
    :   Name {glCreateShader(type)}
    {
        GLint length = static_cast<GLint>(source.length());
        char const* text = source.c_str();
        glShaderSource(name, 1, &text, &length);
        glCompileShader(name);
        GLint shaderStatus = false;
        glGetShaderiv(name, GL_COMPILE_STATUS, &shaderStatus);
        if (!shaderStatus) { throw invalid_argument{"Failed to compile shader: " + show_log_info(name, glGetShaderiv, glGetShaderInfoLog)};}
    }
    
    template<>
    Name<Program>::Name()
    :   name(glCreateProgram())
    {}
    
    template<>
    Name<Program>::~Name()
    {
        if (name) { glDeleteProgram(name); }
    }
    
    Program::Program(const Shader& vertex, const Shader& fragment)
    {
        Link(list<const Shader*> {&vertex, &fragment});
    }
    
    void Program::Activate( ) const
    {
        glUseProgram(name);
    }
    
    template<>
    Program::UniformAccessor<GLint>::operator GLint() const
    {
        GLint value;
        glGetUniformiv(program.name, index, &value);
        return value;
    }
    
    template<>
    Program::UniformAccessor<GLuint>::operator GLuint() const
    {
        GLuint value;
        glGetUniformuiv(program.name, index, &value);
        return value;
    }

	template<>
	Program::UniformAccessor<GLfloat>::operator GLfloat() const
	{
		GLfloat value;
		glGetUniformfv(program.name, index, &value);
		return value;
	}

	template<>
	Program::UniformAccessor<glm::vec3>::operator glm::vec3() const
	{
		glm::vec3 value;
		glGetUniformfv(program.name, index, glm::value_ptr(value));
		return value;
	}

    template<>
    Program::UniformAccessor<glm::vec4>::operator glm::vec4() const
    {
		glm::vec4 value;
        glGetUniformfv(program.name, index, glm::value_ptr(value));
        return value;
    }
    
    template<>
    Program::UniformAccessor<glm::mat4>::operator glm::mat4() const
    {
		glm::mat4 value;
        glGetUniformfv(program.name, index, glm::value_ptr(value));
        return value;
    }

	template <>
	void Program::UniformAccessor<glm::mat4[]>::Copy(glm::mat4* target) const {
		glGetUniformfv(program.name, index, glm::value_ptr(*target));
	}
    
    template<>
    Program::UniformAccessor<GLint>& Program::UniformAccessor<GLint>::operator=(const GLint& value)
    {
        glProgramUniform1i(program.name, index, value);
        return *this;
    }
    
    template<>
    Program::UniformAccessor<GLuint>& Program::UniformAccessor<GLuint>::operator=(const GLuint& value)
    {
        glProgramUniform1ui(program.name, index, value);
        return *this;
    }
	
	template<>
	Program::UniformAccessor<GLfloat>& Program::UniformAccessor<GLfloat>::operator=(const GLfloat& value)
	{
		glProgramUniform1f(program.name, index, value);
		return *this;
	}

	template<>
	Program::UniformAccessor<glm::vec3>& Program::UniformAccessor<glm::vec3>::operator=(const glm::vec3& value)
	{
		glProgramUniform3fv(program.name, index, 1, glm::value_ptr(value));
		return *this;
	}
    
    template<>
    Program::UniformAccessor<glm::vec4>& Program::UniformAccessor<glm::vec4>::operator=(const glm::vec4& value)
    {
        glProgramUniform4fv(program.name, index, 1, glm::value_ptr(value));
        return *this;
    }
    
    template<>
    Program::UniformAccessor<glm::mat4>& Program::UniformAccessor<glm::mat4>::operator= (const glm::mat4& value)
    {
        glProgramUniformMatrix4fv(program.name, index, 1, GL_FALSE, glm::value_ptr(value));
        return *this;
    }

	template <>
	Program::UniformAccessor<glm::mat4[]>& Program::UniformAccessor<glm::mat4[]>::operator= (pair<size_t, const glm::mat4*> values) {
		glProgramUniformMatrix4fv(program.name, index, values.first, GL_FALSE, glm::value_ptr(*values.second));
		return *this;
	}
    
    void Program::Link(std::list<const Shader*> sh)
    {
        for (const Shader* shader: sh) { glAttachShader(name, shader->name); }
        glLinkProgram(name);
        for (const Shader* shader: sh) { glDetachShader(name, shader->name); }
        GLint linkStatus = false;
        glGetProgramiv(name, GL_LINK_STATUS, &linkStatus);
        if (!linkStatus) { throw invalid_argument{"Failed to link program: " + show_log_info(name, glGetProgramiv, glGetProgramInfoLog)}; }
    }
}
