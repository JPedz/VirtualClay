#include "stdafx.h"
#include "MeshOps.h"



MeshOps::MeshOps() {
  MeshGeo = mb::Kernel()->Scene()->SelectedGeometry(0);
  curCam = mb::Kernel()->Scene()->ActiveCamera();
  ssp = curCam->GetScreenSpacePicker();
  pMesh = NULL;
  MeshGeo = NULL;
}


void MeshOps::SelectObject(cameraWrapper *viewCam, mb::Vector screenPos) {
  mb::SurfacePoint sp;
  int w = mb::Kernel()->ViewPort()->Width();
  int h = mb::Kernel()->ViewPort()->Height();
  float screenX = w/2+screenPos.x*(w/2);
  float screenY = h/2+screenPos.y*(h/2);
  ssp = viewCam->getCamera()->GetScreenSpacePicker();
  bool b = ssp->Pick(curCam,screenX,screenY,sp,false);
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


void MeshOps::pickObj() {
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
    QList<int> faces;
    QList<int> vertices;
    QList<mb::SurfacePoint > points;

    mb::Vector vS = mb::Vector(midW-50,midH-50,0);
    mb::Vector vE = mb::Vector(midW + 50, midH + 50, 0);
    //mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,pMesh->Name());
    //mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,QString("BOX SELECTED"+QString::number(points.size())));
    
    mblog("SelectingBox\n");
    //polygonSelect(polyScreen,points,faces,vertices);
    boxSelect(vS,vE,points,faces,vertices);
    
    mbstatus(QString("Polygon SELECTED"+QString::number(points.size())));
    mblog("box selected");
    for(int i = 0 ; i < faces.size() ; i++) {
      mb::Kernel()->Log(QString::number(i)+" "+QString::number(faces.at(i))+"\n");
      pMesh->SetFaceSelected(faces.at(i));
    }
    int vi;
    //for(int i = 0 ; i < vertices.size(); i++) {
    //  vi = vertices.at(i);
    //  mb::Vector v = pMesh->VertexPosition(vi);  
    //  //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v.x)+"\n");
    //  pMesh->AddVertexPosition(vi,mb::Vector(100,100,10));
    //  mb::Vector v2 = pMesh->VertexPosition(vi);
    //  //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v2.x)+"\n"); 
    //}
    
    MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
    MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
    //int subDiv_I = MeshGeo->ActiveLevel()->Index();
    //MeshGeo->ChangeActiveLevel(MeshGeo->Level(subDiv_I));
    mb::Kernel()->ViewPort()->Redraw();
  }
}

void MeshOps::addVertex(unsigned int  fi,QList<int> &v_list) {
  unsigned int v;
  if(pMesh->Type() == mb::Mesh::typeQuadric)  {
    //mblog("\nQuad\n");
    for(int i = 0 ; i < 4 ; i++) {
      v = pMesh->QuadPrimaryIndex(false,fi,i);
        if(checkUniqueInList(v,v_list)) {
          v_list.push_back(v);
          //mb::Kernel()->Log("Vertex: "+QString::number(v)+"\n");
        }
    }
  } else if(pMesh->Type() == mb::Mesh::typeTriangular) {
    //mblog("\nTriangular\n");
    for(int i = 0 ; i < 3 ; i++) {
      v = pMesh->TrianglePrimaryIndex(false,fi,1);
      if(checkUniqueInList(v,v_list)) {
        v_list.push_back(v);
          //mb::Kernel()->Log("Vertex: "+QString::number(v)+"\n");
      }
    }
  } else {
    //mblog("\nNot either mesh type?\n");
  }

}

