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
#ifndef _GUI_TABBOOKCTRL_H_
#define _GUI_TABBOOKCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _GUITABPAGECTRL_H_
#include "gui/containers/guiTabPageCtrl.h"
#endif


/// Tab Book Control for creation of tabbed dialogs
///
/// @see GUI for an overview of the Torque GUI system.
///
/// @section GuiTabBookCtrl_Intro Introduction
///
/// GuiTabBookCtrl is a container class that holds children of type GuiTabPageCtrl
///
/// GuiTabBookCtrl creates an easy to work with system for creating tabbed dialogs
/// allowing for creation of dialogs that store alot of information in a small area
/// by seperating the information into pages which are changeable by clicking their
/// page title on top or bottom of the control
///
/// tabs may be aligned to be on top or bottom of the book and are changeable while
/// the GUI editor is open for quick switching between pages allowing multipage dialogs
/// to be edited quickly and easily.
///
/// The control may only contain children of type GuiTabPageCtrl.
/// If a control is added to the Book that is not of type GuiTabPageCtrl, it will be
/// removed and relocated to the currently active page of the control.
/// If there is no active page in the book, the child control will be relocated to the
/// parent of the book.
///
/// @ref GUI has an overview of the GUI system.
///
/// @nosubgrouping
class GuiTabBookCtrl : public GuiControl
{
public:
    enum TabPosition
    {
		AlignTop,   ///< Align the tabs on the top of the tab book control
		AlignBottom,///< Align the tabs on the bottom of the tab book control
		AlignLeft,  ///< Align the tabs on the left of the tab book control
		AlignRight  ///< Align the tabs on the right of the tab book control
    };

   struct TabHeaderInfo
   {
      GuiTabPageCtrl *Page;
      S32             TextWidth;
      S32             TabRow;
      S32             TabColumn;
      RectI           TabRect;
   };

   GuiControlProfile *mTabProfile; //Used to render the tabs

private:

   typedef GuiControl Parent;                ///< typedef for parent class access

   RectI                   mPageRect;        ///< Rectangle of the tab page portion of the control
   RectI                   mTabRect;         ///< Rectangle of the tab portion of the control
   Vector<TabHeaderInfo>   mPages;           ///< Vector of pages contained by the control
   GuiTabPageCtrl*         mActivePage;      ///< Pointer to the active (selected) tab page child control
   GuiTabPageCtrl*         mHoverTab;        ///< Pointer to the tab page that currently has the mouse positioned ontop of its tab
   S32                     mMinTabWidth;     ///< Minimum Width a tab will display as.
   TabPosition             mTabPosition;     ///< Current tab position (see alignment)
   TabPosition             mLastTabPosition; ///< Last known tab position, stored to compare to tabPosition to know when to resize children
   S32                     mFontHeight;      ///< Last known font height
   S32                     mTabWidth;        ///< Current tab width of the first tab
   
   enum
   {
		TabSelected = 0,		///< Index of selected tab texture
		TabNormal,				///< Index of normal tab texture
		TabHover,				///< Index of hover tab texture
		TabSelectedVertical,	///< Index of selected tab texture
		TabNormalVertical,		///< Index of normal tab texture
		TabHoverVertical,		///< Index of hover tab texture
		TabBackground = 19,     ///< Index of background texture (tiled)
		NumBitmaps				///< Number of bitmaps in this array
   };
   bool  mHasTexture;   ///< Indicates whether we have a texture to render the tabs with
   RectI *mBitmapBounds;///< Array of rectangles identifying textures for tab book

  public:

   GuiTabBookCtrl();
   DECLARE_CONOBJECT(GuiTabBookCtrl);

   static void initPersistFields();

    /// @name Control Events
    /// @{
   bool onAdd();
   void onRemove();
   bool onWake();
   void onSleep();
   void setControlTabProfile(GuiControlProfile* prof);
   void onPreRender();
   void onRender( Point2I offset, const RectI &updateRect );
   /// @}

