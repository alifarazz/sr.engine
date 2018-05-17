#include "h/objmesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string& s, char delim)
{
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> tokens;
  while (getline(ss, item, delim)) {
    tokens.push_back(item);
  }
   return tokens;
}

ObjMesh::ObjMesh(std::string filename)
{
  std::ifstream in;

  in.open(filename, std::ifstream::in);
  if (in.fail())
    return;

  std::string line;
  while (!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line);

    char trash;
    if (!line.compare(0, 2, "v ")) {
      iss >> trash;
      Vec3f v;

      for (int i = 0; i < 3; i++) iss >> v.raw[i];
      verts_.push_back(v);

    } else if (!line.compare(0, 2, "f ")) {
      std::vector<int> faces;
      int itrash;
      int idx;

      iss >> trash;
      while (iss >> idx >> trash >> itrash >> trash >> itrash) {
	// while (iss >> idx >> trash >> trash >> itrash) {
	idx--;  // in wavefront obj, all indices start at 1
	// for (auto i : faces) std::cout << i;
	// std::cout << std::endl;
	faces.push_back(idx);
      }
      faces_.push_back(faces);
    }
  }
  std::cout << "#ObjMesh v# " << verts_.size() << " #f " << faces_.size()
	    << std::endl;
}

ObjMesh::~ObjMesh() {}

size_t ObjMesh::nverts() { return verts_.size(); }

size_t ObjMesh::nfaces() { return faces_.size(); }

std::vector<int> &ObjMesh::getFace(int idx) {
    return faces_[idx];
}

Vec3f ObjMesh::getVert(int idx) {
  return verts_[idx];
}
