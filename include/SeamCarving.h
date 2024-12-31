#pragma once

#include <vector>
#include <agz-utils/texture.h>

using Vec3 = agz::math::float3;

template<typename T>
using Texture = agz::texture::texture2d_t<T>;

template<typename T>
using TextureView = agz::texture::texture2d_view_t<T, true>;

struct MinCostCutData
{
    float cost = 0;
    int lastOffset = 0;
};

std::vector<int> findVerticalMinCostSeam(
    const Texture<Vec3>     &A,
    const TextureView<Vec3> &B,
    int xA,    int yA,
    int xB,    int yB,
    int width, int height);

std::vector<int> findHorizontalMinCostSeam(
    const Texture<Vec3>     &A,
    const TextureView<Vec3> &B,
    int xA,    int yA,
    int xB,    int yB,
    int width, int height);
