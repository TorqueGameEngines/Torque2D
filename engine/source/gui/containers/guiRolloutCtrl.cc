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

#include "gui/containers/guiRolloutCtrl.h"
#include "gui/containers/guiScrollCtrl.h"

//////////////////////////////////////////////////////////////////////////
// GuiRolloutCtrl
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiRolloutCtrl);

GuiRolloutCtrl::GuiRolloutCtrl()
{
   mBounds.set(0,0,200,20);
   mExpanded.set(0,0,200,60);
   mCaption             = StringTable->EmptyString;
   mIsExpanded          = true;
   mIsAnimating         = false;
   mCollapsing          = false;
   mAnimateDestHeight   = 40;
   mAnimateStep         = 1;
   mCanSave             = false;
   mDefaultHeight       = 40;
   mMargin.set( 2,2 );
   mIsContainer = true;

   mCanCollapse = true;
   // Make sure we receive our ticks.
   setProcessTicks();
}

GuiRolloutCtrl::~GuiRolloutCtrl()
{
}

//////////////////////////////////////////////////////////////////////////
// Persistence 
//////////////////////////////////////////////////////////////////////////
void GuiRolloutCtrl::initPersistFields()
{
   Parent::initPersistFields();

   //addField("Caption", TypeCaseString, Offset(mCaption, GuiRolloutCtrl));
   //addField("Margin", TypePoint2I, Offset(mMargin, GuiRolloutCtrl));
   //addField("DefaultHeight", TypeS32, Offset(mDefaultHeight, GuiRolloutCtrl));
   //addProtectedField( "Collapsed", TypeBool, Offset( mIsExpanded, GuiRolloutCtrl), &setCollapsed, &defaultProtectedGetFn, "" );
   //addField("ClickCollapse", TypeBool, Offset(mCanCollapse, GuiRolloutCtrl));
}

//////////////////////////////////////////////////////////////////////////
// Scene Events
//////////////////////////////////////////////////////////////////////////
bool GuiRolloutCtrl::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   mHasTexture = ( mProfile ? mProfile->constructBitmapArray() : false );
   if( mHasTexture )
      mBitmapBounds = mProfile->mBitmapArrayRects.address();

   // Calculate Heights for this control
   calculateHeights();

   return true;
}

bool GuiRolloutCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   if( !mIsAnimating && mIsExpanded )
      sizeToContents();

   return true;
}

void GuiRolloutCtrl::addObject( SimObject *obj )
{
   // Call Parent.
   Parent::addObject( obj );

   sizeToContents();
}

void GuiRolloutCtrl::removeObject( SimObject *obj )
{
   // Call Parent.
   Parent::removeObject( obj );

   // Recalculate our rectangles.
   calculateHeights();
}

//////////////////////////////////////////////////////////////////////////
// Mouse Events
//////////////////////////////////////////////////////////////////////////
void GuiRolloutCtrl::onMouseDown( const GuiEvent &event )
{
   mouseLock();
}

void GuiRolloutCtrl::onMouseUp( const GuiEvent &event )
{
   Point2I localPoint = globalToLocalCoord( event.mousePoint );
   if( mCanCollapse && mHeader.pointInRect( localPoint ) && !mIsAnimating && isMouseLocked() )
   {
      if( !mIsExpanded )
         animateTo( mExpanded.extent.y );
      else
         animateTo( mHeader.extent.y );      
   }

   if( isMouseLocked() )
      mouseUnlock();
}

