#include "stdafx.h"
#include "GestureHUD.h"

IMPLEMENT_VCLASS( GestureHUD, ViewPortFilter, "GestureHUD", 2 );



GestureHUD::~GestureHUD(void) {
}


void GestureHUD::OnNodeEvent( const mb::Attribute &cAttribute, mb::NodeEventType eType ) {
	if(eType == mb::etValueChanged) {
			mb::Kernel()->Redraw();
	}
}

GestureHUD::GestureHUD():
  menuChoice(this,"menuOptions")
{
  timeout = new QTime();
  timeout->start();
  menuChoice.AddItem("NULL");
  menuChoice.AddItem("grab");
  menuChoice.AddItem("Rotate");
  menuChoice.AddItem("Pan");
  menuChoice.AddItem("Tool");
  menuChoice.AddItem("Select");
  menuChoice.AddItem("Undo");
  currentMenuChoice = 0;
  menuChoice.SetValue(0);
  refresh = false;

  grabGesture = mb::CreateInstance<mb::Texture>();
  grabGesture->CreateFromFile(RESOURCESDIR + QString("grabGesture.png"));
  grabGesture->SetLocation( mb::TexturePool::locationGPU );
  
  rotateGesture = mb::CreateInstance<mb::Texture>();
  rotateGesture->CreateFromFile(RESOURCESDIR + QString("rotateGesture.png"));
  rotateGesture->SetLocation( mb::TexturePool::locationGPU );

  panGesture = mb::CreateInstance<mb::Texture>();
  panGesture->CreateFromFile(RESOURCESDIR + QString("panGesture.png"));
  panGesture->SetLocation( mb::TexturePool::locationGPU );
  
  toolGesture = mb::CreateInstance<mb::Texture>();
  toolGesture->CreateFromFile(RESOURCESDIR + QString("toolGesture.png"));
  toolGesture->SetLocation( mb::TexturePool::locationGPU );

  selectGesture = mb::CreateInstance<mb::Texture>();
  selectGesture->CreateFromFile(RESOURCESDIR + QString("selectGesture.png"));
  selectGesture->SetLocation( mb::TexturePool::locationGPU );
  
  undoGesture = mb::CreateInstance<mb::Texture>();
  undoGesture->CreateFromFile(RESOURCESDIR + QString("undoGesture.png"));
  undoGesture->SetLocation( mb::TexturePool::locationGPU );
}

void GestureHUD::SetVisible( bool bVisible )
{
  if(bVisible != ViewPortFilter::Visible()) {
	  ViewPortFilter::SetVisible( bVisible );
  }
}


static void drawFullScreenQuad(mb::Texture* const outputTexture,mb::Texture* const overlay, const float ratio, const float cX, const float cY) {
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();     
  glColor3f(1.0f,1.0f,1.0f);
  if(overlay != NULL)
    glBindTexture(GL_TEXTURE_2D,overlay->OpenGLName());

	glBegin( GL_QUADS );
  
	glTexCoord2f( 0, 0 );
	glVertex2f( cX-0.2f, (cY-0.2f)*ratio );
  
	glTexCoord2f( 1, 0 );
	glVertex2f( cX+0.2f, (cY-0.2f)*ratio );
  
	glTexCoord2f( 1, 1 );
	glVertex2f( cX+0.2f, (cY+0.2f)*ratio );
  
	glTexCoord2f( 0, 1 );
	glVertex2f( cX-0.2f, (cY+0.2f)*ratio );

	glEnd();
}

// Renders full screens quad to texture specified
static void gpuTransform(mb::Texture* const outputTexture,mb::Texture* const overlay, const float ratio, const float cX, const float cY) {
	outputTexture->SetAsRenderTarget();
  drawFullScreenQuad(outputTexture,overlay,ratio,cX,cY);
	outputTexture->RestoreRenderTarget();
}

void GestureHUD::Process(mb::ViewPortState &s) {
  //enum mb::Image::Format eFormat = s.m_bHDRNeeded ? mb::Image::e16float : mb::Image::e8integer;

  mb::Texture *currTex = NULL;

  if(menuChoice != currentMenuChoice || refresh) {
    timeout->restart();
    currentMenuChoice = menuChoice;
    refresh = false;
  }
  if(timeout->elapsed() < 100) {
    switch(menuChoice) {
      case 1:
        currTex = grabGesture;
        break;
      case 2:
        currTex = rotateGesture;
        break;
      case 3:
        currTex = panGesture;
        break;
      case 4:
        currTex = toolGesture;
        break;
      case 5:
        currTex = selectGesture;
        break;
      case 6:
        currTex = undoGesture;
        break;
      default:
        currTex = NULL;
        break;
    }
    if(currTex == NULL) {
      mblog("NULL TEXTURE!");
    } else {
      const float cameraAspectRatio = mb::Kernel()->Scene()->ActiveCamera()->AspectRatio();
        // Set up some gl states
	    glDisable(GL_DEPTH_TEST);
	    glDisable(GL_CULL_FACE);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

      gpuTransform(s.m_pColor,currTex,cameraAspectRatio,-0.7,-0.3);
      s.m_pColor = s.m_pColor;
    }
  } else {
	  ViewPortFilter::SetVisible( false);
  }
}