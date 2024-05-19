#include "WoodTexture.h"
#include <ResourceLayer/Factory.h>

WoodTexture::WoodTexture(const Json& json) : Texture <Spectrum >() {
	 // 给出的示例场景中， json 文件并不包含额外的域； 你可以自行添加
	 // your code here ...
		
}

Spectrum WoodTexture::evaluate(const Intersection & intersection) const {
	 // your code here ...
		
}

Spectrum WoodTexture::evaluate(const TextureCoord & texCoord) const {
	// your code here ...
		
}

// 为了让json 文件可以索引到WoodTexture ， 这一语句不可以删除
REGISTER_CLASS(WoodTexture, "woodTex")