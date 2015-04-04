#include "stdafx.h"
#include "MeshOps.h"

#define OMP_NUM_THREADS 8


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
  tessInfo_L = new std::vector<TessInfo>();
  tessInfo_R = new std::vector<TessInfo>();
  meshStoreToggle = new std::vector<bool>;
  meshStore = new std::vector<mb::Mesh*>;
  midV = new MidVertex;
  firstUse = true;
  lastMidPosition = mb::Vector(0,0,0);
}

MeshOps::MeshOps(mb::Mesh *m) {
  pMesh = m;
  meshLayer = pMesh->AddLayer();
  meshLayer->SetTransparency(1.0f);
  meshLayer->SetVisible(true);
  curCam = mb::Kernel()->Scene()->ActiveCamera();
}

void MeshOps::SetMesh(mb::Mesh *m) {
  pMesh = m;
  meshLayer = pMesh->AddLayer();
  meshLayer->SetTransparency(1.0f);
  meshLayer->SetVisible(true);
  if(pMesh->Parent()->IsKindOf(mb::Geometry::StaticClass())) {
    mb::Geometry *geoPotential = dynamic_cast<mb::Geometry *>(pMesh->Parent());
    MB_SAFELY(geoPotential) {
      mblog("Found GEO FROM PARENT"+geoPotential->Name()+"\n");
      MeshGeo = geoPotential;
      //MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      mb::Kernel()->Scene()->SetActiveGeometry(MeshGeo);
    }
  }
  mbstatus("Selected Mesh: "+pMesh->Name());
  mbhud("Mesh Selected: "+pMesh->Name()+"\nGeo: "+MeshGeo->Name());
  if(!pMesh->HasTC()) {
    mb::SubdivisionLevel *subdiv= MeshGeo->ActiveLevel();
    subdiv->RecreateUVs(true);
  }
  pMesh->RecalculateAdjacency();
  pMesh->RecalculateNormals();
  pMesh->RecalculateVertexAdjacency();
  //pMesh->SetSelected(true);
}

int MeshOps::UniqueTessInfo(LR lr,TessInfo &tI) {
  std::vector<TessInfo> *tessInfo;
  if(lr == l) {
    tessInfo = tessInfo_L;
  } else {
    tessInfo = tessInfo_R;
  }
  int unique = 0;
  for(int i = 0 ; i < tessInfo->size() ; i++) {
    if(tessInfo->at(i).splitA == tI.splitA) {
      tI.vIA = tessInfo->at(i).vIA;
      unique++;
    }
    if(tessInfo->at(i).splitB == tI.splitA) {
      tI.vIA = tessInfo->at(i).vIB;
      unique++;
    }

    if(tessInfo->at(i).splitA == tI.splitB) {
      tI.vIB = tessInfo->at(i).vIA;
      unique++;
    }
    if(tessInfo->at(i).splitB == tI.splitB) {
      tI.vIB = tessInfo->at(i).vIB;
      unique++;
    }
  }

  return MIN(unique,2);
}

void MeshOps::FindTesselationFaces(LR lr) {
  if(pMesh != NULL) {
    tessInfo_L->clear();
    tessInfo_R->clear();
    std::vector<TessInfo> *tessInfo;
    std::vector<int> *faces;
    TessInfo tI;
    if(lr == l) {
      faces = faces_L;
      tessInfo = tessInfo_L;
    } else {
      tessInfo = tessInfo_R;
      faces = faces_R;
    }
    unsigned int oldVertexCount = pMesh->VertexCount();
    unsigned int vertIndex = pMesh->VertexCount();

    for (int i = 0 ; i < faces->size(); i++) {
      mblog("fi = "+QString::number(faces->at(i))+"\n");
      float distA1 = pMesh->QuadVertexPosition(faces->at(i),0).DistanceFrom(pMesh->QuadVertexPosition(faces->at(i),3));
      float distA2 = pMesh->QuadVertexPosition(faces->at(i),1).DistanceFrom(pMesh->QuadVertexPosition(faces->at(i),2));
      float distB1 = pMesh->QuadVertexPosition(faces->at(i),0).DistanceFrom(pMesh->QuadVertexPosition(faces->at(i),1));
      float distB2 = pMesh->QuadVertexPosition(faces->at(i),2).DistanceFrom(pMesh->QuadVertexPosition(faces->at(i),3));
    
      mblog("DistA1: "+QString::number(distA1)+"\n");
      mblog("DistA2: "+QString::number(distA2)+"\n");
      mblog("DistB1: "+QString::number(distB1)+"\n");
      mblog("DistB2: "+QString::number(distB2)+"\n");

      if((distA1+distA2) > (distB1+distB2)) {
        if((distA1 > 30) || (distA2 > 30)) {
          tI.type = true;
          tI.fi = faces->at(i);
          tI.splitA = 0.5f*(pMesh->QuadVertexPosition(faces->at(i),0)+pMesh->QuadVertexPosition(faces->at(i),3));
          tI.splitB = 0.5f*(pMesh->QuadVertexPosition(faces->at(i),1)+pMesh->QuadVertexPosition(faces->at(i),2));
          tI.vIA = vertIndex++;
          tI.vIB = vertIndex++;
          vertIndex -= UniqueTessInfo(lr,tI);
          tessInfo->push_back(tI);
        }
      } else {
        if((distB1 > 30) || (distB2 > 30)) {
          tI.type = false;
          tI.fi = faces->at(i);
          tI.splitA = 0.5f*(pMesh->QuadVertexPosition(faces->at(i),0)+pMesh->QuadVertexPosition(faces->at(i),1));
          tI.splitB = 0.5f*(pMesh->QuadVertexPosition(faces->at(i),2)+pMesh->QuadVertexPosition(faces->at(i),3));
          tI.vIA = vertIndex++;
          tI.vIB = vertIndex++;
          vertIndex -= UniqueTessInfo(lr,tI);
          tessInfo->push_back(tI);
        }
      }
    }
    mblog("TessInfo:"+QString::number(tessInfo->size())+"\n");
    for(int j = 0 ; j < tessInfo->size() ; j++) {
    
      mblog("fi = "+QString::number(tessInfo->at(j).fi)+"\n");
      //mblog("zero = "+VectorToQStringLine(pMesh->QuadVertexPosition(faces->at(tessInfo->at(j).fi),0)));
      //mblog("one = "+VectorToQStringLine(pMesh->QuadVertexPosition(faces->at(tessInfo->at(j).fi),1)));
      //mblog("two = "+VectorToQStringLine(pMesh->QuadVertexPosition(faces->at(tessInfo->at(j).fi),2)));
      //mblog("three = "+VectorToQStringLine(pMesh->QuadVertexPosition(faces->at(tessInfo->at(j).fi),3)));
      //
      mblog("splitA = "+VectorToQStringLine(tessInfo->at(j).splitA));
      mblog("splitB = "+VectorToQStringLine(tessInfo->at(j).splitB));
      mblog("vIA = "+QString::number(tessInfo->at(j).vIA)+"\n");
      mblog("vIB = "+QString::number(tessInfo->at(j).vIB)+"\n");
      mblog("Type = "+QString::number(tessInfo->at(j).type)+"\n");
    }
    if(tessInfo->size() > 0) {
    
      mb::Mesh *storeMesh = dynamic_cast<mb::Mesh*>(pMesh->Duplicate());
      meshStoreToggle->push_back(true);
      meshStore->push_back(storeMesh);
      Tesselate();
    }
  }
}

