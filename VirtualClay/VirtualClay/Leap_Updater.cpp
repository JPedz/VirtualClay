#include "stdafx.h"
#include "Leap_Updater.h"

namespace mb = mudbox;

Leap_Updater::Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r)
  :frameEvent(this)
{
  idList = idl;
  hand_l = l;
  hand_r = r;
  meshOp = new MeshOps();
  Leap_HUD *leapHud = new Leap_HUD();
  meshOp->ChangeCamera(new cameraWrapper(idList->getCam(LR(0))));
  leapReader = new Leap_Reader();
  leapReader->SetScale(mb::Vector(1,1,1));
  frameEvent.Connect(mb::Kernel()->ViewPort()->FrameEvent);
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

void Leap_Updater::getFingerScreenSpace(mb::Vector &camPos,cameraWrapper *viewCam) { 
  meshOp->ChangeCamera(viewCam);
  mb::Vector indexPos = leapReader->getFingerPosition(INDEX,l).at(0);
  indexPos += camPos;
  mb::Vector projPos = viewCam->getCamera()->Project(indexPos);
  mbstatus(VectorToQString(indexPos)+"Pos"+VectorToQStringLine(projPos));
  //mblog(VectorToQString(indexPos)+"Pos"+VectorToQStringLine(projPos));
  meshOp->SelectObject(viewCam,projPos);
}

bool Leap_Updater::selectMeshPinch(mb::Vector &camPos) {
  int leftcamID = idList->getCam(l);
  int viewcamID = idList->getViewCam();
  cameraWrapper *leftHand = new cameraWrapper(viewcamID);
  meshOp->ChangeCamera(leftHand);
  mb::Vector indexPos = camPos + leapReader->getFingerPosition(INDEX,l).at(0);
  mb::Vector thumbPos = camPos + leapReader->getFingerPosition(THUMB,l).at(0);
  mb::Vector thumbProj = leftHand->getCamera()->Project(thumbPos);
  mb::Vector indexProj = leftHand->getCamera()->Project(indexPos);
  mb::Vector midPoint = (thumbProj+indexProj)/2;
  return meshOp->SelectFaces(midPoint,20.0f,0.0f);
}

bool Leap_Updater::selectMesh(mb::Vector &camPos) {
  int leftcamID = idList->getCam(l);
  int viewcamID = idList->getViewCam();
  cameraWrapper *leftHand = new cameraWrapper(viewcamID);
  meshOp->ChangeCamera(leftHand);
  //QList<mb::Vector> polygonSelect;

  //mb::Vector thumbPos = camPos + leapReader->getFingerPosition_L(THUMB);
  //mb::Vector thumbProj = leftHand->getCamera()->Project(thumbPos); 
  //if(thumbProj.z <= 1 && thumbProj.z >= 0)
  //  polygonSelect.push_back(thumbPos);

  mb::Vector cameraMidpoint = mb::Vector(mb::Kernel()->ViewPort()->Width()/2,mb::Kernel()->ViewPort()->Height()/2,0);
  return meshOp->SelectFaces();
}

void Leap_Updater::MoveMesh() {
  mb::Vector currentHandPos = hand_l->GetPos();
  mb::Vector distanceDiff = currentHandPos - lastFrameHandPos;
  //mblog("Moving Mesh currentHandPos: "+VectorToQString(currentHandPos)+
  //  "lastFrameHandPos: "+VectorToQString(lastFrameHandPos)+
  //  "DistanceDiff: "+VectorToQStringLine(distanceDiff));
  meshOp->MoveVertices(distanceDiff);
  lastFrameHandPos = currentHandPos;
}

int Leap_Updater::countIntersectingFingers(LR lOrR) {
  int counter = 0;
  if(lOrR = l) {
    for(int i = 0; i < 5 ; i++) {
      if(meshOp->CheckIntersection(hand_l->GetFingerBoundingBox(fingerEnum(i)))) {
        counter++;
      }
    }
  } else {
      for(int i = 0; i < 5 ; i++) {
      if(meshOp->CheckIntersection(hand_r->GetFingerBoundingBox(fingerEnum(i)))) {
        counter++;
      }
    }
  }
  return counter;
}

void Leap_Updater::SetHandAndFingerPositions(mb::Vector &camRot, mb::Vector &cameraPivot) {
  //TODO:
  // Do I need to actually rotate the fingers?? If so, by what metric?
  // hand_l->SetFingerRot(fingerEnum(i),leapReader->getFingerDirection_L(fingerEnum(i)));
  //mblog("Setting hand visibility!\n");
  hand_l->SetVisi(leapReader->isVisible(l));
  hand_r->SetVisi(leapReader->isVisible(r));
      

  //mblog("Setting hand Camera Positions\n");
  int leftcamID = idList->getCam(l);
  cameraWrapper *leftHand = new cameraWrapper(leftcamID);
  leftHand->getTNode()->SetRotation(leapReader->getDirection_L()+mb::Vector(90,0,0));
  leftHand->getTNode()->SetPosition(cameraPivot + leapReader->getPosition_L());
  //set finger position and orientation
  //mblog("Setting hand positions\n");
  hand_l->SetPos(cameraPivot + leapReader->getPosition_L());
  hand_l->SetRot(leapReader->getDirection_L());
  hand_r->SetPos(cameraPivot + leapReader->getPosition_R());
  hand_r->SetRot(leapReader->getDirection_R());
  hand_l->RotateAroundPivot(-1*camRot,cameraPivot);
  hand_r->RotateAroundPivot(-1*camRot,cameraPivot);
  //mblog("Setting finger positions\n");
  for(int i = 0 ; i < 5 ; i++) {
    for(int j = 0 ; j < 3 ; j++) {
      //TODO: Remove restriction to only fingerTips
        //#Code:111
      if(j == 0) {
        hand_l->SetFingerPos(jointEnum(j),fingerEnum(i),cameraPivot + leapReader->getFingerPosition(fingerEnum(i),l).at(j));
        hand_r->SetFingerPos(jointEnum(j),fingerEnum(i),cameraPivot + leapReader->getFingerPosition(fingerEnum(i),r).at(j));
        hand_l->RotateAroundPivot(jointEnum(j),fingerEnum(i),-1*camRot,cameraPivot);
        hand_r->RotateAroundPivot(jointEnum(j),fingerEnum(i),-1*camRot,cameraPivot);
      }
    }
    ////hand_l->SetFingerPos(fingerEnum(i),cameraPivot + leapReader->getFingerPosition(fingerEnum(i),l).at(0));
    ////hand_r->SetFingerPos(fingerEnum(i),cameraPivot + leapReader->getFingerPosition_R(fingerEnum (i)));
    //hand_r->RotateAroundPivot(fingerEnum(i),-1*camRot,cameraPivot);
  }
}


void Leap_Updater::OnEvent(const mb::EventGate &cEvent) {
  if(cEvent == frameEvent) {
    if(leapReader->updateAll()) {
      if(leapReader->ishands && leapReader->isConnected) {
        //mblog("Getting CameraID\n");
        int viewcamID = idList->getViewCam();
        viewCam = new cameraWrapper(viewcamID);
        mb::Vector camRot = viewCam->getTNode()->Rotation();
        mb::Vector cameraPivot = fitToCameraSpace(viewCam);
        //mb::Vector sceneRotate = rotateCamera(viewCam);
        //viewCam->getTNode()->SetPosition(sceneRotate);
        //mblog("SceneRotate = "+VectorToQStringLine(sceneRotate));
        //mblog("Setting hand and finger positions\n");
        SetHandAndFingerPositions(camRot,cameraPivot);
        if(leapReader->isScreenTap) {
          mblog("got tap\n");
          getFingerScreenSpace(cameraPivot,viewCam);
        }
        if(leapReader->isUndo) {
          mb::Kernel()->Interface()->HUDMessageShow("UNDO");
          meshOp->UndoLast(); 
        }
        if(leapReader->isGrabbing_L) {
          if(!facesAreSelected) {
            if(selectMesh(cameraPivot)) {
              facesAreSelected = true;
              firstmoveswitch = true;
              mblog("Succesfully Selected\n");
            } else {
              mb::Kernel()->Interface()->HUDMessageShow("Failed To Select");
              mblog("Failed to select\n");
            }
            mbstatus("Grabbing");
          } else {
            if(firstmoveswitch) {
              lastFrameHandPos == hand_l->GetPos();
              firstmoveswitch = false;
            } else {
              mbstatus("MovingMesh");
              mblog("Moving Mesh\n");
              MoveMesh();
            }
          }
        } else {
          if(facesAreSelected) {
            meshOp->DeselectAllFaces();
          }
          firstmoveswitch = true;
          facesAreSelected = false;
        }
        lastFrameHandPos == hand_l->GetPos();
        if(meshOp->CheckIntersection(hand_l->GetFingerBoundingBox(INDEX))) {
          mblog("GotIntersection\n");
          mbstatus("Index isIntersecting");
        }
      }
    }
  }
}