//////////////////////////////////////////////////////////////////////////
// Control Sizing Helpers
//////////////////////////////////////////////////////////////////////////
void GuiRolloutCtrl::calculateHeights()
{
   S32 barHeight = 20;

   if( mHasTexture && mProfile->mBitmapArrayRects.size() >= NumBitmaps )
   {
      // Store Header Rectangle
      mHeader.set( 0, 0, mBounds.extent.x, mProfile->mBitmapArrayRects[ CollapsedCenter ].extent.y );

      // Bottom Bar Max
      barHeight = mProfile->mBitmapArrayRects[ BottomLeftHeader ].extent.y 
                + mProfile->mBitmapArrayRects[ TopLeftHeader ].extent.y;
   }
   else
   {
      mHeader.set( 0, 0, mBounds.extent.x, barHeight );
   }

   GuiControl *content = dynamic_cast<GuiControl*>( at(0) );
   if( content != NULL )
      mExpanded.set( 0, 0, mBounds.extent.x , barHeight + content->getHeight() + (mMargin.y * 2) );
   else
      mExpanded.set( 0, 0, mBounds.extent.x, barHeight + mDefaultHeight );
}



void GuiRolloutCtrl::resize( const Point2I &newPosition, const Point2I &newExtent )
{
   Parent::resize( newPosition, newExtent );

   // Recalculate Heights and resize ourself appropriately.
   calculateHeights();

   GuiControl *content = dynamic_cast<GuiControl*>( at(0) );
   // Size Content Properly?!
   if( content != NULL )
   {
      mChildRect.set( mMargin.x, mHeader.extent.y + mMargin.y , mBounds.extent.x - (mMargin.x * 2), content->mBounds.extent.y - ( mMargin.y * 2 ) );
      content->resize( mChildRect.point, mChildRect.extent );
   }
}


void GuiRolloutCtrl::sizeToContents()
{
   calculateHeights();

   // Set destination height
   if( size() > 0 )
      instantExpand();
   else
      instantCollapse();
}

void GuiRolloutCtrl::instantExpand()
{
   mAnimateDestHeight = mExpanded.extent.y;
   mCollapsing = false;
   mIsExpanded = true;
   mIsAnimating = false;
   resize( mBounds.point + mExpanded.point, mExpanded.extent );
}

void GuiRolloutCtrl::instantCollapse()
{
   mAnimateDestHeight = mHeader.extent.y;
   mCollapsing = false;
   mIsExpanded = false;
   mIsAnimating = false;
   resize( mBounds.point + mHeader.point, mHeader.extent );
}

void GuiRolloutCtrl::childResized(GuiControl *child)
{
   Parent::childResized( child );

   calculateHeights();
}


//////////////////////////////////////////////////////////////////////////
// Control Sizing Animation Functions
//////////////////////////////////////////////////////////////////////////
void GuiRolloutCtrl::animateTo( S32 height )
{
   // We do nothing if we're already animating
   if( mIsAnimating )
      return;

   bool collapsing = (bool)( mBounds.extent.y > height );

   // If we're already at the destination height, bail
   if( mBounds.extent.y >= height && !collapsing )
   {
      mIsExpanded = true;
      return;
   }

   // If we're already at the destination height, bail
   if( mBounds.extent.y <= height && collapsing )
   {
      mIsExpanded = false;
      return;
   }

   // Set destination height
   mAnimateDestHeight = height;

   // Set Animation Mode
   mCollapsing = collapsing;

   // Set Animation Step (Increment)
   if( collapsing )
      mAnimateStep = (S32)mFloor( (F32)( mBounds.extent.y - height ) / 2.0f );
   else
      mAnimateStep = (S32)mFloor( (F32)( height - mBounds.extent.y ) / 2.0f );

   // Start our animation
   mIsAnimating = true;

}

