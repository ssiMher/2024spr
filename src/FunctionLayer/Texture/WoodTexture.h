#pragma once

#include "Mipmap.h"
#include "Texture.h"
#include <CoreLayer/ColorSpace/Spectrum.h>
class WoodTexture : public Texture <Spectrum > {
public:
	WoodTexture() = delete;
	
	WoodTexture(const Json & json);
	
	virtual Spectrum evaluate(const Intersection & intersection) const override;
	
	virtual Spectrum evaluate(const TextureCoord & texCoord) const override;
	
private:
	 // necessary data
	 // your code here ...
	 // 用于生成单位向量的随机种子
	unsigned int seed;
	// 格点数量
	int gridSize;
	// 颜色常数
	Spectrum color;

	// 生成单位向量的方法
	Vector2f randomGradient(int ix, int iy) const;

	// 双线性插值的方法
	float perlinNoise(float x, float y) const;

	// 木纹纹理生成的方法
	Spectrum woodTexture(float x, float y) const;
		
};