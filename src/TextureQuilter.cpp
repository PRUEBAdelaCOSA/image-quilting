#include "../include/TextureQuilter.h"
#include <agz-utils/console.h>
#include <map>


TextureQuilter::TextureQuilter()
    : tileWidth_(3), tileHeight_(3),
      seamWidth_(1), seamHeight_(1),
      tolerance_(0.1f),
      enableMSESelection_(true),
      enableMinCut_(true)
{

}

void TextureQuilter::setTileParams(int tileWidth, int tileHeight) noexcept
{
    setTileParams(
        tileWidth,
        tileHeight,
        std::max(1, tileWidth / 6),
        std::max(1, tileHeight / 6),
        0.1f);
}

void TextureQuilter::setTileParams(
    int   tileWidth,
    int   tileHeight,
    int   seamWidth,
    int   seamHeight,
    float tolerance) noexcept
{
    tileWidth_    = tileWidth;
    tileHeight_   = tileHeight;
    seamWidth_    = seamWidth;
    seamHeight_   = seamHeight;
    tolerance_    = tolerance;
}

void TextureQuilter::enableMSESelection(bool enable) noexcept
{
    enableMSESelection_ = enable;
}

void TextureQuilter::enableMinCut(bool enable) noexcept
{
    enableMinCut_ = enable;
}

Texture<Vec3> TextureQuilter::quiltTexture(
    const Texture<Vec3> &source,
    int                  targetWidth,
    int                  targetHeight) const
{
    const int tileCountX = static_cast<int>(std::ceil(
        static_cast<float>(targetWidth - seamWidth_)
                        / (tileWidth_ - seamWidth_)));
    const int tileCountY = static_cast<int>(std::ceil(
        static_cast<float>(targetHeight - seamHeight_)
                        / (tileHeight_ - seamHeight_)));

    const int textureWidth = tileCountX * tileWidth_ - (tileCountX - 1) * seamWidth_;
    const int textureHeight = tileCountY * tileHeight_ - (tileCountY - 1) * seamHeight_;

    Texture<Vec3> target(textureHeight, textureWidth);

    std::default_random_engine rng{ std::random_device()() };

    agz::console::progress_bar_t pbar(tileCountY * tileCountX, 80, '=');
    pbar.display();

    for(int tileY = 0; tileY < tileCountY; ++tileY)
    {
        const int y = tileY * (tileHeight_ - seamHeight_);

        for(int tileX = 0; tileX < tileCountX; ++tileX)
        {
            const int x = tileX * (tileWidth_ - seamWidth_);

            const auto tile = selectSourceTile(source, target, x, y, rng);
            placeTile(tile, target, x, y);

            ++pbar;
        }

        pbar.display();
    }

    pbar.done();

    return target.subtex(0, targetHeight, 0, targetWidth);
}

TextureView<Vec3> TextureQuilter::selectSourceTile(
    const Texture<Vec3>         &source,
    const Texture<Vec3>         &target,
    int                          x,
    int                          y,
    std::default_random_engine  &rng) const
{
    if(!enableMSESelection_)
    {
        std::uniform_int_distribution disX(
            0, static_cast<int>(source.width() - tileWidth_ - 1));
        std::uniform_int_distribution disY(
            0, static_cast<int>(source.height() - tileHeight_ - 1));

        const int srcX = disX(rng);
        const int srcY = disY(rng);

        return source.subview(srcY, srcY + tileHeight_, srcX, srcX + tileWidth_);
    }

    std::multimap<float, agz::math::vec2i> mseToXY;

    for(int srcY = 0; srcY < source.height() - tileHeight_; ++srcY)
    {
        for(int srcX = 0; srcX < source.width() - tileWidth_; ++srcX)
        {
            const float mse = calculateMSE(
                source, target, srcX, srcY, x, y,
                tileWidth_, tileHeight_, seamWidth_, seamHeight_);

            if((x == 0 && y == 0) || mse > 0.001f)
                mseToXY.insert({ mse, agz::math::vec2i{ srcX, srcY } });
        }
    }

    const float maxAllowedMSE = mseToXY.begin()->first * (1 + tolerance_);
    std::vector<agz::math::vec2i> allowedXYs;

    for(auto &p : mseToXY)
    {
        if(p.first <= maxAllowedMSE)
            allowedXYs.push_back(p.second);
        else
            break;
    }

    std::uniform_int_distribution dis(
        0, static_cast<int>(allowedXYs.size() - 1));
    const auto xy = allowedXYs[dis(rng)];

    return source.subview(xy.y, xy.y + tileHeight_, xy.x, xy.x + tileWidth_);
}

void TextureQuilter::placeTile(
    const TextureView<Vec3> &tile,
    Texture<Vec3>           &target,
    int                      x,
    int                      y) const
{
    if(!enableMinCut_)
    {
        for(int yi = 0; yi < tile.height(); ++yi)
        {
            for(int xi = 0; xi < tile.width(); ++xi)
            {
                target(y + yi, x + xi) = tile(yi, xi);
            }
        }
        return;
    }

    if(x > 0 && y == 0)
    {
        const auto verticalSeam = findVerticalMinCostSeam(
            target, tile, x, y, 0, 0, seamWidth_, tileHeight_);

        for(int yi = 0; yi < tile.height(); ++yi)
        {
            for(int xi = 0; xi < tile.width(); ++xi)
            {
                if(xi > verticalSeam[yi])
                    target(y + yi, x + xi) = tile(yi, xi);
            }
        }
    }
    else if(x == 0 && y > 0)
    {
        const auto horizontalSeam = findHorizontalMinCostSeam(
            target, tile, x, y, 0, 0, tileWidth_, seamHeight_);

        for(int yi = 0; yi < tile.height(); ++yi)
        {
            for(int xi = 0; xi < tile.width(); ++xi)
            {
                if(yi > horizontalSeam[xi])
                    target(y + yi, x + xi) = tile(yi, xi);
            }
        }
    }
    else if(x > 0 && y > 0)
    {
        const auto verticalSeam = findVerticalMinCostSeam(
            target, tile, x, y, 0, 0, seamWidth_, tileHeight_);

        const auto horizontalSeam = findHorizontalMinCostSeam(
            target, tile, x, y, 0, 0, tileWidth_, seamHeight_);

        for(int yi = 0; yi < tile.height(); ++yi)
        {
            for(int xi = 0; xi < tile.width(); ++xi)
            {
                if(xi > verticalSeam[yi] && yi > horizontalSeam[xi])
                    target(y + yi, x + xi) = tile(yi, xi);
            }
        }
    }
    else
    {
        for(int yi = 0; yi < tile.height(); ++yi)
        {
            for(int xi = 0; xi < tile.width(); ++xi)
                target(y + yi, x + xi) = tile(yi, xi);
        }
    }
}