void GuiRolloutCtrl::processTick()
{
   // We do nothing here if we're NOT animating
   if( !mIsAnimating )
      return;

   // Sanity check to fix non collapsing panels.
   if( mAnimateStep == 0 )
      mAnimateStep = 1;

   // We're collapsing ourself down (Hiding our contents)
   if( mCollapsing )
   {
      if( mBounds.extent.y < mAnimateDestHeight )
         mBounds.extent.y = mAnimateDestHeight;
      else if( ( mBounds.extent.y - mAnimateStep ) < mAnimateDestHeight )
         mBounds.extent.y = mAnimateDestHeight;

      if( mBounds.extent.y == mAnimateDestHeight )
         mIsAnimating = false;
      else
         mBounds.extent.y -= mAnimateStep;

      if( !mIsAnimating )
         mIsExpanded = false;
   }
   else // We're expanding ourself (Showing our contents)
   {
      if( mBounds.extent.y > mAnimateDestHeight )
         mBounds.extent.y = mAnimateDestHeight;
      else if( ( mBounds.extent.y + mAnimateStep ) > mAnimateDestHeight )
         mBounds.extent.y = mAnimateDestHeight;

      if( mBounds.extent.y == mAnimateDestHeight )
         mIsAnimating = false;
      else
         mBounds.extent.y += mAnimateStep;

      if( !mIsAnimating )
         mIsExpanded = true;
   }

   if( !mIsAnimating )
   {
      if( mCollapsing && isMethod("onCollapsed") )
         Con::executef( this, 2, "onCollapsed" );
      else if( !mCollapsing && isMethod("onExpanded") )
         Con::executef( this, 2, "onExpanded" );

      calculateHeights();
   }

   GuiControl* parent = getParent();
   if( parent )
   {
      parent->childResized( this );
      // if our parent's parent is a scroll control, scrollvisible.
      GuiScrollCtrl* scroll = dynamic_cast<GuiScrollCtrl*>(parent->getParent());
      if(scroll)
      {
         scroll->scrollRectVisible(mBounds);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
// Control Rendering
//////////////////////////////////////////////////////////////////////////
void GuiRolloutCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   if( !mProfile || mProfile->mFont.isNull() )
      return;
  
   // Calculate actual world bounds for rendering
   RectI worldBounds( offset, mBounds.extent );

   if( mProfile->mBitmapArrayRects.size() >= NumBitmaps )
   {
      //dglClearBitmapModulation();

      // Draw Rollout From Skin
      if( !mIsExpanded )
         renderFixedBitmapBordersFilled( worldBounds, 1, mProfile );
      else// Draw Border
         renderSizableBitmapBordersFilledIndex( worldBounds, TopLeftHeader, mProfile );

      // Draw Caption ( Vertically Centered )
      ColorI currColor;
      //dglGetBitmapModulation( &currColor );
      Point2I textPosition = mHeader.point + offset + mProfile->mTextOffset;
      //dglSetBitmapModulation( mProfile->mFontColor );
      renderText( textPosition, mHeader.extent, mCaption, mProfile);
      //dglSetBitmapModulation( currColor );

      // If we're collapsed we contain the first child as our content
      // thus we don't render it when collapsed.  but to support modified
      // rollouts with custom header buttons etc we still render our other
      // children. -JDD
      GuiControl *pChild = dynamic_cast<GuiControl*>( at(0) );
      if(pChild)
      {
         if( !mIsExpanded && pChild->isVisible() )
            pChild->setVisible( false );
         else if( mIsExpanded && !pChild->isVisible())
            pChild->setVisible( true );
      }
      renderChildControls(offset, mBounds, updateRect);
   }
}


//////////////////////////////////////////////////////////////////////////
// Console
//////////////////////////////////////////////////////////////////////////
ConsoleMethod( GuiRolloutCtrl, isExpanded, bool, 2, 2, "() @return Returns true if object is expanded, false otherwise")
{
   return object->isExpanded();
}

ConsoleMethod( GuiRolloutCtrl, collapse, void, 2, 2, "() Collapses control.\n @return No return value.")
{
   object->collapse();
}

ConsoleMethod( GuiRolloutCtrl, expand, void, 2, 2, "() Expands control\n @return No return value.")
{
   object->expand();
}

ConsoleMethod( GuiRolloutCtrl, instantCollapse, void, 2, 2, "() Collapses control without animation.\n @return No return value.")
{
   object->instantCollapse();
}

ConsoleMethod( GuiRolloutCtrl, instantExpand, void, 2, 2, "() Expands control without animation.\n @return No return value.")
{
   object->instantExpand();
}

ConsoleMethod( GuiRolloutCtrl, sizeToContents, void, 2, 2, "() Resizes control to fit its contents\n @return No return value.")
{
   object->sizeToContents();
}
