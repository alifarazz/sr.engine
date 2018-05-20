#ifndef __UTILS_H__
#define __UTILS_H__

#include "./geometry.h"
#include "./tgaimage.h"

TGAColor getColorUV(float u, float v, TGAImage &texture)
{
  int iu = static_cast<int>(u * texture.get_width()),
    iv = static_cast<int>(v * texture.get_height());
  return texture.get(iu, iv);
}

#endif