bool MeshOps::checkUniqueInList(unsigned int &fi,QList<int> &faces) {
  for(int i = 0 ; i < faces.size() ; i++)
    if(fi == faces.at(i))
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



void MeshOps::polygonSelect(QList<mb::Vector> &points, QList<mb::SurfacePoint>&sp,
                         QList<int> &faces, QList<int> &vertices) {
  
  int minX,minY,maxX,maxY;
  minX = points.at(0).x;
  minY = points.at(0).y;
  maxX = points.at(0).x;
  maxY = points.at(0).y;
  for(int i = 0; i < points.size() ; i++) {
    if(points.at(i).x < minX)
      minX = points.at(i).x;
    if(points.at(i).x > maxX)
      maxX = points.at(i).x;
    if(points.at(i).y < minY)
      minY = points.at(i).y;
    if(points.at(i).y > maxY)
      maxY = points.at(i).y;
  }
  int xSize = abs(maxX - minX);
  int ySize = abs(maxY = minY);
  int x = minX;
  int y = minY;
  mb::Vector point;
  mb::Vector extremePoint;
  size_t boundingBoxSize = xSize*ySize;
  
  unsigned int fi;//faceindex
  mb::SurfacePoint p;

  for(int i = 0 ; i < xSize ; i++) {
    y = minY;
    for(int j = 0 ; j < ySize; j++) {
      point = mb::Vector(x,y);
      extremePoint = mb::Vector(maxX+10,y,0);
      if(checkIsInside(points,point,extremePoint)) {
        bool b = ssp->Pick(curCam,x,y,p,false);
        fi = p.FaceIndex();
        if(b && checkUniqueInList(fi,faces)) {
          sp.push_back(p);
          faces.push_back(fi);
          addVertex(fi,vertices);
          mb::Kernel()->Log("Face: "+QString::number(x)+" "
            +QString::number(y)+" "+QString::number(p.FaceIndex())+" "+b+"\n");
        }
      }
      y++;
    }
    x++;
  }
}


void MeshOps::boxSelect(mb::Vector &v1,mb::Vector &v2, QList<mb::SurfacePoint>&sp,
                         QList<int> &faces, QList<int> &vertices) {
  int xSize = abs(v1.x - v2.x);
  int ySize = abs(v1.y - v2.y);
  size_t boxSize = xSize*ySize;
  ssp = curCam->GetScreenSpacePicker();
  mb::Kernel()->Log("\n boxSelect:"+curCam->Name()+" "+ssp->Name()+"\n");
  unsigned int fi;
  mb::SurfacePoint p;
  // TODO: Optimise by reserving space
  // TODO: Ensure it works if v2 is < v1 etc..
  int x = MIN(v1.x,v2.x);
  int y = MIN(v1.y,v2.y);
  mb::Kernel()->Log("\nPicking: \n"+curCam->Name());
  for (int i = 0; i < xSize ; i++) {
    y = MIN(v1.y,v2.y);
    for(int j = 0 ; j < ySize; j++) {
      bool b = ssp->Pick(curCam,x,y,p,false);
      fi = p.FaceIndex();
      if(b && checkUniqueInList(fi,faces)) {
        sp.push_back(p);
        faces.push_back(fi);
        addVertex(fi,vertices);
        mb::Kernel()->Log("Face: "+QString::number(x)+" "
          +QString::number(y)+" "+QString::number(p.FaceIndex())+" "+b+"\n");
      }
      y++;
    }
    x++;
  }
}

void MeshOps::extrudefaces(std::vector<int> &faces) {
  
  //pMesh->EnumerateNeighboringFaces(faces.at(0),);
}

QList<int> MeshOps::getFaces(QList<mb::SurfacePoint> &sPs) {
  QList<int> faces;
  int fi; //Face Index
  bool repeat = false;
  mb::Kernel()->Log("\nGetting Faces: \n");
  for(int i = 0 ; i < sPs.size() ; i++) {
    fi = sPs.at(i).FaceIndex();
    
    mb::Kernel()->Log(QString::number(fi)+"\n");
    //check for uniqueness
    for(int j = 0 ; j < faces.size() ; j++) {
      if(fi == faces.at(j)) {
        repeat = true;
        break;
      } else {
        repeat = false;
      }
    }
    if(!repeat) {
      faces.push_back(sPs.at(i).FaceIndex());
    }
  }
  return faces;
}

MeshOps::MeshOps(mb::Mesh *m) {
  pMesh = m;
  curCam = mb::Kernel()->Scene()->ActiveCamera();

}

void MeshOps::setMesh(mb::Mesh *m) {
  pMesh = m;
}