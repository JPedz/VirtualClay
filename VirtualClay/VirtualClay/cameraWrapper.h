#ifndef CAMERA_WRAPPER_H_
#define CAMERA_WRAPPER_H_
#include "stdafx.h"

namespace mb = mudbox;
class cameraWrapper {

  private:
    mb::Camera *cam;
    mb::Transformation *cam_TNode;
    mb::GroupNode *cam_GNode;
  public:
    cameraWrapper(QString name);
    mb::Camera *getCamera(void);
    mb::Transformation *getTNode(void);
    mb::GroupNode *getGNode(void);
    void setTranslation(mb::Vector);
    void addCameraToScene(void);
    void setAim(mb::Vector v);
    void setTNode(void);
    int getID();
    QString getName();
    void selectPoints(mb::Vector v);
    void deleteCam(void);
};

#endif