void MeshOps::Tesselate() {
  mb::Mesh *newMesh = mb::Kernel()->Scene()->CreateMesh(mb::Topology::typeQuadric);
  newMesh = dynamic_cast<mb::Mesh*>(pMesh->Duplicate());
  mblog("Tesselate\n");
  for(int i = 0 ; i < tessInfo_L->size() ; i ++ ) {

    
    int oldVertCount = newMesh->VertexCount();
    int oldFaceCount = newMesh->FaceCount();
    int oldTCCount = newMesh->TCCount();
    newMesh->SetTCCount(oldTCCount+4);
    newMesh->SetVertexCount(oldVertCount+4);
    newMesh->SetFaceCount(oldFaceCount+4);

    int fi =tessInfo_L->at(i).fi;

    mblog("fi = "+QString::number(tessInfo_L->at(i).fi)+"\n");
    unsigned int zero = newMesh->QuadPrimaryIndex(false,fi,0);
    unsigned int one = newMesh->QuadPrimaryIndex(false,fi,1);
    unsigned int two = newMesh->QuadPrimaryIndex(false,fi,2);
    unsigned int three = newMesh->QuadPrimaryIndex(false,fi,3);
  
    mblog("Zero = "+VectorToQStringLine(newMesh->VertexPosition(zero)));
    mblog("One = "+VectorToQStringLine(newMesh->VertexPosition(one)));
    mblog("Two = "+VectorToQStringLine(newMesh->VertexPosition(two)));
    mblog("Three = "+VectorToQStringLine(newMesh->VertexPosition(three)));
    mblog(QString::number(zero)+" "+QString::number(one)+" "+QString::number(two)+" "+QString::number(three)+"\n");
    
    mb::Vector vPos0 = newMesh->VertexPosition(zero);
    mb::Vector vPos1 = newMesh->VertexPosition(one);
    mb::Vector vPos2 = newMesh->VertexPosition(two);
    mb::Vector vPos3 = newMesh->VertexPosition(three);

    mb::Vector midPos1_2 = 0.5f*(vPos1 + vPos2);
    mb::Vector midPos0_3 = 0.5f*(vPos0 + vPos3);
    mb::Vector midPos0_1 = 0.5f*(vPos0 + vPos1);
    mb::Vector midPos2_3 = 0.5f*(vPos2 + vPos3);

    mb::Vector quartPos1_2 = 0.5f*(vPos1 + midPos1_2);
    mb::Vector quartPos0_3 = 0.5f*(vPos0 + midPos0_3);
    mb::Vector quartPos0_1 = 0.5f*(vPos0 + midPos0_1);
    mb::Vector quartPos2_3 = 0.5f*(vPos3 + midPos2_3);
    


    
    mb::Vector threequartPos1_2 = 0.5f*(vPos2 + midPos1_2);
    mb::Vector threequartPos0_3 = 0.5f*(vPos3 + midPos0_3);
    mb::Vector threequartPos0_1 = 0.5f*(vPos1 + midPos0_1);
    mb::Vector threequartPos2_3 = 0.5f*(vPos2 + midPos2_3);

    
    
    mb::Vector midquartUp = 0.5f*(quartPos0_3+quartPos1_2);
    mb::Vector midThreeQuartUp = 0.5f*(threequartPos0_3+threequartPos1_2);

    mb::Vector midquartRight = 0.5f*(quartPos0_1+quartPos2_3);
    mb::Vector midThreeQuartRight = 0.5f*(threequartPos0_1+threequartPos2_3);

    mb::Vector midPos = 0.5f*(vPos0 + vPos2);

    mblog("Pos1 = "+VectorToQStringLine(midPos1_2));
    mblog("Pos1 = "+VectorToQStringLine(midPos0_3));
    mblog("Vert count: "+QString::number(newMesh->VertexCount())+"\n");
    mblog("Face count: "+QString::number(newMesh->FaceCount())+"\n");
    mblog("TC count: "+QString::number(newMesh->TCCount())+"\n");
    
    /* New Quads
        3______p4___2
         |    |    |
       p2|____|p5__|p1
         |    |    |
        0|____|p3__|1
    */
    newMesh->SetVertexPosition(oldVertCount,midquartUp);
    newMesh->SetVertexPosition(oldVertCount+1,midThreeQuartUp);
    newMesh->SetVertexPosition(oldVertCount+2,midThreeQuartRight);
    newMesh->SetVertexPosition(oldVertCount+3,midquartRight);
/*

    newMesh->SetVertexPosition(oldVertCount+1,midquartUp);
    newMesh->SetVertexPosition(oldVertCount+2,midThreeQuartUp);
    newMesh->SetVertexPosition(oldVertCount+3,midThreeQuartRight);
    newMesh->SetVertexPosition(oldVertCount,midquartRight);

    
    newMesh->SetVertexPosition(oldVertCount+2,midquartUp);
    newMesh->SetVertexPosition(oldVertCount+3,midThreeQuartUp);
    newMesh->SetVertexPosition(oldVertCount,midThreeQuartRight);
    newMesh->SetVertexPosition(oldVertCount+1,midquartRight);

    
    newMesh->SetVertexPosition(oldVertCount+3,midquartUp);
    newMesh->SetVertexPosition(oldVertCount,midThreeQuartUp);
    newMesh->SetVertexPosition(oldVertCount+1,midThreeQuartRight);
    newMesh->SetVertexPosition(oldVertCount+2,midquartRight);*/

    newMesh->SetQuadIndex(fi, 0, zero);
	  newMesh->SetQuadIndex(fi, 1, one);
    newMesh->SetQuadIndex(fi, 2, oldVertCount+1);
	  newMesh->SetQuadIndex(fi, 3, oldVertCount);
  
    newMesh->SetQuadIndex(oldFaceCount, 0, one);
	  newMesh->SetQuadIndex(oldFaceCount, 1, two);
    newMesh->SetQuadIndex(oldFaceCount, 2, oldVertCount+2);
	  newMesh->SetQuadIndex(oldFaceCount, 3, oldVertCount+1);  
  
    newMesh->SetQuadIndex(oldFaceCount+1, 0, oldVertCount+2);
	  newMesh->SetQuadIndex(oldFaceCount+1, 1, two);
    newMesh->SetQuadIndex(oldFaceCount+1, 2, three);
    newMesh->SetQuadIndex(oldFaceCount+1, 3, oldVertCount+3);
    
    newMesh->SetQuadIndex(oldFaceCount+2, 0, zero);
	  newMesh->SetQuadIndex(oldFaceCount+2, 1, oldVertCount);
    newMesh->SetQuadIndex(oldFaceCount+2, 2, oldVertCount+3);
    newMesh->SetQuadIndex(oldFaceCount+2, 3, three);

    newMesh->SetQuadIndex(oldFaceCount+3, 0, oldVertCount);
	  newMesh->SetQuadIndex(oldFaceCount+3, 1, oldVertCount+1);
    newMesh->SetQuadIndex(oldFaceCount+3, 2, oldVertCount+2);
    newMesh->SetQuadIndex(oldFaceCount+3, 3, oldVertCount+3);


    newMesh->SetFaceSelected(oldFaceCount,true);


    
    mb::Vector oldNormal = newMesh->FaceNormal(fi);
    newMesh->SetVertexNormal(oldVertCount,oldNormal);
    newMesh->SetVertexNormal(oldVertCount+1,oldNormal);
    newMesh->SetVertexNormal(oldVertCount+2,oldNormal);
    newMesh->SetVertexNormal(oldVertCount+3,oldNormal);

    mb::TC zeroTC = newMesh->QuadVertexTC(fi,0);
    mb::TC oneTC = newMesh->QuadVertexTC(fi,1);
    mb::TC twoTC = newMesh->QuadVertexTC(fi,2);
    mb::TC threeTC = newMesh->QuadVertexTC(fi,3);
    int zeroTCI = newMesh->QuadTCI(fi,0);
    int oneTCI = newMesh->QuadTCI(fi,1);
    int twoTCI = newMesh->QuadTCI(fi,2);
    int threeTCI = newMesh->QuadTCI(fi,3);
    mblog("TCI: "+QString::number(zeroTCI)+" "+QString::number(oneTCI)+" "+QString::number(twoTCI)+" "+QString::number(threeTCI)+"\n");
        

    mb::TC newTC1 = (oneTC+twoTC)*0.25f;
    mb::TC newTC2 = (zeroTC+threeTC)*0.25f;
    mb::TC newTC3 = (zeroTC+oneTC)*0.25f;
    mb::TC newTC4 = (twoTC+threeTC)*0.25f;
    //mb::TC newTC5 = (newTC3+newTC4)*0.5f;

    newMesh->m_pTCs[oldTCCount] = newTC1;
    newMesh->m_pTCs[oldTCCount+1] = newTC2;
    newMesh->m_pTCs[oldTCCount+2] = newTC3;
    newMesh->m_pTCs[oldTCCount+3] = newTC4;

    mblog("NewTC1 = "+QString::number(newTC1.u)+" "+QString::number(newTC1.v)+"\n");
    mblog("NewTC2 = "+QString::number(newTC2.u)+" "+QString::number(newTC2.v)+"\n");
    newMesh->SetVertexTC(oldTCCount,newTC1);
    newMesh->SetVertexTC(oldTCCount+1,newTC2);
    newMesh->SetVertexTC(oldTCCount+2,newTC3);
    newMesh->SetVertexTC(oldTCCount+3,newTC4);
  
    newMesh->SetQuadTCI(fi,0,zeroTCI);
    newMesh->SetQuadTCI(fi,1,oldTCCount+2);
    newMesh->SetQuadTCI(fi,2,oldTCCount+3);
    newMesh->SetQuadTCI(fi,3,oldTCCount+1);
  
    newMesh->SetQuadTCI(oldFaceCount,0,oldTCCount+2);
    newMesh->SetQuadTCI(oldFaceCount,1,oneTCI);
    newMesh->SetQuadTCI(oldFaceCount,2,oldTCCount);
    newMesh->SetQuadTCI(oldFaceCount,3,oldTCCount+3);
    
    newMesh->SetQuadTCI(oldFaceCount+1,0,oldTCCount+3);
    newMesh->SetQuadTCI(oldFaceCount+1,1,oldTCCount);
    newMesh->SetQuadTCI(oldFaceCount+1,2,twoTCI);
    newMesh->SetQuadTCI(oldFaceCount+1,3,oldTCCount+3);
    
    newMesh->SetQuadTCI(oldFaceCount+2,0,oldTCCount+1);
    newMesh->SetQuadTCI(oldFaceCount+2,1,oldTCCount+2);
    newMesh->SetQuadTCI(oldFaceCount+2,2,oldTCCount+3);
    newMesh->SetQuadTCI(oldFaceCount+2,3,threeTCI);

    newMesh->SetQuadTCI(oldFaceCount+3,0,oldTCCount);
    newMesh->SetQuadTCI(oldFaceCount+3,1,oldTCCount+1);
    newMesh->SetQuadTCI(oldFaceCount+3,2,oldTCCount+2);
    newMesh->SetQuadTCI(oldFaceCount+3,3,oldTCCount+3);
  }
  mblog("Recalculating normals\n");
	newMesh->RecalculateNormals();
  mblog("Recalced normals\n");
  newMesh->RecalculateAdjacency();
  newMesh->RecalculateVertexAdjacency();
  mblog("Nope\n");
  //newMesh->RecalculateTopologicalSymmetry();
  mblog("Recalced Topological symmetry\n");
	//(newMesh->HasTC() == false);
  mblog("Vert count: "+QString::number(newMesh->VertexCount())+"\n");
  mblog("Face count: "+QString::number(newMesh->FaceCount())+"\n");
  mblog("TC count: "+QString::number(newMesh->TCCount())+"\n");
  newMesh->SetName("Subdived_level");

  mb::SubdivisionLevel *newSubDiv = dynamic_cast<mb::SubdivisionLevel*>(newMesh);
  newSubDiv->RecreateUVs(true);
  MeshGeo->AddLevel(newSubDiv);
  MeshGeo->ChangeActiveLevel(newSubDiv);
  MeshGeo->ContentChanged();
  newSubDiv->ApplyChanges();
  while(MeshGeo->LevelCount() > 1) {
    MeshGeo->RemoveLowestLevel();
  }
  SetMesh(newMesh);
  mb::Kernel()->ViewPort()->Redraw();
  mb::Kernel()->Interface()->RefreshUI();
}

