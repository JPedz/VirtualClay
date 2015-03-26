#include "stdafx.h"
#include "MeshOps.h"



MeshOps::MeshOps() {
  pMesh = NULL;
  MeshGeo = NULL;
  meshLayer = NULL;
  cumulativeMove = mb::Vector(0,0,0);
  if(mb::Kernel()->Scene()->SelectedGeometryCount() > 0) {
    MeshGeo = mb::Kernel()->Scene()->SelectedGeometry(0);
    pMesh = MeshGeo->ActiveLevel();
    meshLayer = pMesh->AddLayer();
  }
  curCam = mb::Kernel()->Scene()->ActiveCamera();
  ssp = curCam->GetScreenSpacePicker();
  faces_R = new std::vector<int>();
  vertices_R = new std::vector<VertexModifyInfo>();
  points_R = new std::vector<mb::SurfacePoint>();
  faces_L = new std::vector<int>();
  vertices_L = new std::vector<VertexModifyInfo>();
  points_L = new std::vector<mb::SurfacePoint>();
  midV = new MidVertex;
  firstUse = true;
}

MeshOps::MeshOps(mb::Mesh *m) {
  pMesh = m;
  meshLayer = pMesh->AddLayer();
  meshLayer->SetTransparency(1.0f);
  meshLayer->SetVisible(true);
  curCam = mb::Kernel()->Scene()->ActiveCamera();
}

void MeshOps::setMesh(mb::Mesh *m) {
  pMesh = m;
  meshLayer = pMesh->AddLayer();
  meshLayer->SetTransparency(1.0f);
  meshLayer->SetVisible(true);
  if(!pMesh->HasTC()) {
    mb::SubdivisionLevel *subdiv= MeshGeo->ActiveLevel();
    subdiv->RecreateUVs(true);
  }
  pMesh->SetSelected(true);
}

void MeshOps::refreshMesh(void) {
  //MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
  //MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
  mblog("Mesh Layer Count = "+QString::number(meshLayer->VertexCount())+"\n");
  QTime *t = new QTime();
  t->start();
  mb::LayerMeshData *lmd = pMesh->AddLayer();
  mblog("Add Layer Time: "+QString::number(t->elapsed())+"\n");
  
  t->restart();
  int lvi = lmd->LayerVertexIndex(1);
  mblog("Add Vertex Index Time: "+QString::number(t->elapsed())+"\n");
  
  t->restart();
  meshLayer->SetVertexDelta(lvi,1,mb::Vector(0,1,1),false);
  mblog("Vertex Delta Time Time: "+QString::number(t->elapsed())+"\n");
  t->restart();
  //lmd->FinishChanges();
  mblog("Finish changes time: "+QString::number(t->elapsed())+"\n");
  t->restart();
  mb::Kernel()->ViewPort()->Redraw();
  mblog("Redraw Time: "+QString::number(t->elapsed())+"\n");
}


void MeshOps::SelectObject(cameraWrapper *viewCam, mb::Vector screenPos) {
  mb::SurfacePoint sp;
  int w = mb::Kernel()->ViewPort()->Width();
  int h = mb::Kernel()->ViewPort()->Height();
  float screenX = w/2+screenPos.x*(w/2);
  float screenY = h/2+screenPos.y*(h/2);
  ssp = viewCam->getCamera()->GetScreenSpacePicker();
  //TODO: Select from the actual hand camera for 3D selection
  mblog("SSP X: "+QString::number(screenX)+" y: "+QString::number(screenY)+"\n");
  mblog("SCreenPos X: "+QString::number(screenPos.x)+" y: "+QString::number(screenPos.y)+"\n");
  bool b = ssp->Pick(viewCam->getCamera(),screenX,screenY,sp,false);
  if(b) {
    mb::Kernel()->Log("\n Hit MeshOps\n");
    pMesh = sp.Mesh();
    meshLayer = pMesh->AddLayer();
    meshLayer->SetTransparency(1.0f);
    meshLayer->SetVisible(true);
    if(pMesh->Parent()->IsKindOf(mb::Geometry::StaticClass())) {
      mb::Geometry *geoPotential = dynamic_cast<mb::Geometry *>(pMesh->Parent());
      MB_SAFELY(geoPotential) {
        mblog("Found GEO FROM PARENT\n");
        MeshGeo = geoPotential;
        //MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
        mb::Kernel()->Scene()->SetActiveGeometry(MeshGeo);
      }
    }
    mbstatus("Selected Mesh: "+pMesh->Name());
    mbhud("Mesh Selected: "+pMesh->Name()+"\nGeo: "+MeshGeo->Name());
    mb::Kernel()->Log(pMesh->Name()+" "+QString::number(sp.FaceIndex()));

  } else {
    mbstatus("No ObjectSelected");
  }
}

void MeshOps::ChangeCamera(cameraWrapper *cam) {
  curCam = cam->getCamera();
}


//bool SelectFaces(mb::Vector centrePoint, float width, float height, float dropOffRate) {
//  mblog("Wrong faces select");
//  return false;
//}

