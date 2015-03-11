#include "stdafx.h"
#include "MeshOps.h"



MeshOps::MeshOps() {
  pMesh = NULL;
  MeshGeo = NULL;
  if(mb::Kernel()->Scene()->SelectedGeometryCount() > 0) {
    MeshGeo = mb::Kernel()->Scene()->SelectedGeometry(0);
    pMesh = MeshGeo->ActiveLevel();
  }
  curCam = mb::Kernel()->Scene()->ActiveCamera();
  ssp = curCam->GetScreenSpacePicker();
  faces = new std::vector<int>();
  vertices = new std::vector<VertexModifyInfo>();
  points = new std::vector<mb::SurfacePoint>();
  midV = new MidVertex;
  firstUse = true;
}

void MeshOps::refreshMesh(void) {
  MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
  MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
  //mb::SubdivisionLevel *subdiv = MeshGeo->ActiveLevel();
  //subdiv->ApplyChanges();
  mb::Kernel()->ViewPort()->Redraw();
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
    if(pMesh->Parent()->IsKindOf(mb::Geometry::StaticClass())) {
      mb::Geometry *geoPotential = dynamic_cast<mb::Geometry *>(pMesh->Parent());
      MB_SAFELY(geoPotential) {
        mblog("Found GEO FROM PARENT\n");
        MeshGeo = geoPotential;
        MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      }
    }
    mbstatus("Selected Mesh: "+pMesh->Name());
    mb::Kernel()->Log(pMesh->Name()+" "+QString::number(sp.FaceIndex()));
  } else {
    mbstatus("No ObjectSelected");
  }
}

void MeshOps::ChangeCamera(cameraWrapper *cam) {
  curCam = cam->getCamera();
}


bool SelectFaces(mb::Vector centrePoint, float width, float height, float dropOffRate) {
  mblog("Wrong faces select");
  return false;
}

bool MeshOps::SelectFaces(mb::AxisAlignedBoundingBox box,float spreadDist) {
  if(pMesh != NULL) {
    mb::Vector start = box.m_vStart;
    mb::Vector end = box.m_vEnd;
    mb::Picker *p = new mb::Picker();
    faces->clear();
    vertices->clear();
    points->clear();
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
      addVertex(fi);
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

      return true;
    }
  } else {
    mblog("Error in: Select Faces, -Box- pMesh == NULL");
  }
  return false;
}


bool MeshOps::SelectFaces(mb::Vector centrePoint, float widthHeight, float dropOffRate) {
  bool linearDropoff = true;
  if(pMesh != NULL) {
    //mb::MeshChange *mC = pMesh->StartChange();
    mb::Vector vS = centrePoint - mb::Vector(widthHeight,widthHeight,0);
    mb::Vector vE = centrePoint + mb::Vector(widthHeight,widthHeight,0);
    faces->clear();
    vertices->clear();
    points->clear();
    midV->pos = mb::Vector(0,0,0);
    midV->vI = 0;
    mblog("SelectingBox\n");
    boxSelect(vS,vE);
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
      refreshMesh();
      //MeshGeo->ContentChanged();
      //if(firstUse) {
        StoreUndoQueue();
        
      //} else {
      //  AddToUndoQueue();
      //}
      return true;
    }
  }
  return false;
}



