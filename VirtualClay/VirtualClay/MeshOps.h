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
  typedef struct VertexFaceInfo {
    int vI;
    std::vector<int> fI;
  } VertexFaceInfo;
  std::vector<VertexFaceInfo> *vFI;
  typedef struct MidVertex {
    int vI;
    mb::Vector pos;
  } MidVertex;
  typedef struct VertexInfo {
    int vI;
    mb::Vector pos;
  } VertexInfo;
  typedef struct VertexModifyInfo {
    float strength;
    int vI;
  } VertexModifyInfo;
  std::vector<int> *faces;
  std::vector<VertexModifyInfo> *vertices;
  std::vector<mb::SurfacePoint > *points;
  std::vector<std::vector<VertexInfo>> undoQueue;
  void StoreUndoQueue();
  void MeshOps::AddVFI(int vi, int fi);
  MidVertex *midV;

public:
  MeshOps();
  MeshOps(mb::Mesh *m);
  void UndoLast();
  void SelectObject(cameraWrapper *viewCam, mb::Vector screenPos);
  void SelectFaces(QList<mb::Vector> &poly);
  bool SelectFaces(mb::AxisAlignedBoundingBox box,float spreadDist = 0);
  bool SelectFaces();
  bool SelectFaces(mb::Vector centrePoint, float widthHeight, float dropOffRate);
  bool SelectFaces(mb::Vector centrePoint, float width, float height, float dropOffRate);
  //Select in a box with corners v1 and v2 (Z is ignored)
  void boxSelect(mb::Vector &v1,mb::Vector &v2);
  void polygonSelect(QList<mb::Vector> &points, QList<mb::SurfacePoint>&sp,
                         QList<int> &faces, QList<int> &vertices);
  void addVertex(int fi);
  void setMesh(mb::Mesh *m);
  void ChangeCamera(cameraWrapper *cam);
  void MoveVertices(mb::Vector v);
  bool CheckIntersection(mb::AxisAlignedBoundingBox box1);
  void DeselectAllFaces();
};

#endif