bool MeshOps::SelectFaces(LR lr, mb::AxisAlignedBoundingBox box,float spreadDist) {
  if(pMesh != NULL) {
    mb::Vector start = box.m_vStart;
    mb::Vector end = box.m_vEnd;
    mb::Picker *p = new mb::Picker();
    std::vector<int> *faces;
    std::vector<VertexModifyInfo> *vertices;
    std::vector<mb::SurfacePoint > *points;
    if(lr == l) {
      faces = faces_L;
      vertices = vertices_L;
      points =  points_L;
    } else {
      faces = faces_R;
      vertices = vertices_R;
      points =  points_R;
    }
    faces->clear();
    vertices->clear();
    points->clear();
    pMesh->RemoveLayer(meshLayer);
    meshLayer = pMesh->AddLayer();
    //p->SetMesh(pMesh);
    //p->SetTolerance(spreadDist);
    int fi;
    mb::SurfacePoint sp;
    mb::VertexAdjacency vA;
    mblog("Start: "+VectorToQString(start)+" End: "+VectorToQStringLine(end));
    if(p->Pick(start,end,true,sp,0.5f)) {
      mblog("sp face =: "+QString::number(sp.FaceIndex())+"\n");
      fi = sp.FaceIndex();
      //if(checkUniqueInFaceList(fi)) {
      points->push_back(sp);
      faces->push_back(fi);
      addVertex(lr,fi);
      //}
      //if(spreadDist != 0) {
      //  vA = pMesh->VertexAdjacency(vertices->at(0).vI);
      //  int aFI = vA.FaceIndex();
      //  faces->push_back(aFI);
      //  addVertex(aFI);

    }
    if(faces->size() > 0) {
      for(int i = 0 ; i < faces->size() ; i++) {
        mb::Kernel()->Log(QString::number(i)+" "+QString::number(faces->at(i))+"\n");
        pMesh->SetFaceSelected(faces->at(i));
      }
      MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      return true;
    }
  } else {
    mblog("Error in: Select Faces, -Box- pMesh == NULL");
  }
  return false;
}


bool MeshOps::SelectFaces(LR lr, mb::Vector centrePoint, float widthHeight, float dropOffRate) {
  bool linearDropoff = true;
  if(pMesh != NULL) {
    //mb::MeshChange *mC = pMesh->StartChange();
    mb::Vector vS = centrePoint - mb::Vector(widthHeight,widthHeight,0);
    mb::Vector vE = centrePoint + mb::Vector(widthHeight,widthHeight,0);
    std::vector<int> *faces;
    std::vector<VertexModifyInfo> *vertices;
    std::vector<mb::SurfacePoint > *points;
    if(lr == l) {
      faces = faces_L;
      vertices = vertices_L;
      points =  points_L;
    } else {
      faces = faces_R;
      vertices = vertices_R;
      points =  points_R;
    }
    faces->clear();
    vertices->clear();
    points->clear();
    pMesh->RemoveLayer(meshLayer);
    meshLayer = pMesh->AddLayer();
    midV->pos = mb::Vector(0,0,0);
    midV->vI = 0;
    mblog("SelectingBox\n");
    boxSelect(lr,vS,vE);
    if(faces->size() > 0) {
      mbstatus(QString("Polygon SELECTED"+QString::number(points->size())));
      mblog("box selected");
      for(int i = 0 ; i < faces->size() ; i++) {
        mb::Kernel()->Log(QString::number(i)+" "+QString::number(faces->at(i))+"\n");
        pMesh->SetFaceSelected(faces->at(i));
      }
      if(linearDropoff) {
        for(int i = 0 ; i < vertices->size() ; i++) {
          float dist = midV->pos.DistanceFrom(pMesh->VertexPosition(vertices->at(i).vI));
          vertices->at(i).strength = MIN(dropOffRate/dist,1); //restrict to 1 strength
          if(vertices->at(i).strength < 0.01)
            vertices->at(i).strength = 0;
        }
      }
      //refreshMesh();
      //MeshGeo->ContentChanged();
      //if(firstUse) {
      MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      StoreUndoQueue(r);
        
      //} else {
      //  AddToUndoQueue();
      //}
      return true;
    }
  }
  return false;
}

