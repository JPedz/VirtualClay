#include "stdafx.h"
#include "MeshOps.h"



MeshOps::MeshOps() {
  mb::Geometry *g = mb::Kernel()->Scene()->SelectedGeometry(0);
  curCam = mb::Kernel()->Scene()->ActiveCamera();
  ssp = curCam->GetScreenSpacePicker();
  int w = mb::Kernel()->ViewPort()->Width();
  int h = mb::Kernel()->ViewPort()->Height();
  mb::SurfacePoint sp;
  pMesh = NULL;
  bool b = ssp->Pick(curCam,w/2,h/2,sp,false);
  if(b) {
    mb::Kernel()->Log("\n Hit MeshOps\n");
    pMesh = sp.Mesh();
    mb::Kernel()->Log(pMesh->Name()+" "+QString::number(sp.FaceIndex()));
  } else {
    mb::Kernel()->Log("\n Missed MeshOps \n");
  }
}


void MeshOps::pickObj(void) {
  if(pMesh != NULL) {
    ssp = curCam->GetScreenSpacePicker();
    mb::Kernel()->Log("\nSSP:"+curCam->Name()+" "+ssp->Name()+"\n");
    mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,QString("GOT ScreenPicker"));
  
    int midW = mb::Kernel()->ViewPort()->Width()/2;
    int midH = mb::Kernel()->ViewPort()->Height()/2;
    
    QList<int> faces;
    QList<int> vertices;
    QList<mb::SurfacePoint > points;

    mb::Vector vS = mb::Vector(midW,midH,0);
    mb::Vector vE = mb::Vector(midW + 100, midH + 100, 0);
    mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,pMesh->Name());
    boxSelect(vS,vE,points,faces,vertices);
    mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,QString("BOX SELECTED"+QString::number(points.size())));
    //QList<int> faces = getFaces(points);
    //mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,QString("GOT FACES")+QString::number(faces.size()));

    for(int i = 0 ; i < faces.size() ; i++) {
      mb::Kernel()->Log(QString::number(i)+" "+QString::number(faces.at(i))+"\n");
      pMesh->SetFaceSelected(faces.at(i));
    }
    int vi;
    for(int i = 0 ; i < vertices.size(); i++) {
      vi = vertices.at(i);
      mb::Vector v = pMesh->VertexPosition(vi);  
      //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v.x)+"\n");
      pMesh->AddVertexPosition(vi,mb::Vector(100,100,10));
      mb::Vector v2 = pMesh->VertexPosition(vi);
      //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v2.x)+"\n"); 
    }    
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