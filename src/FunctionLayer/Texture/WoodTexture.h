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
	 // �������ɵ�λ�������������
	unsigned int seed;
	// �������
	int gridSize;
	// ��ɫ����
	Spectrum color;

	// ���ɵ�λ�����ķ���
	Vector2f randomGradient(int ix, int iy) const;

	// ˫���Բ�ֵ�ķ���
	float perlinNoise(float x, float y) const;

	// ľ���������ɵķ���
	Spectrum woodTexture(float x, float y) const;
		
};