bool MeshOps::SelectFaces(LR lr, float size, float strength) {
  if(pMesh != NULL) {    
    std::vector<int> *faces;
    std::vector<VertexModifyInfo> *vertices;
    std::vector<mb::SurfacePoint > *points;
    if(lr == l) {
      faces = faces_L;
      vertices = vertices_L;
      points =  points_L;
    } else {
      faces = faces_R;
      vertices = vertices_R;
      points =  points_R;
    }
    int midW = mb::Kernel()->ViewPort()->Width()/2;
    int midH = mb::Kernel()->ViewPort()->Height()/2;
    mb::Vector vS = mb::Vector(midW-40,midH-40,0);
    mb::Vector vE = mb::Vector(midW + 40, midH + 40, 0);
    faces->clear();
    vertices->clear();
    points->clear();
    midV->pos = mb::Vector(0,0,0);
    midV->vI = 0;
    mblog("SelectingBox\n");
    //polygonSelect(polyScreen,points,faces,vertices);
    boxSelect(lr,vS,vE,size,strength);
    mblog("Selected face Count "+QString::number(faces->size())+"\n");
    
    if(faces->size() > 0) {
      mbstatus(QString("Polygon SELECTED"+QString::number(points->size())));
      mblog("box selected");
      for(int i = 0 ; i < faces->size() ; i++) {
        mb::Kernel()->Log(QString::number(i)+" "+QString::number(faces->at(i))+"\n");
        pMesh->SetFaceSelected(faces->at(i));
      }
      //refreshMesh();
      StoreUndoQueue(lr);
      MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      return true;
    } else {
      mblog("No faces\n");
    }
  }
  return false;
}


bool MeshOps::ToolManip(mb::Vector centrePoint, float widthHeight, Leap_Tool *tool) {
  if(pMesh != NULL) {
    //mb::MeshChange *mC = pMesh->StartChange();
    mb::Vector vS = centrePoint - mb::Vector(widthHeight,widthHeight,0);
    mb::Vector vE = centrePoint + mb::Vector(widthHeight,widthHeight,0);
    faces_R->clear();
    vertices_R->clear();
    points_R->clear();
    midV->pos = mb::Vector(0,0,0);
    midV->vI = 0;
    mblog("SelectingBox\n");
    QTime *t = new QTime();
    t->start();
    boxSelect(r,vS,vE,tool);
    mblog("Box SelectTime Mesh time: "+QString::number(t->elapsed())+"\n");
    if(faces_R->size() > 0) {
      mbstatus(QString("Polygon SELECTED"+QString::number(points_R->size())));
      mblog("box selected\n");
      for(int i = 0 ; i < faces_R->size() ; i++) {
        mb::Kernel()->Log(QString::number(i)+" "+QString::number(faces_R->at(i))+"\n");
        pMesh->SetFaceSelected(faces_R->at(i));
      }
      MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      //MeshGeo->ContentChanged();
      t->restart();
      //refreshMesh();
      mblog("Refresh Mesh time: "+QString::number(t->elapsed())+"\n");
      if(firstUse) {
        t->restart();
        StoreUndoQueue(r);
        mblog("Store Undo time: "+QString::number(t->elapsed())+"\n");
      } else {
        t->restart();
        AddToUndoQueue(r);
        mblog("Add To Undo time: "+QString::number(t->elapsed())+"\n");
      }
      return true;
    }
  }
  return false;
}

//bool MeshOps::ToolManip(mb::Vector centrePoint, float size, float dropOffRate) {
//  return false;
//}


void MeshOps::boxSelect(LR lr, mb::Vector &v1,mb::Vector &v2,float maxDist, float strength) {
  std::vector<int> *faces;
  std::vector<VertexModifyInfo> *vertices;
  std::vector<mb::SurfacePoint > *points;
  if(lr == l) {
    faces = faces_L;
    vertices = vertices_L;
    points =  points_L;
  } else {
    faces = faces_R;
    vertices = vertices_R;
    points =  points_R;
  }
  int x = (v1.x + v2.x)*0.5;
  int y = (v1.y + v2.y)*0.5;
  mb::SurfacePoint p;
  std::vector<int> faceIndices;
  int fi;
  int vertexCountInRange = 0;
  VertexModifyInfo vMI;
  float dist;
  mb::Base basePlane;
  mb::Vector bpVector;
  if(curCam->Pick(x,y,p)) {
    mblog("Selected :"+p.Mesh()->Name()+" Mesh \n");
    if(p.Mesh()->ID() == pMesh->ID()) {
      basePlane = p.TangentBase();
      faceIndices.push_back(p.FaceIndex());
      faces->push_back(p.FaceIndex());
      mblog("facesPushBack\n");
      midPos = p.WorldPosition();
      QTime *t = new QTime();
      t->start();
      for( unsigned int i = 0 ; i < pMesh->VertexCount() ; i++) {
        dist = pMesh->VertexPosition(i).DistanceFrom(midPos);
        if(dist < maxDist) {
          if(checkUniqueInVertexList(lr,i)) {
            bpVector = basePlane.TransformTo(pMesh->VertexPosition(i));
            mblog("Vertex position = "+VectorToQStringLine(pMesh->VertexPosition(i)));
            mblog("Vertex Base Plane Position = "+VectorToQStringLine(bpVector));
            vertexCountInRange++;
            vMI.vI = (int)i;
            if(meshLayer == NULL) {
                mblog("Meshlayer Null\n");
            } else { 
              mblog("int i = "+QString::number(i)+"\n");
            }
            vMI.lVI = meshLayer->LayerVertexIndex(i);
              mblog("int lvi = "+QString::number(vMI.lVI)+"\n");
            vMI.strength = MIN(10/dist,1);
            vertices->push_back(vMI);
            //fi = floor(i/4);
            //if(((fi != 0)|| (fi != 1) || (fi != 2) || (fi != 3)) && checkUniqueInFaceList(lr,fi)) {
            //  faces->push_back(fi);
            //}
          }
        }
      }
      mblog("Box Select VerticesLoop: "+QString::number(t->elapsed())+"\n");
    }//End of checking for PMeshID
    else {
      mblog("Not the right mesh?\n");
    }
  }
}


