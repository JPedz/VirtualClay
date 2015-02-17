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
  bool MeshOps::checkUniqueInList(unsigned int  &fi,QList<int> &faces);
  QList<int> MeshOps::getFaces(QList<mb::SurfacePoint> &sPs);
  void MeshOps::extrudefaces(std::vector<int> &faces);

public:
  MeshOps();
  MeshOps(mb::Mesh *m);
  void SelectObject(cameraWrapper *viewCam, mb::Vector screenPos);
  void pickObj(QList<mb::Vector> &poly);
  void pickObj();
  //Select in a box with corners v1 and v2 (Z is ignored)
  void boxSelect(mb::Vector &v1,mb::Vector &v2, QList<mb::SurfacePoint> &sp,
                        QList<int> &faces, QList<int> &vertices);
  void polygonSelect(QList<mb::Vector> &points, QList<mb::SurfacePoint>&sp,
                         QList<int> &faces, QList<int> &vertices);
  void addVertex(unsigned int  fi, QList<int> &v_list);
  void setMesh(mb::Mesh *m);
  void MeshOps::ChangeCamera(cameraWrapper *cam);
};

#endif