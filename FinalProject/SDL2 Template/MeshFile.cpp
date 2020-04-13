#include "MeshFile.hpp"
#include <sstream>
using namespace std;

namespace mesh {
	OBJ::Index::Index(const string& encoding, GLshort pCount, GLshort tCount, GLshort nCount)
	{
		istringstream description{ encoding };

		p = Enter(description, pCount);
		t = Enter(description, tCount);
		n = Enter(description, nCount);
	}

	OBJ::OBJ(const string& filename)
	{
		string line;
		vector<pair<gl::Point3, int>> vList;
		vector<gl::Vector3> vnList;
		vector<gl::Vector2> vtList;
		unordered_map<Index, GLshort, OBJ::hash> verts;
		int current_group = 0;
		int group_count = 0;
		for (ifstream source{ filename }; getline(source, line); ) {
			istringstream input{ line };
			string command;
			input >> command;
			if (command == "#vg") {
				string group_name;
				input >> ws >> group_name;
				if (group_name == "") {
					current_group = 0;
				}
				else {
					if (groups.count(group_name) > 0) {
						current_group = groups[group_name];
					}
					else {
						group_count += 1;
						groups[group_name] = group_count;
						current_group = group_count;
					}
				}
			}
			else if (command == "v") {
				gl::Point3 v;
				input >> v.x >> v.y >> v.z;
				vList.emplace_back(v, current_group);
			}
			else if (command == "vt") {
				gl::Vector2 t;
				input >> t.x >> t.y;
				vtList.push_back(t);
			}
			else if (command == "vn") {
				gl::Vector3 n;
				input >> n.x >> n.y >> n.z;
				vnList.push_back(n);
			}
			else if (command == "f") {
				string corner;
				int corners = 0;
				while (input >> corner) {
					Index id(corner, vList.size(), vtList.size(), vnList.size());
					if (!verts.count(id)) {
						gl::Vertex v;
						if (id.p >= 0) { v.position = vList[id.p].first; v.group = vList[id.p].second; }
						if (id.t >= 0) { v.uv = vtList[id.t]; }
						if (id.n >= 0) { v.normal = vnList[id.n]; }
						verts[id] = vertices.size();
						vertices.push_back(v);
					}
					corners++;
					faces.push_back(verts[id]);
				}
				ranges.push_back(corners);
			}
		}
	}
}