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
  tools.resize(2);
  tools.at(0) = new Leap_Fingers("ToolTIP");
  tools.at(1) = new Leap_Fingers("ToolBase");
  tools.at(0)->ImportGeo();
  tools.at(1)->ImportGeo();
  tools.at(0)->SetScale(mb::Vector(0.1f,0.1f,0.1f));
  tools.at(1)->SetScale(mb::Vector(0.1f,0.1f,0.1f));
  Leap_HUD *leapHud = new Leap_HUD();
  meshOp->ChangeCamera(new cameraWrapper(idList->getCam(LR(0))));
  leapReader = new Leap_Reader();
  leapReader->SetScale(mb::Vector(1,1,1));
  frameEvent.Connect(mb::Kernel()->ViewPort()->FrameEvent);
  menuFilter = new Leap_HUD();
  mb::Kernel()->ViewPort()->AddFilter(menuFilter);
  menuFilter->SetVisible(false);
  inMenu_L = false;
  inMenu_R = false;
  menuLeft = false;
  menuDown = false;
  menuRight = false;
  menuUp = false;
  collisionToggle = false;
  thumbGrabModeToggle = false;
  stickyMovement = false;
  pinchGrab = true;
  thumbMoveStrength = 10.0f; //Strength and distance for movement intersecting thumb
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

void Leap_Updater::rotateCamera(mb::Vector r) {
    mb::Vector camPos = viewCam->getTNode()->Position();
    mb::Vector aimPoint = viewCam->getCamera()->Aim();
    viewCam->setPosition(RotateVectorAroundPivot(camPos,aimPoint,r));
    viewCam->getCamera()->SetTarget(aimPoint);
}

void Leap_Updater::ScreenTap(mb::Vector &camPos) { 
  meshOp->ChangeCamera(viewCam);
  mb::Vector indexPos = hand_l->GetFingerPos(INDEX,TIP);
  mb::Vector projPos = viewCam->getCamera()->Project(indexPos);
  mblog(VectorToQString(indexPos)+"Pos"+VectorToQStringLine(projPos));
  meshOp->SelectObject(viewCam,projPos);
}

mb::Vector Leap_Updater::GetRelativeScreenSpaceFromWorldPos(mb::Vector &wPos) {
  mb::Camera *activeCam = mb::Kernel()->Scene()->ActiveCamera();
  return activeCam->Project(wPos);
}

void Leap_Updater::CollisionDectectionMovement() {

}


bool Leap_Updater::selectMeshPinch(mb::Vector &camPos) {
  meshOp->ChangeCamera(viewCam);
  mb::Vector indexPos = hand_l->GetFingerPos(INDEX);
  mb::Vector thumbPos = hand_l->GetFingerPos(THUMB);
  if(indexPos.DistanceFrom(thumbPos) < 10) {
    mb::Vector thumbProj = viewCam->getCamera()->Project(thumbPos);
    mb::Vector indexProj = viewCam->getCamera()->Project(indexPos);
    mb::Vector midPoint = (thumbProj+indexProj)*0.5f;
    return meshOp->SelectFaces(midPoint,0.0f,0.0f);
  } else {
    mblog("Fingers too far apart");
    return false;
  }
}

