#include "../include/SeamCarving.h"

void computeVerticalSeamCost(
    const Texture<Vec3>     &A,
    const TextureView<Vec3> &B,
    int xA,    int yA,
    int xB,    int yB,
    int width, int height,
    Texture<MinCostCutData> &result)
{
    assert(result.width() == width - 1 && result.height() == height);

    auto seamCost = [&](int x, int y)
    {
        return agz::math::sqr(
            abs(A(yA + y, xA + x) - B(yB + y, xB + x + 1)).lum());
    };

    for(int xi = 0; xi < width - 1; ++xi)
        result(0, xi).cost = seamCost(xi, 0);

    for(int yi = 1; yi < height; ++yi)
    {
        for(int xi = 0; xi < width - 1; ++xi)
        {
            const float costLeft = xi > 0 ?
                result(yi - 1, xi - 1).cost : std::numeric_limits<float>::max();
            const float costMiddle = result(yi - 1, xi).cost;
            const float costRight = xi < width - 2 ?
                result(yi - 1, xi + 1).cost : std::numeric_limits<float>::max();

            const float ec = seamCost(xi, yi);

            if(costLeft <= costMiddle && costLeft <= costRight)
                result(yi, xi) = { costLeft + ec, -1 };
            else if(costMiddle <= costLeft && costMiddle <= costRight)
                result(yi, xi) = { costMiddle + ec, 0 };
            else
                result(yi, xi) = { costRight + ec, 1 };
        }
    }
}

void computeHorizontalSeamCost(
    const Texture<Vec3>     &A,
    const TextureView<Vec3> &B,
    int xA,    int yA,
    int xB,    int yB,
    int width, int height,
    Texture<MinCostCutData> &result)
{
    assert(result.width() == width && result.height() == height - 1);

    auto seamCost = [&](int x, int y)
    {
        return agz::math::sqr(
            abs(A(yA + y, xA + x) - B(yB + y + 1, xB + x)).lum());
    };

    for(int yi = 0; yi < height - 1; ++yi)
        result(yi, 0).cost = seamCost(0, yi);

    for(int xi = 1; xi < width; ++xi)
    {
        for(int yi = 0; yi < height - 1; ++yi)
        {
            const float costAbove = yi > 0 ?
                result(yi - 1, xi - 1).cost : std::numeric_limits<float>::max();
            const float costMiddle = result(yi, xi - 1).cost;
            const float costBelow = yi < height - 2 ?
                result(yi + 1, xi - 1).cost : std::numeric_limits<float>::max();

            const float ec = seamCost(xi, yi);

            if(costAbove <= costMiddle && costAbove <= costBelow)
                result(yi, xi) = { costAbove + ec, -1 };
            else if(costMiddle <= costAbove && costMiddle <= costBelow)
                result(yi, xi) = { costMiddle + ec, 0 };
            else
                result(yi, xi) = { costBelow + ec, 1 };
        }
    }
}

std::vector<int> findVerticalMinCostSeam(
    const Texture<Vec3>     &A,
    const TextureView<Vec3> &B,
    int xA,    int yA,
    int xB,    int yB,
    int width, int height)
{
    thread_local static Texture<MinCostCutData> seamCosts;
    if(seamCosts.size() != agz::math::vec2i{ width - 1, height })
        seamCosts.initialize(height, width - 1);

    computeVerticalSeamCost(
        A, B, xA, yA, xB, yB, width, height, seamCosts);

    float bestCost  = std::numeric_limits<float>::max();
    int bestCostXi = -1, nextXi = -1;
    for(int xi = 0; xi < width - 1; ++xi)
    {
        if(seamCosts(height - 1, xi).cost < bestCost)
        {
            bestCost   = seamCosts(height - 1, xi).cost;
            bestCostXi = xi;
            nextXi     = xi + seamCosts(height - 1, xi).lastOffset;
        }
    }

    std::vector<int> seam(height);
    seam[height - 1] = bestCostXi;

    for(int yi = height - 2; yi >= 0; --yi)
    {
        seam[yi] = nextXi;
        nextXi = nextXi + seamCosts(yi, nextXi).lastOffset;
    }

    return seam;
}

std::vector<int> findHorizontalMinCostSeam(
    const Texture<Vec3>     &A,
    const TextureView<Vec3> &B,
    int xA,    int yA,
    int xB,    int yB,
    int width, int height)
{
    thread_local static Texture<MinCostCutData> seamCosts;
    if(seamCosts.size() != agz::math::vec2i{ width, height - 1 })
        seamCosts.initialize(height - 1, width);

    computeHorizontalSeamCost(
        A, B, xA, yA, xB, yB, width, height, seamCosts);

    float bestCost = std::numeric_limits<float>::max();
    int bestCostYi = -1, nextYi = -1;
    for(int yi = 0; yi < height - 1; ++yi)
    {
        if(seamCosts(yi, width - 1).cost < bestCost)
        {
            bestCost   = seamCosts(yi, width - 1).cost;
            bestCostYi = yi;
            nextYi     = yi + seamCosts(yi, width - 1).lastOffset;
        }
    }

    std::vector<int> seam(width);
    seam[width - 1] = bestCostYi;

    for(int xi = width - 2; xi >= 0; --xi)
    {
        seam[xi] = nextYi;
        nextYi = nextYi + seamCosts(nextYi, xi).lastOffset;
    }

    return seam;
}
