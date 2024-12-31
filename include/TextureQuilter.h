#pragma once

#include <random>
#include <agz-utils/texture.h>
#include "SeamCarving.h"
#include "ErrorMetrics.h"

using Vec3 = agz::math::float3;

template<typename T>
using Texture = agz::texture::texture2d_t<T>;

template<typename T>
using TextureView = agz::texture::texture2d_view_t<T, true>;

class TextureQuilter
{
public:

    TextureQuilter();

    void setTileParams(int tileWidth, int tileHeight) noexcept;

    void setTileParams(
        int   tileWidth,
        int   tileHeight,
        int   seamWidth,
        int   seamHeight,
        float tolerance) noexcept;

    void enableMSESelection(bool enable) noexcept;

    void enableMinCut(bool enable) noexcept;

    Texture<Vec3> quiltTexture(
        const Texture<Vec3> &source,
        int                  targetWidth,
        int                  targetHeight) const;

private:

    TextureView<Vec3> selectSourceTile(
        const Texture<Vec3>        &source,
        const Texture<Vec3>        &target,
        int                         x,
        int                         y,
        std::default_random_engine &rng) const;

    void placeTile(
        const TextureView<Vec3> &tile,
        Texture<Vec3>           &target,
        int                      x,
        int                      y) const;

    int tileWidth_;
    int tileHeight_;
    int seamWidth_;
    int seamHeight_;

    float tolerance_;

    bool enableMSESelection_;
    bool enableMinCut_;
};
