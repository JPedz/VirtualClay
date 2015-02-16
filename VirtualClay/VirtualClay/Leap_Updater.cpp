#include "stdafx.h"
#include "Leap_Updater.h"

namespace mb = mudbox;

Leap_Updater::Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r)
  :frameEvent(this)
{
  idList = idl;
  hand_l = l;
  hand_r = r;
  leapReader = new Leap_Reader();
  frameEvent.Connect(mb::Kernel()->ViewPort()->FrameEvent);
	mb::Kernel()->Interface()->HUDMessageShow("Frame count has started.", 
    mudbox::Interface::HUDmsgFade);
}

mb::Vector Leap_Updater::fitToCameraSpace(cameraWrapper *viewCam) {
  mb::Vector camPos = viewCam->getPosision();
  mb::Vector pO = mb::Vector(0,150,300);
  mb::Vector heightOffset = mb::Vector(0,150,0);
  mb::Vector camForward = viewCam->getForward();
  int forwardfactor = (int)(pO.Length());
  mb::Vector centrePoint = camPos - (camForward*forwardfactor) - heightOffset;
  return centrePoint;
}

mb::Vector Leap_Updater::rotateCamera(cameraWrapper *viewCam) {
    mb::Vector camPos = viewCam->getTNode()->Position();
    mb::Vector sceneRotate = leapReader->rotateScene();
    //sceneRotate = mb::Vector(0,1,0);
    //mb::Vector aimPoint = viewCam->getCamera()->Aim();
    mb::Vector aimPoint = fitToCameraSpace(viewCam);
    viewCam->getTNode()->AddRotation(sceneRotate);
    return RotateVectorAroundPivot(camPos,mb::Vector(0,0,0),-RAD_TO_DEG*sceneRotate);
}

void Leap_Updater::OnEvent(const mb::EventGate &cEvent) {
  if(cEvent == frameEvent) {
    leapReader->updateAll();
    if(leapReader->ishands && leapReader->isConnected) {
      int viewcamID = idList->getViewCam();
      cameraWrapper *viewCam = new cameraWrapper(viewcamID);
      mb::Vector camRot = viewCam->getTNode()->Rotation();
      mb::Vector cameraPivot = fitToCameraSpace(viewCam);
      hand_l->SetVisi(leapReader->isVisible(l));
      hand_r->SetVisi(leapReader->isVisible(r));
      //set finger position and orientation
      hand_l->SetPos(cameraPivot + leapReader->getPosition_L());
      hand_l->SetRot(leapReader->getDirection_L());
      hand_r->SetPos(cameraPivot + leapReader->getPosition_R());
      hand_r->SetRot(leapReader->getDirection_R());
      hand_l->RotateAroundPivot(-1*camRot,cameraPivot);
      hand_r->RotateAroundPivot(-1*camRot,cameraPivot);
      mb::Vector sceneRotate = rotateCamera(viewCam);
      viewCam->getTNode()->SetPosition(sceneRotate);
      mblog("SceneRotate = "+VectorToQStringLine(sceneRotate));
      for(int i = 0 ; i < 5 ; i++) {
        hand_l->SetFingerPos(fingerEnum(i),cameraPivot + leapReader->getFingerPosition_L(fingerEnum(i)));
        hand_l->SetFingerRot(fingerEnum(i),leapReader->getFingerDirection_L(fingerEnum(i)));
        hand_r->SetFingerPos(fingerEnum(i),cameraPivot + leapReader->getFingerPosition_R(fingerEnum (i)));
        hand_r->SetFingerRot(fingerEnum(i),leapReader->getFingerDirection_R(fingerEnum(i)));
        hand_l->RotateAroundPivot(fingerEnum(i),-1*camRot,cameraPivot);
        hand_r->RotateAroundPivot(fingerEnum(i),-1*camRot,cameraPivot);
      }
    }
  }
}
