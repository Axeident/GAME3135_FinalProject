#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "OpenGL.hpp"
#include "Vertex.hpp"

namespace mesh {
	class OBJ {
	public:
		OBJ(const std::string& filename);
		const std::vector<gl::Vertex>& vData() { return vertices; }
		const std::vector<GLushort>& fData() { return faces; }
		const std::vector<GLushort>& faceSegments() { return ranges; }
		const std::unordered_map<std::string, GLint>& vertex_groups() { return groups; }
	private:
		std::vector<gl::Vertex> vertices;
		std::vector<GLushort> faces;
		std::vector<GLushort> ranges;
		std::unordered_map<std::string, GLint> groups;

		struct Index {
			GLshort p, t, n;
			Index(const std::string& encoding, GLshort pCount, GLshort tCount, GLshort nCount);

			bool operator==(const Index& other) const
			{
				return ((p == other.p) || (p < 0 && other.p < 0))
					&& ((t == other.t) || (t < 0 && other.t < 0))
					&& ((n == other.n) || (n < 0 && other.n < 0));
			}
		private:
			GLshort Enter(std::istream& source, GLshort limit)
			{
				GLshort result;
				if (source.peek() != '/' && source >> result) {
					result = result < 0 ? limit - result : --result;
				}
				else {
					result = -1;
				}
				source.ignore();
				return result;
			}
		};

		class hash : public std::hash<std::string> {
		public:
			size_t operator() (const Index i) const
			{
				return std::hash<std::string>::operator () (std::string{ reinterpret_cast<char const*>(&i), sizeof(Index) });
			}
		};
	};
}