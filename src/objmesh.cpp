#include "./h/objmesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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
    int itrash;
    if (!line.compare(0, 2, "v ")) {
      iss >> trash;
      Vec3f v;

      for (int i = 0; i < 3; i++) iss >> v.raw[i];
      verts_.push_back(v);
    } else if (!line.compare(0, 3, "vt")) {
      float u, v, w;

      iss >> trash;  // eat one white-space
      iss >> u >> v >> w;
      vertTex_.push_back(Vec3f(u, v, w));
    } else if (!line.compare(0, 2, "f ")) {
      std::vector<int> faces;
      std::vector<int> facesTex;
      int idxFace, idxTex;

      iss >> trash;  // eat one white-space
      while (iss >> idxFace >> trash >> idxTex >> trash >> itrash) {
	// while (iss >> idx >> trash >> trash >> itrash) {
	faces.push_back(idxFace - 1);  // in wavefront obj, all indices start at 1
	facesTex.push_back(idxTex - 1);
      }
      faces_.push_back(faces);
      facesTex_.push_back(facesTex);
    }
  }
  std::cout << "#ObjMesh v# " << verts_.size() << " #f " << faces_.size() << std::endl;
}

ObjMesh::ObjMesh() {}

ObjMesh::~ObjMesh() {}

size_t ObjMesh::nVerts() { return verts_.size(); }

size_t ObjMesh::nFaces() { return faces_.size(); }

size_t ObjMesh::nVertTex() { return vertTex_.size(); }

size_t ObjMesh::nFacesTex() { return facesTex_.size(); }

Vec3f ObjMesh::getVert(int idx) { return verts_[idx]; }

Vec3f ObjMesh::getVertTex(int idx) { return vertTex_[idx]; }

std::vector<int>& ObjMesh::getFace(int idx) { return faces_[idx]; }

std::vector<int>& ObjMesh::getFaceTex(int idx) { return facesTex_[idx]; }