bool Leap_Updater::selectMesh(mb::Vector &camPos) {
  int leftCamID = idList->getCam(l);
  cameraWrapper *leftHand = new cameraWrapper(leftCamID);
  meshOp->ChangeCamera(leftHand);
  hand_l->SetVisi(false);
  bool b = meshOp->SelectFaces();
  hand_l->SetVisi(true);
  return b;
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
  //float save = thumbProj.x;
  //thumbProj.x = thumbProj.y;
  //thumbProj.y = save;
  meshOp->ChangeCamera(viewCam);
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

bool Leap_Updater::ThumbSmoothMove() {
  mb::Vector thumbPos = hand_l->GetFingerPos(THUMB,TIP);
  mb::Vector thumbProj = viewCam->getCamera()->Project(thumbPos);
  hand_l->SetVisi(false);
  mblog("Thumb Proj Pos = "+VectorToQStringLine(thumbProj));
  mblog("Thumb Proj Pos Pixels = "+VectorToQStringLine(ScreenSpaceToPixels(thumbProj)));
  meshOp->ChangeCamera(viewCam);
  if(meshOp->SelectFaces(ScreenSpaceToPixels(thumbProj),10.0f,10)) {
    mb::Vector dirNorm = leapReader->getMotionDirection(THUMB,l);
    mblog("Normalised Direction = "+VectorToQStringLine(dirNorm));
    mb::Vector dist = dirNorm*thumbMoveStrength;
    meshOp->MoveVertices(dist);
    
    hand_l->SetVisi(true);
    return true;
  }
  hand_l->SetVisi(true);
  return false;
}

void Leap_Updater::SetHandAndFingerPositions(mb::Vector &cameraPivot) {
  //TODO:
  // Do I need to actually rotate the fingers?? If so, by what metric?
  // hand_l->SetFingerRot(fingerEnum(i),leapReader->getFingerDirection_L(fingerEnum(i)));
  
  mb::Vector camRot = viewCam->getTNode()->Rotation();
  hand_l->SetVisi(leapReader->isVisible(l));
  hand_r->SetVisi(leapReader->isVisible(r));

  int leftCamID = idList->getCam(l);
  int rightCamID = idList->getCam(r);
  // Set hand position and orientation
  hand_l->SetPos(cameraPivot + leapReader->getPosition_L());
  hand_l->SetRot(leapReader->getDirection_L());
  hand_r->SetPos(cameraPivot + leapReader->getPosition_R());
  hand_r->SetRot(leapReader->getDirection_R());
  hand_l->RotateAroundPivot(-1*camRot,cameraPivot);
  hand_r->RotateAroundPivot(-1*camRot,cameraPivot);

  //Setting Cameras to follow Hand;
  cameraWrapper *leftHand = new cameraWrapper(leftCamID);
  leftHand->getTNode()->SetRotation(hand_l->GetRot()+mb::Vector(80,0,0));
  leftHand->getTNode()->SetPosition(hand_l->GetPos());
  leftHand->MoveForward(-10.0f);
  cameraWrapper *rightHand = new cameraWrapper(rightCamID);
  rightHand->getTNode()->SetRotation(hand_r->GetRot()+mb::Vector(80,0,0));
  rightHand->getTNode()->SetPosition(hand_r->GetPos());
  rightHand->MoveForward(-20.0f);
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
    for(int j = 0 ; j < 4 ; j++) {
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
            if(j == 0) {
              hand_l->SetFingerPos(jointEnum(j),fingerEnum(i),fingerPos_L);
            }
            //mblog("L Hand collision!\n");
          }
          if(meshOp->CheckIntersection(hand_r->GetFingerBoundingBox(fingerEnum(i),jointEnum(j)))) {
            if(j == 0) {
              hand_r->SetFingerPos(jointEnum(j),fingerEnum(i),fingerPos_R);
            }
            //mblog("R Hand collision!\n");
          }
        }
        //mblog("Updating Bone Pos :"+QString::number(j)+"\n");
        //hand_r->UpdateBone(boneEnum(j),fingerEnum(i),leapReader->getBoneOrients(fingerEnum(i),r).at(j));
        //hand_l->UpdateBone(boneEnum(j),fingerEnum(i),leapReader->getBoneOrients(fingerEnum(i),l).at(j));
        //mblog("Updated Bone Pos\n");
      }
    }
  }
  if(leapReader->isTool) {
    tools.at(0)->SetVisi(true);
    tools.at(1)->SetVisi(true);
    std::vector<mb::Vector> toolLocs = leapReader->GetToolPositions();
    tools.at(0)->SetPos(cameraPivot+toolLocs.at(0));
    tools.at(1)->SetPos(cameraPivot+toolLocs.at(1));
    tools.at(0)->RotateAroundPivot(-1*camRot,cameraPivot);
    tools.at(1)->RotateAroundPivot(-1*camRot,cameraPivot);
  } else {
    tools.at(0)->SetVisi(false);
    tools.at(1)->SetVisi(false);
  }
}

void Leap_Updater::CameraMovement() {
  const float deadzone = 20.0f;
  mb::Vector handRot = leapReader->getDirection_R()+mb::Vector(30,0,0);
  mbstatus("HandRot: "+VectorToQString(handRot));
  if(abs(handRot.x) > deadzone || abs(handRot.y) > deadzone || abs(handRot.z) > deadzone) {
  
    if(abs(handRot.x) > abs(handRot.y) && abs(handRot.x) > abs(handRot.z)) {
      mblog("Pitch\n");
      mbhud("Rotate Pitch");
      if(handRot.x > 0) 
        rotateCamera(mb::Vector(0.5,0,0));
      else
        rotateCamera(mb::Vector(-0.5,0,0));
    }
    if(abs(handRot.y) > abs(handRot.x) && abs(handRot.y) > abs(handRot.z)) {
      mblog("Roll\n");
      mbhud("Rotate Roll");
      if(handRot.y > 0) 
        rotateCamera(mb::Vector(0,0.5,0));
      else
        rotateCamera(mb::Vector(0,-0.5,0));
    }
    if(abs(handRot.z) > abs(handRot.x) && abs(handRot.z) > abs(handRot.y)) {
      mblog("Yaw\n");
      mbhud("Rotate Yaw");
      if(handRot.z > 0) 
        rotateCamera(mb::Vector(0,0,0.5));
      else
        rotateCamera(mb::Vector(0,0,-0.5));
    }

  }
  //viewCam->getTNode()->SetPosition(sceneRotate);
}

