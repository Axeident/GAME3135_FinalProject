#pragma once

#include <string>
#include <list>
#include <unordered_map>

#include "OpenGL.hpp"

namespace gl {
    class Shader: public Name<Shader> {
    public:
        Shader(GLenum kind, const std::string& source);
    protected:
        friend class Program;
    };
    
    class Program: public Name<Program> {
		struct UniformReference {
		public:
			UniformReference(GLint i, const Program& prog) : index{ i }, program{ prog } {}
			const GLint index;
			const Program& program;
		};
    public:
        Program(Shader const& vertex, Shader const& fragment);
        template <typename M>
        Program(Shader const& vertex, Shader const& fragment, const M& bindings)
        {
            Bind(bindings);
            Link(std::list<const Shader*> {&vertex, &fragment});
        }
        
        template<typename C>
        Program(const C& sh)
        {
            Link(std::list<const Shader*>{begin(sh), end(sh)});
        }
        
        template<typename C, typename M>
        Program(const C& sh, const M& bindings)
        {
            Bind(bindings);
            Link(std::list<const Shader*>{begin(sh), end(sh)});
        }
        
        void Activate( ) const;
        
        template <typename V>
        class UniformAccessor: private UniformReference {
        public:
            UniformAccessor& operator= (const V& value);
            
            operator V () const;
        private:
            friend class Program;
			using UniformReference::UniformReference;
        };

		template <typename V>
		class UniformAccessor<V[]> : private UniformReference {
		public:
			template <std::size_t n>
			UniformAccessor<V[]>& operator= (const V (&values)[n]) {
				glProgramUniformMatrix4fv(program.name, index, n, GL_FALSE, glm::value_ptr(values[0]));
				return *this;
			}

			UniformAccessor<V[]>& operator= (const std::pair<std::size_t, const V*> values);

			template <std::size_t n>
			void Copy(V(&target)[n]) const {
				glGetUniformfv(program.name, index, glm::value_ptr(target[0]));
			}

			void Copy(V* target) const;
		private:
			friend class Program;
			using UniformReference::UniformReference;
		};
        
        template <typename V>
        UniformAccessor<V> Uniform(const std::string& uniformName)
        {
            return UniformAccessor<V>{glGetUniformLocation(name, uniformName.c_str()), *this};
        }
        
        template <typename V>
        const UniformAccessor<V> Uniform(const std::string& uniformName) const
        {
            return UniformAccessor<V>{glGetUniformLocation(name, uniformName.c_str()), *this};
        }
        
    private:

        template <typename M>
        void Bind(const M& bindings)
        {
            for (const std::pair<std::string, GLint>& binding: bindings) {
                glBindAttribLocation(name, binding.second, binding.first.c_str());
            }
        }
        void Link(std::list<Shader const*> sh);
    };
}