//void MeshOps::boxSelect2(mb::Vector &v1,mb::Vector &v2) {
//  int xSize = abs(v1.x - v2.x);
//  int ySize = abs(v1.y - v2.y);
//  //size_t boxSize = xSize*ySize;
//  mblog("Getting Screen Space Picker: "+curCam->Name()+"\n");
//  ssp = curCam->GetScreenSpacePicker();
//  if(ssp == NULL) {
//    mblog("SSP might be NULL?\n");
//  }
//  mblog("\n boxSelect:"+curCam->Name()+"\n");
//  int fi;
//  mb::SurfacePoint p;
//  // TODO: Optimise by reserving space
//  // TODO: Ensure it works if v2 is < v1 etc..
//  int x = MIN(v1.x,v2.x);
//  int y = MIN(v1.y,v2.y);
//  
//  mblog("Pixels select Range :"+QString::number(x)+" "+QString::number(y)+"\nTo: "+
//    QString::number(x+xSize)+" "+QString::number(y+ySize));
//  //MidVertex Special;
//  for (int i = 0; i < xSize ; i++) {
//    y = MIN(v1.y,v2.y);
//    for(int j = 0 ; j < ySize; j++) {
//      //If there is a surface point in this pixel
//      if(curCam->Pick(x,y,p)) {
//        //If it belongs to the same mesh
//        if(p.Mesh()->ID() == pMesh->ID()) {
//
//          fi = p.FaceIndex();
//          //mblog("Cur Loop :"+QString::number(i)+" "+QString::number(j)+"\n");
//          if((j == floor(ySize/2)) && (i == floor(xSize/2))) {
//            mblog("Saving mid point");
//            if(pMesh != NULL) {
//              midV->vI = pMesh->QuadPrimaryIndex(false,fi,2);
//              midV->pos = p.WorldPosition();
//            } else {
//              mblog("PMeshNULL WTF?!");
//            }
//          }
//          if(checkUniqueInFaceList(fi)) {  
//            points->push_back(p);
//            faces->push_back(fi);
//            addVertex(fi);
//            mblog("Face: "+QString::number(x)+" "
//              +QString::number(y)+" "+QString::number(p.FaceIndex())+"\n");
//          }
//        }
//      }
//      y++;
//    }
//    x++;
//  }
//  if(midV->pos == mb::Vector(0,0,0)) {
//    if(vertices->size() > 0) {
//      mb::Vector sumVector = mb::Vector(0,0,0);
//      for(int ind = 0 ; ind < vertices->size() ; ind++) {
//        sumVector += pMesh->VertexPosition(vertices->at(ind).vI);
//      }
//      midV->pos = sumVector/(float)vertices->size();
//    }
//  }
//}


void MeshOps::boxSelect(LR lr, mb::Vector &v1,mb::Vector &v2,Leap_Tool *tool) {
  std::vector<int> *faces;
  int fi;
  std::vector<VertexModifyInfo> *vertices;
  if(lr == l) {
    faces = faces_L;
    vertices = vertices_L;
  } else {
    faces = faces_R;
    vertices = vertices_R;
  } 
  int x = (v1.x + v2.x)*0.5;
  int y = (v1.y + v2.y)*0.5;
  int height = 20;
  mblog("StampHeight = "+QString::number(tool->GetStamp()->Height()));
  mb::SurfacePoint p;
  std::vector<int> faceIndices;
  int vertexCountInRange = 0;
  VertexModifyInfo vMI;
  float dist;
  mb::Base basePlane;
  mb::Vector uvSpace;
  //http://geomalgorithms.com/a04-_planes.html
  mb::Vector worldPoint;
  if(curCam->Pick(x,y,p)) {
    if(p.Mesh()->ID() == pMesh->ID()) {
      basePlane = p.TangentBase();
      faceIndices.push_back(p.FaceIndex());
      faces->push_back(p.FaceIndex());
      midPos = p.WorldPosition();
      QTime *t = new QTime();
      t->start();
      for( unsigned int i = 0 ; i < pMesh->VertexCount() ; i++) {
        worldPoint = pMesh->VertexPosition(i);
        dist = worldPoint.DistanceFrom(midPos);
        if(dist < height) {
          mblog("Vertex in range\n");
          if(checkUniqueInVertexList(lr,i)) {
            //mblog("MID POINT COORDINATES = "+VectorToQStringLine(midPos));
            //mblog("WORLD POINT COORDINATES = "+VectorToQStringLine(worldPoint));
            //mblog("NORMAL = "+VectorToQStringLine(p.WorldNormal().Normalized()));
            uvSpace = findDisplacementUV(basePlane,midPos,worldPoint);
            //mblog("UVSPACE COORDINATES = "+VectorToQStringLine(uvSpace));
            //mblog("\n");
            vertexCountInRange++;
            vMI.vI = (int)i;
            vMI.lVI = meshLayer->LayerVertexIndex((int)i);
            vMI.strength = tool->GetStampStrength(uvSpace);
            mblog("\n");
            vertices->push_back(vMI);
            /*fi = i%4;
            if(((fi != 0)|| (fi != 1) || (fi != 2) || (fi != 3)) && checkUniqueInFaceList(lr,fi)) {
              faces->push_back(fi);
            }*/
          }
        } else {
          mblog("dist = "+QString::number(dist));
        }
      }
      mblog("Box Select VerticesLoop: "+QString::number(t->elapsed())+"\n");
    }//End of checking for PMeshID
  }
}