void Leap_Updater::MenuSettings_R() {
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
    if(leapReader->isCircleCCW_R) {
      mbstatus("Got Anti-Clockwise Circle");
      menuFilter->SetVisible(false);
      inMenu_R = false;
    }
  }
  if(menuUp) {
    menuFilter->SetVisible(false);
    thumbGrabModeToggle = !thumbGrabModeToggle;
    if(thumbGrabModeToggle)
      mbhud("Thumb Grab Mode: On");
    else
      mbhud("Thumb Grab Mode: Off");
    inMenu_R = false; 
    menuUp = false;
  } else if(menuRight) {
    collisionToggle = !collisionToggle;
    if(collisionToggle)
      mbhud("Collision Mode: On");
    else
      mbhud("Collision Mode: Off");
    menuFilter->SetVisible(false);
    inMenu_R = false;
    menuRight = false;
  } else if(menuDown) {
    if(pinchGrab)
      mbhud("Pinch Grab On");
    else
      mbhud("PinchGrab Off");
    pinchGrab = !pinchGrab;
    menuFilter->SetVisible(false);
    inMenu_R = false;
    menuDown = false;
  } else if(menuLeft) {
    menuFilter->SetVisible(false);
    inMenu_R = false; 
    menuLeft = false;
  }
}

void Leap_Updater::MenuSettings_L() {
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
    if(leapReader->isCircleCCW_L) {
      mbstatus("Got Anti-Clockwise Circle");
      menuFilter->SetVisible(false);
      inMenu_L = false;
    }
  }
  if(menuUp) {
    menuFilter->SetVisible(false);
    thumbGrabModeToggle = !thumbGrabModeToggle;
    if(thumbGrabModeToggle)
      mbhud("Thumb Grab Mode: On");
    else
      mbhud("Thumb Grab Mode: Off");
    inMenu_L = false; 
    menuUp = false;
  } else if(menuRight) {
    collisionToggle = !collisionToggle;
    if(collisionToggle)
      mbhud("Collision Mode: On");
    else
      mbhud("Collision Mode: Off");
    menuFilter->SetVisible(false);
    inMenu_L = false;
    menuRight = false;
  } else if(menuDown) {
    if(pinchGrab)
      mbhud("Pinch Grab On");
    else
      mbhud("PinchGrab Off");
    pinchGrab = !pinchGrab;
    menuFilter->SetVisible(false);
    inMenu_L = false;
    menuDown = false;
  } else if(menuLeft) {
    menuFilter->SetVisible(false);
    inMenu_L = false; 
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

__inline void Leap_Updater::checkRotateGesture() {
  
  //If Only 1 hand is showing
  if(leapReader->isVisible(l) ^ leapReader->isVisible(r)) {
    //if index pinky and thumb extended
    if(leapReader->CheckRotateHandGesture(r)) {
      CameraMovement();
    }
  }
}

__inline void Leap_Updater::checkMenuGesture() {
    //If Circle Clockwise
  if(leapReader->isCircleCW_R) {
    menuStartSpace = GetRelativeScreenSpaceFromWorldPos(hand_r->GetFingerPos(INDEX));
    menuFilter->SetCentre(menuStartSpace);
    menuFilter->SetVisible(true);
    inMenu_R = true;
  }
  if(leapReader->isCircleCW_L) {
    menuStartSpace = GetRelativeScreenSpaceFromWorldPos(hand_l->GetFingerPos(INDEX));
    menuFilter->SetCentre(menuStartSpace);
    menuFilter->SetVisible(true);
    inMenu_R = true;
  }
}

__inline void Leap_Updater::checkScreenTapGesture(mb::Vector &cameraPivot) {
  //If they tapped the screen
  if(leapReader->isScreenTap) {
    mblog("got tap\n");
    ScreenTap(cameraPivot);
  }
}

__inline void Leap_Updater::checkUndoGesture() {
  //If they Swiped Left
  if(leapReader->isUndo) {
    mb::Kernel()->Interface()->HUDMessageShow("UNDO");
    meshOp->UndoLast(); 
  }
}

__inline void Leap_Updater::checkGrabbingGesture(mb::Vector &cameraPivot) {
  if(thumbGrabModeToggle) {
    if((meshOp->CheckIntersection(hand_l->GetFingerBoundingBox(THUMB,TIP)))) {
      if(stickyMovement)
        ThumbSelect();
      else
        ThumbSmoothMove();
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

void Leap_Updater::OnEvent(const mb::EventGate &cEvent) {
  if(cEvent == frameEvent) {
    if(leapReader->updateAll()) {
      if((leapReader->ishands || leapReader->isTool )&& leapReader->isConnected) {
        //mblog("Getting CameraID\n");
        int viewcamID = idList->getViewCam();
        viewCam = new cameraWrapper(viewcamID);
        mb::Vector cameraPivot = fitToCameraSpace();
        SetHandAndFingerPositions(cameraPivot);
        //If Circle Anti-Clockwise
        if(inMenu_R) {
          MenuSettings_R();
        } else if(inMenu_L) {
          MenuSettings_L();
        } else {
          checkRotateGesture();
          checkMenuGesture();
          checkScreenTapGesture(cameraPivot);
          checkUndoGesture();
          checkGrabbingGesture(cameraPivot);
        }
      }
    }
  }
}