//
//void MeshOps::Tesselate() {
//  mb::Mesh *newMesh = mb::Kernel()->Scene()->CreateMesh(mb::Topology::typeQuadric);
//  newMesh = dynamic_cast<mb::Mesh*>(pMesh->Duplicate());
//  mblog("Tesselate\n");
//  for(int i = 0 ; i < tessInfo_L->size() ; i ++ ) {
//    int fi =tessInfo_L->at(i).fi;
//
//    mblog("fi = "+QString::number(tessInfo_L->at(i).fi)+"\n");
//    unsigned int zero = newMesh->QuadPrimaryIndex(false,fi,0);
//    unsigned int one = newMesh->QuadPrimaryIndex(false,fi,1);
//    unsigned int two = newMesh->QuadPrimaryIndex(false,fi,2);
//    unsigned int three = newMesh->QuadPrimaryIndex(false,fi,3);
//  
//    mblog("Zero = "+VectorToQStringLine(newMesh->VertexPosition(zero)));
//    mblog("One = "+VectorToQStringLine(newMesh->VertexPosition(one)));
//    mblog("Two = "+VectorToQStringLine(newMesh->VertexPosition(two)));
//    mblog("Three = "+VectorToQStringLine(newMesh->VertexPosition(three)));
//    mblog(QString::number(zero)+" "+QString::number(one)+" "+QString::number(two)+" "+QString::number(three)+"\n");
//    
//    mb::Vector vPos0 = newMesh->VertexPosition(zero);
//    mb::Vector vPos1 = newMesh->VertexPosition(one);
//    mb::Vector vPos2 = newMesh->VertexPosition(two);
//    mb::Vector vPos3 = newMesh->VertexPosition(three);
//
//    mb::Vector midPos1_2 = 0.5f*(vPos1 + vPos2);
//    mb::Vector midPos0_3 = 0.5f*(vPos0 + vPos3);
//    mb::Vector midPos0_1 = 0.5f*(vPos0 + vPos1);
//    mb::Vector midPos2_3 = 0.5f*(vPos2 + vPos3);
//
//    mb::Vector quartPos1_2 = 0.5f*(vPos1 + midPos1_2);
//    mb::Vector quartPos0_3 = 0.5f*(vPos0 + midPos0_3);
//    mb::Vector quartPos0_1 = 0.5f*(vPos0 + midPos0_1);
//    mb::Vector quartPos2_3 = 0.5f*(vPos2 + midPos2_3);
//    
//
//
//    
//    mb::Vector threequartPos1_2 = 0.5f*(vPos2 + midPos1_2);
//    mb::Vector threequartPos0_3 = 0.5f*(vPos3 + midPos0_3);
//    mb::Vector threequartPos0_1 = 0.5f*(vPos1 + midPos0_1);
//    mb::Vector threequartPos2_3 = 0.5f*(vPos3 + midPos2_3);
//
//    
//    
//    mb::Vector midquartUp = 0.5f*(quartPos0_3+quartPos1_2);
//    mb::Vector midThreeQuartUp = 0.5f*(threequartPos0_3+threequartPos1_2);
//
//    mb::Vector midquartRight = 0.5f*(quartPos0_1+quartPos2_3);
//    mb::Vector midThreeQuartRight = 0.5f*(threequartPos0_1+threequartPos2_3);
//
//    mb::Vector midPos = 0.5f*(vPos0 + vPos2);
//
//    mblog("Pos1 = "+VectorToQStringLine(midPos1_2));
//    mblog("Pos1 = "+VectorToQStringLine(midPos0_3));
//    int oldVertCount = newMesh->VertexCount();
//    int oldFaceCount = newMesh->FaceCount();
//    int oldTCCount = newMesh->TCCount();
//    mblog("Vert count: "+QString::number(newMesh->VertexCount())+"\n");
//    mblog("Face count: "+QString::number(newMesh->FaceCount())+"\n");
//    mblog("TC count: "+QString::number(newMesh->TCCount())+"\n");
//    newMesh->SetTCCount(oldTCCount+5);
//    newMesh->SetVertexCount(oldVertCount+5);
//    newMesh->SetFaceCount(oldFaceCount+3);
//    mblog("Setting vertex count\n");
//    mblog("Set vertex count\n");
//    
//    /* New Quads
//        3______p4___2
//         |    |    |
//       p2|____|p5__|p1
//         |    |    |
//        0|____|p3__|1
//    */
//    newMesh->SetVertexPosition(oldVertCount,quartPos1_2);
//    newMesh->SetVertexPosition(oldVertCount+1,quartPos0_3);
//    newMesh->SetVertexPosition(oldVertCount+2,quartPos0_1);
//    newMesh->SetVertexPosition(oldVertCount+3,quartPos2_3);
//    newMesh->SetVertexPosition(oldVertCount+4,midPos);
//
//    newMesh->SetQuadIndex(fi, 0, zero);
//	  newMesh->SetQuadIndex(fi, 1, oldVertCount+2);
//    newMesh->SetQuadIndex(fi, 2, oldVertCount+4);
//	  newMesh->SetQuadIndex(fi, 3, oldVertCount+1);
//  
//    newMesh->SetQuadIndex(oldFaceCount, 0, oldVertCount+2);
//	  newMesh->SetQuadIndex(oldFaceCount, 1, one);
//    newMesh->SetQuadIndex(oldFaceCount, 2, oldVertCount);
//	  newMesh->SetQuadIndex(oldFaceCount, 3, oldVertCount+4);  
//  
//    newMesh->SetQuadIndex(oldFaceCount+1, 0, oldVertCount+4);
//	  newMesh->SetQuadIndex(oldFaceCount+1, 1, oldVertCount);
//    newMesh->SetQuadIndex(oldFaceCount+1, 2, two);
//    newMesh->SetQuadIndex(oldFaceCount+1, 3, oldVertCount+3);
//
//    newMesh->SetQuadIndex(oldFaceCount+2, 0, oldVertCount+1);
//	  newMesh->SetQuadIndex(oldFaceCount+2, 1, oldVertCount+4);
//    newMesh->SetQuadIndex(oldFaceCount+2, 2, oldVertCount+3);
//    newMesh->SetQuadIndex(oldFaceCount+2, 3, three);
//
//
//
//    
//    mb::Vector oldNormal = newMesh->FaceNormal(fi);
//    newMesh->SetVertexNormal(oldVertCount,oldNormal);
//    newMesh->SetVertexNormal(oldVertCount+1,oldNormal);
//    newMesh->SetVertexNormal(oldVertCount+2,oldNormal);
//    newMesh->SetVertexNormal(oldVertCount+3,oldNormal);
//    newMesh->SetVertexNormal(oldVertCount+4,oldNormal);
//
//    mb::TC zeroTC = newMesh->QuadVertexTC(fi,0);
//    mb::TC oneTC = newMesh->QuadVertexTC(fi,1);
//    mb::TC twoTC = newMesh->QuadVertexTC(fi,2);
//    mb::TC threeTC = newMesh->QuadVertexTC(fi,3);
//    int zeroTCI = newMesh->QuadTCI(fi,0);
//    int oneTCI = newMesh->QuadTCI(fi,1);
//    int twoTCI = newMesh->QuadTCI(fi,2);
//    int threeTCI = newMesh->QuadTCI(fi,3);
//    mblog("TCI: "+QString::number(zeroTCI)+" "+QString::number(oneTCI)+" "+QString::number(twoTCI)+" "+QString::number(threeTCI)+"\n");
//        
//
//    mb::TC newTC1 = (oneTC+twoTC)*0.25f;
//    mb::TC newTC2 = (zeroTC+threeTC)*0.25f;
//    mb::TC newTC3 = (zeroTC+oneTC)*0.25f;
//    mb::TC newTC4 = (twoTC+threeTC)*0.25f;
//    mb::TC newTC5 = (newTC3+newTC4)*0.5f;
//
//    newMesh->m_pTCs[oldTCCount] = newTC1;
//    newMesh->m_pTCs[oldTCCount+1] = newTC2;
//    newMesh->m_pTCs[oldTCCount+2] = newTC3;
//    newMesh->m_pTCs[oldTCCount+3] = newTC4;
//    newMesh->m_pTCs[oldTCCount+4] = newTC5;
//    mblog("NewTC1 = "+QString::number(newTC1.u)+" "+QString::number(newTC1.v)+"\n");
//    mblog("NewTC2 = "+QString::number(newTC2.u)+" "+QString::number(newTC2.v)+"\n");
//    newMesh->SetVertexTC(oldTCCount,newTC1);
//    newMesh->SetVertexTC(oldTCCount+1,newTC2);
//    newMesh->SetVertexTC(oldTCCount+2,newTC3);
//    newMesh->SetVertexTC(oldTCCount+3,newTC4);
//    newMesh->SetVertexTC(oldTCCount+4,newTC5);
//  
//    newMesh->SetQuadTCI(fi,0,zeroTCI);
//    newMesh->SetQuadTCI(fi,1,oldTCCount+2);
//    newMesh->SetQuadTCI(fi,2,oldTCCount+4);
//    newMesh->SetQuadTCI(fi,3,oldTCCount+1);
//  
//    newMesh->SetQuadTCI(oldFaceCount,0,oldTCCount+2);
//    newMesh->SetQuadTCI(oldFaceCount,1,oneTCI);
//    newMesh->SetQuadTCI(oldFaceCount,2,oldTCCount);
//    newMesh->SetQuadTCI(oldFaceCount,3,oldTCCount+4);
//    
//    newMesh->SetQuadTCI(oldFaceCount+1,0,oldTCCount+4);
//    newMesh->SetQuadTCI(oldFaceCount+1,1,oldTCCount);
//    newMesh->SetQuadTCI(oldFaceCount+1,2,twoTCI);
//    newMesh->SetQuadTCI(oldFaceCount+1,3,oldTCCount+3);
//
//    newMesh->SetQuadTCI(oldFaceCount+2,0,oldTCCount+1);
//    newMesh->SetQuadTCI(oldFaceCount+2,1,oldTCCount+4);
//    newMesh->SetQuadTCI(oldFaceCount+2,2,oldTCCount+3);
//    newMesh->SetQuadTCI(oldFaceCount+2,3,threeTCI);
//  }
//  mblog("Recalculating normals\n");
//	newMesh->RecalculateNormals();
//  mblog("Recalced normals\n");
//  newMesh->RecalculateAdjacency();
//  newMesh->RecalculateVertexAdjacency();
//  mblog("Nope\n");
//  //newMesh->RecalculateTopologicalSymmetry();
//  mblog("Recalced Topological symmetry\n");
//	//(newMesh->HasTC() == false);
//  mblog("Vert count: "+QString::number(newMesh->VertexCount())+"\n");
//  mblog("Face count: "+QString::number(newMesh->FaceCount())+"\n");
//  mblog("TC count: "+QString::number(newMesh->TCCount())+"\n");
//  newMesh->SetName("Subdived_level");
//
//  mb::SubdivisionLevel *newSubDiv = dynamic_cast<mb::SubdivisionLevel*>(newMesh);
//  newSubDiv->RecreateUVs(true);
//  MeshGeo->AddLevel(newSubDiv);
//  MeshGeo->ChangeActiveLevel(newSubDiv);
//  MeshGeo->ContentChanged();
//  newSubDiv->ApplyChanges();
//  while(MeshGeo->LevelCount() > 1) {
//    MeshGeo->RemoveLowestLevel();
//  }
//  SetMesh(newMesh);
//  mb::Kernel()->ViewPort()->Redraw();
//  mb::Kernel()->Interface()->RefreshUI();
//}

