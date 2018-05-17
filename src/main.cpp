// In his exalted name
#include <iostream>
#include <sstream>
#include <tuple>

#include "./h/tgaimage.h"
#include "h/objmesh.h"
#include "h/utils.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);

void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
  bool isSteep = false;
  int dx, dy;
  int error2, derror2;

  if (std::abs(x0 - x1) < std::abs(y0 - y1)) {  // if m > 1 then transpose
    std::swap(x0, y0);
    std::swap(x1, y1);
    isSteep = true;
  }
  if (x0 > x1) {  // right-to-left
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  dx      = x1 - x0;
  dy      = y1 - y0;
  derror2 = std::abs(dy) * 2;
  error2  = 0;
  int y   = y0;
  for (int x = x0; x <= x1; x++) {  // dx > dy
    if (isSteep)
      image.set(y, x, color);
    else
      image.set(x, y, color);
    // Each line can't be pixel prefect,
    // so at each iteration, there exists an error.
    // To compensate for it, we accommulate the error
    // at each iteration and if the error is big enough
    // to become a pixel, we adjust y accordingly.
    // To avoid the use of floats, we multipy the errors
    // by 2 and use int types.
    error2 += derror2;
    if (error2 > dx) {
      y += (y1 > y0) ? 1 : -1;
      error2 -= dx * 2;
    }
  }
}

void draw_wireframe(ObjMesh &mesh, TGAImage &image, int width, int height)
{
  for (size_t i = 0; i < mesh.nfaces(); i++) {
    std::vector<int> f = mesh.getFace(i);
    for (int j = 0; j < 3; j++) {
      Vec3f u = mesh.getVert(f[j]),
	    v = mesh.getVert(f[(j + 1) % 3]);  // next point in triangle
      draw_line(static_cast<int>((u.x + 1.0f) * width / 2.0f),
		static_cast<int>((u.y + 1.0f) * height / 2.0f),
		static_cast<int>((v.x + 1.0f) * width / 2.0f),
		static_cast<int>((v.y + 1.0f) * height / 2.0f), image, white);
    }
  }
}

Vec3f calc_barycentric(Vec2i coords[], Vec2i &P)
{
  Vec2i a = coords[0], b = coords[1], c = coords[2];
  Vec2i ab = b - a, ac = c - a, pa = a - P;
  Vec3f cp = Vec3f(ab.x, ac.x, pa.x) ^ Vec3f(ab.y, ac.y, pa.y);

  if (std::abs(cp.z) < 1)
    return Vec3f(-1, 1, 1);  // tri is degenerate, discard pls

  float u = cp.x / cp.z, v = cp.y / cp.z, w = 1.0f - ((cp.x + cp.y) / cp.z);
  return Vec3f(w, u, v);
}

void draw_tri(Vec2i coords[], TGAImage &image, TGAColor color)
{
  Vec2i boxmin(image.get_width() - 1, image.get_height() - 1);
  Vec2i boxmax(0, 0);
  Vec2i clamp = boxmin;

  for (int i = 0; i < 3; i++) {
    Vec2i coord = coords[i];
    for (int j = 0; j < 2; j++) {
      boxmax.raw[j] = std::min(clamp.raw[j], std::max(boxmax.raw[j], coord.raw[j]));
      boxmin.raw[j] = std::max(0, std::min(boxmin.raw[j], coord.raw[j]));
    }
  }
  std::cout << coords[0] << coords[1] << coords[2] << std::endl;
  std::cout << boxmin << std::endl;
  std::cout << boxmax << std::endl;
  std::cout << clamp << std::endl;

  Vec2i P;
  for (P.x = boxmin.x; P.x <= boxmax.x; P.x++)
    for (P.y = boxmin.y; P.y <= boxmax.y; P.y++) {
      Vec3f u = calc_barycentric(coords, P);
      if (u.x < 0 || u.y < 0 || u.z < 0)
	continue;
      image.set(P.x, P.y, color);  // it's inside tri, draw it
    }
}

int main(int argc, char *argv[])
{
  TGAImage image(200, 200, TGAImage::RGB);
  Vec2i triCoords[3] = {Vec2i(10, 10), Vec2i(100, 30), Vec2i(190, 160)};
  draw_tri(triCoords, image, red);
  image.flip_vertically();
  image.write_tga_file("tri_test.tga");
  return 0;
}

// int main()
// {
//   using namespace std;
//   string line("f 5//3 2//3 1//3");
//   char foo, foo1, foo2, foo3;
//   int bar, idx, br3;
//   istringstream iss(line);

//   iss >> foo >> bar >> foo1 >> foo2 >> idx >> br3;
//   cout << foo << "  " << bar << "  " << foo1 << "  " << foo2 << "  " << idx
//   <<
//       "  " << br3 << endl;

//   ObjMesh mesh("./mesh.obj");
//   return 0;
// }

// int main(int argc, char *argv[])
//{
//  int width = 2000, height = 2000;
//
//  TGAImage image(width, height, TGAImage::RGB);
//  if (argc == 1)
//    return -1;
//  ObjMesh mesh = std::string(argv[1]);
//
//  draw_wireframe(mesh, image, width, height);
//
//  image.flip_vertically();  // set origin to bottom left
//  image.write_tga_file("output.tga");
//  return 0;
//}
