#ifndef MESHOPS_H_
#define MESHOPS_H_
#include "stdafx.h"
#include "cameraWrapper.h"
#include <QtCore/QTime>
#include "LeapMath.h"
#include "Leap.h"
#include "Leap_Tool.h"
//#include <omp.h>

namespace mb = mudbox;

class MeshOps {
  //Types
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
    unsigned int lVI;
  } VertexModifyInfo;
  typedef struct TessInfo {
    int fi;
    mb::Vector splitA;
    mb::Vector splitB;
    int vIA;
    int vIB;
    bool type;
  } TessInfo;

  //Temporary
  mb::Mesh *pMesh;
  mb::Geometry *MeshGeo;
  mb::ScreenSpacePicker *ssp;
  mb::LayerMeshData *meshLayer;
  mb::Picker *p;
  mb::Camera *curCam;
  std::vector<int> *faces_L;
  std::vector<VertexModifyInfo> *vertices_L;
  std::vector<mb::SurfacePoint > *points_L;
  std::vector<int> *faces_R;
  std::vector<VertexModifyInfo> *vertices_R;
  std::vector<mb::SurfacePoint > *points_R;
  std::vector<bool> *meshStoreToggle;
  std::vector<mb::Mesh*> *meshStore;
  std::vector<std::vector<VertexInfo> > undoQueue;
  std::vector<mb::Vector> undoMoveQueue;
  MidVertex *midV;
  mb::Vector midPos;
  
  mb::Vector cumulativeMove;

  void refreshMesh(bool update = true);
  bool checkUniqueInFaceList(std::vector<int> *faces, int fi);
  bool checkUniqueInVertexList(std::vector<VertexModifyInfo> *vertices, int fi);
  void StoreUndoQueue(LR lr);
  void AddToUndoQueue(LR lr);
  void AddVFI(int vi, int fi);
  
  void SelectObjectFromHands();

  //TesselationInfo;
  int UniqueTessInfo(LR lr,TessInfo &tI);
  std::vector<TessInfo> *tessInfo_L;
  std::vector<TessInfo> *tessInfo_R;

public:
  MeshOps();
  MeshOps(mb::Mesh *m);
  void UndoLast();
  void SelectObject(cameraWrapper *viewCam, mb::Vector screenPos);
  void SelectFaces(QList<mb::Vector> &poly);
  //bool SelectFaces(LR lr, mb::AxisAlignedBoundingBox box,float spreadDist = 0);
  bool SelectFaces(LR lr, float size = 30, float strength = 10);
  bool SelectFaces(LR lr, mb::Vector centrePoint, float widthHeight, float dropOffRate);
//  bool SelectFaces(LR lr, mb::Vector centrePoint, float width, float height, float dropOffRate);
  //Select in a box with corners v1 and v2 (Z is ignored)
  void boxSelect(LR lr, mb::Vector &v1,mb::Vector &v2, float maxDist = 30, float strength = 10);
  void boxSelect(LR lr, mb::Vector centrePoint ,Leap_Tool *tool);
  //void polygonSelect(QList<mb::Vector> &points, QList<mb::SurfacePoint>&sp,
  //                       QList<int> &faces, QList<int> &vertices);
  void addVertex(LR lr, int fi);
  void SetMesh(mb::Mesh *m);
  void ChangeCamera(cameraWrapper *cam);
  void MoveVertices(LR lr, mb::Vector v, bool update = true);
  void MoveVertices(LR lr, float dist);
  bool CheckIntersection(mb::AxisAlignedBoundingBox box1);
  bool CheckTouching(mb::AxisAlignedBoundingBox box1);
  void DeselectAllFaces();

  //Tools:
  bool firstUse;
  bool ToolManip(mb::Vector centrePoint, float size, Leap_Tool *tool);
  bool ToolManip(mb::Vector centrePoint, float size, float dropOffRate);
  mb::Vector lastMidPosition;

  //Movement
  void SelectWholeMesh();
  void MoveObject(mb::Vector dist);
  void UndoLastMove();
  void StoreLastMoveUndoQueue();

  void Tesselate();
  void FindTesselationFaces(LR lr);
  void SubDivide();

};

#endif