//void MeshOps::Tesselate() {
//  mb::Mesh *newMesh = mb::Kernel()->Scene()->CreateMesh(mb::Topology::typeQuadric);
//  newMesh = dynamic_cast<mb::Mesh*>(pMesh->Duplicate());
//  mblog("Tesselate\n");
//  for(int i = 0 ; i < tessInfo_L->size() ; i ++ ) {
//    int fi =tessInfo_L->at(i).fi;
//
//    mblog("fi = "+QString::number(tessInfo_L->at(i).fi)+"\n");
//    unsigned int zero = newMesh->QuadPrimaryIndex(false,fi,0);
//    unsigned int one = newMesh->QuadPrimaryIndex(false,fi,1);
//    unsigned int two = newMesh->QuadPrimaryIndex(false,fi,2);
//    unsigned int three = newMesh->QuadPrimaryIndex(false,fi,3);
//  
//    mblog("Zero = "+VectorToQStringLine(newMesh->VertexPosition(zero)));
//    mblog("One = "+VectorToQStringLine(newMesh->VertexPosition(one)));
//    mblog("Two = "+VectorToQStringLine(newMesh->VertexPosition(two)));
//    mblog("Three = "+VectorToQStringLine(newMesh->VertexPosition(three)));
//    mblog(QString::number(zero)+" "+QString::number(one)+" "+QString::number(two)+" "+QString::number(three)+"\n");
//    
//    mb::Vector quater0_1 = 0.25f*(newMesh->VertexPosition(one)-newMesh->VertexPosition(zero));
//    mb::Vector threeQuater0_1= 0.75f*(newMesh->VertexPosition(one)-newMesh->VertexPosition(zero));
//    mb::Vector quater1_2 = 0.25f*(newMesh->VertexPosition(one) + newMesh->VertexPosition(two));
//    mb::Vector threeQuater1_2= 0.75f*(newMesh->VertexPosition(one) + newMesh->VertexPosition(two));
//    mb::Vector quater2_3 = 0.25f*(newMesh->VertexPosition(two) + newMesh->VertexPosition(three));
//    mb::Vector threeQuater2_3= 0.75f*(newMesh->VertexPosition(two) + newMesh->VertexPosition(three));
//    mb::Vector quater0_3 = 0.25f*(newMesh->VertexPosition(three)-newMesh->VertexPosition(zero));
//    mb::Vector threeQuater0_3= 0.75f*(newMesh->VertexPosition(three)-newMesh->VertexPosition(zero));
//
//
//    //mb::Vector pos1 = 0.5f*(newMesh->VertexPosition(one) + newMesh->VertexPosition(two));
//    //mb::Vector pos2 = 0.5f*(newMesh->VertexPosition(zero) + newMesh->VertexPosition(three));
//    //mb::Vector pos3 = 0.5f*(newMesh->VertexPosition(zero) + newMesh->VertexPosition(one));
//    //mb::Vector pos4 = 0.5f*(newMesh->VertexPosition(two) + newMesh->VertexPosition(three));
//    mb::Vector pos4 = (quater0_1+quater0_3)+newMesh->VertexPosition(zero);
//    mb::Vector pos5 = (threeQuater0_1+quater0_3)+newMesh->VertexPosition(zero);
//    mb::Vector pos6 = (threeQuater0_1+threeQuater0_3)+newMesh->VertexPosition(zero);
//    mb::Vector pos7 = (quater0_1+threeQuater0_3)+newMesh->VertexPosition(zero);
//    mblog("Pos1 = "+VectorToQStringLine(pos4));
//    mblog("Pos1 = "+VectorToQStringLine(pos5));
//    int oldVertCount = newMesh->VertexCount();
//    int oldFaceCount = newMesh->FaceCount();
//    int oldTCCount = newMesh->TCCount();
//    mblog("Vert count: "+QString::number(newMesh->VertexCount())+"\n");
//    mblog("Face count: "+QString::number(newMesh->FaceCount())+"\n");
//    mblog("TC count: "+QString::number(newMesh->TCCount())+"\n");
//    newMesh->SetTCCount(oldTCCount+4);
//    newMesh->SetVertexCount(oldVertCount+4);
//    newMesh->SetFaceCount(oldFaceCount+4);
//    mblog("Setting vertex count\n");
//    mblog("Set vertex count\n");
//    
//    /* New Quads
//        3______p4___2
//         |    |    |
//       p2|____|p5__|p1
//         |    |    |
//        0|____|p3__|1
//    */
//    newMesh->SetVertexPosition(oldVertCount,pos4);
//    newMesh->SetVertexPosition(oldVertCount+1,pos5);
//    newMesh->SetVertexPosition(oldVertCount+2,pos6);
//    newMesh->SetVertexPosition(oldVertCount+3,pos7);
//
//    newMesh->SetQuadIndex(fi, 0, zero);
//	  newMesh->SetQuadIndex(fi, 1, one);
//    newMesh->SetQuadIndex(fi, 2, oldVertCount+1);
//	  newMesh->SetQuadIndex(fi, 3, oldVertCount);
//  
//    newMesh->SetQuadIndex(oldFaceCount, 0, oldVertCount+1);
//	  newMesh->SetQuadIndex(oldFaceCount, 1, one);
//    newMesh->SetQuadIndex(oldFaceCount, 2, two);
//	  newMesh->SetQuadIndex(oldFaceCount, 3, oldVertCount+2);  
//  
//    newMesh->SetQuadIndex(oldFaceCount+1, 0, oldVertCount+3);
//	  newMesh->SetQuadIndex(oldFaceCount+1, 1, oldVertCount+2);
//    newMesh->SetQuadIndex(oldFaceCount+1, 2, two);
//    newMesh->SetQuadIndex(oldFaceCount+1, 3, three);
//
//    newMesh->SetQuadIndex(oldFaceCount+2, 0, zero);
//	  newMesh->SetQuadIndex(oldFaceCount+2, 1, oldVertCount);
//    newMesh->SetQuadIndex(oldFaceCount+2, 2, oldVertCount+3);
//    newMesh->SetQuadIndex(oldFaceCount+2, 3, three);
//    
//   // newMesh->SetQuadIndex(oldFaceCount+3, 0, oldVertCount);
//	  //newMesh->SetQuadIndex(oldFaceCount+3, 1, oldVertCount+1);
//   // newMesh->SetQuadIndex(oldFaceCount+3, 2, oldVertCount+2);
//   // newMesh->SetQuadIndex(oldFaceCount+3, 3, oldVertCount+3);
//
//    newMesh->SetQuadIndex(oldFaceCount+3, 0, zero);
//	  newMesh->SetQuadIndex(oldFaceCount+3, 1, one);
//    newMesh->SetQuadIndex(oldFaceCount+3, 2, two);
//    newMesh->SetQuadIndex(oldFaceCount+3, 3, three);
//
//
//
//    
//    mb::Vector oldNormal = newMesh->FaceNormal(fi);
//    newMesh->SetVertexNormal(oldVertCount,oldNormal);
//    newMesh->SetVertexNormal(oldVertCount+1,oldNormal);
//    newMesh->SetVertexNormal(oldVertCount+2,oldNormal);
//    newMesh->SetVertexNormal(oldVertCount+3,oldNormal);
//
//    mb::TC zeroTC = newMesh->QuadVertexTC(fi,0);
//    mb::TC oneTC = newMesh->QuadVertexTC(fi,1);
//    mb::TC twoTC = newMesh->QuadVertexTC(fi,2);
//    mb::TC threeTC = newMesh->QuadVertexTC(fi,3);
//    int zeroTCI = newMesh->QuadTCI(fi,0);
//    int oneTCI = newMesh->QuadTCI(fi,1);
//    int twoTCI = newMesh->QuadTCI(fi,2);
//    int threeTCI = newMesh->QuadTCI(fi,3);
//    mblog("TCI: "+QString::number(zeroTCI)+" "+QString::number(oneTCI)+" "+QString::number(twoTCI)+" "+QString::number(threeTCI)+"\n");
//        
//
//
//    
//    
//    mb::TC tcq0_1 = ((oneTC-zeroTC)*0.25f);
//    mb::TC tctq0_1 = ((oneTC-zeroTC)*0.75f);
//    mb::TC tcq0_3 = ((threeTC-zeroTC)*0.25f);
//    mb::TC tctq0_3 = ((threeTC-zeroTC)*0.75f);
//
//
//    mb::TC newTC1 = (tcq0_1+tcq0_3)+zeroTC;
//    mb::TC newTC2 = (tctq0_1+tcq0_3)+zeroTC;
//    mb::TC newTC3 = (tctq0_1+tctq0_3)+zeroTC;
//    mb::TC newTC4 = (twoTC+tctq0_3)+zeroTC;
//
//    newMesh->m_pTCs[oldTCCount] = newTC1;
//    newMesh->m_pTCs[oldTCCount+1] = newTC2;
//    newMesh->m_pTCs[oldTCCount+2] = newTC3;
//    newMesh->m_pTCs[oldTCCount+3] = newTC4;
//    mblog("NewTC1 = "+QString::number(newTC1.u)+" "+QString::number(newTC1.v)+"\n");
//    mblog("NewTC2 = "+QString::number(newTC2.u)+" "+QString::number(newTC2.v)+"\n");
//    newMesh->SetVertexTC(oldTCCount,newTC1);
//    newMesh->SetVertexTC(oldTCCount+1,newTC2);
//    newMesh->SetVertexTC(oldTCCount+2,newTC3);
//    newMesh->SetVertexTC(oldTCCount+3,newTC4);
//  
//
//    
//    newMesh->SetQuadIndex(fi, 0, zero);
//	  newMesh->SetQuadIndex(fi, 1, one);
//    newMesh->SetQuadIndex(fi, 2, oldVertCount+1);
//	  newMesh->SetQuadIndex(fi, 3, oldVertCount);
//  
//    newMesh->SetQuadIndex(oldFaceCount, 0, oldVertCount+1);
//	  newMesh->SetQuadIndex(oldFaceCount, 1, one);
//    newMesh->SetQuadIndex(oldFaceCount, 2, two);
//	  newMesh->SetQuadIndex(oldFaceCount, 3, oldVertCount+2);  
//  
//    newMesh->SetQuadIndex(oldFaceCount+1, 0, oldVertCount+3);
//	  newMesh->SetQuadIndex(oldFaceCount+1, 1, oldVertCount+2);
//    newMesh->SetQuadIndex(oldFaceCount+1, 2, two);
//    newMesh->SetQuadIndex(oldFaceCount+1, 3, three);
//
//    newMesh->SetQuadIndex(oldFaceCount+2, 0, zero);
//	  newMesh->SetQuadIndex(oldFaceCount+2, 1, oldVertCount);
//    newMesh->SetQuadIndex(oldFaceCount+2, 2, oldVertCount+3);
//    newMesh->SetQuadIndex(oldFaceCount+2, 3, three);
//    
//   // newMesh->SetQuadIndex(oldFaceCount+3, 0, oldVertCount);
//	  //newMesh->SetQuadIndex(oldFaceCount+3, 1, oldVertCount+1);
//   // newMesh->SetQuadIndex(oldFaceCount+3, 2, oldVertCount+2);
//   // newMesh->SetQuadIndex(oldFaceCount+3, 3, oldVertCount+3);
//
//    newMesh->SetQuadIndex(oldFaceCount+3, 0, zero);
//	  newMesh->SetQuadIndex(oldFaceCount+3, 1, one);
//    newMesh->SetQuadIndex(oldFaceCount+3, 2, two);
//    newMesh->SetQuadIndex(oldFaceCount+3, 3, three);
//
//
//    newMesh->SetQuadTCI(fi,0,zeroTCI);
//    newMesh->SetQuadTCI(fi,1,oneTCI);
//    newMesh->SetQuadTCI(fi,2,oldTCCount+1);
//    newMesh->SetQuadTCI(fi,3,oldTCCount);
//  
//    newMesh->SetQuadTCI(oldFaceCount,0,oldTCCount+1);
//    newMesh->SetQuadTCI(oldFaceCount,1,oneTCI);
//    newMesh->SetQuadTCI(oldFaceCount,2,twoTCI);
//    newMesh->SetQuadTCI(oldFaceCount,3,oldTCCount+2);
//    
//    newMesh->SetQuadTCI(oldFaceCount+1,0,oldTCCount+3);
//    newMesh->SetQuadTCI(oldFaceCount+1,1,oldTCCount+2);
//    newMesh->SetQuadTCI(oldFaceCount+1,2,twoTCI);
//    newMesh->SetQuadTCI(oldFaceCount+1,3,threeTCI);
//    
//    newMesh->SetQuadTCI(oldFaceCount+2,0,zeroTCI);
//    newMesh->SetQuadTCI(oldFaceCount+2,1,oldTCCount);
//    newMesh->SetQuadTCI(oldFaceCount+2,2,oldTCCount+3);
//    newMesh->SetQuadTCI(oldFaceCount+2,3,threeTCI);
//    
//    newMesh->SetQuadTCI(oldFaceCount+3,0,zeroTCI);
//    newMesh->SetQuadTCI(oldFaceCount+3,1,oneTCI);
//    newMesh->SetQuadTCI(oldFaceCount+3,2,twoTCI);
//    newMesh->SetQuadTCI(oldFaceCount+3,3,threeTCI);
///*
//    newMesh->SetQuadTCI(oldFaceCount+3,0,oldTCCount);
//    newMesh->SetQuadTCI(oldFaceCount+3,1,oldTCCount+1);
//    newMesh->SetQuadTCI(oldFaceCount+3,2,oldTCCount+2);
//    newMesh->SetQuadTCI(oldFaceCount+3,3,oldTCCount+3);*/
//  }
//  mblog("Recalculating normals\n");
//	newMesh->RecalculateNormals();
//  mblog("Recalced normals\n");
//  newMesh->RecalculateAdjacency();
//  newMesh->RecalculateVertexAdjacency();
//  mblog("Nope\n");
//  //newMesh->RecalculateTopologicalSymmetry();
//  mblog("Recalced Topological symmetry\n");
//	//(newMesh->HasTC() == false);
//  mblog("Vert count: "+QString::number(newMesh->VertexCount())+"\n");
//  mblog("Face count: "+QString::number(newMesh->FaceCount())+"\n");
//  mblog("TC count: "+QString::number(newMesh->TCCount())+"\n");
//  newMesh->SetName("Subdived_level");
//
//  mb::SubdivisionLevel *newSubDiv = dynamic_cast<mb::SubdivisionLevel*>(newMesh);
//  newSubDiv->RecreateUVs(true);
//  MeshGeo->AddLevel(newSubDiv);
//  MeshGeo->ChangeActiveLevel(newSubDiv);
//  MeshGeo->ContentChanged();
//  newSubDiv->ApplyChanges();
//  while(MeshGeo->LevelCount() > 1) {
//    MeshGeo->RemoveLowestLevel();
//  }
//  SetMesh(newMesh);
//  mb::Kernel()->ViewPort()->Redraw();
//  mb::Kernel()->Interface()->RefreshUI();
//}

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

  t->restart();
  mb::SubdivisionLevel *subdiv = MeshGeo->ActiveLevel();
  subdiv->RecreateUVs(true);
  subdiv->ApplyChanges();
  mblog("Apply Changes Time: "+QString::number(t->elapsed())+"\n");
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
    SetMesh(sp.Mesh());
    //Tesselate();
    mblog(pMesh->Name()+" "+QString::number(sp.FaceIndex()));

  } else {
    mbstatus("No ObjectSelected");
  }
}