   /// @name Child events
   /// @{
   void onChildRemoved( GuiControl* child );
   void onChildAdded( GuiControl *child );
   /// @}

   /// @name Rendering methods
   /// @{

   /// Tab rendering routine, iterates through all tabs rendering one at a time
   /// @param   offset   the render offset to accomodate global coords
   void renderTabs( const Point2I &offset );

   /// Tab rendering subroutine, renders one tab with specified options
   /// @param   tabRect   the rectangle to render the tab into
   /// @param   tab   pointer to the tab page control for which to render the tab
   void renderTab( RectI tabRect, GuiTabPageCtrl* tab );
   /// @}

   /// @name Page Management
   /// @{

   /// Create a new tab page child in the book
   ///
   /// Pages created are not titled and appear with no text on their tab when created.
   /// This may change in the future.
   void addNewPage();

   U32 getSelectedPage();

   /// Select a tab page based on an index
   /// @param   index   The index in the list that specifies which page to select
   void selectPage( S32 index );

   /// Select a tab page by a pointer to that page
   /// @param   page   A pointer to the GuiTabPageCtrl to select.  This page must be a child of the tab book.
   void selectPage( GuiTabPageCtrl *page );

   /// Select a tab page by it's name in the book
   /// @param   pageName   A string represeting the 'Text' of the GuiTabPage to be shown
   void selectPage( const char* pageName );

   /// Select the Next page in the tab book
   void selectNextPage();

   /// Select the Previous page in the tab book
   void selectPrevPage();

   /// @}

   /// @name Internal Utility Functions
   /// @{

   /// Update ourselves by hooking common GuiControl functionality.
   void setUpdate();

   /// Balance a top/bottom tab row
   void balanceRow( S32 row, S32 totalTabWidth );

   /// Balance a left/right tab column
   void balanceColumn( S32 row, S32 totalTabWidth );

   /// Checks to see if a tab option has changed and we need to resize children, resizes if necessary
   void solveDirty();

   /// Calculate the tab width of a page, given it's caption
   S32 calculatePageTabWidth( GuiTabPageCtrl *page );

   /// Calculate Page Header Information
   void calculatePageTabs();

   /// Find the tab that was hit by the current event, if any
   /// @param   event   The GuiEvent that caused this function call
   GuiTabPageCtrl *findHitTab( const GuiEvent &event );

   /// Find the tab that was hit, based on a point
   /// @param    hitPoint   A Point2I that specifies where to search for a tab hit
   GuiTabPageCtrl *findHitTab( Point2I hitPoint );

   /// Changes a local point to a point in the inner rect of the tab section.
   Point2I getTabLocalCoord(const Point2I &src);

   /// @}

   /// @name Sizing
   /// @{

   /// Rezize our control
   /// This method is overridden so that we may handle resizing of our child tab
   /// pages when we are resized.
   /// This ensures we keep our sizing in sync when we are moved or sized.
   ///
   /// @param   newPosition   The new position of the control
   /// @param   newExtent   The new extent of the control
   void resize(const Point2I &newPosition, const Point2I &newExtent);

   /// Called when a child page is resized
   /// This method is overridden so that we may handle resizing of our child tab
   /// pages when one of them is resized.
   /// This ensures we keep our sizing in sync when our children are sized or moved.
   ///
   /// @param   child   A pointer to the child control that has been resized
   void childResized(GuiControl *child);

   /// @}


   virtual bool onKeyDown(const GuiEvent &event);


   /// @name Mouse Events
   /// @{
   void onTouchDown(const GuiEvent &event);
   void onTouchMove(const GuiEvent &event);
   void onTouchLeave(const GuiEvent &event);
   virtual bool onMouseDownEditor(const GuiEvent &event, Point2I offset);
   /// @}
};

#endif //_GUI_TABBOOKCTRL_H_
