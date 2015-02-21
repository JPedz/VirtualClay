#ifndef MESHOPS_H_
#define MESHOPS_H_
#include "stdafx.h"
#include "cameraWrapper.h"

namespace mb = mudbox;

class MeshOps {
  mb::Mesh *pMesh;
  mb::Geometry *MeshGeo;
  mb::ScreenSpacePicker *ssp;
  mb::Picker *p;
  mb::Camera *curCam;
  bool checkUniqueInFaceList(int fi);
  bool checkUniqueInVertexList(int fi);
  QList<mb::Vector> *vertexPositions;

  typedef struct VertexFaceInfo {
    int vI;
    std::vector<int> fI;
  } VertexFaceInfo;
  std::vector<VertexFaceInfo> *vFI;
  typedef struct MidVertex {
    int vI;
    mb::Vector pos;
  };
  QList<int> *faces;
  QList<int> *vertices;
  QList<mb::SurfacePoint > *points;
  typedef struct VertexInfo {
    int vI;
    mb::Vector pos;
  } VertexInfo;
  std::vector<std::vector<VertexInfo>> undoQueue;
  void StoreUndoQueue();
  void MeshOps::AddVFI(int vi, int fi);

public:
  MeshOps();
  MeshOps(mb::Mesh *m);
  void UndoLast();
  void SelectObject(cameraWrapper *viewCam, mb::Vector screenPos);
  void SelectFaces(QList<mb::Vector> &poly);
  bool SelectFaces();
  bool MeshOps::SelectFaces(mb::AxisAlignedBoundingBox box);
  bool SelectFaces(mb::Vector centrePoint, float widthHeight, float dropOffRate);
  bool SelectFaces(mb::Vector centrePoint, float width, float height, float dropOffRate);
  //Select in a box with corners v1 and v2 (Z is ignored)
  void boxSelect(mb::Vector &v1,mb::Vector &v2);
  void polygonSelect(QList<mb::Vector> &points, QList<mb::SurfacePoint>&sp,
                         QList<int> &faces, QList<int> &vertices);
  void addVertex(int fi);
  void setMesh(mb::Mesh *m);
  void MeshOps::ChangeCamera(cameraWrapper *cam);
  void MoveVertices(mb::Vector v);
  bool MeshOps::CheckIntersection(mb::AxisAlignedBoundingBox box1);
  void MeshOps::DeselectAllFaces();
};

#endif