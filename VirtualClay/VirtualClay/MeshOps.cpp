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
  faces = new QList<int>();
  vertices = new QList<int>();
  points = new QList<mb::SurfacePoint>();
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
    int subDiv_I = MeshGeo->ActiveLevel()->Index();
    MeshGeo->ChangeActiveLevel(MeshGeo->Level(subDiv_I));
    mb::Kernel()->Log(pMesh->Name()+" "+QString::number(sp.FaceIndex()));
  } else {
    mbstatus("\n No ObjectSelected \n");
  }
}

void MeshOps::ChangeCamera(cameraWrapper *cam) {
  curCam = cam->getCamera();
}


bool SelectFaces(mb::Vector centrePoint, float width, float height, float dropOffRate) {
  mblog("Wrong faces select");
  return false;
}

bool MeshOps::SelectFaces(mb::AxisAlignedBoundingBox box) {
  if(pMesh != NULL) {
    mb::Vector start = box.m_vStart;
    mb::Vector end = box.m_vEnd;
    mb::Picker *p = new mb::Picker();
    p->SetMesh(pMesh);
    mb::SurfacePoint sp;
    mblog("Start: "+VectorToQString(start)+" End: "+VectorToQStringLine(end));
    if(p->Pick(start,end,true,sp,0.5f)) {
      mblog("sp face =: "+QString::number(sp.FaceIndex())+"\n");
    }
    return true;
  } else {
    mblog("Error in: Select Faces, -Box- pMesh == NULL");
  }



  return false;
}


bool MeshOps::SelectFaces(mb::Vector centrePoint, float widthHeight, float dropOffRate) {
  
  if(pMesh != NULL) {
    mb::MeshChange *mC = pMesh->StartChange();
    mb::Vector vS = centrePoint - mb::Vector(widthHeight,widthHeight,0);
    mb::Vector vE = centrePoint + mb::Vector(widthHeight,widthHeight,0);
    faces->clear();
    vertices->clear();
    points->clear();
    mblog("SelectingBox\n");
    boxSelect(vS,vE);
    if(faces->size() > 0) {
      mbstatus(QString("Polygon SELECTED"+QString::number(points->size())));
      mblog("box selected");
      for(int i = 0 ; i < faces->size() ; i++) {
        mb::Kernel()->Log(QString::number(i)+" "+QString::number(faces->at(i))+"\n");
        pMesh->SetFaceSelected(faces->at(i));
      }
      mbstatus("Selected Mesh: "+pMesh->Name());
      int subDiv_I = MeshGeo->ActiveLevel()->Index();
      MeshGeo->ChangeActiveLevel(MeshGeo->Level(subDiv_I));

      MeshGeo->ContentChanged();
      mb::Kernel()->ViewPort()->Redraw();
      StoreUndoQueue();
      return true;
      
    }
  }
  return false;
}

bool MeshOps::SelectFaces() {
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

    mb::Vector vS = mb::Vector(midW-20,midH-20,0);
    mb::Vector vE = mb::Vector(midW + 20, midH + 20, 0);
    //mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,pMesh->Name());
    //mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,QString("BOX SELECTED"+QString::number(points.size())));
    faces->clear();
    vertices->clear();
    points->clear();
    mblog("SelectingBox\n");
    //polygonSelect(polyScreen,points,faces,vertices);
    boxSelect(vS,vE);
    
    if(faces->size() > 0) {
      mbstatus(QString("Polygon SELECTED"+QString::number(points->size())));
      mblog("box selected");
      for(int i = 0 ; i < faces->size() ; i++) {
        mb::Kernel()->Log(QString::number(i)+" "+QString::number(faces->at(i))+"\n");
        pMesh->SetFaceSelected(faces->at(i));
      }
      
      MeshGeo->ContentChanged();
      MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      //int subDiv_I = MeshGeo->ActiveLevel()->Index();
      //MeshGeo->ChangeActiveLevel(MeshGeo->Level(subDiv_I));
      mb::Kernel()->ViewPort()->Redraw();
      StoreUndoQueue();
      return true;
    }
  }
  return false;
}

void MeshOps::MoveVertices(mb::Vector v) {
  int vi;
  for(int i = 0 ; i < vertices->size(); i++) {
    vi = vertices->at(i);
    mblog("Moving Vertex"+QString::number(i)+"\n");
    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v.x)+"\n");
    pMesh->AddVertexPosition(vi,v);
    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v2.x)+"\n"); 
  }
  if(MeshGeo != NULL) {
    
      MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      mb::Kernel()->ViewPort()->Redraw();
  //    MeshGeo->ContentChanged();
  //    //pMesh->RecalculateNormals();
  //    pMesh->Modified.Trigger();
  //    pMesh->ContentChanged();
  //    mb::Kernel()->ViewPort()->Redraw();
  }
}

