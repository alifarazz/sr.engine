#include <iostream>
#include <sstream>
#include "./h/objmesh.hpp"
#include "./h/tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void draw_line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
  bool isSteep = false;

  if (std::abs(x0 - x1) < std::abs(y0 - y1)) {  // if m > 1 then transpose
    std::swap(x0, y0);
    std::swap(x1, y1);
    isSteep = true;
  }
  if (x0 > x1) {  // right-to-left
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  int dx = x1 - x0, dy = y1 - y0;
  int derror2 = std::abs(dy) * 2, error2 = 0;
  int y = y0;
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

// void draw_tri_xy(int x0, int y0, int x1, int y1, int x2, int y2,
//		 TGAImage& image, TGAColor& color)
// {
// }

int main(int argc, char* argv[])
{
  int width = 2000, height = 2000;

  TGAImage image(width, height, TGAImage::RGB);
  // ObjMesh mesh("./mesh.obj");
  if (argc == 1) return -1;
  ObjMesh mesh = std::string(argv[1]);

  // for (size_t i = 0; i < mesh.getVerts().size(); i++) {
  //   std::cout << "(" << mesh.getVerts()[i].x << ", " <<
    //	      << ", " << mesh.getVerts()[i].z << ")" << std::endl;
  // }

  for (size_t i = 0; i < mesh.getFaces().size(); i++) {
    std::vector<int> f = mesh.getFaces()[i];

    for (int j = 0; j < 3; j++) {
      Vec3f u = mesh.getVerts()[f[j]],
	    v = mesh.getVerts()[f[(j + 1) % 3]];  // next point in triangle

      draw_line((u.x + 1.0f) * width / 2.0f, (u.y + 1.0f) * height / 2.0f,
		(v.x + 1.0f) * width / 2.0f, (v.y + 1.0f) * height / 2.0f,
		image, white);
    }
  }

  image.flip_vertically();  // set origin to bottom left
  image.write_tga_file("output.tga");

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
//   cout << foo << "  " << bar << "  " << foo1 << "  " << foo2 << "  " << idx <<
//       "  " << br3 << endl;

//   ObjMesh mesh("./mesh.obj");
//   return 0;
// }