void MeshOps::SelectObjectFromHands() {
  mb::SurfacePoint sp;
  int midW = mb::Kernel()->ViewPort()->Width()*0.5;
  int midH = mb::Kernel()->ViewPort()->Height()*0.5;
  ssp = curCam->GetScreenSpacePicker();

  if(curCam->Pick(midW,midH,sp)) {
    mblog("\n Hit MeshOps\n");
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
    mblog(pMesh->Name()+" "+QString::number(sp.FaceIndex()));

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

//bool MeshOps::SelectFaces(LR lr, mb::AxisAlignedBoundingBox box,float spreadDist) {
//  if(pMesh != NULL) {
//    mb::Vector start = box.m_vStart;
//    mb::Vector end = box.m_vEnd;
//    mb::Picker *p = new mb::Picker();
//    std::vector<int> *faces;
//    std::vector<VertexModifyInfo> *vertices;
//    std::vector<mb::SurfacePoint > *points;
//    if(lr == l) {
//      faces = faces_L;
//      vertices = vertices_L;
//      points =  points_L;
//    } else {
//      faces = faces_R;
//      vertices = vertices_R;
//      points =  points_R;
//    }
//    faces->clear();
//    vertices->clear();
//    points->clear();
//    pMesh->RemoveLayer(meshLayer);
//    meshLayer = pMesh->AddLayer();
//    //p->SetMesh(pMesh);
//    //p->SetTolerance(spreadDist);
//    int fi;
//    mb::SurfacePoint sp;
//    mb::VertexAdjacency vA;
//    mblog("Start: "+VectorToQString(start)+" End: "+VectorToQStringLine(end));
//    if(p->Pick(start,end,true,sp,0.5f)) {
//      mblog("sp face =: "+QString::number(sp.FaceIndex())+"\n");
//      fi = sp.FaceIndex();
//      //if(checkUniqueInFaceList(fi)) {
//      points->push_back(sp);
//      faces->push_back(fi);
//      addVertex(lr,fi);
//      //}
//      //if(spreadDist != 0) {
//      //  vA = pMesh->VertexAdjacency(vertices->at(0).vI);
//      //  int aFI = vA.FaceIndex();
//      //  faces->push_back(aFI);
//      //  addVertex(aFI);
//
//    }
//    if(faces->size() > 0) {
//      for(int i = 0 ; i < faces->size() ; i++) {
//        mb::Kernel()->Log(QString::number(i)+" "+QString::number(faces->at(i))+"\n");
//        pMesh->SetFaceSelected(faces->at(i));
//      }
//      //MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
//      //MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
//      return true;
//    }
//  } else {
//    mblog("Error in: Select Faces, -Box- pMesh == NULL");
//  }
//  return false;
//}

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
    boxSelect(lr,vS,vE,widthHeight,dropOffRate);
    if(vertices->size() > 0) {
      mbstatus(QString("Polygon SELECTED"+QString::number(points->size())));
      mblog("box selected");
      for(int i = 0 ; i < faces->size() ; i++) {
        mb::Kernel()->Log("Face: "+QString::number(i)+" "+QString::number(faces->at(i))+"\n");
        pMesh->SetFaceSelected(faces->at(i));
      }
      if(firstUse) {
        /*MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
        MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());*/
        StoreUndoQueue(lr);
      } else {
        AddToUndoQueue(lr);
      }
      if(linearDropoff) {
        for(int i = 0 ; i < vertices->size() ; i++) {
          float dist = midV->pos.DistanceFrom(pMesh->VertexPosition(vertices->at(i).vI));
          
          mb::Kernel()->Log("Vertex: "+QString::number(i)+" "+QString::number(dist)+"\n");
          vertices->at(i).strength = MIN(1-(dist/widthHeight),1); //restrict to 1 strength
          if(vertices->at(i).strength < 0.0001)
            vertices->at(i).strength = 0;
        }
      }
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
    mb::Vector vS = mb::Vector(midW-size,midH-size,0);
    mb::Vector vE = mb::Vector(midW + size, midH + size, 0);
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
      //for(int i = 0 ; i < faces->size() ; i++) {
      //  mb::Kernel()->Log(QString::number(i)+" "+QString::number(faces->at(i))+"\n");
      //  pMesh->SetFaceSelected(faces->at(i));
      //}
      //refreshMesh();
      StoreUndoQueue(lr);
      //MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      //MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());
      return true;
    } else {
      mblog("No faces\n");
    }
  } else {
    SelectObjectFromHands();
  }
  return false;
}

