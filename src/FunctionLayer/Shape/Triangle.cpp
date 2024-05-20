#include "Triangle.h"
#include <FunctionLayer/Acceleration/Linear.h>
//--- Triangle ---
Triangle::Triangle(int _primID, int _vtx0Idx, int _vtx1Idx, int _vtx2Idx,
                   const TriangleMesh *_mesh)
    : primID(_primID), vtx0Idx(_vtx0Idx), vtx1Idx(_vtx1Idx), vtx2Idx(_vtx2Idx),
      mesh(_mesh) {
  Point3f vtx0 = mesh->transform.toWorld(mesh->meshData->vertexBuffer[vtx0Idx]),
          vtx1 = mesh->transform.toWorld(mesh->meshData->vertexBuffer[vtx1Idx]),
          vtx2 = mesh->transform.toWorld(mesh->meshData->vertexBuffer[vtx2Idx]);
  boundingBox.Expand(vtx0);
  boundingBox.Expand(vtx1);
  boundingBox.Expand(vtx2);
  this->geometryID = mesh->geometryID;
}

bool Triangle::rayIntersectShape(Ray &ray, int *primID, float *u,
                                 float *v) const {
  Point3f origin = ray.origin;
  Vector3f direction = ray.direction;
  Point3f vtx0 = mesh->transform.toWorld(mesh->meshData->vertexBuffer[vtx0Idx]),
          vtx1 = mesh->transform.toWorld(mesh->meshData->vertexBuffer[vtx1Idx]),
          vtx2 = mesh->transform.toWorld(mesh->meshData->vertexBuffer[vtx2Idx]);

  Vector3f edge0 = vtx1 - vtx0, edge1 = vtx2 - vtx0;

  Vector3f paralNormal = normalize(cross(edge0, edge1));
  float d = -dot(paralNormal, Vector3f{vtx0[0], vtx0[1], vtx0[2]});
  float a = dot(paralNormal, Vector3f{origin[0], origin[1], origin[2]}) + d;
  float b = dot(paralNormal, direction);
  if (b == .0f)
    return false; // miss
  float t = -a / b;

  if (t < ray.tNear || t > ray.tFar)
    return false;

  Point3f hitpoint = origin + t * direction;
  // hitpoint = vtx0 + u * e0 + v * e1, 0 <= u, v <= 1
  Vector3f v1 = cross(hitpoint - vtx0, edge1), v2 = cross(edge0, edge1);
  float u_ = v1.length() / v2.length();
  if (dot(v1, v2) < 0)
    u_ *= -1;

  v1 = cross(hitpoint - vtx0, edge0);
  v2 = cross(edge1, edge0);
  float v_ = v1.length() / v2.length();
  if (dot(v1, v2) < 0)
    v_ *= -1;

  if (u_ >= .0f && v_ >= .0f && (u_ + v_ <= 1.f)) {
    ray.tFar = t;
    *primID = this->primID;
    *u = u_;
    *v = v_;
    return true;
  }

  return false;
}

void Triangle::fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const {
  // 该函数实际上不会被调用
  return;
}

//--- TriangleMesh ---
TriangleMesh::TriangleMesh(const Json &json) : Shape(json) {
  const auto &filepath = fetchRequired<std::string>(json, "file");
  meshData = MeshData::loadFromFile(filepath);
}

RTCGeometry TriangleMesh::getEmbreeGeometry(RTCDevice device) const {
  RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

  float *vertexBuffer = (float *)rtcSetNewGeometryBuffer(
      geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float),
      meshData->vertexCount);
  for (int i = 0; i < meshData->vertexCount; ++i) {
    Point3f vertex = transform.toWorld(meshData->vertexBuffer[i]);
    vertexBuffer[3 * i] = vertex[0];
    vertexBuffer[3 * i + 1] = vertex[1];
    vertexBuffer[3 * i + 2] = vertex[2];
  }

  unsigned *indexBuffer = (unsigned *)rtcSetNewGeometryBuffer(
      geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
      3 * sizeof(unsigned), meshData->faceCount);
  for (int i = 0; i < meshData->faceCount; ++i) {
    indexBuffer[i * 3] = meshData->faceBuffer[i][0].vertexIndex;
    indexBuffer[i * 3 + 1] = meshData->faceBuffer[i][1].vertexIndex;
    indexBuffer[i * 3 + 2] = meshData->faceBuffer[i][2].vertexIndex;
  }
  rtcCommitGeometry(geometry);
  return geometry;
}