//void MeshOps::boxSelect2(mb::Vector &v1,mb::Vector &v2,Leap_Tool *tool) {
//  int xSize = abs(v1.x - v2.x);
//  int ySize = abs(v1.y - v2.y);
//  mblog("Getting Screen Space Picker: "+curCam->Name()+"\n");
//  ssp = curCam->GetScreenSpacePicker();
//  if(ssp == NULL) {
//    mblog("SSP might be NULL?\n");
//  }
//  mblog("\n boxSelect:"+curCam->Name()+"\n");
//  int fi;
//  mb::SurfacePoint p;
//  // TODO: Optimise by reserving space
//  // TODO: Ensure it works if v2 is < v1 etc..
//  int x = MIN(v1.x,v2.x);
//  int y = MIN(v1.y,v2.y);
//  
//  mblog("Pixels select Range :"+QString::number(x)+" "+QString::number(y)+"\nTo: "+
//    QString::number(x+xSize)+" "+QString::number(y+ySize));
//  
//  QTime *t = new QTime();
//  t->start();
//  for (int i = 0; i < xSize ; i++) {
//    y = MIN(v1.y,v2.y);
//    for(int j = 0 ; j < ySize; j++) {
//      //If there is a surface point in this pixel
//      if(curCam->Pick(x,y,p)) {
//        //If it belongs to the same mesh
//        if(p.Mesh()->ID() == pMesh->ID()) {
//          fi = p.FaceIndex();
//          if((j == floor(ySize/2)) && (i == floor(xSize/2))) {
//            mblog("Saving mid point");
//            if(pMesh != NULL) {
//              midV->vI = pMesh->QuadPrimaryIndex(false,fi,2);
//              midV->pos = p.WorldPosition();
//            } else {
//              mblog("PMeshNULL WTF?!");
//            }
//          }
//          if(checkUniqueInFaceList(fi)) {  
//            points->push_back(p);
//            faces->push_back(fi);
//            addVertex(fi);
//            mblog("Face: "+QString::number(x)+" "
//              +QString::number(y)+" "+QString::number(p.FaceIndex())+"\n");
//          }
//        }
//      }
//      y++;
//    }
//    x++;
//  }
//  
//  mblog("Box Select First Loop: "+QString::number(t->elapsed())+"\n");
//  mb::Vector posi;
//  float closestDist;
//  float curDist;
//  float closestX;
//  float closestY;
//  x = MIN(v1.x,v2.x);
//  y = MIN(v1.y,v2.y);
//  t->restart();
//  for(int k = 0 ; k < vertices->size() ; k++) {
//    closestDist = 999999.0f;
//    posi = pMesh->VertexPosition(vertices->at(k).vI);
//    //mblog("VertexPos = "+VectorToQStringLine(posi)+"\n");
//    for(int i = 0; i < xSize; i++) {
//      y = MIN(v1.y,v2.y);
//      for(int j = 0 ; j < ySize; j++) {
//        if(curCam->Pick(x,y,p)) {
//          curDist = posi.DistanceFrom(p.WorldPosition());
//          //mblog("Picking X "+QString::number(x)+" Y "+QString::number(y)+" World Pos: "+VectorToQString(p.WorldPosition())+" curDist "+QString::number(curDist)+"\n");
//          if(curDist < closestDist) {
//            //mblog("Closest dist for X "+QString::number(x)+" Y"+QString::number(y)+" "+QString::number(curDist)+"\n");
//            closestDist = curDist;
//            closestX = i;
//            closestY = j;
//          }
//        }
//        y++;
//      }
//      x++;
//    }
//    mblog("ShortestDist = "+QString::number(closestDist)+"\n");
//    //vertices->at(k).strength = stamp->ColorAt(closestX,closestY).Luminance();
//    mblog("Vertex : "+QString::number(k)+" Strength = "+QString::number(vertices->at(k).strength)+"\n");
//  }
//   mblog("Box Select Second Loop: "+QString::number(t->elapsed())+"\n");
//
//}

