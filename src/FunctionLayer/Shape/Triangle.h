#pragma once
#include "Shape.h"
#include <FunctionLayer/Acceleration/Acceleration.h>
#include <ResourceLayer/Factory.h>
#include <ResourceLayer/Mesh.h>
class TriangleMesh;

class Triangle : public Shape {
public:
  Triangle() = default;

  Triangle(int _primID, int _vtx0Idx, int _vtx1Idx, int _vtx2Idx,
           const TriangleMesh *_mesh);

  virtual bool rayIntersectShape(Ray &ray, int *primID, float *u,
                                 float *v) const override;

  virtual void fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const override;

  virtual void uniformSampleOnSurface(Vector2f sample,
                                      Intersection *intersection,
                                      float *pdf) const override {
    // TODO finish this
    return;
  }

public:
  int primID;
  int vtx0Idx, vtx1Idx, vtx2Idx;
  const TriangleMesh *mesh = nullptr;
};

class TriangleMesh : public Shape {
public:
  TriangleMesh() = default;

  TriangleMesh(const Json &json);

  //* 当使用embree时，我们使用embree内置的求交函数，故覆盖默认方法
  virtual RTCGeometry getEmbreeGeometry(RTCDevice device) const override;

  virtual bool rayIntersectShape(Ray &ray, int *primID, float *u,
                                 float *v) const override;

  virtual void fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const override;

  virtual void uniformSampleOnSurface(Vector2f sample,
                                      Intersection *intersection,
                                      float *pdf) const override {
    // TODO finish this
      // 计算mesh的总面积以及概率分布表
      float totalArea = getArea();
      setArea(totalArea);
      assert(totalArea > 0);
      calculateAreas();
      assert(cumulativeProbabilities.size() > 0);
      int faceCount = meshData->faceCount;
      //std::vector<float> areas(faceCount, 0);

      // 使用第一个随机数（sample[0]）在CDF中选择一个三角形
      //float areaSum = 0.0;
      int chosenFace = -1;
      int left = 0, right = faceCount - 1;
      
      while (left <= right) {
          int mid = left + (right - left) / 2;
          if (cumulativeProbabilities[mid] < sample[0]) {
              left = mid + 1;
          }
          else {
              right = mid - 1;
          }
      }
      chosenFace = left;
      if (meshData->faceBuffer.size() == chosenFace)chosenFace = meshData->faceBuffer.size() - 1;
      /*for (int i = 0; i < faceCount; ++i) {
          areaSum += areas[i];
          if (sample[0] * totalArea <= areaSum) {
              chosenFace = i;
              break;
          }
      }*/
      // 从选定的三角形面中采样一个点
      // 使用三角形面的顶点进行插值
      //std::cout<<"faceBuffer size: "<< meshData->faceBuffer.size()<<"  chosenFace: "<<chosenFace<<std::endl;
      assert(meshData->faceBuffer.size() > chosenFace);
      std::array<DataIndex, 3> face = meshData->faceBuffer[chosenFace];
      assert(meshData->vertexBuffer.size() > face[0].vertexIndex);
      Point3f v0 = meshData->vertexBuffer[face[0].vertexIndex];
      assert(meshData->vertexBuffer.size() > face[1].vertexIndex);
      Point3f v1 = meshData->vertexBuffer[face[1].vertexIndex];
      assert(meshData->vertexBuffer.size() > face[2].vertexIndex);
      Point3f v2 = meshData->vertexBuffer[face[2].vertexIndex];

      // 生成重心坐标
      float sqrtSample = std::sqrt(sample[1]);
      float u = 1.0f - sqrtSample;
      float v = sample[1] * sqrtSample;
      v = sample[0] * sqrtSample;
      assert(u >= 0 && u < 1);
      assert(v >= 0 && v < 1);
      fillIntersection(0, chosenFace, u, v, intersection);
      // 计算pdf
      *pdf = 1.0f / totalArea;

    return;
  }

  virtual void initInternalAcceleration() override;

  //my add
  float getArea(int index) const;// 计算第 index 个三角形的面积
  virtual float getArea() const override; // 计算总面积
  void setArea(float area) const;// 设置成员变量totalArea
  void calculateAreas() const; // 设置累积概率分布表
  friend class Triangle;

  mutable float totalArea = -1; // 总面积
  mutable std::vector<float> cumulativeProbabilities; // 累积概率分布表
private:
  std::shared_ptr<MeshData> meshData;
  std::shared_ptr<Acceleration> acceleration;
};