bool MeshOps::ToolManip(mb::Vector centrePoint, float widthHeight, Leap_Tool *tool) {
  if(pMesh != NULL) {
    //mb::Vector vS = centrePoint - mb::Vector(widthHeight,widthHeight,0);
    //mb::Vector vE = centrePoint + mb::Vector(widthHeight,widthHeight,0);
    faces_R->clear();
    vertices_R->clear();
    points_R->clear();
    midV->pos = mb::Vector(0,0,0);
    midV->vI = 0;
    mblog("SelectingBox\n");
    QTime *t = new QTime();
    t->start();
    boxSelect(r,centrePoint,tool);
    mblog("Box Select Total Time : "+QString::number(t->elapsed())+"\n");
    if(faces_R->size() > 0) {
      mbstatus(QString("Polygon SELECTED"+QString::number(points_R->size())));
      mblog("box selected\n");
      //for(int i = 0 ; i < faces_R->size() ; i++) {
      //  //mblog(QString::number(i)+" "+QString::number(faces_R->at(i))+"\n");
      //  pMesh->SetFaceSelected(faces_R->at(i));
      //}
      /*MeshGeo->ChangeActiveLevel(MeshGeo->LowestLevel());
      MeshGeo->ChangeActiveLevel(MeshGeo->HighestLevel());*/

      t->restart();
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


void MeshOps::SubDivide() {
  if(pMesh != NULL) {
    QTime *t = new QTime();
    t->start();
    mblog("Getting subdiv\n");
    mb::SubdivisionLevel *subdiv = dynamic_cast<mb::SubdivisionLevel*>(pMesh);
    mblog("Subdividing\n");
    subdiv->Subdivide();
    mblog("Setting Active Level\n");
    MeshGeo->SetActiveLevel(MeshGeo->HighestLevel());
    mblog("Subdivided in "+QString::number(t->elapsed())+"\n");
    SetMesh(dynamic_cast<mb::Mesh*>(MeshGeo->HighestLevel()));
    pMesh->RecalculateAdjacency();
    pMesh->RecalculateNormals();
    pMesh->RecalculateVertexAdjacency();
  } else {
    mbhud("No Mesh Selected\n");
  }
}

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
  //int vertexCountInRange = 0;
  VertexModifyInfo vMI;
  float dist;
  mb::Base basePlane;
  mb::Vector bpVector;
  mb::VertexAdjacency vA;
  if(curCam->Pick(x,y,p)) {
    mblog("Selected :"+p.Mesh()->Name()+" Mesh \n");
    if(p.Mesh()->ID() == pMesh->ID()) {
      basePlane = p.TangentBase();
      faceIndices.push_back(p.FaceIndex());
      faces->push_back(p.FaceIndex());
      mblog("facesPushBack\n");
      midPos = p.WorldPosition();
      midV->pos = midPos;
      QTime *t = new QTime();
      t->start();
#pragma omp parallel for private(bpVector,vMI,dist,vA,fi) firstprivate(faces,vertices,pMesh,meshLayer,basePlane,lr,maxDist)
      for( unsigned int i = 0 ; i < pMesh->VertexCount() ; i++) {
        
        //mblog("MidPos= "+VectorToQStringLine(midPos));
        dist = pMesh->VertexPosition(i).DistanceFrom(midPos);
        //mblog("Dist = "+QString::number(dist)+"\n");
        if(dist < maxDist) {
          if(checkUniqueInVertexList(vertices,i)) {
            bpVector = basePlane.TransformTo(pMesh->VertexPosition(i));
            /*mblog("Vertex position = "+VectorToQStringLine(pMesh->VertexPosition(i)));
            mblog("Vertex Base Plane Position = "+VectorToQStringLine(bpVector));*/
            //vertexCountInRange++;
            vMI.vI = (int)i;
            //if(meshLayer == NULL) {
            //    mblog("Meshlayer Null\n");
            //} else { 
            //  mblog("int i = "+QString::number(i)+"\n");
            //}
            vMI.lVI = meshLayer->LayerVertexIndex(i);
              //mblog("int lvi = "+QString::number(vMI.lVI)+"\n");
            vMI.strength = MIN((1-(dist/maxDist)),1);
            vertices->push_back(vMI);
            mblog("here\n");
            vA = pMesh->VertexAdjacency(i);
            mblog("here2\n");
            fi = vA.FaceIndex();
            faces->push_back(fi);
            if(checkUniqueInFaceList(faces,fi)) {
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


void MeshOps::boxSelect(LR lr, mb::Vector centrePoint, Leap_Tool *tool) {
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
  int x = centrePoint.x;
  int y = centrePoint.y;
  int height = 20;
  QTime *t1 = new QTime();
  t1->start();
  mb::ScreenSpacePicker *ssp = mb::Kernel()->Scene()->ActiveCamera()->GetScreenSpacePicker();
  //ssp->SetFramebufferSize(mb::Kernel()->ViewPort()->Width(),mb::Kernel()->ViewPort()->Height());
  mblog("screenSpacePicker Init Time: "+QString::number(t1->elapsed())+"\n");
  mblog("StampHeight = "+QString::number(tool->GetStamp()->Height()));
  mb::SurfacePoint p;
  VertexModifyInfo vMI;
  float dist;
  mb::Base basePlane;
  mb::Vector uvSpace;
  mb::VertexAdjacency vA;
  //http://geomalgorithms.com/a04-_planes.html
  mb::Vector worldPoint;
  t1->restart();
  if(ssp->Pick(curCam,x,y,p,false)) {
  //if(curCam->Pick(x,y,p)) {
    mblog("curCamPick Time: "+QString::number(t1->elapsed())+"\n");
    if(p.Mesh()->ID() == pMesh->ID()) {
      //pMesh->RecalculateAdjacency();
      basePlane = p.TangentBase();
      faces->push_back(p.FaceIndex());
      midPos = p.WorldPosition();
      mblog("mid pos = "+VectorToQStringLine(midPos));
      mblog("Last mid pos = "+VectorToQStringLine(lastMidPosition));
      if(midPos.DistanceFrom(lastMidPosition) > 50) {
        QTime *t = new QTime();
        t->start();
        lastMidPosition = midPos;
        //#pragma omp parallel for private(bpVector,vMI,dist,vA,fi,uvSpace) firstprivate(faces,vertices,basePlane,midPos,worldPoint,tool,pMesh,meshLayer,basePlane,lr,height)
        for( unsigned int i = 0 ; i < pMesh->VertexCount() ; i++) {
          worldPoint = pMesh->VertexPosition(i);
          dist = worldPoint.DistanceFrom(midPos);
          if(dist < height) {
            //mblog("Vertex in range\n");
            if(checkUniqueInVertexList(vertices,i)) {
              //mblog("MID POINT COORDINATES = "+VectorToQStringLine(midPos));
              //mblog("WORLD POINT COORDINATES = "+VectorToQStringLine(worldPoint));
              //mblog("NORMAL = "+VectorToQStringLine(p.WorldNormal().Normalized()));
              uvSpace = findDisplacementUV(basePlane,midPos,worldPoint);
              //mblog("UVSPACE COORDINATES = "+VectorToQStringLine(uvSpace));
              //mblog("\n");
              vMI.vI = (int)i;
              vMI.lVI = meshLayer->LayerVertexIndex((int)i);
              vMI.strength = tool->GetStampStrength(uvSpace);
              //mblog("\n");
              vertices->push_back(vMI);
              vA = pMesh->VertexAdjacency(i);
              fi = vA.FaceIndex();
              //mblog("Face index = "+QString::number(fi)+"\n");
              if(checkUniqueInFaceList(faces,fi)) {
                faces->push_back(fi);
              }
            }
          }
        }
        mblog("Box Select VerticesLoop: "+QString::number(t->elapsed())+"\n");
      }
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
    if(strength > 0) {
      strength *=dist;
      v = pMesh->VertexNormal(vi).Normalize();
    //mblog("Vertex Normal = "+VectorToQStringLine(v));
    //mblog("Moving Vertex "+QString::number(i)+"by "+VectorToQStringLine(v*dist));
    //mb::Kernel()->Log(QString::number(vi)+ " " + QString::number(v.x)+"\n");
    
    //mblog("Moving Vertex "+QString::number(i)+" "+QString::number(lvi)+"by "+VectorToQStringLine(v*strength));
      meshLayer->SetVertexDelta(lvi,vi,v*strength,true);
    }
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
  meshStoreToggle->push_back(false);
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
    //mblog("Getting last vertex info list");
    std::vector<VertexInfo> vertInfoList = undoQueue.back();
    bool newVert = true;
    for(int i = 0 ; i < vertices->size(); i++) {
      newVert = true;
      //if(pMesh != NULL) {
      //  mblog("getting vertexAdjacency\n");
      //  vA = &pMesh->VertexAdjacency(vertices->at(i));
      //}
      //mblog("vertex "+ QString::number(i)+"\n");
      for(int j = 0 ; j < vertInfoList.size() ; j++) {
        if(vertices->at(i).vI == vertInfoList.at(j).vI) {
          newVert = false;
          break;
        }
      }
      if(newVert) {
        vertInfo.vI = vertices->at(i).vI;
        atleastOneNewVert = true;
        //mblog("new vertex "+ QString::number(i)+"\n");
        vertInfo.pos = pMesh->VertexPosition(vertInfo.vI);
        vertInfoList.push_back(vertInfo);
      }
      //mblog("Storing: "+QString::number(vertInfo.vI)+" "+VectorToQStringLine(vertInfo.pos));
    }
    if(atleastOneNewVert) {
      //undoQueue.pop_back();
      undoQueue.push_back(vertInfoList);
    }
  } else {
    StoreUndoQueue(lr);
  }
  //mblog("Stored Undo\n");
}

void MeshOps::UndoLast() {
  if(pMesh != NULL) {
    //if(meshStoreToggle->size() > 0) {
    //  meshStoreToggle->pop_back();
      //if(meshStoreToggle->size() > 0) {
      //  meshStoreToggle->pop_back();
      //  if(meshStoreToggle->back()) {
      //    mblog("getting pmesh\n");
      //    mb::Mesh *newMesh = meshStore->back();
      //    mblog("Vert count: "+QString::number(newMesh->VertexCount())+"\n");
      //    mblog("Face count: "+QString::number(newMesh->FaceCount())+"\n");
      //    mblog("TC count: "+QString::number(newMesh->TCCount())+"\n");
      //    mblog("Recalculating normals\n");
      //    newMesh->RecalculateNormals();
      //    mblog("Recalced normals\n");
      //    newMesh->RecalculateAdjacency();
      //    newMesh->RecalculateVertexAdjacency();
      //    mb::SubdivisionLevel *newSubDiv = dynamic_cast<mb::SubdivisionLevel*>(newMesh);
      //    newSubDiv->RecreateUVs(true);
      //    MeshGeo->AddLevel(newSubDiv);
      //    MeshGeo->ChangeActiveLevel(newSubDiv);
      //    MeshGeo->ContentChanged();
      //    newSubDiv->ApplyChanges();
    //      while(MeshGeo->LevelCount() > 1) {
    //        MeshGeo->RemoveLowestLevel();
    //      }
    //      SetMesh(newMesh);
    //      mb::Kernel()->ViewPort()->Redraw();
    //      mb::Kernel()->Interface()->RefreshUI();
    //      meshStore->pop_back();
    //    }
    //  }
    //}
    if(undoQueue.size() > 0) {
      std::vector<VertexInfo> vertInfoList = undoQueue.back();
      undoQueue.pop_back();
      for(int i = 0 ; i < vertInfoList.size() ; i++) {
        pMesh->SetVertexPosition(vertInfoList.at(i).vI,vertInfoList.at(i).pos);
        //mC->Add(vertInfoList.at(i).vI,vertInfoList.at(i).fI,false);
        //mblog("Retrieving: "+QString::number(vertInfoList.at(i).vI)+" "+VectorToQStringLine(vertInfoList.at(i).pos));
      }
      pMesh->RecalculateNormals();
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
      if(checkUniqueInVertexList(vertices,v)) {
        vMI.vI = v;
        vMI.lVI = meshLayer->LayerVertexIndex(v);
        vMI.strength = 1;
        vertices->push_back(vMI);
      }
    }
  } else if(pMesh->Type() == mb::Mesh::typeTriangular) {
    for(int i = 0 ; i < 3 ; i++) {
      v = pMesh->TrianglePrimaryIndex(false,fi,i);
      if(checkUniqueInVertexList(vertices,v)) {
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

bool MeshOps::checkUniqueInFaceList(std::vector<int> *faces, int fi) {
  for(int i = 0 ; i < faces->size() ; i++)
    if(fi == faces->at(i))
      return false;//not unique
  return true;//unique
}

bool MeshOps::checkUniqueInVertexList(std::vector<VertexModifyInfo> *vertices, int vi) {
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