void MeshOps::DeselectAllFaces() {
  //TODO: 
  if(pMesh != NULL) {
    mblog("Deselecting faces\n");
    pMesh->SetSelected(false);
    MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
    MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());

  }
}

 //pMesh->EnumerateNeighboringFaces(faces.at(0),);
bool MeshOps::CheckTouching(mb::AxisAlignedBoundingBox box1) {
  //mblog("Gothere\n");
  if(pMesh != NULL) {
    mb::AxisAlignedBoundingBox box2 = pMesh->BoundingBox(true);
    return box2.IsTouching(box1) && !box2.IsContaining(box1);
  } else {
    return false;
  }
}
bool MeshOps::CheckIntersection(mb::AxisAlignedBoundingBox box1) {
  //mblog("Gothere\n");
  if(pMesh != NULL) {
    mb::AxisAlignedBoundingBox box2 = pMesh->BoundingBox(true);
    return box2.IsTouching(box1) || box2.IsContaining(box1);
  } else {
    return false;
  }
}

void MeshOps::MoveVertices(LR lr, mb::Vector v) {
  //Move distance of the vector and in vector direction.
  int vi;
  int lvi;
  float strength;
  int strokeID;
  mb::Vector curDelta;
  std::vector<VertexModifyInfo> *vertices;
  if(lr == l) 
    vertices = vertices_L;
  else 
    vertices = vertices_R;
  for(int i = 0 ; i < vertices->size(); i++) {
    vi = vertices->at(i).vI;
    lvi = vertices->at(i).lVI;
    strength = vertices->at(i).strength;
    mblog("Moving Vertex "+QString::number(i)+"by "+VectorToQStringLine(v*strength));
    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v.x)+"\n");
    curDelta = meshLayer->VertexDelta(lvi);
    mblog("Current Delta = "+VectorToQStringLine(curDelta));
    meshLayer->SetVertexDelta(lvi,vi,curDelta+(v*strength),true);
    //pMesh->AddVertexPosition(vi,v*strength);
    //strokeID = pMesh->VertexStrokeID(vi);
    //pMesh->SetVertexStrokeID(vi,strokeID++);
    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v2.x)+"\n"); 
  }
  if(MeshGeo != NULL) {
    refreshMesh();
    //pMesh->RecalculateNormals();
  }
}

//void MeshOps::MoveVertices(LR lr, mb::Vector v) {
//  //Move distance of the vector and in vector direction.
//  int vi;
//  float strength;
//  int strokeID;  
//  std::vector<VertexModifyInfo> *vertices;
//  if(lr == l) 
//    vertices = vertices_L;
//  else 
//    vertices = vertices_R;
//  for(int i = 0 ; i < vertices->size(); i++) {
//    vi = vertices->at(i).vI;
//    strength = vertices->at(i).strength;
//    //mblog("Moving Vertex "+QString::number(i)+"by "+VectorToQStringLine(v*strength));
//    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v.x)+"\n");
//    pMesh->AddVertexPosition(vi,v*strength);
//    strokeID = pMesh->VertexStrokeID(vi);
//    pMesh->SetVertexStrokeID(vi,strokeID++);
//    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v2.x)+"\n"); 
//  }
//  if(MeshGeo != NULL) {
//    refreshMesh();
//    //pMesh->RecalculateNormals();
//  }
//}



void MeshOps::MoveVertices(LR lr, float dist) {
  //move in direction of normals
  QTime *t = new QTime();
  t->start();
  int vi,lvi;
  float strength;
  int strokeID;
  mb::Vector v = mb::Vector(0,0,0);
  std::vector<VertexModifyInfo> *vertices;
  if(lr == l) 
    vertices = vertices_L;
  else 
    vertices = vertices_R;
  for(int i = 0 ; i < vertices->size(); i++) {
    vi = vertices->at(i).vI;
    lvi = vertices->at(i).lVI;
    strength = vertices->at(i).strength;
    v = pMesh->VertexNormal(vi);
    //mblog("Vertex Normal = "+VectorToQStringLine(v));
    //mblog("Moving Vertex "+QString::number(i)+"by "+VectorToQStringLine(v*dist));
    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v.x)+"\n");
    
    mblog("Moving Vertex "+QString::number(i)+" "+QString::number(lvi)+"by "+VectorToQStringLine(v*strength));
    meshLayer->SetVertexDelta(lvi,vi,v*strength,true);
    //pMesh->AddVertexPosition(vi,v*strength*dist);
    //strokeID = pMesh->VertexStrokeID(vi);
    //pMesh->SetVertexStrokeID(vi,strokeID++);
    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v2.x)+"\n"); 
  }
  mblog("MoveVertices time: "+QString::number(t->elapsed())+"\n");
  if(MeshGeo != NULL) {
    t->restart();
    refreshMesh();
    mblog("Refresh Mesh time: "+QString::number(t->elapsed())+"\n");

  } 
  refreshMesh();
  //pMesh->RecalculateNormals();
  //mb::Kernel()->ViewPort()->Redraw();
}
//
//void MeshOps::MoveVertices2(LR lr, float dist) {
//  //move in direction of normals
//  QTime *t = new QTime();
//  t->start();
//  int vi;
//  float strength;
//  int strokeID;
//  mb::Vector v = mb::Vector(0,0,0);
//  std::vector<VertexModifyInfo> *vertices;
//  if(lr == l) 
//    vertices = vertices_L;
//  else 
//    vertices = vertices_R;
//  for(int i = 0 ; i < vertices->size(); i++) {
//    vi = vertices->at(i).vI;
//    strength = vertices->at(i).strength;
//    v = pMesh->VertexNormal(vi);
//    //mblog("Vertex Normal = "+VectorToQStringLine(v));
//    //mblog("Moving Vertex "+QString::number(i)+"by "+VectorToQStringLine(v*dist));
//    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v.x)+"\n");
//    pMesh->AddVertexPosition(vi,v*strength*dist);
//    strokeID = pMesh->VertexStrokeID(vi);
//    pMesh->SetVertexStrokeID(vi,strokeID++);
//    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v2.x)+"\n"); 
//  }
//  mblog("MoveVertices time: "+QString::number(t->elapsed())+"\n");
//  if(MeshGeo != NULL) {
//    t->restart();
//    refreshMesh();
//    mblog("Refresh Mesh time: "+QString::number(t->elapsed())+"\n");
//
//  } 
//  refreshMesh();
//  //pMesh->RecalculateNormals();
//  //mb::Kernel()->ViewPort()->Redraw();
//}

