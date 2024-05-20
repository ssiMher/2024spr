#include "Texture.h"

TextureCoord UVMapping::map(const Intersection &intersection) const {
    //texturecoord tc;
    //vector3f p = intersection.position - point3f(0,0,0);
    ////vector3f normalizedp = normalize(p);

    //vector3f v_alpha = vector3f(1.0f, 0.0f, 0.0f);
    //vector3f v_beta = vector3f(0.0f, 1.0f, 0.0f);

    //float u_prime = dot(v_alpha, p);  // ͶӰ����ά���Կռ�
    //float v_prime = dot(v_beta, p);

    //float u = u_prime - floor(u_prime);  // ӳ�䵽[0, 1]����
    //float v = v_prime - floor(v_prime);

    //tc.coord = vector2f(u, v);
    //// Բ������ӳ��
    //vector3f normalizedp = normalize(p);
    //float theta = atan2(normalizedp[1], normalizedp[0]);  // ��λ��

    //float u = theta / (2.0f * m_pi);  // ӳ������������
    //float v = (normalizedp[2] >= 0.0f) ? normalizedp[2] : normalizedp[2] + 1.0f;

    //// ʹ��fmod���������Դ���
    //u = u - floor(u);
    //v = v - floor(v);

    //tc.coord = vector2f(u, v);

    //float phi = acos(normalizedp[2]);  // ������
    //float theta = atan2(normalizedp[1], normalizedp[0]);  // ��λ��

    //float u = theta / (2.0f * m_pi);  // ӳ������������
    //float v = phi / m_pi;

    //u = u - floor(u);
    //v = v - floor(v);
    //tc.coord = vector2f(u, v);
    //return tc;
  return TextureCoord{intersection.texCoord,
                      Vector2f{intersection.dudx, intersection.dvdx},
                      Vector2f{intersection.dudy, intersection.dvdy}};
}