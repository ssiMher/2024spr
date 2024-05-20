#include "WoodTexture.h"
#include <ResourceLayer/Factory.h>
#include <cmath>
#include <random>
WoodTexture::WoodTexture(const Json& json) : Texture<Spectrum>() {
    seed = fetchRequired<unsigned int>(json, "seed");
    gridSize = fetchRequired<int>(json, "gridSize");
    color = fetchRequired<Spectrum>(json, "color");
}

Vector2f WoodTexture::randomGradient(int ix, int iy) const {
    std::mt19937 generator(seed + ix * 73856093 + iy * 19349663);
    std::uniform_real_distribution<float> distribution(0.0f, 2.0f * M_PI);
    float angle = distribution(generator);
    return Vector2f(cos(angle), sin(angle));
}

float WoodTexture::perlinNoise(float x, float y) const {
    int x0 = static_cast<int>(floor(x));
    int x1 = x0 + 1;
    int y0 = static_cast<int>(floor(y));
    int y1 = y0 + 1;

    Vector2f g00 = randomGradient(x0, y0);
    Vector2f g10 = randomGradient(x1, y0);
    Vector2f g01 = randomGradient(x0, y1);
    Vector2f g11 = randomGradient(x1, y1);

    Vector2f p(x, y);
    Vector2f d00 = p - Vector2f(x0, y0);
    Vector2f d10 = p - Vector2f(x1, y0);
    Vector2f d01 = p - Vector2f(x0, y1);
    Vector2f d11 = p - Vector2f(x1, y1);

    float f00 = dot(d00, g00);
    float f10 = dot(d10, g10);
    float f01 = dot(d01, g01);
    float f11 = dot(d11, g11);

    float rx = (x - x0) / (x1 - x0);
    float ry = (y - y0) / (y1 - y0);

    float xlow = (1 - rx) * f00 + rx * f10;
    float xhigh = (1 - rx) * f01 + rx * f11;
    float f = (1 - ry) * xlow + ry * xhigh;

    return f;
}

Spectrum WoodTexture::woodTexture(float x, float y) const {
    float noise = perlinNoise(x, y);
    float woodValue = (noise * 5 - std::floor(noise * 5));
    return color * woodValue;
}

Spectrum WoodTexture::evaluate(const Intersection& intersection) const {
    Vector2f uv = intersection.texCoord;
    int width = gridSize;
    int height = gridSize;
    float x = uv[0] * width;
    float y = uv[1] * height;
    return woodTexture(x, y);
}

Spectrum WoodTexture::evaluate(const TextureCoord& texCoord) const {
    Vector2f uv = texCoord.coord;
    int width = gridSize;
    int height = gridSize;
    float x = uv[0] * width;
    float y = uv[1] * height;
    return woodTexture(x, y);
}

// 为了让json 文件可以索引到WoodTexture ， 这一语句不可以删除
REGISTER_CLASS(WoodTexture, "woodTex")