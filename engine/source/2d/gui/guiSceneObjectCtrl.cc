//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "graphics/dgl.h"
#include "console/consoleTypes.h"
#include "gui/guiDefaultControlRender.h"
#include "guiSceneObjectCtrl.h"
#include "debug/profiler.h"

// -----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiSceneObjectCtrl);

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
GuiSceneObjectCtrl::GuiSceneObjectCtrl(void)
{
   // Reset Scene Object Name.
   mSceneObjectName = StringTable->EmptyString;

   // Default to no render margin
   mMargin = 0;

   mCaption = StringTable->EmptyString;

   //mStateOn = false;
   //mButtonType = ButtonTypeRadio;

   // Set-up private batcher.
   mBatchRenderer.setDebugStats( &mDebugStats );
   mBatchRenderer.setStrictOrderMode( true, true );
   mBatchRenderer.setBatchEnabled( false );
}

// -----------------------------------------------------------------------------
// Persistent Fields.
// -----------------------------------------------------------------------------
void GuiSceneObjectCtrl::initPersistFields()
{
   // Call Parent.
   Parent::initPersistFields();

   // Define Fields.
   addGroup("GuiSceneObjectCtrl");
   addField("renderMargin", TypeS32, Offset(mMargin, GuiSceneObjectCtrl));
   addField("sceneObject", TypeString, Offset(mSceneObjectName, GuiSceneObjectCtrl));
   endGroup("GuiSceneObjectCtrl");
}


// -----------------------------------------------------------------------------
// Wake!
// -----------------------------------------------------------------------------
bool GuiSceneObjectCtrl::onWake()
{
   // Call Parent.
   if (!Parent::onWake())
      return false;

   if( mProfile->constructBitmapArray() >= 36 )
      mHasTexture = true;
   else
      mHasTexture = false;

   // Activate.
   setActive(true);

   // All Okay.
   return true;
}


// -----------------------------------------------------------------------------
// Sleep!
// -----------------------------------------------------------------------------
void GuiSceneObjectCtrl::onSleep()
{
   Parent::onSleep();
}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void GuiSceneObjectCtrl::inspectPostApply()
{
   // Call Parent.
   Parent::inspectPostApply();

   // Set the Scene Object.
   setSceneObject( mSceneObjectName );
}


// -----------------------------------------------------------------------------
// Set Scene Object.
// -----------------------------------------------------------------------------
ConsoleMethod( GuiSceneObjectCtrl, setSceneObject, void, 3, 3, "(string obj) Set the scene-object displayed in the control."
              "@param obj Either the object's ID or its name."
              "@return No return value.")
{
   // Set Scene Object.
   object->setSceneObject( argv[2] );
}
// Set Scene Object.
void GuiSceneObjectCtrl::setSceneObject( const char* name )
{
   // Reset existing object.
   mSelectedSceneObject = NULL;

   // Get Scene Name.
   mSceneObjectName = StringTable->insert( name ? name : "" );

   // Valid Scene Object Name?
   if ( *mSceneObjectName )
   {
      // Fetch Scene Object.
      SceneObject* pSceneObject = dynamic_cast<SceneObject*>(Sim::findObject( name ));
      // Valid?
      if ( pSceneObject )
      {
        // Yes, so set Scene Object.
        mSelectedSceneObject = pSceneObject;

        // The scene object needs to be integrated otherwise the render OOBB which this GUI control
        // relies upon will be undefined.  This dependency needs resolving but I suspect it never will.
        DebugStats debugStats;
        mSelectedSceneObject->preIntegrate(0.0f, 0.0f, &debugStats );
      }
   }

   else
      mSelectedSceneObject = NULL;

   // Do an update.
   setUpdate();
}

ConsoleMethod( GuiSceneObjectCtrl, getSceneObject, const char*, 2,2, "() \n@return Returns displaying sceneobject id")
{
   SceneObject *sceneObject = object->getSceneObject();
   if( !sceneObject )
      return "";

   return sceneObject->getIdString();
}



ConsoleMethod( GuiSceneObjectCtrl, setCaption, void, 3, 3, "(string caption) Sets the object caption to specified string.\n"
              "@return No return value.")
{
   object->setCaption( argv[2] );
}

void GuiSceneObjectCtrl::setCaption( const char* caption )
{
   if( caption != NULL )
      mCaption = StringTable->insert( caption );
}

//
//
//
void GuiSceneObjectCtrl::onMouseUp(const GuiEvent &event)
{
   if( mDepressed && ( event.mouseClickCount % 2 ) == 0 )
         Con::executef( this, 2, "onDoubleClick" );    

   Parent::onTouchUp( event );
}

void GuiSceneObjectCtrl::onMouseLeave( const GuiEvent &event )
{
   Con::executef( this, 2, "onMouseLeave" );

   Parent::onTouchLeave( event );
}

