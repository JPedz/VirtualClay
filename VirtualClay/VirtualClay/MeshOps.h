#ifndef MESHOPS_H_
#define MESHOPS_H_
#include "stdafx.h"

namespace mb = mudbox;

class MeshOps {

    mb::Mesh *pMesh;
    mb::ScreenSpacePicker *ssp;
    mb::Picker *p;
    mb::Camera *curCam;
    bool MeshOps::checkUniqueInList(unsigned int  &fi,QList<int> &faces);
    QList<int> MeshOps::getFaces(QList<mb::SurfacePoint> &sPs);
    void MeshOps::extrudefaces(std::vector<int> &faces);

  public:
    MeshOps();
    MeshOps(mb::Mesh *m);
    void pickObj(void);
    //Select in a box with corners v1 and v2 (Z is ignored)
    void boxSelect(mb::Vector &v1,mb::Vector &v2, QList<mb::SurfacePoint> &sp,
                         QList<int> &faces, QList<int> &vertices);
    void addVertex(unsigned int  fi, QList<int> &v_list);
    void setMesh(mb::Mesh *m);
};


#endif