bool MeshOps::ToolManip(mb::Vector centrePoint, float widthHeight, mb::Image *stamp) {
  if(pMesh != NULL) {
    //mb::MeshChange *mC = pMesh->StartChange();
    mb::Vector vS = centrePoint - mb::Vector(widthHeight,widthHeight,0);
    mb::Vector vE = centrePoint + mb::Vector(widthHeight,widthHeight,0);
    faces->clear();
    vertices->clear();
    points->clear();
    midV->pos = mb::Vector(0,0,0);
    midV->vI = 0;
    mblog("SelectingBox\n");
    QTime *t = new QTime();
    t->start();
    boxSelect(vS,vE,stamp);
    mblog("Box SelectTime Mesh time: "+QString::number(t->elapsed())+"\n");
    if(faces->size() > 0) {
      mbstatus(QString("Polygon SELECTED"+QString::number(points->size())));
      mblog("box selected\n");
      for(int i = 0 ; i < faces->size() ; i++) {
        mb::Kernel()->Log(QString::number(i)+" "+QString::number(faces->at(i))+"\n");
        pMesh->SetFaceSelected(faces->at(i));
      }
      //MeshGeo->ContentChanged();
      t->restart();
      refreshMesh();
      
      mblog("Refresh Mesh time: "+QString::number(t->elapsed())+"\n");
      //if(firstUse) {
      t->restart();
        StoreUndoQueue();
    mblog("StoreUndo time: "+QString::number(t->elapsed())+"\n");
      //} else {
      //  AddToUndoQueue();
      //}
      return true;
    }
  }
  return false;
}

bool MeshOps::ToolManip(mb::Vector centrePoint, float size, float dropOffRate) {
  return false;
}

bool MeshOps::SelectFaces(float size, float strength) {
  bool linearDropoff = true;
  if(pMesh != NULL) {
    //ssp = curCam->GetScreenSpacePicker();
    //mb::Kernel()->Log("\nSSP:"+curCam->Name()+" "+ssp->Name()+"\n");
    //mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,QString("GOT ScreenPicker"));
    //QList<mb::Vector> polyScreen;
    //mb::Vector polyScreenVector;
    int midW = mb::Kernel()->ViewPort()->Width()/2;
    int midH = mb::Kernel()->ViewPort()->Height()/2;
    /*for(int i = 0 ; i < poly.size() ; i++) {
      polyScreenVector.x = midW+poly.at(i).x*midW;
      polyScreenVector.y = midH+poly.at(i).y*midH;
      polyScreen.push_back(polyScreenVector);
    }*/
    mb::Vector vS = mb::Vector(midW-40,midH-40,0);
    mb::Vector vE = mb::Vector(midW + 40, midH + 40, 0);
    //mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,pMesh->Name());
    //mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,QString("BOX SELECTED"+QString::number(points.size())));
    faces->clear();
    vertices->clear();
    points->clear();
    midV->pos = mb::Vector(0,0,0);
    midV->vI = 0;
    mblog("SelectingBox\n");
    //polygonSelect(polyScreen,points,faces,vertices);
    boxSelect(vS,vE,size,strength);
    mblog("Selected face Count "+QString::number(faces->size())+"\n");
    
    if(faces->size() > 0) {
      mbstatus(QString("Polygon SELECTED"+QString::number(points->size())));
      mblog("box selected");
      for(int i = 0 ; i < faces->size() ; i++) {
        mb::Kernel()->Log(QString::number(i)+" "+QString::number(faces->at(i))+"\n");
        pMesh->SetFaceSelected(faces->at(i));
      }
      //if(linearDropoff) {
      //  mblog("Performing LinearDropOff mid point"+VectorToQStringLine(midV->pos));
      //  for(int i = 0 ; i < vertices->size() ; i++) {
      //    float dist = midV->pos.DistanceFrom(pMesh->VertexPosition(vertices->at(i).vI));
      //    vertices->at(i).strength = MIN(10/dist,1); //restrict to 1 strength
      //    mb::Kernel()->Log("Strength: "+QString::number(i)+": "+QString::number(MIN(10000/dist*dist*dist,1))+"\n");
      //    if(vertices->at(i).strength < 0.001)
      //      vertices->at(i).strength = 0;
      //  }
      //}
      //MeshGeo->ContentChanged();
      MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      //int subDiv_I = MeshGeo->ActiveLevel()->Index();
      //MeshGeo->ChangeActiveLevel(MeshGeo->Level(subDiv_I));
      mb::Kernel()->ViewPort()->Redraw();
      StoreUndoQueue();
      return true;
    } else {
      mblog("No faces\n");
    }

  }
  return false;
}

