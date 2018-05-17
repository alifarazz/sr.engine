#ifndef __OBJMESH_H__
#define __OBJMESH_H__ 1

#include <vector>
#include "geometry.hpp"

class ObjMesh {
private:
  std::vector<Vec3f> verts_;
  std::vector<std::vector<int>> faces_;
public:
  ObjMesh(std::string filename);
  ~ObjMesh();

  std::vector<Vec3f>& getVerts();
  std::vector<std::vector<int>>& getFaces();
};

#endif
