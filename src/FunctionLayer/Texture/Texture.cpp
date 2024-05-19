#include "Texture.h"

TextureCoord UVMapping::map(const Intersection &intersection) const {
    TextureCoord tc;
    Vector3f p = intersection.position - Point3f(0,0,0);
    //Vector3f normalizedP = normalize(p);

    Vector3f v_alpha = Vector3f(1.0f, 0.0f, 0.0f);
    Vector3f v_beta = Vector3f(0.0f, 1.0f, 0.0f);

    float u_prime = dot(v_alpha, p);  // 投影到二维线性空间
    float v_prime = dot(v_beta, p);

    float u = u_prime - floor(u_prime);  // 映射到[0, 1]区间
    float v = v_prime - floor(v_prime);

    tc.coord = Vector2f(u, v);
    // 圆柱纹理映射
    //Vector3f normalizedP = normalize(p);
    //float theta = atan2(normalizedP[1], normalizedP[0]);  // 方位角

    //float u = theta / (2.0f * M_PI);  // 映射至纹理坐标
    //float v = (normalizedP[2] >= 0.0f) ? normalizedP[2] : normalizedP[2] + 1.0f;

    //// 使用fmod进行周期性处理
    //u = u - floor(u);
    //v = v - floor(v);

    //tc.coord = Vector2f(u, v);

    //float phi = acos(normalizedP[2]);  // 俯仰角
    //float theta = atan2(normalizedP[1], normalizedP[0]);  // 方位角

    //float u = theta / (2.0f * M_PI);  // 映射至纹理坐标
    //float v = phi / M_PI;

    //u = u - floor(u);
    //v = v - floor(v);
    //tc.coord = Vector2f(u, v);
    return tc;
  return TextureCoord{intersection.texCoord,
                      Vector2f{intersection.dudx, intersection.dvdx},
                      Vector2f{intersection.dudy, intersection.dvdy}};
}