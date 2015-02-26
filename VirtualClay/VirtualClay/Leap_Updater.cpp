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
  menuFilter = new Leap_HUD();
  mb::Kernel()->ViewPort()->AddFilter(menuFilter);
  menuFilter->SetVisible(false);
  inMenu = false;
  menuLeft = false;
  menuDown = false;
  menuRight = false;
  menuUp = false;
  collisionToggle = true;
  thumbGrabModeToggle = true;
}

mb::Vector Leap_Updater::fitToCameraSpace() {
  mb::Vector camPos = viewCam->getPosision();
  mb::Vector pO = mb::Vector(0,150,300);
  mb::Vector heightOffset = mb::Vector(0,150,0);
  mb::Vector camForward = viewCam->getForward();
  int forwardfactor = (int)(pO.Length());
  mb::Vector centrePoint = camPos - (camForward*forwardfactor) - heightOffset;
  return centrePoint;
}

mb::Vector Leap_Updater::rotateCamera() {
    mb::Vector camPos = viewCam->getTNode()->Position();
    mb::Vector sceneRotate = leapReader->rotateScene();
    //sceneRotate = mb::Vector(0,1,0);
    //mb::Vector aimPoint = viewCam->getCamera()->Aim();
    viewCam->getCamera()->SetAim(mb::Vector(0,0,0));
    //mb::Vector aimPoint = fitToCameraSpace();
    viewCam->getTNode()->AddRotation(sceneRotate);
    return RotateVectorAroundPivot(camPos,mb::Vector(0,0,0),-RAD_TO_DEG*sceneRotate);
}

void Leap_Updater::getFingerScreenSpace(mb::Vector &camPos) { 
  meshOp->ChangeCamera(viewCam);
  mb::Vector indexPos = leapReader->getFingerPosition(INDEX,l).at(0);
  indexPos += camPos;
  mb::Vector projPos = viewCam->getCamera()->Project(indexPos);
  mbstatus(VectorToQString(indexPos)+"Pos"+VectorToQStringLine(projPos));
  //mblog(VectorToQString(indexPos)+"Pos"+VectorToQStringLine(projPos));
  meshOp->SelectObject(viewCam,projPos);
}

mb::Vector Leap_Updater::GetRelativeScreenSpaceFromWorldPos(mb::Vector &wPos) {
  mb::Camera *activeCam = mb::Kernel()->Scene()->ActiveCamera();
  return activeCam->Project(wPos);
}

void Leap_Updater::CollisionDectectionMovement() {

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
  //int viewcamID = idList->getViewCam();
  //cameraWrapper *leftHand = new cameraWrapper(viewcamID);
  int leftCamID = idList->getCam(l);
  mblog("Camera ID = "+QString::number(leftCamID)+"\n");

  cameraWrapper *leftHand = new cameraWrapper(leftCamID);
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
  if(distanceDiff.x > 10 || distanceDiff.y > 10 || distanceDiff.z > 10) {
    distanceDiff = mb::Vector(0,0,0);
  }
  //mblog("Moving Mesh currentHandPos: "+VectorToQString(currentHandPos)+
  //  "lastFrameHandPos: "+VectorToQString(lastFrameHandPos)+
  //  "DistanceDiff: "+VectorToQStringLine(distanceDiff));
  meshOp->MoveVertices(distanceDiff);
  lastFrameHandPos = currentHandPos;
}