bool TriangleMesh::rayIntersectShape(Ray &ray, int *primID, float *u,
                                     float *v) const {
  //* 当使用embree加速时，该方法不会被调用
  int geomID = -1;
  return acceleration->rayIntersect(ray, &geomID, primID, u, v);
}

void TriangleMesh::fillIntersection(float distance, int primID, float u,
                                    float v, Intersection *intersection) const {
  intersection->distance = distance;
  intersection->shape = this;
  //* 在三角形内部用插值计算交点、法线以及纹理坐标
  auto faceInfo = meshData->faceBuffer[primID];
  float w = 1.f - u - v;

  //* 计算交点
  Point3f pw = transform.toWorld(
              meshData->vertexBuffer[faceInfo[0].vertexIndex]),
          pu = transform.toWorld(
              meshData->vertexBuffer[faceInfo[1].vertexIndex]),
          pv = transform.toWorld(
              meshData->vertexBuffer[faceInfo[2].vertexIndex]);
  intersection->position = Point3f{w * pw[0] + u * pu[0] + v * pv[0],
                                   w * pw[1] + u * pu[1] + v * pv[1],
                                   w * pw[2] + u * pu[2] + v * pv[2]};
  //* 计算法线
  if (meshData->normalBuffer.size() != 0) {
    Vector3f nw = transform.toWorld(
                 meshData->normalBuffer[faceInfo[0].normalIndex]),
             nu = transform.toWorld(
                 meshData->normalBuffer[faceInfo[1].normalIndex]),
             nv = transform.toWorld(
                 meshData->normalBuffer[faceInfo[2].normalIndex]);
    intersection->normal = normalize(w * nw + u * nu + v * nv);
  } else {
    intersection->normal = normalize(cross(pu - pw, pv - pw));
  }

  //* 计算纹理坐标
  if (meshData->texcodBuffer.size() != 0) {
    Vector2f tw = meshData->texcodBuffer[faceInfo[0].texcodIndex],
             tu = meshData->texcodBuffer[faceInfo[1].texcodIndex],
             tv = meshData->texcodBuffer[faceInfo[2].texcodIndex];
    intersection->texCoord = w * tw + u * tu + v * tv;
  } else {
    intersection->texCoord = Vector2f{.0f, .0f};
  }

  // TODO 计算交点的切线和副切线
  Vector3f tangent{1.f, 0.f, .0f};
  Vector3f bitangent;
  if (std::abs(dot(tangent, intersection->normal)) > .9f) {
    tangent = Vector3f(.0f, 1.f, .0f);
  }
  bitangent = normalize(cross(tangent, intersection->normal));
  tangent = normalize(cross(intersection->normal, bitangent));
  intersection->tangent = tangent;
  intersection->bitangent = bitangent;

  // 计算偏微分dpdu 和dpdv
  // your code here ...
  Point3f p0 = pw;
  Point3f p1 = pu;
  Point3f p2 = pv;
  Vector2f uv0 = meshData->texcodBuffer[faceInfo[0].texcodIndex];
  Vector2f uv1 = meshData->texcodBuffer[faceInfo[1].texcodIndex];
  Vector2f uv2 = meshData->texcodBuffer[faceInfo[2].texcodIndex];

  Vector3f dp1 = p1 - p0;
  Vector3f dp2 = p2 - p0;
  Vector2f duv1 = uv1 - uv0;
  Vector2f duv2 = uv2 - uv0;
  float A[2][2] = { {duv1[0], duv1[1]}, {duv2[0], duv2[1]} };
  float Bx[2] = { dp1[0], dp2[0] };
  float By[2] = { dp1[1], dp2[1] };
  float Bz[2] = { dp1[2], dp2[2] };

  auto solveLinearSystem2x2 = [](const float A[2][2], const float B[2],
      float* x0, float* x1) {
          float det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
          if (std::abs(det) < 1e-10f)
              return false;
          *x0 = (A[1][1] * B[0] - A[0][1] * B[1]) / det;
          *x1 = (A[0][0] * B[1] - A[1][0] * B[0]) / det;
          if (std::isnan(*x0) || std::isnan(*x1))
              return false;
          return true;
  };
  float dpdu, dpdv;
  if (!solveLinearSystem2x2(A, Bx, &dpdu, &dpdv)) {
      intersection->dpdu[0] = intersection->dpdv[0] = 0.0f;
  }
  else {
      intersection->dpdu[0] = dpdu;
      intersection->dpdv[0] = dpdv;
  }
  if (!solveLinearSystem2x2(A, By, &dpdu, &dpdv)) {
      intersection->dpdu[1] = intersection->dpdv[1] = 0.0f;
  }
  else {
      intersection->dpdu[1] = dpdu;
      intersection->dpdv[1] = dpdv;
  }
  if (!solveLinearSystem2x2(A, Bz, &dpdu, &dpdv)) {
      intersection->dpdu[2] = intersection->dpdv[2] = 0.0f;
  }
  else {
      intersection->dpdu[2] = dpdu;
      intersection->dpdv[2] = dpdv;
  }
  return;
}