void MeshOps::StoreUndoQueue(LR lr) {
  VertexInfo vertInfo;
  //mblog("Storing Undo\n");
  //mb::VertexAdjacency *vA = new mb::VertexAdjacency();
  std::vector<VertexInfo> vertInfoList;
  std::vector<VertexModifyInfo> *vertices;
  if(lr == l) 
    vertices = vertices_L;
  else 
    vertices = vertices_R;
  for(int i = 0 ; i < vertices->size(); i++) {
    //if(pMesh != NULL) {
    //  mblog("getting vertexAdjacency\n");
    //  vA = &pMesh->VertexAdjacency(vertices->at(i));
    //}
    vertInfo.vI = vertices->at(i).vI;
    //if(!vA->IsValid()) {
    //  mblog("NOT VALID VA\n");
    //} else {
    //  vertInfo.fI = vA->FaceIndex();
    //}
    //mblog("getting vertex Position\n");
    vertInfo.pos = pMesh->VertexPosition(vertInfo.vI);
    vertInfoList.push_back(vertInfo);
    //mblog("Storing: "+QString::number(vertInfo.vI)+" "+VectorToQStringLine(vertInfo.pos));
  }
  undoQueue.push_back(vertInfoList);
  //mblog("Stored Undo\n");
}

void MeshOps::AddToUndoQueue(LR lr) {
  mblog("Adding Undo\n");
  VertexInfo vertInfo;
  bool atleastOneNewVert = false;
  if(undoQueue.size() > 0) {
    std::vector<VertexModifyInfo> *vertices;
    if(lr == l)
      vertices = vertices_L;
    else
      vertices = vertices_R;
    //mb::VertexAdjacency *vA = new mb::VertexAdjacency();
    mblog("Getting last vertex info list");
    std::vector<VertexInfo> vertInfoList = undoQueue.back();
    bool newVert = true;
    for(int i = 0 ; i < vertices->size(); i++) {
      //if(pMesh != NULL) {
      //  mblog("getting vertexAdjacency\n");
      //  vA = &pMesh->VertexAdjacency(vertices->at(i));
      //}
      mblog("vertex "+ QString::number(i)+"\n");
      for(int j = 0 ; j < vertInfoList.size() ; j++) {
        if(vertices->at(i).vI == vertInfoList.at(j).vI) {
          newVert = false;
          break;
        }
      }
      vertInfo.vI = vertices->at(i).vI;
      if(newVert) {
        atleastOneNewVert = true;
        mblog("new vertex "+ QString::number(i)+"\n");
        vertInfo.pos = pMesh->VertexPosition(vertInfo.vI);
        vertInfoList.push_back(vertInfo);
      }
      //mblog("Storing: "+QString::number(vertInfo.vI)+" "+VectorToQStringLine(vertInfo.pos));
    }
    if(atleastOneNewVert) {
      undoQueue.pop_back();
      undoQueue.push_back(vertInfoList);
    }
  } else {
    StoreUndoQueue(lr);
  }
  //mblog("Stored Undo\n");
}