int Leap_Updater::countIntersectingFingers(LR lOrR) {
  int counter = 0;
  if(lOrR == l) {
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


bool Leap_Updater::ThumbSelect() {  
  mb::Vector thumbPos = hand_l->GetFingerPos(THUMB,TIP);
  mb::Vector thumbProj = viewCam->getCamera()->Project(thumbPos);
  hand_l->SetVisi(false);
  mblog("Thumb Proj Pos = "+VectorToQStringLine(thumbProj));
  mblog("Thumb Proj Pos Pixels = "+VectorToQStringLine(ScreenSpaceToPixels(thumbProj)));
  if(!facesAreSelected && meshOp->SelectFaces(ScreenSpaceToPixels(thumbProj),10.0f,5)) {
    facesAreSelected = true;
  } else {
    mb::Vector currentThumbPos = hand_l->GetFingerPos(THUMB,TIP);
    mb::Vector distanceDiff = currentThumbPos - lastFrameThumbPos;
    if(distanceDiff.x > 10 || distanceDiff.y > 10 || distanceDiff.z > 10) {
      distanceDiff = mb::Vector(0,0,0);
    }
    //mblog("Moving Mesh currentHandPos: "+VectorToQString(currentHandPos)+
    //  "lastFrameHandPos: "+VectorToQString(lastFrameHandPos)+
    //  "DistanceDiff: "+VectorToQStringLine(distanceDiff));
    meshOp->MoveVertices(distanceDiff);
    lastFrameThumbPos = currentThumbPos;
    mbstatus("Moving faces");
    mblog("Moving Faces");
  }
  return true;
}

void Leap_Updater::SetHandAndFingerPositions(mb::Vector &camRot, mb::Vector &cameraPivot) {
  //TODO:
  // Do I need to actually rotate the fingers?? If so, by what metric?
  // hand_l->SetFingerRot(fingerEnum(i),leapReader->getFingerDirection_L(fingerEnum(i)));
  //mblog("Setting hand visibility!\n");
  hand_l->SetVisi(leapReader->isVisible(l));
  hand_r->SetVisi(leapReader->isVisible(r));
      

  //mblog("Setting hand Camera Positions\n");
  int leftCamID = idList->getCam(l);
  int rightCamID = idList->getCam(r);
  //cameraWrapper *leftHand = new cameraWrapper(leftCamID);
  //leftHand->getTNode()->SetRotation(leapReader->getDirection_L()+mb::Vector(78,0,0));
  //leftHand->getTNode()->SetPosition(cameraPivot + leapReader->getPosition_L());
  cameraWrapper *rightHand = new cameraWrapper(rightCamID);
  rightHand->getTNode()->SetRotation(leapReader->getDirection_L()+mb::Vector(78,0,0));
  rightHand->getTNode()->SetPosition(cameraPivot + leapReader->getPosition_R());
  //set finger position and orientation
  //mblog("Setting hand positions\n");
  hand_l->SetPos(cameraPivot + leapReader->getPosition_L());
  hand_l->SetRot(leapReader->getDirection_L());
  hand_r->SetPos(cameraPivot + leapReader->getPosition_R());
  hand_r->SetRot(leapReader->getDirection_R());
  hand_l->RotateAroundPivot(-1*camRot,cameraPivot);
  hand_r->RotateAroundPivot(-1*camRot,cameraPivot);

  cameraWrapper *leftHand = new cameraWrapper(leftCamID);
  leftHand->getTNode()->SetRotation(hand_l->GetRot()+mb::Vector(80,0,0));
  leftHand->getTNode()->SetPosition(hand_l->GetPos());
  //mblog("Setting finger positions\n");
  bool leftColl = false;  
  //mblog("Finger IntersectCount = "+QString::number(countIntersectingFingers(l))+"\n");
  
  //For Coll detection and replacement:
  mb::Vector fingerPos_L = mb::Vector(0,0,0);
  mb::Vector fingerPos_R = mb::Vector(0,0,0);
  
  for(int i = 0 ; i < 5 ; i++) {
    //Check for collisions
    leftColl = false;
    //hand_l->UpdateCollisionPos(fingerEnum(i),cameraPivot+leapReader->getFingerPosition(fingerEnum(i),l).at(0),-1*camRot,cameraPivot);
    //if(meshOp->CheckIntersection(hand_l->getCollisionBox(cameraPivot+leapReader->getFingerPosition(fingerEnum(i),l).at(0),-1*camRot,cameraPivot))) {
    //  mblog("Collision detected!\n");
    //  leftColl = true;
    //}
    for(int j = 0 ; j < 3 ; j++) {
      //TODO: Remove restriction to only fingerTips
        //#Code:111
      if(j == 0) {
        fingerPos_L = hand_l->GetFingerPos(fingerEnum(i),jointEnum(j));
        fingerPos_R = hand_r->GetFingerPos(fingerEnum(i),jointEnum(j));
        hand_l->SetFingerPos(jointEnum(j),fingerEnum(i),cameraPivot + leapReader->getFingerPosition(fingerEnum(i),l).at(j));
        hand_l->RotateAroundPivot(jointEnum(j),fingerEnum(i),-1*camRot,cameraPivot);

        hand_r->SetFingerPos(jointEnum(j),fingerEnum(i),cameraPivot + leapReader->getFingerPosition(fingerEnum(i),r).at(j));
        hand_r->RotateAroundPivot(jointEnum(j),fingerEnum(i),-1*camRot,cameraPivot);
        if(collisionToggle) {
          if(meshOp->CheckIntersection(hand_l->GetFingerBoundingBox(fingerEnum(i),jointEnum(j)))) {
            hand_l->SetFingerPos(jointEnum(j),fingerEnum(i),fingerPos_L);
            //mblog("L Hand collision!\n");
          }
          if(meshOp->CheckIntersection(hand_r->GetFingerBoundingBox(fingerEnum(i),jointEnum(j)))) {
            hand_r->SetFingerPos(jointEnum(j),fingerEnum(i),fingerPos_R);
            //mblog("R Hand collision!\n");
          }
        }
      }
    }
    ////hand_l->SetFingerPos(fingerEnum(i),cameraPivot + leapReader->getFingerPosition(fingerEnum(i),l).at(0));
    ////hand_r->SetFingerPos(fingerEnum(i),cameraPivot + leapReader->getFingerPosition_R(fingerEnum (i)));
    //hand_r->RotateAroundPivot(fingerEnum(i),-1*camRot,cameraPivot);
  }
}

void Leap_Updater::CameraMovement() {
  mb::Vector sceneRotate = rotateCamera();
  //viewCam->getTNode()->SetPosition(sceneRotate);
  mblog("SceneRotate = "+VectorToQStringLine(sceneRotate));
}

void Leap_Updater::MenuSettings() {
  mb::Vector PosDifference = menuStartSpace - GetRelativeScreenSpaceFromWorldPos(hand_r->GetFingerPos(INDEX));
  menuFilter->menuChoice = 0;
  if(PosDifference.y > 0.1) {
    menuFilter->menuChoice = 3;
    if(PosDifference.y > 0.3)
      menuDown = true;
  } else if(PosDifference.y < -0.1) {
    menuFilter->menuChoice = 1;
    if(PosDifference.y < -0.3)
      menuUp = true;
  }
  if(abs(PosDifference.x) > abs(PosDifference.y)) {
    if(PosDifference.x > 0.1) {
      menuFilter->menuChoice = 4;
      if(PosDifference.x > 0.3) {
        menuLeft = true;
      }
    } else if(PosDifference.x < -0.1) {
      menuFilter->menuChoice = 2;
      if(PosDifference.x < -0.3) {
        menuRight = true;
      }
    }
    if(leapReader->isCircleCCW) {
      mbstatus("Got Anti-Clockwise Circle");
      menuFilter->SetVisible(false);
      inMenu = false;
    }
  }
  if(menuUp){
    menuFilter->SetVisible(false);
    thumbGrabModeToggle = !thumbGrabModeToggle;
    if(thumbGrabModeToggle)
      mbhud("Thumb Grab Mode: On");
    else
      mbhud("Thumb Grab Mode: Off");
    inMenu = false; 
    menuUp = false;
  } else 
  if(menuRight) {
    collisionToggle = !collisionToggle;
    if(collisionToggle)
      mbhud("Collision Mode: On");
    else
      mbhud("Collision Mode: Off");
    menuFilter->SetVisible(false);
    inMenu = false;
    menuRight = false;
  } else
  if(menuDown) {
    menuFilter->SetVisible(false);
    inMenu = false;
    menuDown = false;
  } else
  if(menuLeft) {
    menuFilter->SetVisible(false);
    inMenu = false; 
    menuLeft = false;

  }
}


void Leap_Updater::Extrusion(mb::Vector &cameraPivot) {
  mblog("Finger IntersectCount = "+QString::number(countIntersectingFingers(l))+"\n");
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
}


void Leap_Updater::OnEvent(const mb::EventGate &cEvent) {
  if(cEvent == frameEvent) {
    if(leapReader->updateAll()) {
      if(leapReader->ishands && leapReader->isConnected) {
        //mblog("Getting CameraID\n");
        int viewcamID = idList->getViewCam();
        viewCam = new cameraWrapper(viewcamID);
        mb::Vector cameraPivot = fitToCameraSpace();
        mb::Vector camRot = viewCam->getTNode()->Rotation();
        SetHandAndFingerPositions(camRot,cameraPivot);
        //If Circle Anti-Clockwise
        if(inMenu) {
          MenuSettings();
        } else {
                    //If Only 1 hand is showing
          if(leapReader->isVisible(l) ^ leapReader->isVisible(r)) {
            //CameraMovement();
          }
          //If Circle Clockwise
          if(leapReader->isCircleCW) {
            menuStartSpace = GetRelativeScreenSpaceFromWorldPos(hand_r->GetFingerPos(INDEX));
            menuFilter->SetCentre(menuStartSpace);
            menuFilter->SetVisible(true);
            inMenu = true;
          }
          //If they tapped the screen
          if(leapReader->isScreenTap) {
            mblog("got tap\n");
            getFingerScreenSpace(cameraPivot);
          }
          //If they Swiped Left
          if(leapReader->isUndo) {
            mb::Kernel()->Interface()->HUDMessageShow("UNDO");
            meshOp->UndoLast(); 
          }
          if(thumbGrabModeToggle) {
            if((meshOp->CheckIntersection(hand_l->GetFingerBoundingBox(THUMB,TIP)))) {
              ThumbSelect();
            } else {
              if(facesAreSelected) {
                meshOp->DeselectAllFaces();
              }
              facesAreSelected = false;
            }
          } else {
          //If they are Grabbing and Thumb Toggle is Off
            if(leapReader->isGrabbing_L ) {
              Extrusion(cameraPivot);
            } else {
              if(facesAreSelected) {
                meshOp->DeselectAllFaces();
              }
              firstmoveswitch = true;
              facesAreSelected = false;
            }
            lastFrameHandPos == hand_l->GetPos();
          }
        }
      }
    }
  }
}


