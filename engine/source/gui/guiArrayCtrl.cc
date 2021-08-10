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

#include "console/console.h"
#include "graphics/dgl.h"
#include "platform/event.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gui/guiArrayCtrl.h"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

GuiArrayCtrl::GuiArrayCtrl()
{
   mActive = true;

   mCellSize.set(80, 30);
   mSize = Point2I(5, 30);
   mSelectedCell.set(-1, -1);
   mMouseOverCell.set(-1, -1);
   mIsContainer = true;
}

bool GuiArrayCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   //get the font
   mFont = mProfile->mFont;

   return true;
}

void GuiArrayCtrl::onSleep()
{
   Parent::onSleep();
   mFont = NULL;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiArrayCtrl::setSize(Point2I newSize)
{
   mSize = newSize;
   Point2I newExtent(newSize.x * mCellSize.x, newSize.y * mCellSize.y);

   resize(mBounds.point, newExtent);
}

void GuiArrayCtrl::getScrollDimensions(S32 &cell_size, S32 &num_cells)
{
   cell_size = mCellSize.y;
   num_cells = mSize.y;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

bool GuiArrayCtrl::cellSelected(Point2I cell)
{
   if (cell.x < 0 || cell.x >= mSize.x || cell.y < 0 || cell.y >= mSize.y)
   {
      mSelectedCell = Point2I(-1,-1);
      return false;
   }

   mSelectedCell = cell;
   scrollSelectionVisible();
   onCellSelected(cell);
    setUpdate();
   return true;
}

void GuiArrayCtrl::onCellSelected(Point2I cell)
{
    Con::executef(this, 3, "onSelect", Con::getFloatArg(cell.x), Con::getFloatArg(cell.y));

   //call the console function
   execConsoleCallback();
}

// Called when a cell is highlighted
void GuiArrayCtrl::onCellHighlighted(Point2I cell)
{
    // Do nothing
}

void GuiArrayCtrl::setSelectedCell(Point2I cell)
{
   cellSelected(cell);
}

Point2I GuiArrayCtrl::getSelectedCell()
{
   return mSelectedCell;
}

void GuiArrayCtrl::scrollSelectionVisible()
{
   scrollCellVisible(mSelectedCell);
}

void GuiArrayCtrl::scrollCellVisible(Point2I cell)
{
   //make sure we have a parent
   //make sure we have a valid cell selected
   GuiScrollCtrl *parent = dynamic_cast<GuiScrollCtrl*>(getParent());
   if(!parent || cell.x < 0 || cell.y < 0)
      return;

   RectI cellBounds(cell.x * mCellSize.x, cell.y * mCellSize.y, mCellSize.x, mCellSize.y);
   parent->scrollRectVisible(cellBounds);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiArrayCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   //make sure we have a parent
   GuiControl *parent = getParent();
   if (! parent)
      return;

   S32 i, j;
   RectI headerClip;
   RectI clipRect = dglGetClipRect();

   Point2I parentOffset = parent->localToGlobalCoord(Point2I(0, 0));

   //save the original for clipping the row headers
   RectI origClipRect = clipRect;

   for (j = 0; j < mSize.y; j++)
   {
      //skip until we get to a visible row
      if ((j + 1) * mCellSize.y + offset.y < updateRect.point.y)
         continue;

      //break once we've reached the last visible row
      if(j * mCellSize.y + offset.y >= updateRect.point.y + updateRect.extent.y)
         break;

      //render the cells for the row
      for (i = 0; i < mSize.x; i++)
      {
         //skip past columns off the left edge
         if ((i + 1) * mCellSize.x + offset.x < updateRect.point.x)
            continue;

         //break once past the last visible column
         if (i * mCellSize.x + offset.x >= updateRect.point.x + updateRect.extent.x)
            break;

         S32 cellx = offset.x + i * mCellSize.x;
         S32 celly = offset.y + j * mCellSize.y;

         RectI cellClip(cellx, celly, mCellSize.x, mCellSize.y);

         //make sure the cell is within the update region
         if (cellClip.intersect(clipRect))
         {
            //set the clip rect
            dglSetClipRect(cellClip);

            //render the cell
            onRenderCell(Point2I(cellx, celly), Point2I(i, j),
               i == mSelectedCell.x && j == mSelectedCell.y,
               i == mMouseOverCell.x && j == mMouseOverCell.y);
         }
      }
   }
}

void GuiArrayCtrl::onTouchDown(const GuiEvent &event)
{
   if ( !mActive || !mAwake || !mVisible )
      return;

   //let the guiControl method take care of the rest
   Parent::onTouchDown(event);

   Point2I pt = globalToLocalCoord(event.mousePoint);
   Point2I cell(
         (pt.x < 0 ? -1 : pt.x / mCellSize.x), 
         (pt.y < 0 ? -1 : pt.y / mCellSize.y)
      );

   if (cell.x >= 0 && cell.x < mSize.x && cell.y >= 0 && cell.y < mSize.y)
   {

      //store the previously selected cell
      Point2I prevSelected = mSelectedCell;

      //select the new cell
      cellSelected(Point2I(cell.x, cell.y));

      //if we double clicked on the *same* cell, evaluate the altConsole Command
      if ( ( event.mouseClickCount > 1 ) && ( prevSelected == mSelectedCell ) && mAltConsoleCommand[0] )
         Con::evaluate( mAltConsoleCommand, false );
   }
}

void GuiArrayCtrl::onTouchEnter(const GuiEvent &event)
{
   Point2I pt = globalToLocalCoord(event.mousePoint);

   //get the cell
   Point2I cell((pt.x < 0 ? -1 : pt.x / mCellSize.x), (pt.y < 0 ? -1 : pt.y / mCellSize.y));
   if (cell.x >= 0 && cell.x < mSize.x && cell.y >= 0 && cell.y < mSize.y)
   {
      mMouseOverCell = cell;
      setUpdateRegion(Point2I(cell.x * mCellSize.x, cell.y * mCellSize.y), mCellSize );
      onCellHighlighted(mMouseOverCell);
   }
}

void GuiArrayCtrl::onTouchLeave(const GuiEvent & /*event*/)
{
   setUpdateRegion(Point2I(mMouseOverCell.x * mCellSize.x, mMouseOverCell.y * mCellSize.y), mCellSize);
   mMouseOverCell.set(-1,-1);
   onCellHighlighted(mMouseOverCell);
}

void GuiArrayCtrl::onTouchDragged(const GuiEvent &event)
{
   // for the array control, the behaviour of onMouseDragged is the same
   // as on mouse moved - basically just recalc the currend mouse over cell
   // and set the update regions if necessary
   GuiArrayCtrl::onTouchMove(event);
}

void GuiArrayCtrl::onTouchMove(const GuiEvent &event)
{
   Point2I pt = globalToLocalCoord(event.mousePoint);
   Point2I cell((pt.x < 0 ? -1 : pt.x / mCellSize.x), (pt.y < 0 ? -1 : pt.y / mCellSize.y));
   if (cell.x != mMouseOverCell.x || cell.y != mMouseOverCell.y)
   {
      if (mMouseOverCell.x != -1)
      {
         setUpdateRegion(Point2I(mMouseOverCell.x * mCellSize.x, mMouseOverCell.y * mCellSize.y), mCellSize);
      }

      if (cell.x >= 0 && cell.x < mSize.x && cell.y >= 0 && cell.y < mSize.y)
      {
         setUpdateRegion(Point2I(cell.x * mCellSize.x, cell.y * mCellSize.y), mCellSize);
         mMouseOverCell = cell;
      }
      else
         mMouseOverCell.set(-1,-1);
   }
   onCellHighlighted(mMouseOverCell);
}

bool GuiArrayCtrl::onKeyDown(const GuiEvent &event)
{
   //if this control is a dead end, kill the event
   if ((! mVisible) || (! mActive) || (! mAwake)) return true;

   //get the parent
   S32 pageSize = 1;
   GuiControl *parent = getParent();
   if (parent && mCellSize.y > 0)
   {
      pageSize = getMax(1, (parent->mBounds.extent.y / mCellSize.y) - 1);
   }

   Point2I delta(0,0);
   switch (event.keyCode)
   {
      case KEY_LEFT:
         delta.set(-1, 0);
         break;
      case KEY_RIGHT:
         delta.set(1, 0);
         break;
      case KEY_UP:
         delta.set(0, -1);
         break;
      case KEY_DOWN:
         delta.set(0, 1);
         break;
      case KEY_PAGE_UP:
         delta.set(0, -pageSize);
         break;
      case KEY_PAGE_DOWN:
         delta.set(0, pageSize);
         break;
      case KEY_HOME:
         cellSelected( Point2I( 0, 0 ) );
         return( true );
      case KEY_END:
         cellSelected( Point2I( 0, mSize.y - 1 ) );
         return( true );
      default:
         return Parent::onKeyDown(event);
   }
   if (mSize.x < 1 || mSize.y < 1)
      return true;

   //select the first cell if no previous cell was selected
   if (mSelectedCell.x == -1 || mSelectedCell.y == -1)
   {
      cellSelected(Point2I(0,0));
      return true;
   }

   //select the cell
   Point2I cell = mSelectedCell;
   cell.x = getMax(0, getMin(mSize.x - 1, cell.x + delta.x));
   cell.y = getMax(0, getMin(mSize.y - 1, cell.y + delta.y));
   cellSelected(cell);

   return true;
}

void GuiArrayCtrl::onRightMouseDown(const GuiEvent &event)
{
   if ( !mActive || !mAwake || !mVisible )
      return;

   Parent::onRightMouseDown( event );

   Point2I pt = globalToLocalCoord( event.mousePoint );
   Point2I cell((pt.x < 0 ? -1 : pt.x / mCellSize.x), (pt.y < 0 ? -1 : pt.y / mCellSize.y));
   if (cell.x >= 0 && cell.x < mSize.x && cell.y >= 0 && cell.y < mSize.y)
   {
      char buf[32];
      dSprintf( buf, sizeof( buf ), "%d %d", event.mousePoint.x, event.mousePoint.y );
      // Pass it to the console:
       Con::executef(this, 4, "onRightMouseDown", Con::getIntArg(cell.x), Con::getIntArg(cell.y), buf);
   }
}
