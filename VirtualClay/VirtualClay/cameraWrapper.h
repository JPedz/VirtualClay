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
    cameraWrapper(int id);
    mb::Camera *getCamera(void);
    mb::Transformation *getTNode(void);
    mb::GroupNode *getGNode(void);
    void setTranslation(mb::Vector);
    void addCameraToScene(void);
    void setAim(mb::Vector v);
    void setTNode(void);
    int getID();
    QString getName();
    void deleteCam(void);
    mb::Vector getForward(void);
    void setRotation(mb::Vector &ypr);
    void setPosition(mb::Vector &pos);
    void setPosition(mb::Vector pos);
    void MoveForward(float dist);
    mb::Vector getPosition(void);
};

#endif