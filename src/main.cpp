// In his exalted name
#include <iostream>
#include <limits>
#include <sstream>

#include "./h/tgaimage.h"
#include "h/objmesh.h"
#include "h/utils.hpp"

#define WIDTH 800
#define HEIGHT 800

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue  = TGAColor(0, 0, 255, 255);

void drawLine(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color)
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

void drawLine(Vec2i u, Vec2i v, TGAImage& image, const TGAColor& color)
{
  drawLine(u.x, u.y, v.x, v.y, image, color);
}

void drawWireframe(ObjMesh& mesh, TGAImage& image, int width, int height)
{
  for (size_t i = 0; i < mesh.nFaces(); i++) {
    std::vector<int> f = mesh.getFace(i);
    for (int j = 0; j < 3; j++) {
      Vec3f u = mesh.getVert(f[j]),
	    v = mesh.getVert(f[(j + 1) % 3]);  // next point in triangle
      drawLine(static_cast<int>((u.x + 1.0f) * width / 2.0f),
	       static_cast<int>((u.y + 1.0f) * height / 2.0f),
	       static_cast<int>((v.x + 1.0f) * width / 2.0f),
	       static_cast<int>((v.y + 1.0f) * height / 2.0f), image, white);
    }
  }
}

Vec3f calcBarycentric(Vec3f coords[], Vec3f& P)
{
  Vec2i a(coords[0].x, coords[0].y), b(coords[1].x, coords[1].y), c(coords[2].x, coords[2].y);
  Vec2i ab(b - a), ac(c - a), pa(a.x - P.x, a.y - P.y);
  Vec3f cp = Vec3f(ab.x, ac.x, pa.x) ^ Vec3f(ab.y, ac.y, pa.y);

  if (std::abs(cp.z) < 1)
    return Vec3f(-1, 1, 1);  // tri is degenerate, discard pls

  float u = cp.x / cp.z, v = cp.y / cp.z, w = 1.0f - ((cp.x + cp.y) / cp.z);
  return Vec3f(w, u, v);
}

void drawTri(Vec3f coords[], Vec3f texs[], float zbuffer[], TGAImage diffuseTexture,
	     TGAImage& image, const float intensity)
{
  Vec2f boxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
      boxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
  const Vec2f clamp(image.get_width() - 1, image.get_height() - 1);

  for (int i = 0; i < 3; i++) {
    Vec3f coord = coords[i];
    for (int j = 0; j < 2; j++) {
      boxmax.raw[j] = std::min(clamp.raw[j], std::max(boxmax.raw[j], coord.raw[j]));
      boxmin.raw[j] = std::max(0.0f, std::min(boxmin.raw[j], coord.raw[j]));
    }
  }
  Vec3f P;
  for (P.x = boxmin.x; P.x <= boxmax.x; P.x++)
    for (P.y = boxmin.y; P.y <= boxmax.y; P.y++) {
      Vec3f u = calcBarycentric(coords, P);
      if (u.x < 0 || u.y < 0 || u.z < 0)
	continue;
      P.z = 0;
      for (int i = 0; i < 3; i++) P.z += coords[i].z * u.raw[i];
      if (zbuffer[static_cast<int>(P.x + P.y * image.get_width())] < P.z) {
	zbuffer[static_cast<int>(P.x + P.y * image.get_width())] = P.z;
	TGAColor color = getColorUV(texs[0].u, texs[0].v, diffuseTexture);
	image.set(
	    P.x, P.y,
	    TGAColor(static_cast<int>(color.r * intensity), static_cast<int>(color.g * intensity),
		     static_cast<int>(color.b * intensity), 255));
      }
    }
}

// draw mesh using drawTri with random color
void drawFlatShadingRender(float zbuffer[], ObjMesh& mesh, TGAImage& diffuseTexture,
			   TGAImage& image)
{
  Vec3f lightdir(0, 0, -1);

  for (size_t i = 0; i < mesh.nFaces(); i++) {
    std::vector<int> f = mesh.getFace(i);
    Vec3f triTex[3];
    Vec3f triCoords[3];
    Vec3f worldCoords[3];

    for (size_t j = 0; j < 3; j++) {
      Vec3f v	= mesh.getVert(f[i]);
      triCoords[j]   = Vec3f((v.x + 1.0f) * image.get_width() / 2.0f,
			     (v.y + 1.0f) * image.get_height() / 2.0f, (v.z + 1.0f) / 2.0f);
      triTex[j]      = mesh.getVertTex(mesh.getFaceTex(i)[j]);
      worldCoords[j] = v;
    }

    Vec3f n = (worldCoords[2] - worldCoords[0]) ^ (worldCoords[1] - worldCoords[0]);
    n.normalize();
    float intensity = n * lightdir;
    drawTri(triCoords, triTex, zbuffer, diffuseTexture, image, intensity);
  }
}

int main(int argc, char* argv[])
{
  TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
  float zbuffer[WIDTH * HEIGHT];
  ObjMesh mesh;
  TGAImage diffuseTexture;

  if (argc <= 2) {
    mesh = ObjMesh(std::string("../res/meshes/head.obj"));
    diffuseTexture.read_tga_file("../res/textures/african_head_diffuse.tga");
  } else {
    mesh = ObjMesh(std::string(argv[1]));
    diffuseTexture.read_tga_file(argv[2]);
  }

  // drawWireframe(mesh, image, width, height);
  drawFlatShadingRender(zbuffer, mesh, diffuseTexture, image);

  image.flip_vertically();  // set origin to bottom left
  image.write_tga_file("output.tga");
  return 0;
}

// int main(int argc, char *argv[])
// {
//   TGAImage image(200, 200, TGAImage::RGB);
//   Vec2i triCoords[3] = {Vec2i(10, 10), Vec2i(100, 30), Vec2i(190, 160)};
//   drawTri(triCoords, image, red);
//   image.flip_vertically();
//   image.write_tga_file("tri_test.tga");
//   return 0;
// }

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
