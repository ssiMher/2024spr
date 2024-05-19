#include "Texture.h"

TextureCoord UVMapping::map(const Intersection &intersection) const {
    TextureCoord tc;
    Vector3f p = intersection.position - Point3f(0,0,0);
    //Vector3f normalizedP = normalize(p);

    Vector3f v_alpha = Vector3f(1.0f, 0.0f, 0.0f);
    Vector3f v_beta = Vector3f(0.0f, 1.0f, 0.0f);

    float u_prime = dot(v_alpha, p);  // ͶӰ����ά���Կռ�
    float v_prime = dot(v_beta, p);

    float u = u_prime - floor(u_prime);  // ӳ�䵽[0, 1]����
    float v = v_prime - floor(v_prime);

    tc.coord = Vector2f(u, v);
    // Բ������ӳ��
    //Vector3f normalizedP = normalize(p);
    //float theta = atan2(normalizedP[1], normalizedP[0]);  // ��λ��

    //float u = theta / (2.0f * M_PI);  // ӳ������������
    //float v = (normalizedP[2] >= 0.0f) ? normalizedP[2] : normalizedP[2] + 1.0f;

    //// ʹ��fmod���������Դ���
    //u = u - floor(u);
    //v = v - floor(v);

    //tc.coord = Vector2f(u, v);

    //float phi = acos(normalizedP[2]);  // ������
    //float theta = atan2(normalizedP[1], normalizedP[0]);  // ��λ��

    //float u = theta / (2.0f * M_PI);  // ӳ������������
    //float v = phi / M_PI;

    //u = u - floor(u);
    //v = v - floor(v);
    //tc.coord = Vector2f(u, v);
    return tc;
  return TextureCoord{intersection.texCoord,
                      Vector2f{intersection.dudx, intersection.dvdx},
                      Vector2f{intersection.dudy, intersection.dvdy}};
}