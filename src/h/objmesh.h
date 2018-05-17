#ifndef __OBJMESH_H__
#define __OBJMESH_H__ 1

#include <vector>
#include "geometry.h"

class ObjMesh {
private:
  std::vector<Vec3f> verts_;
  std::vector<std::vector<int>> faces_;
public:
  ObjMesh(std::string filename);
  ~ObjMesh();

  size_t nverts();
  size_t nfaces();
  Vec3f getVert(int idx);
  std::vector<int>& getFace(int idx);
};
#endif
