//
// Created by Salah Mezraoui on 30.08.24.
//
#include "../include/ErrorMetrics.h"

float calculateErrorSum(
    const Texture<Vec3> &A, int xA, int yA,
    const Texture<Vec3> &B, int xB, int yB,
    int width, int height)
{
    float squaredErrorSum = 0;
    for(int ix = 0, ixA = xA, ixB = xB; ix < width; ++ix, ++ixA, ++ixB)
    {
        for(int iy = 0, iyA = yA, iyB = yB; iy < height; ++iy, ++iyA, ++iyB)
        {
            squaredErrorSum += agz::math::sqr(
                (A(iyA, ixA) - B(iyB, ixB)).lum());
        }
    }
    return squaredErrorSum;
}

float calculateMSE(
    const Texture<Vec3> &source,
    const Texture<Vec3> &target,
    int srcX, int srcY,
    int tgtX, int tgtY,
    int tileW,
    int tileH,
    int seamW,
    int seamH)
{
    if(tgtX <= 0  && tgtY <= 0)
        return 0;

    if(tgtX > 0 && tgtY <= 0)
    {
        const float squaredErrorSum = calculateErrorSum(
            source, srcX, srcY, target, tgtX, tgtY, seamW, tileH);

        const int pixelCount = seamW * tileH;

        return squaredErrorSum / pixelCount;
    }

    if(tgtX <= 0 && tgtY > 0)
    {
        const float squaredErrorSum = calculateErrorSum(
            source, srcX, srcY, target, tgtX, tgtY, tileW, seamH);

        const int pixelCount = seamH * tileW;

        return squaredErrorSum / pixelCount;
    }

    const float A = calculateErrorSum(
        source, srcX, srcY, target, tgtX, tgtY, seamW, seamH);

    const float B = calculateErrorSum(
        source, srcX, srcY + seamH, target, tgtX, tgtY + seamH, seamW, tileH - seamH);

    const float C = calculateErrorSum(
        source, srcX + seamW, srcY, target, tgtX + seamW, tgtY, tileW - seamW, seamH);

    const int pixelCount = seamW * seamH
                       + seamW * (tileH - seamH)
                       + seamH * (tileW - seamW);

    return (A + B + C) / pixelCount;
}