void MeshOps::MoveVertices(mb::Vector v) {
  //Move distance of the vector and in vector direction.
  int vi;
  float strength;
  int strokeID;
  for(int i = 0 ; i < vertices->size(); i++) {
    vi = vertices->at(i).vI;
    strength = vertices->at(i).strength;
    mblog("Moving Vertex "+QString::number(i)+"by "+VectorToQStringLine(v*strength));
    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v.x)+"\n");
    pMesh->AddVertexPosition(vi,v*strength);
    strokeID = pMesh->VertexStrokeID(vi);
    pMesh->SetVertexStrokeID(vi,strokeID++);
    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v2.x)+"\n"); 
  }
  if(MeshGeo != NULL) {
      //MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      //MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      mb::Kernel()->ViewPort()->Redraw();
      refreshMesh();
  //    MeshGeo->ContentChanged();
      pMesh->RecalculateNormals();
  //    pMesh->Modified.Trigger();
  //    pMesh->ContentChanged();
  //    mb::Kernel()->ViewPort()->Redraw();
  }
}

void MeshOps::MoveVertices(float dist) {
  //move in direction of normals
  QTime *t = new QTime();
  t->start();
  int vi;
  float strength;
  int strokeID;
  mb::Vector v = mb::Vector(0,0,0);
  for(int i = 0 ; i < vertices->size(); i++) {
    vi = vertices->at(i).vI;
    strength = vertices->at(i).strength;
    v = pMesh->VertexNormal(vi);
    //mblog("Vertex Normal = "+VectorToQStringLine(v));
    //mblog("Moving Vertex "+QString::number(i)+"by "+VectorToQStringLine(v*dist));
    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v.x)+"\n");
    pMesh->AddVertexPosition(vi,v*strength*dist);
    strokeID = pMesh->VertexStrokeID(vi);
    pMesh->SetVertexStrokeID(vi,strokeID++);
    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v2.x)+"\n"); 
  }
  mblog("MoveVertices time: "+QString::number(t->elapsed())+"\n");
  if(MeshGeo != NULL) {
    t->restart();
    refreshMesh();
    mblog("Refresh Mesh time: "+QString::number(t->elapsed())+"\n");

  }    
  pMesh->RecalculateNormals();
  mb::Kernel()->ViewPort()->Redraw();
}