void MeshOps::StoreUndoQueue() {
  VertexInfo vertInfo;
  mblog("Storing Undo\n");
  //mb::VertexAdjacency *vA = new mb::VertexAdjacency();
  std::vector<VertexInfo> vertInfoList;
  for(int i = 0 ; i < vertices->size(); i++) {
    //if(pMesh != NULL) {
    //  mblog("getting vertexAdjacency\n");
    //  vA = &pMesh->VertexAdjacency(vertices->at(i));
    //}
    vertInfo.vI = vertices->at(i);
    //if(!vA->IsValid()) {
    //  mblog("NOT VALID VA\n");
    //} else {
    //  vertInfo.fI = vA->FaceIndex();
    //}
      mblog("getting vertex Position\n");
    vertInfo.pos = pMesh->VertexPosition(vertInfo.vI);
    vertInfoList.push_back(vertInfo);
    mblog("Storing: "+QString::number(vertInfo.vI)+" "+VectorToQStringLine(vertInfo.pos));
  }
  undoQueue.push_back(vertInfoList);
  mblog("Stored Undo\n");
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
      MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      mb::Kernel()->ViewPort()->Redraw();
    }
  }
}

void MeshOps::addVertex(int  fi) {
  unsigned int v;
  if(pMesh->Type() == mb::Mesh::typeQuadric)  {
    //mblog("\nQuad\n");
    for(int i = 0 ; i < pMesh->SideCount() ; i++) {
      v = pMesh->QuadPrimaryIndex(false,fi,i);
        if(checkUniqueInVertexList(v)) {
          vertices->push_back(v);
          //mb::Kernel()->Log("Vertex: "+QString::number(v)+"\n");
        }
    }
  } else if(pMesh->Type() == mb::Mesh::typeTriangular) {
    //mblog("\nTriangular\n");
    for(int i = 0 ; i < 3 ; i++) {
      v = pMesh->TrianglePrimaryIndex(false,fi,1);
      if(checkUniqueInVertexList(v)) {
        vertices->push_back(v);
          //mb::Kernel()->Log("Vertex: "+QString::number(v)+"\n");
      }
    }
  } else {
    //mblog("\nNot either mesh type?\n");
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
    if(vi == vertices->at(i))
      return false;//not unique
  return true;//unique
}


bool checkIsInside(QList<mb::Vector> &points,mb::Vector point,mb::Vector extreme) {
  bool isInside = false;
  int j = points.size()-1;
  float x = point.x;
  float y = point.y;
  for (int i = 0 ; i < points.size() ; i++) {
    if ((points.at(i).y< y && points.at(j).y>=y || points.at(j).y< y && points.at(i).y>=y) &&
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


void MeshOps::boxSelect(mb::Vector &v1,mb::Vector &v2) {
  int xSize = abs(v1.x - v2.x);
  int ySize = abs(v1.y - v2.y);
  size_t boxSize = xSize*ySize;
  ssp = curCam->GetScreenSpacePicker();
  mblog("\n boxSelect:"+curCam->Name()+" "+ssp->Name()+"\n");
  int fi;
  mb::SurfacePoint p;
  // TODO: Optimise by reserving space
  // TODO: Ensure it works if v2 is < v1 etc..
  int x = MIN(v1.x,v2.x);
  int y = MIN(v1.y,v2.y);
  for (int i = 0; i < xSize ; i++) {
    y = MIN(v1.y,v2.y);
    for(int j = 0 ; j < ySize; j++) {
      bool b = ssp->Pick(curCam,x,y,p,false);
      fi = p.FaceIndex();
      if(b && checkUniqueInFaceList(fi)) {
        points->push_back(p);
        faces->push_back(fi);
        addVertex(fi);
        mblog("Face: "+QString::number(x)+" "
          +QString::number(y)+" "+QString::number(p.FaceIndex())+" "+b+"\n");
      }
      y++;
    }
    x++;
  }
}

void MeshOps::DeselectAllFaces() {
  //TODO: 
  if(pMesh != NULL) {
    pMesh->SetSelected(false);
    mb::Kernel()->Redraw();
  }
}

 //pMesh->EnumerateNeighboringFaces(faces.at(0),);
bool MeshOps::CheckIntersection(mb::AxisAlignedBoundingBox box1) {
  //mblog("Gothere\n");
  if(pMesh != NULL) {
    mb::AxisAlignedBoundingBox box2 = pMesh->BoundingBox();
    //mblog("box1 vol:"+QString::number(box1.Volume())+"Box2 Vol: "+QString::number(box2.Volume())+"\n");
    return box2.IsTouching(box1);
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
}