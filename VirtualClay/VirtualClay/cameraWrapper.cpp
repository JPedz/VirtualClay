#include "stdafx.h"
#include "cameraWrapper.h"


cameraWrapper::cameraWrapper(QString name) {
  cam = mb::CreateInstance<mb::Camera>();
  cam->SetName(name);
  cam_GNode = dynamic_cast<mb::GroupNode *>(cam);
  cam_TNode = cam_GNode->Transformation();
}

mb::Camera *cameraWrapper::getCamera(void) {
  return cam;
}
mb::GroupNode *cameraWrapper::getGNode(void) {
  return cam_GNode;
}

mb::Transformation*cameraWrapper::getTNode(void) {
  return cam_TNode;
}

void cameraWrapper::setTNode(void) {
  cam_TNode = cam_GNode->Transformation();
}

void cameraWrapper::addCameraToScene(void) {
  mb::Kernel()->Scene()->AddCamera(cam);  
	mb::Kernel()->Interface()->RefreshUI();
}

void cameraWrapper::setAim(mb::Vector v) {
  cam->SetAim(v);
}

void cameraWrapper::setTranslation(mb::Vector v) {
  //cam_TNode = cam_GNode->Transformation();
  cam_TNode->SetPosition(v);
}