void GuiSceneObjectCtrl::onMouseEnter( const GuiEvent &event )
{
   Con::executef( this, 2, "onMouseEnter" );

   Parent::onTouchEnter( event );
}

void GuiSceneObjectCtrl::onMouseDragged( const GuiEvent &event )
{
   Con::executef( this, 2, "onMouseDragged" );

   Parent::onTouchDragged( event );
}

// -----------------------------------------------------------------------------
// Render any selected Scene Object.
// -----------------------------------------------------------------------------
void GuiSceneObjectCtrl::onRender(Point2I offset, const RectI& updateRect)
{
    PROFILE_SCOPE(guiT2DObjectCtrl_onRender);

   RectI ctrlRect( offset, mBounds.extent );

   // Draw Background
      if( mDepressed )
      {
         if( mHasTexture )
            renderSizableBitmapBordersFilled( ctrlRect, 3, mProfile );
         else
            dglDrawRectFill( ctrlRect, mProfile->mFillColorHL );
      }
      else if ( mMouseOver )
      {
         if( mHasTexture )
            renderSizableBitmapBordersFilled( ctrlRect, 2, mProfile );
         else
            dglDrawRectFill( ctrlRect, mProfile->mFillColorHL );
      }
      else
      {
         if( mHasTexture )
            renderSizableBitmapBordersFilled( ctrlRect, 1, mProfile );
         else
            dglDrawRectFill( ctrlRect, mProfile->mFillColor );
      }
   

   //// Render Border.
   //if( mProfile->mBorder || mStateOn )    
   //   dglDrawRect(ctrlRect, mProfile->mBorderColor);


   // Valid Scene Object?
   if ( !mSelectedSceneObject.isNull() )
   {
      RectI objRect = updateRect;
      objRect.inset( mMargin, mMargin );
      RectI ctrlRectInset = ctrlRect;
      ctrlRectInset.inset( mMargin, mMargin); 

      // Draw Canvas color for object
	if( mHasTexture )
		renderSizableBitmapBordersFilled( objRect, 4, mProfile );
	else
		dglDrawRectFill( objRect, mProfile->mFillColorNA );

      // Yes, so fetch object clip boundary.
      const b2Vec2* pClipBoundary = mSelectedSceneObject->getRenderOOBB();

      // Calculate the GUI-Control Clip Boundary.
      const F32 xscale = (pClipBoundary[1].x - pClipBoundary[0].x) / ctrlRectInset.extent.x;
      const F32 yscale = (pClipBoundary[2].y - pClipBoundary[0].y) / ctrlRectInset.extent.y;

      F32 x1 = pClipBoundary[0].x + ( objRect.point.x - ctrlRectInset.point.x) * xscale;
      F32 x2 = pClipBoundary[1].x + ( objRect.point.x + objRect.extent.x - ctrlRectInset.extent.x - ctrlRectInset.point.x) * xscale;
      F32 y1 = pClipBoundary[0].y + ( objRect.point.y - ctrlRectInset.point.y) * yscale;
      F32 y2 = pClipBoundary[2].y + ( objRect.point.y + objRect.extent.y - ctrlRectInset.extent.y - ctrlRectInset.point.y) * yscale;



      Vector2 size = mSelectedSceneObject->getSize();
      if (size.x > size.y)
      {
          S32 center = ctrlRectInset.point.y + (ctrlRectInset.extent.y / 2);

          // Save the base  state of the objRect before transforming it
          S32 baseTop = objRect.point.y;
          S32 baseExtent = objRect.extent.y;
          S32 baseBottom = baseTop + baseExtent;

          // Transform the objRect extent by the aspect ratio
          objRect.extent.y = (S32)(ctrlRectInset.extent.y * (size.y / size.x));

          // Save the transformed positions of the objRect before clipping
          S32 targetTop = (S32)(center - ((ctrlRectInset.extent.y * (size.y / size.x)) / 2));
          S32 targetExtent = (S32)(objRect.extent.y);
          S32 targetBottom = (S32)(targetTop + objRect.extent.y);

          // Set the objRect position based on its aspect ratio
          objRect.point.y = (S32)(center - ((ctrlRectInset.extent.y * (size.y / size.x)) / 2));

          // Reset the clipping bounds
          y1 = pClipBoundary[0].y;
          y2 = pClipBoundary[2].y;

          // If the object clips of the top or bottom, adjust the extent and clipping bounds accordingly
          if (baseTop > targetTop)
          {
              // Adjust the position and extent
              objRect.point.y = baseTop;
              objRect.extent.y = getMax(targetBottom - baseTop, 0);
              
              // Set the top clipping boundary
              F32 clipRatio = 1.0f - ((1.0f * objRect.extent.y)/targetExtent);
              y1 = pClipBoundary[0].y + (pClipBoundary[2].y - pClipBoundary[0].y)*(clipRatio);
             
          }
          if (baseBottom < targetBottom)
          {
              // Adjust the extent
              objRect.extent.y = getMax(baseBottom - targetTop, 0);

              // Set the bottom clipping boundary
              F32 clipRatio = 1.0f - ((1.0f * objRect.extent.y)/targetExtent);
              y2 = pClipBoundary[2].y - (pClipBoundary[2].y - pClipBoundary[0].y)*(clipRatio);
          }
      }
      else
      {
          S32 center = ctrlRectInset.point.x + (ctrlRectInset.extent.x / 2);

          // Save the base  state of the objRect before transforming it
          S32 baseLeft = objRect.point.x;
          S32 baseExtent = objRect.extent.x;
          S32 baseRight = baseLeft + baseExtent;

          // Transform the objRect extent by the aspect ratio
          objRect.extent.x = (S32)(ctrlRectInset.extent.x * (size.x / size.y));

          // Save the transformed positions of the objRect before clipping
          S32 targetLeft = (S32)(center - ((ctrlRectInset.extent.x * (size.x / size.y)) / 2));
          S32 targetExtent = (S32)(objRect.extent.x);
          S32 targetRight = (S32)(targetLeft + objRect.extent.x);

          // Set the objRect position based on its aspect ratio
          objRect.point.x = (S32)(center - ((ctrlRectInset.extent.x * (size.x / size.y)) / 2));

          // Reset the clipping bounds
          x1 = pClipBoundary[0].x;
          x2 = pClipBoundary[1].x;

          // If the object clips of the left or right, adjust the extent and clipping bounds accordingly
          if (baseLeft > targetLeft)
          {
              // Adjust the position and extent
              objRect.point.x = baseLeft;
              objRect.extent.x = getMax(targetRight - baseLeft, 0);
              
              // Set the left clipping boundary
              F32 clipRatio = 1.0f - ((1.0f * objRect.extent.x)/targetExtent);
              x1 = pClipBoundary[0].x + (pClipBoundary[1].x - pClipBoundary[0].x)*(clipRatio);
             
          }
          if (baseRight < targetRight)
          {
              // Adjust the extent
              objRect.extent.x = getMax(baseRight - targetLeft, 0);

              // Set the right clipping boundary
              F32 clipRatio = 1.0f - ((1.0f * objRect.extent.x)/targetExtent);
              x2 = pClipBoundary[1].x - (pClipBoundary[1].x - pClipBoundary[0].x)*(clipRatio);
          }
      }

      // Negate the y-clippingpoints to convert to positive-y-up coordinate system of the object
      y1 *= -1;
      y2 *= -1;

      // Setup new logical coordinate system.
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      RectI viewport;
      dglGetViewport(&viewport);

      if (x1 > x2)
      {
         F32 temp = x1;
         x1 = x2;
         x2 = temp;
      }
      if (y1 > y2)
      {
         F32 temp = y1;
         y1 = y2;
         y2 = temp;
      }

      // Setup Orthographic Projection for Object Area.
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID)
      glOrthof( x1, x2, y1, y2, 0.0f, MAX_LAYERS_SUPPORTED );
#else
      glOrtho( x1, x2, y1, y2, 0.0f, MAX_LAYERS_SUPPORTED );
#endif
      // Setup new viewport.
      dglSetViewport(objRect);

      // Set ModelView.
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();

      // Enable Alpha Test.
      glEnable        ( GL_ALPHA_TEST );
      glAlphaFunc     ( GL_GREATER, 0.0f );

      // Calculate maximal clip bounds.
      RectF clipBounds( -x1,-y1, x2-x1, y2-y1 );

      DebugStats debugStats;

      // Render Object in GUI-space.
      SceneRenderState guiSceneRenderState(
          clipBounds,
          clipBounds.centre(),
          0.0f,
          MASK_ALL,
          MASK_ALL,
          Vector2::getOne(),
          &debugStats,
          this );

      SceneRenderRequest guiSceneRenderRequest;
      guiSceneRenderRequest.set(
          mSelectedSceneObject,
          mSelectedSceneObject->getRenderPosition(),
          0.0f );

      mSelectedSceneObject->sceneRender( &guiSceneRenderState, &guiSceneRenderRequest, &mBatchRenderer );

      // Restore Standard Settings.
      glDisable       ( GL_DEPTH_TEST );
      glDisable       ( GL_ALPHA_TEST );

      // Restore Matrices.
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
   }
   else
   {
      // No Object so reset name.
      mSceneObjectName = NULL;
   }


   RectI captionRect = ctrlRect;
   captionRect.point.y += (captionRect.extent.y / 8);


   captionRect.inset(1, 1);
   dglSetBitmapModulation( ColorI(0,0,0,255) );
   renderText(captionRect.point, captionRect.extent, mCaption, mProfile);
   captionRect.inset(1, 1);   
   dglSetBitmapModulation( mProfile->mFontColor );
   renderText(captionRect.point, captionRect.extent, mCaption, mProfile);

   

   // Render Child Controls.
   renderChildControls(offset, mBounds, updateRect);
}
