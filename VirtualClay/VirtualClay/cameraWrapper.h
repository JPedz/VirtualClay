#ifndef CAMERA_WRAPPER
#define CAMERA_WRAPPER
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
};

#endif