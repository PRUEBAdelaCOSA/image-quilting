#pragma once

#include <agz-utils/texture.h>

using Vec3 = agz::math::float3;

template<typename T>
using Texture = agz::texture::texture2d_t<T>;

float calculateErrorSum(
    const Texture<Vec3> &A, int xA, int yA,
    const Texture<Vec3> &B, int xB, int yB,
    int width, int height);

float calculateMSE(
    const Texture<Vec3> &source,
    const Texture<Vec3> &target,
    int srcX, int srcY,
    int tgtX, int tgtY,
    int tileW,
    int tileH,
    int seamW,
    int seamH);