void TriangleMesh::initInternalAcceleration() {
  acceleration = Acceleration::createAcceleration();
  int primCount = meshData->faceCount;
  for (int primID = 0; primID < primCount; ++primID) {
    int vtx0Idx = meshData->faceBuffer[primID][0].vertexIndex,
        vtx1Idx = meshData->faceBuffer[primID][1].vertexIndex,
        vtx2Idx = meshData->faceBuffer[primID][2].vertexIndex;
    std::shared_ptr<Triangle> triangle =
        std::make_shared<Triangle>(primID, vtx0Idx, vtx1Idx, vtx2Idx, this);
    acceleration->attachShape(triangle);
  }
  acceleration->build();
  // TriangleMesh的包围盒就是其内部加速结构的包围盒
  boundingBox = acceleration->boundingBox;
}
REGISTER_CLASS(TriangleMesh, "triangle")


//my add
//计算一个三角形的面积
float TriangleMesh::getArea(int index) const {
    std::array<DataIndex, 3> face = meshData->faceBuffer[index];
    Point3f v0 = meshData->vertexBuffer[face[0].vertexIndex];
    Point3f v1 = meshData->vertexBuffer[face[1].vertexIndex];
    Point3f v2 = meshData->vertexBuffer[face[2].vertexIndex];

    // 计算三角形面的面积
    return 0.5 * cross(v1 - v0, v2 - v0).length();
}

// 计算整个TriangleMesh的面积
float TriangleMesh::getArea() const {
    if (this->totalArea >= 0)return this->totalArea;    //只计算一次面积
    float totalArea = 0.0;
    int faceCount = meshData->faceCount;
    for (int i = 0; i < faceCount; ++i) {
        // 计算三角形面的面积
        float a = getArea(i);
        totalArea += a;
        //this->areas[0] = a;
    }
    return totalArea;
}
void TriangleMesh::setArea(float area) const{
    this->totalArea = area;
    return;
}
void TriangleMesh::calculateAreas() const{
    if (this->cumulativeProbabilities.size() > 0)return;
    float totalArea = this->totalArea;
    std::vector<float> areas(meshData->faceCount);

    // 计算每个三角形的面积
    for (int i = 0; i < meshData->faceCount; ++i) {
        areas[i] = getArea(i);
    }

    // 计算累积概率
    cumulativeProbabilities.resize(meshData->faceCount);
    float accum = 0.0f;
    for (int i = 0; i < areas.size(); i++) {
        accum += areas[i] / totalArea;
        cumulativeProbabilities[i] = accum;
    }
}
