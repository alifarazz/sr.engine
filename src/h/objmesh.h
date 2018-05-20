#ifndef __OBJMESH_H__
#define __OBJMESH_H__ 1

#include <vector>
#include "./geometry.h"

class ObjMesh {
private:
  std::vector<Vec3f> verts_;
  std::vector<Vec3f> vertTex_;

  std::vector<std::vector<int>> faces_;
  std::vector<std::vector<int>> facesTex_;

public:
  ObjMesh(std::string filename);
  ObjMesh();
  ~ObjMesh();

  size_t nVerts();
  size_t nFaces();
  size_t nVertTex();
  size_t nFacesTex();

  Vec3f getVert(int idx);
  Vec3f getVertTex(int idx);
  std::vector<int>& getFace(int idx);
  std::vector<int>& getFaceTex(int idx);
};
#endif
