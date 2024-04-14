#pragma once
#include "Shape.h"
// TODO 当前只有Transform中的translate对sphere生效
class Sphere : public Shape {
public:
  Sphere() = delete;

  Sphere(const Json &json);

  virtual bool rayIntersectShape(Ray &ray, int *primID, float *u,
                                 float *v) const override;

  virtual void fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const override;
  virtual void uniformSampleOnSurface(Vector2f sample,
                                      Intersection *intersection,
                                      float *pdf) const override {
    // TODO finish this
    /*
    sample: 两个[0,1] 之间的随机数；
    intersection: 作为返回值使用， 至少需要填写position 和normal 两个域；
    pdf: 作为返回值使用， 表示在这一物体表面上采样的概率密度函数， 以面积计。
    */
      // write your code here ...
      float theta = 2 * PI * sample[0]; // 方位角
      float phi = std::acos(2 * sample[1] - 1) - PI / 2; // 俯仰角

      // 转换成球坐标系下的点
      float x = radius * std::cos(theta) * std::cos(phi);
      float y = radius * std::sin(phi);
      float z = radius * std::sin(theta) * std::cos(phi);

      // 设置采样点的位置
      intersection->position = center + Vector3f(x, y, z);
      // 设置法线
      intersection->normal = normalize(Vector3f(x, y, z));
      // 均匀采样在球面上的pdf为1 / (4 * pi * r^2)
      *pdf = 1.0 / (4 * PI * radius * radius);
    return;
  }

public:
  Point3f center;
  float radius;
};