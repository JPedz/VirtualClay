#include "stdafx.h"
#include "Leap_Updater.h"

namespace mb = mudbox;

Leap_Updater::Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r)
  :frameEvent(this)
{
  //To resize the image, use Image.GenerateUpscaledImage(*targetImg,factor);
  mblog("Creating texture\n");
  idList = idl;
  hand_l = l;
  hand_r = r;
  tool = new Leap_Tool(hand_l);
  tool->SetStamp(RESOURCESDIR+"stamp1.png");
  meshOp = new MeshOps();
  
  //mblog("Listing Nodes\n");
  //for(mb::Node *nodes = mb::Node::First() ; nodes ; nodes = nodes->Next()) {
  //  mblog("Node: "+nodes->Name()+" "+QString::number(nodes->ID())+"\n");
  //}
  //mblog("Listed Nodes\n");
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
  toolStamp = true;
  brushSizeMenuToggle = false;
  SceneNavigationToggle = true;
  pivotHandsOnMesh = true;
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

void Leap_Updater::ScreenTap() { 
  meshOp->ChangeCamera(viewCam);
  mb::Vector indexPos = hand_l->GetFingerPos(INDEX,TIP);
  hand_l->SetVisi(false);
  mb::Vector projPos = viewCam->getCamera()->Project(indexPos);
  projPos = projPos * mb::Vector(1,-1,1);
  mblog(VectorToQString(indexPos)+"Pos"+VectorToQStringLine(projPos));
  meshOp->SelectObject(viewCam,projPos);
  hand_l->SetVisi(true);
}

mb::Vector Leap_Updater::GetRelativeScreenSpaceFromWorldPos(mb::Vector &wPos) {
  mb::Camera *activeCam = mb::Kernel()->Scene()->ActiveCamera();
  return activeCam->Project(wPos);
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
  leftHand->MoveForward(50);
  hand_l->SetVisi(false);
  hand_l->SetPos(mb::Vector(0,0,0));
  mb::Kernel()->Scene()->SetActiveCamera(leftHand->getCamera());
  mb::Kernel()->ViewPort()->Redraw();
  bool b = meshOp->SelectFaces();
  hand_l->SetVisi(true);
  mb::Kernel()->Scene()->SetActiveCamera(viewCam->getCamera());
  mb::Kernel()->ViewPort()->Redraw();
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
  thumbProj = thumbProj * mb::Vector(1,-1,1);
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

void Leap_Updater::SetHandAndFingerPositions() {
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
  hand_r->SetPos(cameraPivot + leapReader->getPosition_R());
  
  //Rotate the hands XZY
  mb::Vector rotation = (mb::Vector(1,1,-1)*camRot) + leapReader->getDirection_L();
  mb::Matrix rX = createRotateXMatrix(rotation.x);
  mb::Matrix rY = createRotateYMatrix(rotation.y);
  mb::Matrix rZ = createRotateZMatrix(rotation.z);
  mb::Matrix rotationMatrix = rX*rZ*rY;
  hand_l->GetTNode()->SetRotation(rotationMatrix);

  mb::Vector rotation_r = (mb::Vector(-1,1,-1)*camRot) + leapReader->getDirection_R();
  mb::Matrix rX_r = createRotateXMatrix(rotation_r.x);
  mb::Matrix rY_r = createRotateYMatrix(rotation_r.y);
  mb::Matrix rZ_r = createRotateZMatrix(rotation_r.z);
  mb::Matrix rotationMatrix_r = rZ_r*rX_r*rY_r;
  hand_r->GetTNode()->SetRotation(rotationMatrix_r);


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
    for(int j = 0 ; j < 4 ; j++) {
      fingerPos_L = hand_l->GetFingerPos(fingerEnum(i),jointEnum(j));
      fingerPos_R = hand_r->GetFingerPos(fingerEnum(i),jointEnum(j));
      hand_l->SetFingerPos(jointEnum(j),fingerEnum(i),cameraPivot + leapReader->getFingerPosition(fingerEnum(i),l).at(j));
      hand_l->RotateAroundPivot(jointEnum(j),fingerEnum(i),-1*camRot,cameraPivot);
      hand_r->SetFingerPos(jointEnum(j),fingerEnum(i),cameraPivot + leapReader->getFingerPosition(fingerEnum(i),r).at(j));
      hand_r->RotateAroundPivot(jointEnum(j),fingerEnum(i),-1*camRot,cameraPivot);
      
      //if(collisionToggle) {
      //  if(meshOp->CheckIntersection(hand_l->GetFingerBoundingBox(fingerEnum(i),jointEnum(j)))) {
      //    if(j == 0) {
      //      hand_l->SetFingerPos(jointEnum(j),fingerEnum(i),fingerPos_L);
      //    }
      //    //mblog("L Hand collision!\n");
      //  }
      //  if(meshOp->CheckIntersection(hand_r->GetFingerBoundingBox(fingerEnum(i),jointEnum(j)))) {
      //    if(j == 0) {
      //      hand_r->SetFingerPos(jointEnum(j),fingerEnum(i),fingerPos_R);
      //    }
      //    //mblog("R Hand collision!\n");
      //  }
      //}
      // }
      //mblog("Updating Bone Pos :"+QString::number(j)+"\n");
      //hand_r->UpdateBone(boneEnum(j),fingerEnum(i),leapReader->getBoneOrients(fingerEnum(i),r).at(j));
      //hand_l->UpdateBone(boneEnum(j),fingerEnum(i),leapReader->getBoneOrients(fingerEnum(i),l).at(j));
      //mblog("Updated Bone Pos\n");
      }
    }
  if(leapReader->isTool) {
    tool->SetVisi(true);
    std::vector<mb::Vector> toolLocs = leapReader->GetToolPositions();
    tool->SetPos(0,cameraPivot+toolLocs.at(0));
    tool->SetPos(1,cameraPivot+toolLocs.at(1));
    tool->SetRot( (mb::Vector(-1,1,-1)*camRot)+leapReader->GetToolDirection());
    tool->RotateAroundPivot(-1*camRot,cameraPivot);
    tool->RotateAroundPivot(-1*camRot,cameraPivot);
  } else {
    tool->SetVisi(false);
  }
}

void Leap_Updater::CameraRotate(LR lOrR) {
  const float deadzone = 20.0f;
  mb::Vector handRot;
  if(lOrR == r) {
    handRot = leapReader->getDirection_R()+mb::Vector(30,0,0);
  } else {
    handRot = leapReader->getDirection_L()+mb::Vector(30,0,0);
  }
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

void Leap_Updater::CameraZoom(LR lOrR) {
  const float deadzone = 20.0f;
  mb::Vector handRot;
  if(lOrR == r) {
    handRot = leapReader->getPosition_R()+mb::Vector(30,0,0);
  } else {
    handRot = leapReader->getPosition_L()+mb::Vector(30,0,0);
  }
  mbstatus("HandRot: "+VectorToQString(handRot));
  if(abs(handRot.z) > deadzone) {
      if(handRot.z > 0) {
        mblog("Forward\n");
        viewCam->getCamera()->MoveForward(mb::Vector(5,0,0));  
      } else {
        mbhud("Backward\n");
        viewCam->getCamera()->MoveBackward(mb::Vector(5,0,0));  
      }
    }
  //viewCam->getTNode()->SetPosition(sceneRotate);
}

void Leap_Updater::MenuSettings_R() {
  mb::Vector PosDifference = menuStartSpace - GetRelativeScreenSpaceFromWorldPos(hand_r->GetFingerPos(INDEX));
  menuFilter->menuChoice = 5;
  if(PosDifference.y > 0.1) {
    menuFilter->menuChoice = 8;
    if(PosDifference.y > 0.3)
      menuDown = true;
  } else if(PosDifference.y < -0.1) {
    menuFilter->menuChoice = 6;
    if(PosDifference.y < -0.3)
      menuUp = true;
  }
  if(abs(PosDifference.x) > abs(PosDifference.y)) {
    if(PosDifference.x > 0.1) {
      menuFilter->menuChoice = 9;
      if(PosDifference.x > 0.3) {
        menuLeft = true;
      }
    } else if(PosDifference.x < -0.1) {
      menuFilter->menuChoice = 7;
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
    thumbGrabModeToggle = !thumbGrabModeToggle;
    if(thumbGrabModeToggle)
      mbhud("Thumb Grab Mode: On");
    else
      mbhud("Thumb Grab Mode: Off");
    menuFilter->SetVisible(false);
    inMenu_R = false; 
    menuUp = false;
  } else if(menuRight) {
    collisionToggle = !collisionToggle;
    pivotHandsOnMesh =! pivotHandsOnMesh;
    if(collisionToggle)
      mbhud("Collision Mode: On");
    else
      mbhud("Collision Mode: Off");
    menuFilter->SetVisible(false);
    inMenu_R = false;
    menuRight = false;
  } else if(menuDown) {
    pinchGrab = !pinchGrab;
    if(pinchGrab)
      mbhud("Pinch Grab On");
    else
      mbhud("PinchGrab Off");
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
  mb::Vector PosDifference = menuStartSpace - GetRelativeScreenSpaceFromWorldPos(hand_l->GetFingerPos(INDEX));
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
    brushSizeMenuToggle = !brushSizeMenuToggle;
    if(brushSizeMenuToggle)
      mbhud("Brush Size Mode: On");
    else
      mbhud("Brush Size Mode: Off");
    menuFilter->SetVisible(false);
    inMenu_L = false; 
    menuUp = false;
  } else if(menuRight) {
    toolStamp = !toolStamp;
    if(toolStamp)
      mbhud("Tool Stamp Mode: On");
    else
      mbhud("Tool Stamp Mode: Off");
    menuFilter->SetVisible(false);
    inMenu_L = false;
    menuRight = false;
  } else if(menuDown) {
    SceneNavigationToggle = !SceneNavigationToggle;
    if(SceneNavigationToggle)
      mbhud("SceneNavigation On");
    else
      mbhud("SceneNavigation Off");
    menuFilter->SetVisible(false);
    inMenu_L = false;
    menuDown = false;
  } else if(menuLeft) {
    menuFilter->SetVisible(false);
    inMenu_L = false; 
    menuLeft = false;
  }
}

void Leap_Updater::Extrusion() {
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
      lastFrameHandPos = hand_l->GetPos();
      firstmoveswitch = false;
    } else {
      mbstatus("MovingMesh");
      mblog("Moving Mesh\n");
      MoveMesh();
    }
  }
}

__inline void Leap_Updater::checkNavigationGestures() {
  if(SceneNavigationToggle) {
    if(leapReader->isVisible(l)) {
      if(leapReader->CheckRotateHandGesture(l)) {
        CameraRotate(l);
      } else if(leapReader->CheckScaleHandGesture(l)) {
        CameraZoom(l);
      }
    }
    if(leapReader->isVisible(r)) {
      if(leapReader->CheckRotateHandGesture(r)) {
        CameraRotate(r);
      } else if(leapReader->CheckScaleHandGesture(r)) {
        CameraZoom(r);
      }
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
    inMenu_L = true;
  }
}

__inline void Leap_Updater::checkScreenTapGesture() {
  //If they tapped the screen
  if(leapReader->CheckFingerExtensions(l,false,true,false,false,false)) {
    mblog("Got fingers\n");
    if(leapReader->isScreenTap) {
      mblog("got tap\n");
      ScreenTap();
    }
  }
}

__inline void Leap_Updater::checkUndoGesture() {
  //If they Swiped Left
  if(leapReader->isUndo) {
    mb::Kernel()->Interface()->HUDMessageShow("UNDO");
    meshOp->UndoLast(); 
  }
}

__inline void Leap_Updater::checkGrabbingGesture() {
  if(thumbGrabModeToggle) {
    if((meshOp->CheckIntersection(hand_l->GetFingerBoundingBox(THUMB,TIP)))) {
      if(meshOp->firstUse) {
        meshOp->firstUse = false;
      }
      if(stickyMovement)
        ThumbSelect();
      else
        ThumbSmoothMove();
    } else {
      meshOp->firstUse = true;
      if(facesAreSelected) {
        meshOp->DeselectAllFaces();
      }
      facesAreSelected = false;
    }
  } else {
  //If they are Grabbing and Thumb Toggle is Off
    if(leapReader->isGrabbing_L ) {
      Extrusion();
    } else {
      if(facesAreSelected) {
        meshOp->DeselectAllFaces();
      }
      firstmoveswitch = true;
      facesAreSelected = false;
    }
    lastFrameHandPos = hand_l->GetPos();
  }
}

void Leap_Updater::ToolStampMove() {  
  int toolCamID = idList->getToolCam();
  cameraWrapper *toolCam= new cameraWrapper(toolCamID);
  toolCam->setPosition(tool->GetPos(1));
  toolCam->setAim(tool->GetPos(0));
  toolCam->MoveForward(-50);
  meshOp->ChangeCamera(viewCam);
  tool->SetVisi(false);
  mblog("ToolStamp\n");
  mb::Vector toolPos = tool->GetPos(0);
  mb::Vector toolProj = viewCam->getCamera()->Project(toolPos);
  mblog("Tool Proj Pos = "+VectorToQStringLine(toolProj));
  toolProj = toolProj * mb::Vector(1,-1,1);
  mblog("Tool Proj Pos Pixels = "+VectorToQStringLine(ScreenSpaceToPixels(toolProj)));
  if(meshOp->ToolManip(ScreenSpaceToPixels(toolProj),20.0f,tool->GetStamp())) {
    mblog("Moving vertices maybe?\n");
    tool->ResizeStamp(20,20);
    meshOp->MoveVertices(10);
  }
  tool->SetVisi(true);
}

void Leap_Updater::ToolSmoothMove() {
  int toolCamID = idList->getToolCam();
  cameraWrapper *toolCam= new cameraWrapper(toolCamID);
  toolCam->setPosition(tool->GetPos(1));
  toolCam->MoveForward(-50);
  toolCam->setAim(tool->GetPos(0));
  meshOp->ChangeCamera(viewCam);
  tool->SetVisi(false);
  mb::Vector toolPos = tool->GetPos(0);
  mb::Vector toolProj = viewCam->getCamera()->Project(toolPos);
  toolProj = toolProj * mb::Vector(1,-1,1);
  mblog("ToolSmoothMove\n");
  mblog("Tool Proj Pos = "+VectorToQStringLine(toolProj));
  mblog("Tool Proj Pos Pixels = "+VectorToQStringLine(ScreenSpaceToPixels(toolProj)));
  //meshOp->ChangeCamera(viewCam);
  if(meshOp->SelectFaces(ScreenSpaceToPixels(toolProj),10.0f,10)) {
    mb::Vector dirNorm = leapReader->getToolMotionDirection();
    mblog("Normalised Direction = "+VectorToQStringLine(dirNorm));
    mb::Vector dist = dirNorm*thumbMoveStrength;
    meshOp->MoveVertices(dist);
  }
  tool->SetVisi(true);
}

__inline void Leap_Updater::checkToolIntersection() {
  if(leapReader->isTool) {
    if(meshOp->CheckIntersection(tool->GetBoundingBox(1))) {
      mblog("Tool bounding box Pos = "+VectorToQStringLine(tool->GetBoundingBox(0).Center()));
     // mblog("Test Tool bounding box Pos = "+VectorToQStringLine(mb::AxisAlignedBoundingBox(tool->GetPos(0),0.2f).Center()));

      //Set the undo list to iterate on.
      if(meshOp->firstUse) {
        meshOp->firstUse = false;
      }
      if(toolStamp) {
        ToolStampMove();
      } else {
        ToolSmoothMove();
      }
    } else {
      //Set the undo list to iterate on.
      meshOp->firstUse = true;
    }
  }
}

void Leap_Updater::OnEvent(const mb::EventGate &cEvent) {
  if(cEvent == frameEvent) {
    if(leapReader->updateAll()) {
      if((leapReader->ishands || leapReader->isTool) && leapReader->isConnected) {
        //mblog("Getting CameraID\n");
        int viewcamID = idList->getViewCam();
        viewCam = new cameraWrapper(viewcamID);
        if(pivotHandsOnMesh) {
          cameraPivot = fitToCameraSpace();
        } else {
          cameraPivot = mb::Vector(0,0,0);
        }
        SetHandAndFingerPositions();
        //If Circle Anti-Clockwise
        if(inMenu_R) {
          MenuSettings_R();
        } else if(inMenu_L) {
          MenuSettings_L();
        } else {
          checkNavigationGestures();
          checkMenuGesture();
          checkScreenTapGesture();
          checkUndoGesture();
          checkGrabbingGesture();
          checkToolIntersection();
        }
      }
    }
  }
}