void MeshOps::StoreUndoQueue() {
  VertexInfo vertInfo;
  //mblog("Storing Undo\n");
  //mb::VertexAdjacency *vA = new mb::VertexAdjacency();
  std::vector<VertexInfo> vertInfoList;
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

void MeshOps::AddToUndoQueue() {
  //mblog("Storing Undo\n");
  VertexInfo vertInfo;
  //mb::VertexAdjacency *vA = new mb::VertexAdjacency();
  std::vector<VertexInfo> vertInfoList = undoQueue.back();
  bool newVert = true;
  for(int i = 0 ; i < vertices->size(); i++) {
    //if(pMesh != NULL) {
    //  mblog("getting vertexAdjacency\n");
    //  vA = &pMesh->VertexAdjacency(vertices->at(i));
    //}
    for(int j = 0 ; i < vertInfoList.size() ; j++) {
      if(vertices->at(i).vI == vertInfoList.at(j).vI) {
        newVert = false;
        break;
      }
    }
    vertInfo.vI = vertices->at(i).vI;
    if(newVert) {
      vertInfo.pos = pMesh->VertexPosition(vertInfo.vI);
      vertInfoList.push_back(vertInfo); 
    }
    //mblog("Storing: "+QString::number(vertInfo.vI)+" "+VectorToQStringLine(vertInfo.pos));
  }
  //mblog("Stored Undo\n");
}

void MeshOps::UndoLast() {
  if(pMesh != NULL) {
    if(undoQueue.size() > 0) {
      //mb::MeshChange *mC = pMesh->StartChange();
      std::vector<VertexInfo> vertInfoList = undoQueue.back();
      undoQueue.pop_back();
      for(int i = 0 ; i < vertInfoList.size() ; i++) {
        pMesh->SetVertexPosition(vertInfoList.at(i).vI,vertInfoList.at(i).pos);
        //mC->Add(vertInfoList.at(i).vI,vertInfoList.at(i).fI,false);
        mblog("Retrieving: "+QString::number(vertInfoList.at(i).vI)+" "+VectorToQStringLine(vertInfoList.at(i).pos));
      }
      //mC->Finish();
      //MeshGeo->ContentChanged();
      //mblog("contentchanging\n");
      //MeshGeo->ContentChanged();
      //mblog("contentchanged\n");
      //pMesh->SetVersion(pMesh->Version()+1);
      //pMesh->Modified.Trigger();
      //pMesh->ContentChanged();
      //MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      //MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      refreshMesh();
      //MeshGeo->AddAttribute(mb::Attribute::AttributeType::typeFloat,"NEW MESH MAYBE");
//      mb::Kernel()->ViewPort()->Redraw();
    }
  }
}

void MeshOps::addVertex(int  fi) {
  unsigned int v;
  VertexModifyInfo vMI;
  if(pMesh->Type() == mb::Mesh::typeQuadric)  {
    for(int i = 0 ; i < pMesh->SideCount() ; i++) {
      v = pMesh->QuadPrimaryIndex(false,fi,i);
      if(checkUniqueInVertexList(v)) {
        vMI.vI = v;
        vMI.strength = 1;
        vertices->push_back(vMI);
      }
    }
  } else if(pMesh->Type() == mb::Mesh::typeTriangular) {
    for(int i = 0 ; i < 3 ; i++) {
      v = pMesh->TrianglePrimaryIndex(false,fi,i);
      if(checkUniqueInVertexList(v)) {
        vMI.vI = v;
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

bool MeshOps::checkUniqueInFaceList(int fi) {
  for(int i = 0 ; i < faces->size() ; i++)
    if(fi == faces->at(i))
      return false;//not unique
  return true;//unique
}

bool MeshOps::checkUniqueInVertexList(int vi) {
  for(int i = 0 ; i < vertices->size() ; i++)
    if(vi == vertices->at(i).vI)
      return false;//not unique
  return true;//unique
}

bool checkIsInside(QList<mb::Vector> &points,mb::Vector point,mb::Vector extreme) {
  bool isInside = false;
  int j = points.size()-1;
  float x = point.x;
  float y = point.y;
  for (int i = 0 ; i < points.size() ; i++) {
    if (((points.at(i).y< y && points.at(j).y>=y) || (points.at(j).y< y && points.at(i).y>=y)) &&
      (points.at(i).x<=x || points.at(j).x<=x)) {
      if (points.at(i).x+(y-points.at(i).y)/(points.at(j).y-points.at(i).y)*(points.at(j).x-points.at(i).x)<x) {
        isInside=!isInside; 
      }
    }
    j=i; 
  }
  return isInside;
}

//void MeshOps::polygonSelect(QList<mb::Vector> &points, QList<mb::SurfacePoint>&sp,
//                         QList<int> &faces, QList<int> &vertices) {
//  
//  int minX,minY,maxX,maxY;
//  minX = points.at(0).x;
//  minY = points.at(0).y;
//  maxX = points.at(0).x;
//  maxY = points.at(0).y;
//  for(int i = 0; i < points.size() ; i++) {
//    if(points.at(i).x < minX)
//      minX = points.at(i).x;
//    if(points.at(i).x > maxX)
//      maxX = points.at(i).x;
//    if(points.at(i).y < minY)
//      minY = points.at(i).y;
//    if(points.at(i).y > maxY)
//      maxY = points.at(i).y;
//  }
//  int xSize = abs(maxX - minX);
//  int ySize = abs(maxY = minY);
//  int x = minX;
//  int y = minY;
//  mb::Vector point;
//  mb::Vector extremePoint;
//  size_t boundingBoxSize = xSize*ySize;
//  
//  unsigned int fi;//faceindex
//  mb::SurfacePoint p;
//
//  for(int i = 0 ; i < xSize ; i++) {
//    y = minY;
//    for(int j = 0 ; j < ySize; j++) {
//      point = mb::Vector(x,y);
//      extremePoint = mb::Vector(maxX+10,y,0);
//      if(checkIsInside(points,point,extremePoint)) {
//        bool b = ssp->Pick(curCam,x,y,p,false);
//        fi = p.FaceIndex();
//        if(b && checkUniqueInFaceList(fi)) {
//          sp.push_back(p);
//          faces.push_back(fi);
//          addVertex(fi);
//          mb::Kernel()->Log("Face: "+QString::number(x)+" "
//            +QString::number(y)+" "+QString::number(p.FaceIndex())+" "+b+"\n");
//        }
//      }
//      y++;
//    }
//    x++;
//  }
//}


void MeshOps::boxSelect(mb::Vector &v1,mb::Vector &v2,float maxDist, float strength) {
  int x = (v1.x + v2.x)*0.5;
  int y = (v1.y + v2.y)*0.5;
  mb::SurfacePoint p;
  std::vector<int> faceIndices;
  int fi;
  int vertexCountInRange = 0;
  VertexModifyInfo vMI;
  float dist;
  if(curCam->Pick(x,y,p)) {
    mblog("Selected :"+p.Mesh()->Name()+" Mesh \n");
    if(p.Mesh()->ID() == pMesh->ID()) {
      faceIndices.push_back(p.FaceIndex());
      faces->push_back(p.FaceIndex());
      mblog("facesPushBack\n");
      midPos = p.WorldPosition();
      QTime *t = new QTime();
      t->start();
      for( unsigned int i = 0 ; i < pMesh->VertexCount() ; i++) {
        dist = pMesh->VertexPosition(i).DistanceFrom(midPos);
        if(dist < maxDist) {
          if(checkUniqueInVertexList(i)) {
            vertexCountInRange++;
            vMI.vI = (int)i;
            vMI.strength = MIN(10/dist,1);
            vertices->push_back(vMI);
            fi = i%4;
            if(checkUniqueInFaceList(fi)) {
              faces->push_back(fi);
            }
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


void MeshOps::boxSelect2(mb::Vector &v1,mb::Vector &v2) {
  int xSize = abs(v1.x - v2.x);
  int ySize = abs(v1.y - v2.y);
  size_t boxSize = xSize*ySize;
  mblog("Getting Screen Space Picker: "+curCam->Name()+"\n");
  ssp = curCam->GetScreenSpacePicker();
  if(ssp == NULL) {
    mblog("SSP might be NULL?\n");
  }
  mblog("\n boxSelect:"+curCam->Name()+"\n");
  int fi;
  mb::SurfacePoint p;
  // TODO: Optimise by reserving space
  // TODO: Ensure it works if v2 is < v1 etc..
  int x = MIN(v1.x,v2.x);
  int y = MIN(v1.y,v2.y);
  
  mblog("Pixels select Range :"+QString::number(x)+" "+QString::number(y)+"\nTo: "+
    QString::number(x+xSize)+" "+QString::number(y+ySize));
  //MidVertex Special;
  for (int i = 0; i < xSize ; i++) {
    y = MIN(v1.y,v2.y);
    for(int j = 0 ; j < ySize; j++) {
      //If there is a surface point in this pixel
      if(curCam->Pick(x,y,p)) {
        //If it belongs to the same mesh
        if(p.Mesh()->ID() == pMesh->ID()) {

          fi = p.FaceIndex();
          //mblog("Cur Loop :"+QString::number(i)+" "+QString::number(j)+"\n");
          if((j == floor(ySize/2)) && (i == floor(xSize/2))) {
            mblog("Saving mid point");
            if(pMesh != NULL) {
              midV->vI = pMesh->QuadPrimaryIndex(false,fi,2);
              midV->pos = p.WorldPosition();
            } else {
              mblog("PMeshNULL WTF?!");
            }
          }
          if(checkUniqueInFaceList(fi)) {  
            points->push_back(p);
            faces->push_back(fi);
            addVertex(fi);
            mblog("Face: "+QString::number(x)+" "
              +QString::number(y)+" "+QString::number(p.FaceIndex())+"\n");
          }
        }
      }
      y++;
    }
    x++;
  }
  if(midV->pos == mb::Vector(0,0,0)) {
    if(vertices->size() > 0) {
      mb::Vector sumVector = mb::Vector(0,0,0);
      for(int ind = 0 ; ind < vertices->size() ; ind++) {
        sumVector += pMesh->VertexPosition(vertices->at(ind).vI);
      }
      midV->pos = sumVector/(float)vertices->size();
    }
  }
}


void MeshOps::boxSelect(mb::Vector &v1,mb::Vector &v2,mb::Image *stamp) {
  
  int x = (v1.x + v2.x)*0.5;
  int y = (v1.y + v2.y)*0.5;

  int height = stamp->Height();
  mb::SurfacePoint p;
  std::vector<int> faceIndices;
  int vertexCountInRange = 0;
  VertexModifyInfo vMI;
  float dist;
  if(curCam->Pick(x,y,p)) {

    if(p.Mesh()->ID() == pMesh->ID()) {
      faceIndices.push_back(p.FaceIndex());
      faces->push_back(p.FaceIndex());
      //Search for closest to middle point
      //for(int c = 0 ; c < pMesh->SideCount() ; c++) {
      //  posi = pMesh->QuadVertexPosition(faceIndices.back(),c);
      //  curDist = posi.DistanceFrom(p.WorldPosition());
      //  //mblog("Picking X "+QString::number(x)+" Y "+QString::number(y)+" World Pos: "+VectorToQString(p.WorldPosition())+" curDist "+QString::number(curDist)+"\n");
      //  if(curDist < closestDist) {
      //    //mblog("Closest dist for X "+QString::number(x)+" Y"+QString::number(y)+" "+QString::number(curDist)+"\n");
      //    closestDist = curDist;
      //    closestCorner = c;
      //  }
      //}
      midPos = p.WorldPosition();
      QTime *t = new QTime();
      //while(faceIndices.size() > 0) {
      t->start();
      for( unsigned int i = 0 ; i < pMesh->VertexCount() ; i++) {
        /*start = false;
        fi = faceIndices.back();
        faceIndices.pop_back();
        for(int c = 0 ; c < pMesh->SideCount() ; c++) {
          vi = pMesh->QuadIndex(fi,c);*/
          //dist = pMesh->QuadVertexPosition(fi,c).DistanceFrom(midPos) ;
          dist = pMesh->VertexPosition(i).DistanceFrom(midPos);
          if(dist < height) {
            if(checkUniqueInVertexList(i)) {
              vertexCountInRange++;
              vMI.vI = (int)i;
              vMI.strength = MIN(10/dist,1);
              vertices->push_back(vMI);
            }
          //}
        }
      }
      mblog("Box Select VerticesLoop: "+QString::number(t->elapsed())+"\n");

  //    }//End of VertexCountInRange

    }//End of checking for PMeshID
  }
}

void MeshOps::boxSelect2(mb::Vector &v1,mb::Vector &v2,mb::Image *stamp) {
  int xSize = abs(v1.x - v2.x);
  int ySize = abs(v1.y - v2.y);
  mblog("Getting Screen Space Picker: "+curCam->Name()+"\n");
  ssp = curCam->GetScreenSpacePicker();
  if(ssp == NULL) {
    mblog("SSP might be NULL?\n");
  }
  mblog("\n boxSelect:"+curCam->Name()+"\n");
  int fi;
  mb::SurfacePoint p;
  // TODO: Optimise by reserving space
  // TODO: Ensure it works if v2 is < v1 etc..
  int x = MIN(v1.x,v2.x);
  int y = MIN(v1.y,v2.y);
  
  mblog("Pixels select Range :"+QString::number(x)+" "+QString::number(y)+"\nTo: "+
    QString::number(x+xSize)+" "+QString::number(y+ySize));
  
  QTime *t = new QTime();
  t->start();
  for (int i = 0; i < xSize ; i++) {
    y = MIN(v1.y,v2.y);
    for(int j = 0 ; j < ySize; j++) {
      //If there is a surface point in this pixel
      if(curCam->Pick(x,y,p)) {
        //If it belongs to the same mesh
        if(p.Mesh()->ID() == pMesh->ID()) {
          fi = p.FaceIndex();
          if((j == floor(ySize/2)) && (i == floor(xSize/2))) {
            mblog("Saving mid point");
            if(pMesh != NULL) {
              midV->vI = pMesh->QuadPrimaryIndex(false,fi,2);
              midV->pos = p.WorldPosition();
            } else {
              mblog("PMeshNULL WTF?!");
            }
          }
          if(checkUniqueInFaceList(fi)) {  
            points->push_back(p);
            faces->push_back(fi);
            addVertex(fi);
            mblog("Face: "+QString::number(x)+" "
              +QString::number(y)+" "+QString::number(p.FaceIndex())+"\n");
          }
        }
      }
      y++;
    }
    x++;
  }
  
  mblog("Box Select First Loop: "+QString::number(t->elapsed())+"\n");
  mb::Vector posi;
  float closestDist;
  float curDist;
  float closestX;
  float closestY;
  x = MIN(v1.x,v2.x);
  y = MIN(v1.y,v2.y);
  t->restart();
  for(int k = 0 ; k < vertices->size() ; k++) {
    closestDist = 999999.0f;
    posi = pMesh->VertexPosition(vertices->at(k).vI);
    //mblog("VertexPos = "+VectorToQStringLine(posi)+"\n");
    for(int i = 0; i < xSize; i++) {
      y = MIN(v1.y,v2.y);
      for(int j = 0 ; j < ySize; j++) {
        if(curCam->Pick(x,y,p)) {
          curDist = posi.DistanceFrom(p.WorldPosition());
          //mblog("Picking X "+QString::number(x)+" Y "+QString::number(y)+" World Pos: "+VectorToQString(p.WorldPosition())+" curDist "+QString::number(curDist)+"\n");
          if(curDist < closestDist) {
            //mblog("Closest dist for X "+QString::number(x)+" Y"+QString::number(y)+" "+QString::number(curDist)+"\n");
            closestDist = curDist;
            closestX = i;
            closestY = j;
          }
        }
        y++;
      }
      x++;
    }
    mblog("ShortestDist = "+QString::number(closestDist)+"\n");
    vertices->at(k).strength = stamp->ColorAt(closestX,closestY).Luminance();
    mblog("Vertex : "+QString::number(k)+" Strength = "+QString::number(vertices->at(k).strength)+"\n");
  }
   mblog("Box Select Second Loop: "+QString::number(t->elapsed())+"\n");

}

void MeshOps::DeselectAllFaces() {
  //TODO: 
  if(pMesh != NULL) {
    pMesh->SetSelected(false);
    MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
    MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
    mb::Kernel()->Redraw();
  }
}

 //pMesh->EnumerateNeighboringFaces(faces.at(0),);
bool MeshOps::CheckIntersection(mb::AxisAlignedBoundingBox box1) {
  //mblog("Gothere\n");
  if(pMesh != NULL) {
    mb::AxisAlignedBoundingBox box2 = pMesh->BoundingBox(true);
    //if(box2.IsTouching(box1))
      //mblog("Box2 is touching\n");
    //if(box2.IsContaining(box1))
      //mblog("Box2 is Containing\n");
    //mblog("box1 vol:"+QString::number(box1.Volume())+"Box2 Vol: "+QString::number(box2.Volume())+"\n");
    return box2.IsTouching(box1) && !box2.IsContaining(box1);
  } else {
    return false;
  }
}

MeshOps::MeshOps(mb::Mesh *m) {
  pMesh = m;
  curCam = mb::Kernel()->Scene()->ActiveCamera();

}

void MeshOps::setMesh(mb::Mesh *m) {
  pMesh = m;
  pMesh->SetSelected(true);
}