void MeshOps::UndoLast() {
  if(pMesh != NULL) {
    if(undoQueue.size() > 0) {
      mb::MeshChange *mC = pMesh->StartChange();
      std::vector<VertexInfo> vertInfoList = undoQueue.back();
      undoQueue.pop_back();
      for(int i = 0 ; i < vertInfoList.size() ; i++) {
        pMesh->SetVertexPosition(vertInfoList.at(i).vI,vertInfoList.at(i).pos);
        //mC->Add(vertInfoList.at(i).vI,vertInfoList.at(i).fI,false);
        mblog("Retrieving: "+QString::number(vertInfoList.at(i).vI)+" "+VectorToQStringLine(vertInfoList.at(i).pos));
      }
      mC->Finish();
      MeshGeo->ContentChanged();
      mblog("contentchanging\n");
      MeshGeo->ContentChanged();
      mblog("contentchanged\n");
      pMesh->SetVersion(pMesh->Version()+1);
      pMesh->Modified.Trigger();
      pMesh->ContentChanged();
      mb::Kernel()->Redraw();
      //MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      //MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      refreshMesh();
    }
  }
}

void MeshOps::addVertex(LR lr, int  fi) {
  unsigned int v;
  VertexModifyInfo vMI;
  std::vector<VertexModifyInfo> *vertices;
  if(lr == l) 
    vertices = vertices_L;
  else 
    vertices = vertices_R;
  if(pMesh->Type() == mb::Mesh::typeQuadric)  {
    for(int i = 0 ; i < pMesh->SideCount() ; i++) {
      v = pMesh->QuadPrimaryIndex(false,fi,i);
      if(checkUniqueInVertexList(lr,v)) {
        vMI.vI = v;
        vMI.lVI = meshLayer->LayerVertexIndex(v);
        vMI.strength = 1;
        vertices->push_back(vMI);
      }
    }
  } else if(pMesh->Type() == mb::Mesh::typeTriangular) {
    for(int i = 0 ; i < 3 ; i++) {
      v = pMesh->TrianglePrimaryIndex(false,fi,i);
      if(checkUniqueInVertexList(lr,v)) {
        vMI.vI = v;
        vMI.lVI = meshLayer->LayerVertexIndex(v);
        vMI.strength = 1;
        vertices->push_back(vMI);
      }
    }
  }
}

void MeshOps::AddVFI(int vi, int fi) {
  bool foundVI = false;
  int i = 0;
  for( i = 0 ; i < vFI->size() ; i++) {
    if(vFI->at(i).vI == vi) {
      foundVI = true;
      for(int j = 0 ; j < vFI->at(i).fI.size() ; j++) {
        if(vFI->at(i).fI.at(j) == fi)
          return;//Already in the VFI  
      }
      break;
    }
  }
  if(foundVI) {
    vFI->at(i).fI.push_back(fi);
  } else {
    VertexFaceInfo vfi;
    vfi.vI = vi;
    vfi.fI.push_back(fi);
    vFI->push_back(vfi);
  }
}

bool MeshOps::checkUniqueInFaceList(LR lr, int fi) {
  std::vector<int> *faces;
  if(lr == l) 
    faces = faces_L;
  else 
    faces = faces_R;
  for(int i = 0 ; i < faces->size() ; i++)
    if(fi == faces->at(i))
      return false;//not unique
  return true;//unique
}

bool MeshOps::checkUniqueInVertexList(LR lr, int vi) {
  std::vector<VertexModifyInfo> *vertices;
  if(lr == l) 
    vertices = vertices_L;
  else 
    vertices = vertices_R;
  for(int i = 0 ; i < vertices->size() ; i++)
    if(vi == vertices->at(i).vI)
      return false;//not unique
  return true;//unique
}

//bool checkIsInside(QList<mb::Vector> &points,mb::Vector point) {
//  bool isInside = false;
//  int j = points.size()-1;
//  float x = point.x;
//  float y = point.y;
//  for (int i = 0 ; i < points.size() ; i++) {
//    if (((points.at(i).y< y && points.at(j).y>=y) || (points.at(j).y< y && points.at(i).y>=y)) &&
//      (points.at(i).x<=x || points.at(j).x<=x)) {
//      if (points.at(i).x+(y-points.at(i).y)/(points.at(j).y-points.at(i).y)*(points.at(j).x-points.at(i).x)<x) {
//        isInside=!isInside; 
//      }
//    }
//    j=i; 
//  }
//  return isInside;
//}
void MeshOps::SelectWholeMesh() {
  if(pMesh != NULL) {
    pMesh->SetSelected(true);
  }
}


void MeshOps::MoveObject(mb::Vector dist) {
  if(MeshGeo != NULL) {
    cumulativeMove += dist;
    MeshGeo->Transformation()->SetPosition(dist + MeshGeo->Transformation()->Position());
  }
}

void MeshOps::StoreLastMoveUndoQueue() {
  undoMoveQueue.push_back(cumulativeMove);
  cumulativeMove = mb::Vector(0,0,0);
}


void MeshOps::UndoLastMove() {
  if(MeshGeo != NULL) {
    if(undoMoveQueue.size() > 0) {
      mb::Vector dist = undoMoveQueue.back();
      undoMoveQueue.pop_back();
      MeshGeo->Transformation()->SetPosition(MeshGeo->Transformation()->Position()-dist);
    }
  }
}
