#include "WoodTexture.h"
#include <ResourceLayer/Factory.h>

WoodTexture::WoodTexture(const Json& json) : Texture <Spectrum >() {
	 // ������ʾ�������У� json �ļ���������������� ������������
	 // your code here ...
		
}

Spectrum WoodTexture::evaluate(const Intersection & intersection) const {
	 // your code here ...
		
}

Spectrum WoodTexture::evaluate(const TextureCoord & texCoord) const {
	// your code here ...
		
}

// Ϊ����json �ļ�����������WoodTexture �� ��һ��䲻����ɾ��
REGISTER_CLASS(WoodTexture, "woodTex")