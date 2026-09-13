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

#include "console/consoleTypes.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/codeBlock.h"
#include "platform/event.h"
#include "graphics/gBitmap.h"
#include "graphics/dgl.h"
#include "input/actionMap.h"
#include "gui/guiCanvas.h"
#include "gui/guiControl.h"
#include "gui/guiDefaultControlRender.h"
#include "gui/editor/guiEditCtrl.h"
#include "string/unicode.h"
#include "collection/vector.h"
#include "2d/core/Utility.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gui/editor/guiEditCtrl.h"

#include <sstream>
#include <iostream>
#include <vector>
#include <string>

#include "guiControl_ScriptBinding.h"

#ifndef _FRAMEALLOCATOR_H_
#include "memory/frameAllocator.h"
#endif

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT_CHILDREN(GuiControl);

/// Counts are 4, not 3: "default" is a real, reachable value.
///
/// It used to be excluded, and that was a bug rather than a choice. A control
/// starts on DefaultAlign, which getAlignmentType resolves to the PROFILE's
/// alignment -- so it is the setting that means "inherit", and every control
/// has it until someone picks otherwise. Hiding it from the table meant
/// ConsoleGetType could not name the value it found and answered with an empty
/// string, so an untouched control's align read back as nothing, and there was
/// no way to put a control back to inheriting once you had chosen a side.
static EnumTable::Enums alignCtrlEnums[] =
{
   { AlignmentType::LeftAlign,          "left"      },
   { AlignmentType::CenterAlign,        "center"    },
   { AlignmentType::RightAlign,         "right"     },
   { AlignmentType::DefaultAlign,       "default"   }   ///< Inherit the profile's alignment.
};
static EnumTable gAlignCtrlTable(4, &alignCtrlEnums[0]);

static EnumTable::Enums vAlignCtrlEnums[] =
{
   { VertAlignmentType::TopVAlign,          "top"      },
   { VertAlignmentType::MiddleVAlign,        "middle"    },
   { VertAlignmentType::BottomVAlign,         "bottom"     },
   { VertAlignmentType::DefaultVAlign,       "default"   }   ///< Inherit the profile's alignment.
};
static EnumTable gVAlignCtrlTable(4, &vAlignCtrlEnums[0]);

//used to locate the next/prev responder when tab is pressed
S32 GuiControl::smCursorChanged           = -1;
GuiControl *GuiControl::smPrevResponder = NULL;
GuiControl *GuiControl::smCurResponder = NULL;

GuiEditCtrl *GuiControl::smEditorHandle = NULL;

bool GuiControl::smDesignTime = false;

GuiControl::GuiControl()
{
   mLayer = 0;
   mBounds.set(0, 0, 64, 64);
   mStoredExtent.set(0, 0);
   mRenderInsetLT.set(0, 0);
   mRenderInsetRB.set(0, 0);
   mMinExtent.set(0, 0);
   mStoredRelativePosH.set(0, 0);
   mStoredRelativePosV.set(0, 0);
   mUseRelPosH = false;
   mUseRelPosV = false;

   mProfile = NULL;

   mConsoleVariable     = StringTable->EmptyString;
   mConsoleCommand      = StringTable->EmptyString;
   mAltConsoleCommand   = StringTable->EmptyString;
   mAcceleratorKey      = StringTable->EmptyString;
   mLangTableName       = StringTable->EmptyString;
   mText                = StringTable->EmptyString;
   mTextID              = StringTable->EmptyString;

   mAlignment           = AlignmentType::DefaultAlign;
   mVAlignment          = VertAlignmentType::DefaultVAlign;
   mFontSizeAdjust      = 1;
   mFontColor.set(0, 0, 0, 255);
   mOverrideFontColor   = false;

   mLangTable           = NULL;
   mFirstResponder      = NULL;
   mCanSaveFieldDictionary = false;
   mVisible             = true;
   mActive              = false;
   mAwake               = false;
   mCanSave				= true;
   mHorizSizing         = horizResizeRight;
   mVertSizing          = vertResizeBottom;
   mTooltipProfile      = NULL;
   mLazyTooltipProfile  = NULL;
   mTooltip             = StringTable->EmptyString;
   mTipHoverTime        = DEFAULT_TOOLTIP_HOVERTIME;
   mTooltipWidth        = DEFAULT_TOOLTIP_WIDTH;
   mRendersChildren     = true;
   mIsContainer         = true;
   mAllowEventPassThru  = false;
   mTextWrap			= false;
   mTextExtend          = false;
   mFittedText          = NULL;
   mFittedWidth         = -1;
   mFittedFont          = NULL;
   mFittedProfile       = NULL;
   mFittedWrap          = false;
   mUseInput            = true;
}

GuiControl::~GuiControl()
{
}

bool GuiControl::onAdd()
{
   // Let Parent Do Work.
   if(!Parent::onAdd())
      return false;

   // Grab the classname of this object
   const char *cName = getClassName();

   // Clamp to minExtent
   mBounds.extent.x = getMax( mMinExtent.x, mBounds.extent.x );
   mBounds.extent.y = getMax( mMinExtent.y, mBounds.extent.y );

   // Nothing below this class may be left without a profile. Most constructors
   // setField one themselves, but several never did -- GuiChainCtrl,
   // GuiTabPageCtrl, GuiSliderCtrl, GuiTextEditCtrl, GuiInputCtrl,
   // GuiSpriteCtrl and SceneWindow among them -- and a control with a null
   // mProfile is a crash waiting for the first thing that reads it. A chain
   // does not even need to be rendered: adding a child runs calculateExtent,
   // which asks the profile for its borders.
   //
   // Doing it here rather than in each constructor is the point: it is one
   // place, it covers every class that already exists, and a class added later
   // cannot forget. GuiDefaultProfile is created during engine start-up
   // (defaultGame.cc) and is the same fallback the TypeGuiProfile setter uses,
   // so this is the behaviour every named profile already had on a miss.
   if( mProfile == NULL )
      setField( "profile", "GuiDefaultProfile" );


   // Add to root group.
   Sim::getGuiGroup()->addObject(this);

   // Return Success.
   return true;
}

void GuiControl::onChildAdded( GuiControl *child )
{
	if(mProfile)
	{
		//This will cause the child control to be centered if it needs to be.
		RectI innerRect = getInnerRect();
		child->parentResized(innerRect.extent, innerRect.extent);
		
		if (isMethod("onChildAdded"))
		{
			Con::executef(this, 3, "onChildAdded", child->getIdString());
		}
	}
}

void GuiControl::onChildRemoved(GuiControl* child)
{
	if (mProfile && isMethod("onChildRemoved"))
	{
		Con::executef(this, 3, "onChildRemoved", child->getIdString());
	}
}

/// The sizing names, and why there are two sets of them.
///
/// The original names describe the edge that MOVES; the field controls the edge
/// that STAYS. So "right" pins the LEFT edge (parentResized has no branch for
/// it, so nothing moves) and "left" pins the RIGHT one (newPosition.x += delta).
/// Reading a Gui file meant inverting every one of them in your head, and
/// picking one from a list was a memory test.
///
/// The anchor names say what actually happens. Ordering is load-bearing in two
/// directions:
///
///   ConsoleGetType returns the FIRST label whose value matches, so whichever
///   name is listed first is what a field reads back as and what TAML writes.
///   The preferred names are therefore at the top.
///
///   ConsoleSetType accepts ANY label in the table, case-insensitively, so the
///   deprecated names below still load. Every .gui.taml already on disk, and
///   every script that spells a sizing flag the old way, keeps working.
///
/// Note that a Gui saved by this build writes the new names, which an older
/// build cannot read -- its table has no "anchorLeft", and ConsoleSetType
/// silently falls back to index 0 on a miss.
///
/// "width"/"height" (both edges pinned) and "center"/"fill" were never
/// misleading and keep their names. "relative" gains "scale", which is what it
/// does.
static EnumTable::Enums horzEnums[] =
{
    { GuiControl::horizResizeRight,       "anchorLeft"  },   ///< Left edge stays put.
    { GuiControl::horizResizeLeft,        "anchorRight" },   ///< Right edge stays put.
    { GuiControl::horizResizeWidth,       "width"       },   ///< Both edges stay; the width follows the parent.
    { GuiControl::horizResizeCenter,      "center"      },   ///< Neither edge; stays centred.
    { GuiControl::horizResizeRelative,    "scale"       },   ///< Both edges scale with the parent.
    { GuiControl::horizResizeFill,        "fill"        },   ///< Fills the parent's inner rect.

    // Deprecated. Accepted for reading; never written.
    { GuiControl::horizResizeRight,       "right"       },   ///< \deprecated Use anchorLeft.
    { GuiControl::horizResizeLeft,        "left"        },   ///< \deprecated Use anchorRight.
    { GuiControl::horizResizeRelative,    "relative"    }    ///< \deprecated Use scale.
};
static EnumTable gHorizSizingTable(9, &horzEnums[0]);

static EnumTable::Enums vertEnums[] =
{
    { GuiControl::vertResizeBottom,       "anchorTop"    },  ///< Top edge stays put.
    { GuiControl::vertResizeTop,          "anchorBottom" },  ///< Bottom edge stays put.
    { GuiControl::vertResizeHeight,       "height"       },  ///< Both edges stay; the height follows the parent.
    { GuiControl::vertResizeCenter,       "center"       },  ///< Neither edge; stays centred.
    { GuiControl::vertResizeRelative,     "scale"        },  ///< Both edges scale with the parent.
    { GuiControl::vertResizeFill,         "fill"         },  ///< Fills the parent's inner rect.

    // Deprecated. Accepted for reading; never written.
    { GuiControl::vertResizeBottom,       "bottom"       },  ///< \deprecated Use anchorTop.
    { GuiControl::vertResizeTop,          "top"          },  ///< \deprecated Use anchorBottom.
    { GuiControl::vertResizeRelative,     "relative"     }   ///< \deprecated Use scale.
};
static EnumTable gVertSizingTable(9, &vertEnums[0]);

void GuiControl::initPersistFields()
{
   Parent::initPersistFields();

   // Things relevant only to the editor.
   addGroup("Editing");
   addProtectedField("isContainer",       TypeBool,      Offset(mIsContainer, GuiControl), &setIsContainerFn, &defaultProtectedGetFn, &writeIsContainerFn, "True if the container should accept children in the editor. Some controls cannot be containers.");
   endGroup("Editing");

   // Parent Group.
   addGroup("GuiControl");

   addField("Profile",           TypeGuiProfile,	Offset(mProfile, GuiControl));
   addField("HorizSizing",       TypeEnum,			Offset(mHorizSizing, GuiControl), 1, &gHorizSizingTable);
   addField("VertSizing",        TypeEnum,			Offset(mVertSizing, GuiControl), 1, &gVertSizingTable);

   addProtectedField("Position",          TypePoint2I,		Offset(mBounds.point, GuiControl), &setPositionFn, &defaultProtectedGetFn, "The location of the control in relation to its parent's content area.");
   addProtectedField("Extent",            TypePoint2I,		Offset(mBounds.extent, GuiControl), &setExtentFn, &defaultProtectedGetFn, "The size of the control writen as width and height.");
   addProtectedField("MinExtent",         TypePoint2I,		Offset(mMinExtent, GuiControl), &setMinExtentFn, &defaultProtectedGetFn, &writeMinExtentFn, "The extent will not shrink below this size.");
   addField("canSave",           TypeBool,          Offset(mCanSave, GuiControl), &defaultProtectedNotWriteFn);
   addField("Visible",           TypeBool,          Offset(mVisible, GuiControl), &writeVisibleFn);
   addField("useInput",          TypeBool,          Offset(mUseInput, GuiControl), &writeUseInputFn);

   addField("Variable",          TypeString,		Offset(mConsoleVariable, GuiControl));
   addField("Command",           TypeString,		Offset(mConsoleCommand, GuiControl));
   addField("AltCommand",        TypeString,		Offset(mAltConsoleCommand, GuiControl));
   addField("Accelerator",       TypeString,		Offset(mAcceleratorKey, GuiControl));
   addField("Active",			 TypeBool,			Offset(mActive, GuiControl));
   endGroup("GuiControl");	

   addGroup("ToolTip");
   addField("tooltipprofile",    TypeGuiProfile,	Offset(mTooltipProfile, GuiControl));
   addField("tooltip",           TypeString,		Offset(mTooltip, GuiControl));
   addField("tooltipWidth",      TypeS32,			Offset(mTooltipWidth, GuiControl), &writeToolTipWidthFn);
   addField("hovertime",         TypeS32,			Offset(mTipHoverTime, GuiControl), &writeToolTipHoverTimeFn);
   endGroup("ToolTip");


   addGroup("Localization");
   addField("langTableMod",      TypeString,		Offset(mLangTableName, GuiControl));
   endGroup("Localization");

   addGroup("Text");
   addProtectedField("text", TypeCaseString, Offset(mText, GuiControl), setTextProperty, getTextProperty, "");
   addField("textID", TypeString, Offset(mTextID, GuiControl));
   addField("textWrap", TypeBool, Offset(mTextWrap, GuiControl), &writeTextWrapFn, "If true, text will wrap to additional lines.");
   addField("textExtend", TypeBool, Offset(mTextExtend, GuiControl), &writeTextExtendFn, "If true, extent will change based on the size of the control's text when possible.");
   addField("align", TypeEnum, Offset(mAlignment, GuiControl), 1, &gAlignCtrlTable);
   addField("vAlign", TypeEnum, Offset(mVAlignment, GuiControl), 1, &gVAlignCtrlTable);
   addField("fontSizeAdjust", TypeF32, Offset(mFontSizeAdjust, GuiControl), &writeFontSizeAdjustFn, "A decimal value that is multiplied with the profile's fontSize to determine the control's actual font size.");
   addField("overrideFontColor", TypeBool, Offset(mOverrideFontColor, GuiControl), &writeOverrideFontColorFn, "If true, the control's fontColor will override the profile's font color.");
   addField("fontColor", TypeColorI, Offset(mFontColor, GuiControl), &writeFontColorFn, "A color to override the font color of the control's profile. OverrideFontColor must be set to true for this to work.");
   endGroup("Text");
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

LangTable * GuiControl::getGUILangTable()
{
    if(mLangTable)
        return mLangTable;

    if(mLangTableName && *mLangTableName)
    {
        mLangTable = (LangTable *)getModLangTable((const UTF8*)mLangTableName);
        return mLangTable;
    }

    GuiControl *parent = getParent();
    if(parent)
        return parent->getGUILangTable();

    return NULL;
}

const UTF8 * GuiControl::getGUIString(S32 id)
{
    LangTable *lt = getGUILangTable();
    if(lt)
        return lt->getString(id);

    return NULL;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //


void GuiControl::addObject(SimObject *object)
{
   GuiControl *ctrl = dynamic_cast<GuiControl *>(object);
   if(!ctrl)
   {
      AssertWarn(0, "GuiControl::addObject: attempted to add NON GuiControl to set");
      return;
   }

   if(object->getGroup() == this)
      return;

    Parent::addObject(object);

   AssertFatal(!ctrl->isAwake(), "GuiControl::addObject: object is already awake before add");
   if(mAwake)
      ctrl->awaken();

   // Two pieces of cached sizing state describe the parent the control has just
   // left, and neither means anything here. Both are cleared before
   // onChildAdded, which is the first thing to read them.
   //
   // mStoredRelativePos is the proportion of its parent a SCALED control
   // occupies, cached so that a run of layout passes cannot round its edges away
   // a pixel at a time. Writing a position is the moment it stops describing the
   // control, which is why the Position and Extent field setters clear it -- and
   // changing parent is that moment too. Left alone, onChildAdded applied the OLD
   // parent's proportion to the NEW parent's extent: a button 200 wide at x=100
   // in an 800-wide container arrived 50 wide at x=25 in a 200-wide one. Reset,
   // the proportion is recaptured against the parent it has now, old and new
   // extents are the same value in that call, and the arithmetic is the identity
   // -- so a move keeps the size it moved, which is what every other sizing mode
   // already did.
   //
   // mStoredExtent is extent given up to minExtent and owed back when there is
   // room again. A debt run up under one parent is not the next one's to pay.
   //
   // Note this cannot fire on a re-add: addObject returns above when the object
   // is already a child of this control.
   ctrl->resetStoredRelPos();
   ctrl->resetStoredExtent();

    onChildAdded( ctrl );
}

void GuiControl::removeObject(SimObject *object)
{
	GuiControl *ctrl = dynamic_cast<GuiControl *>(object);
	if (!ctrl)
	{
		AssertWarn(0, "GuiControl::removeObject: attempted to remove NON GuiControl from set");
		return;
	}

   AssertFatal(mAwake == ctrl->isAwake(), "GuiControl::removeObject: child control wake state is bad");
   if (mAwake)
      ctrl->sleep();
    Parent::removeObject(object);

	// If we are a child, notify our parent that we've been removed
	onChildRemoved(ctrl);
}

GuiControl *GuiControl::getParent()
{
    SimObject *obj = getGroup();
    if (GuiControl* gui = dynamic_cast<GuiControl*>(obj))
      return gui;
   return 0;
}

GuiCanvas *GuiControl::getRoot()
{
   GuiControl *root = NULL;
    GuiControl *parent = getParent();
   while (parent)
   {
      root = parent;
      parent = parent->getParent();
   }
   if (root)
      return dynamic_cast<GuiCanvas*>(root);
   else
      return NULL;
}

void GuiControl::inspectPreApply()
{
   if(isEditMode())
      smEditorHandle->controlInspectPreApply(this);
   
   // The canvas never sleeps
   // This forced sleep will allow us to unload and reload things in the editor.
   mPreviouslyAwake = mAwake;
   if(mAwake && dynamic_cast<GuiCanvas*>(this) == NULL )
   {
      onSleep(); // release all our resources.
   }
}

void GuiControl::inspectPostApply()
{
   // Shhhhhhh, you don't want to wake the canvas!
   // If this control was awake before we should revive it.
   if(mPreviouslyAwake && !mAwake && dynamic_cast<GuiCanvas*>(this) == NULL )
   {
      onWake();
   }
   
   if(isEditMode())
      smEditorHandle->controlInspectPostApply(this);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

Point2I GuiControl::localToGlobalCoord(const Point2I &src)
{
   Point2I ret = src;
   ret += (mBounds.point + mRenderInsetLT);
   GuiControl *walk = getParent();
   while(walk)
   {
      ret += (walk->getPosition() + walk->mRenderInsetLT);
      walk = walk->getParent();
   }
   return ret;
}

Point2I GuiControl::globalToLocalCoord(const Point2I &src)
{
   Point2I ret = src;
   ret -= (mBounds.point + mRenderInsetLT);
   GuiControl *walk = getParent();
   while(walk)
   {
      ret -= (walk->getPosition() + walk->mRenderInsetLT);
      walk = walk->getParent();
   }
   return ret;
}

//----------------------------------------------------------------
void GuiControl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
   Point2I actualNewExtent = Point2I(getMax(mMinExtent.x, newExtent.x),
      getMax(mMinExtent.y, newExtent.y));

	Point2I oldExtent = mBounds.extent;

    //force center if using center positioning
	Point2I oldPosition = mBounds.point;
    Point2I actualNewPosition = Point2I(newPosition);
    GuiControl* parent = getParent();
    if (parent && parent->mProfile)
    {
		Point2I parentInnerExtent = parent->getInnerRect().extent;
        if (mHorizSizing == horizResizeCenter)
        {
            actualNewPosition.x = (parentInnerExtent.x - actualNewExtent.x) / 2;
        }
		else if (mHorizSizing == horizResizeFill)
		{
			actualNewPosition.x = 0;
			actualNewExtent.x = parentInnerExtent.x;
		}
        if (mVertSizing == vertResizeCenter)
        {
            actualNewPosition.y = (parentInnerExtent.y - actualNewExtent.y) / 2;
        }
		else if (mVertSizing == vertResizeFill)
		{
			actualNewPosition.y = 0;
			actualNewExtent.y = parentInnerExtent.y;
		}
    }

   // only do the child control resizing stuff if you really need to.
   bool extentChanged = (actualNewExtent != oldExtent);
   bool positionChanged = (actualNewPosition != oldPosition);

   if (extentChanged) {
      //call set update both before and after
      setUpdate();
      mBounds.set(actualNewPosition, actualNewExtent);
      iterator i;
      for(i = begin(); i != end(); i++)
      {
         GuiControl *ctrl = static_cast<GuiControl *>(*i);
         ctrl->parentResized(oldExtent - (ctrl->mRenderInsetLT + ctrl->mRenderInsetRB), actualNewExtent - (ctrl->mRenderInsetLT + ctrl->mRenderInsetRB));
      }

      if (parent)
         parent->childResized(this);
      setUpdate();

	  if (isMethod("onResize"))
	  {
		  Con::executef(this, 2, "onResize");
	  }
   }
   if(positionChanged) 
   {
      mBounds.point = actualNewPosition;
	  if(parent)
		parent->childMoved(this);

	  if (isMethod("onMoved"))
	  {
		  Con::executef(this, 2, "onMoved");
	  }
   }

   // A wrapped control's height follows from its width, so a new width is new
   // text to fit. Last, because fitting resizes, and a centered control's
   // position depends on its height.
   if (extentChanged && actualNewExtent.x != oldExtent.x)
      fitToText();
}
void GuiControl::setPosition( const Point2I &newPosition )
{
   resize( newPosition, mBounds.extent );
}

void GuiControl::setExtent( const Point2I &newExtent )
{
   resize( mBounds.point, newExtent );
}

void GuiControl::setBounds( const RectI &newBounds )
{
   resize( newBounds.point, newBounds.extent );
}

void GuiControl::setLeft( S32 newLeft )
{
   resize( Point2I( newLeft, mBounds.point.y), mBounds.extent );
}

void GuiControl::setTop( S32 newTop )
{
   resize( Point2I( mBounds.point.x, newTop ), mBounds.extent );
}

void GuiControl::setWidth( S32 newWidth )
{
   resize( mBounds.point, Point2I( newWidth, mBounds.extent.y ) );
}

void GuiControl::setHeight( S32 newHeight )
{
   resize( mBounds.point, Point2I( mBounds.extent.x, newHeight ) );
}

void GuiControl::childResized(GuiControl *child)
{
   // Default to do nothing. Do not call resize from here as it will create an infinite loop.

	if (isMethod("onChildResized"))
	{
		Con::executef(this, 3, "onChildResized", child->getIdString());
	}
}

void GuiControl::childMoved(GuiControl* child)
{
	// Default to do nothing. Do not call resize from here as it will create an infinite loop.

	if (isMethod("onChildMoved"))
	{
		Con::executef(this, 3, "onChildMoved", child->getIdString());
	}
}

void GuiControl::childrenReordered()
{
	// Default to do nothing.

	if (isMethod("onChildrenReordered"))
	{
		Con::executef(this, 2, "onChildrenReordered");
	}
}

void GuiControl::parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent)
{
   Point2I newPosition = getPosition();
   Point2I newExtent = getExtent();

    S32 deltaX = newParentExtent.x - oldParentExtent.x;
    S32 deltaY = newParentExtent.y - oldParentExtent.y;

	//In the case of centering, we want to make doubly sure we are using the inner rect.
	GuiControl* parent = getParent();
	Point2I parentInnerExt = Point2I(newParentExtent);
	if(mHorizSizing == horizResizeCenter || mVertSizing == vertResizeCenter ||
		mHorizSizing == horizResizeFill || mVertSizing == vertResizeFill)
	{
		//This is based on the "new" outer extent of the parent.
		parentInnerExt = parent->getInnerRect().extent;
	}

    if (mHorizSizing == horizResizeCenter)
       newPosition.x = (parentInnerExt.x - mBounds.extent.x) >> 1;
    else if (mHorizSizing == horizResizeWidth)
        newExtent.x += deltaX;
    else if (mHorizSizing == horizResizeLeft)
      newPosition.x += deltaX;
	else if (mHorizSizing == horizResizeFill)
	{
		newPosition.x = 0;
		newExtent.x = parentInnerExt.x;
	}
    else if (mHorizSizing == horizResizeRelative && oldParentExtent.x != 0)
    {
        Point2F percent = relPosBatteryH(newPosition.x, newExtent.x, oldParentExtent.x);
        S32 newLeft = mRound(percent.x * newParentExtent.x);
        S32 newRight = mRound(percent.y * newParentExtent.x);

        newPosition.x = newLeft;
        newExtent.x = newRight - newLeft;
    }

    if (mVertSizing == vertResizeCenter)
       newPosition.y = (parentInnerExt.y - mBounds.extent.y) >> 1;
    else if (mVertSizing == vertResizeHeight)
        newExtent.y += deltaY;
    else if (mVertSizing == vertResizeTop)
      newPosition.y += deltaY;
	else if (mVertSizing == vertResizeFill)
	{
		newPosition.y = 0;
		newExtent.y = parentInnerExt.y;
	}
    else if(mVertSizing == vertResizeRelative && oldParentExtent.y != 0)
    {
        Point2F percent = relPosBatteryV(newPosition.y, newExtent.y, oldParentExtent.y);
        S32 newTop = mRound(percent.x * newParentExtent.y);
        S32 newBottom = mRound(percent.y * newParentExtent.y);

        newPosition.y = newTop;
        newExtent.y = newBottom - newTop;
    }

   newExtent = extentBattery(newExtent);

   resize(newPosition, newExtent);
}

// One axis of the rescue. Nothing of the control is visible when its far edge is
// at or before the parent's near edge, or its near edge is at or past the
// parent's far edge -- and a parent with no room at all shows nothing wherever
// the control is put, which the second test catches on its own (pos >= 0).
static S32 rescuedAxis(S32 pos, S32 extent, S32 parentInnerExtent)
{
    const bool offTheNearSide = (pos + extent) <= 0;
    const bool offTheFarSide = pos >= parentInnerExtent;

    return (offTheNearSide || offTheFarSide) ? 0 : pos;
}

Point2I GuiControl::rescuedPosition(const Point2I &pos, const Point2I &extent, const Point2I &parentInnerExtent)
{
    return Point2I(rescuedAxis(pos.x, extent.x, parentInnerExtent.x),
        rescuedAxis(pos.y, extent.y, parentInnerExtent.y));
}

bool GuiControl::pullIntoView()
{
    GuiControl* parent = getParent();
    if (parent == NULL)
    {
        return false;
    }

    const Point2I rescued = rescuedPosition(mBounds.point, mBounds.extent, parent->getInnerRect().extent);
    if (rescued == mBounds.point)
    {
        return false;
    }

    // Through resize rather than a direct write to mBounds, so a container that
    // places its own children hears about it -- and then reset the cached
    // proportion, because a scaled control that has just been moved must measure
    // from where it landed. resize() does not do that for its caller: it is what
    // parentResized itself calls, and clearing the cache there would defeat it.
    resize(rescued, mBounds.extent);
    resetStoredRelPos();

    return true;
}

void GuiControl::preventResizeModeFill()
{
	preventHorizResizeModeFill();
	preventVertResizeModeFill();
}

void GuiControl::preventResizeModeCenter()
{
	preventHorizResizeModeCenter();
	preventVertResizeModeCenter();
}

void GuiControl::preventHorizResizeModeFill()
{
	if (getHorizSizing() == horizResizeFill)
	{
		setHorizSizing(horizResizeRight);
	}
}

void GuiControl::preventVertResizeModeFill()
{
	if (getVertSizing() == vertResizeFill)
	{
		setVertSizing(vertResizeBottom);
	}
}

void GuiControl::preventHorizResizeModeCenter()
{
	if (getHorizSizing() == horizResizeCenter)
	{
		setHorizSizing(horizResizeRight);
	}
}

void GuiControl::preventVertResizeModeCenter()
{
	if (getVertSizing() == vertResizeCenter)
	{
		setVertSizing(vertResizeBottom);
	}
}

Point2I GuiControl::extentBattery(Point2I &newExtent)
{
	if (mMinExtent.x == 0 && mMinExtent.y == 0)
	{
		return newExtent;
	}

	Point2I result = Point2I(newExtent);
    if (mHorizSizing != horizResizeRelative)
    {
        if (newExtent.x < mBounds.extent.x && newExtent.x < mMinExtent.x)
        {
            mStoredExtent.x += mBounds.extent.x > mMinExtent.x ? (mMinExtent.x - newExtent.x) : (mBounds.extent.x - newExtent.x);
            result.x = mMinExtent.x;
        }
        else if (newExtent.x > mBounds.extent.x && mStoredExtent.x > 0)
        {
            S32 charge = getMin(newExtent.x - mBounds.extent.x, mStoredExtent.x);
            mStoredExtent.x -= charge;
            result.x = newExtent.x - charge;
        }
    }

    if (mVertSizing != vertResizeRelative)
    {
        if (newExtent.y < mBounds.extent.y && newExtent.y < mMinExtent.y)
        {
            mStoredExtent.y += mBounds.extent.y > mMinExtent.y ? (mMinExtent.y - newExtent.y) : (mBounds.extent.y - newExtent.y);
            result.y = mMinExtent.y;
        }
        else if (newExtent.y > mBounds.extent.y && mStoredExtent.y > 0)
        {
            S32 charge = getMin(newExtent.y - mBounds.extent.y, mStoredExtent.y);
            mStoredExtent.y -= charge;
            result.y = newExtent.y - charge;
        }
    }
	return result;
}

Point2F GuiControl::relPosBatteryH(S32 pos, S32 ext, S32 parentExt)
{
    if (!mUseRelPosH)
    {
        relPosBattery(mStoredRelativePosH, pos, ext, parentExt);
        mUseRelPosH = true;
    }
    return mStoredRelativePosH;
}

Point2F GuiControl::relPosBatteryV(S32 pos, S32 ext, S32 parentExt)
{
    if (!mUseRelPosV)
    {
        relPosBattery(mStoredRelativePosV, pos, ext, parentExt);
        mUseRelPosV = true;
    }
    return mStoredRelativePosV;
}

void GuiControl::relPosBattery(Point2F& battery, S32 pos, S32 ext, S32 parentExt)
{
    battery.x = static_cast<F32>(pos) / parentExt;
    battery.y = static_cast<F32>(pos + ext) / parentExt;
}

//----------------------------------------------------------------

void GuiControl::onRender(Point2I offset, const RectI &updateRect)
{
    RectI ctrlRect = applyMargins(offset, mBounds.extent, NormalState, mProfile);

	if (!ctrlRect.isValidRect())
	{
		return;
	}

	renderUniversalRect(ctrlRect, mProfile, NormalState);

	//Render Text
	dglSetBitmapModulation(getFontColor(mProfile));
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, NormalState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, NormalState, mProfile);

	if(contentRect.isValidRect())
	{
		renderText(contentRect.point, contentRect.extent, mText, mProfile);

		//Render the childen
		renderChildControls(offset, contentRect, updateRect);
	}
}

RectI GuiControl::applyMargins(Point2I &offset, Point2I &extent, GuiControlState currentState, GuiControlProfile *profile)
{
	//Get the border profiles
	GuiBorderProfile *leftProfile = profile->getLeftBorder();
	GuiBorderProfile *rightProfile = profile->getRightBorder();
	GuiBorderProfile *topProfile = profile->getTopBorder();
	GuiBorderProfile *bottomProfile = profile->getBottomBorder();

	S32 leftSize = (leftProfile) ? leftProfile->getMargin(currentState) : 0;
	S32 rightSize = (rightProfile) ? rightProfile->getMargin(currentState) : 0;
	S32 topSize = (topProfile) ? topProfile->getMargin(currentState) : 0;
	S32 bottomSize = (bottomProfile) ? bottomProfile->getMargin(currentState) : 0;

	return RectI(offset.x + leftSize, offset.y + topSize, (extent.x - leftSize) - rightSize, (extent.y - topSize) - bottomSize);
}

RectI GuiControl::applyBorders(Point2I &offset, Point2I &extent, GuiControlState currentState, GuiControlProfile *profile)
{
	//Get the border profiles
	GuiBorderProfile *leftProfile = profile->getLeftBorder();
	GuiBorderProfile *rightProfile = profile->getRightBorder();
	GuiBorderProfile *topProfile = profile->getTopBorder();
	GuiBorderProfile *bottomProfile = profile->getBottomBorder();

	S32 leftSize = (leftProfile) ? leftProfile->getBorder(currentState) : 0;
	S32 rightSize = (rightProfile) ? rightProfile->getBorder(currentState) : 0;
	S32 topSize = (topProfile) ? topProfile->getBorder(currentState) : 0;
	S32 bottomSize = (bottomProfile) ? bottomProfile->getBorder(currentState) : 0;

	return RectI(offset.x + leftSize, offset.y + topSize, (extent.x - leftSize) - rightSize, (extent.y - topSize) - bottomSize);
}

RectI GuiControl::applyPadding(Point2I &offset, Point2I &extent, GuiControlState currentState, GuiControlProfile *profile)
{
	//Get the border profiles
	GuiBorderProfile *leftProfile = profile->getLeftBorder();
	GuiBorderProfile *rightProfile = profile->getRightBorder();
	GuiBorderProfile *topProfile = profile->getTopBorder();
	GuiBorderProfile *bottomProfile = profile->getBottomBorder();

	S32 leftSize = (leftProfile) ? leftProfile->getPadding(currentState) : 0;
	S32 rightSize = (rightProfile) ? rightProfile->getPadding(currentState) : 0;
	S32 topSize = (topProfile) ? topProfile->getPadding(currentState) : 0;
	S32 bottomSize = (bottomProfile) ? bottomProfile->getPadding(currentState) : 0;

	return RectI(offset.x + leftSize, offset.y + topSize, (extent.x - leftSize) - rightSize, (extent.y - topSize) - bottomSize);
}

RectI GuiControl::getInnerRect(GuiControlState currentState)
{
	return getInnerRect(mBounds.point, mBounds.extent, currentState, mProfile);
}

RectI GuiControl::getInnerRect(Point2I& offset, GuiControlState currentState)
{
	return getInnerRect(offset, mBounds.extent, currentState, mProfile);
}

RectI GuiControl::getInnerRect(Point2I &offset, Point2I &extent, GuiControlState currentState, GuiControlProfile *profile)
{
	if (!profile)
	{
		return mBounds;
	}
	//Get the border profiles
	GuiBorderProfile *leftProfile = profile->getLeftBorder();
	GuiBorderProfile *rightProfile = profile->getRightBorder();
	GuiBorderProfile *topProfile = profile->getTopBorder();
	GuiBorderProfile *bottomProfile = profile->getBottomBorder();

	S32 leftSize = (leftProfile) ? leftProfile->getMargin(currentState) + leftProfile->getBorder(currentState) + leftProfile->getPadding(currentState) : 0;
	S32 rightSize = (rightProfile) ? rightProfile->getMargin(currentState) + rightProfile->getBorder(currentState) + rightProfile->getPadding(currentState) : 0;
	S32 topSize = (topProfile) ? topProfile->getMargin(currentState) + topProfile->getBorder(currentState) + topProfile->getPadding(currentState) : 0;
	S32 bottomSize = (bottomProfile) ? bottomProfile->getMargin(currentState) + bottomProfile->getBorder(currentState) + bottomProfile->getPadding(currentState) : 0;

	return RectI(offset.x + leftSize, offset.y + topSize, (extent.x - leftSize) - rightSize, (extent.y - topSize) - bottomSize);
}

Point2I GuiControl::getOuterExtent(Point2I &innerExtent, GuiControlState currentState, GuiControlProfile *profile)
{
    return Point2I(getOuterWidth(innerExtent.x, currentState, profile), getOuterHeight(innerExtent.y, currentState, profile));
}

S32 GuiControl::getOuterWidth(S32 innerWidth, GuiControlState currentState, GuiControlProfile* profile)
{
    //Get the border profiles
    GuiBorderProfile* leftProfile = profile->getLeftBorder();
    GuiBorderProfile* rightProfile = profile->getRightBorder();

    S32 leftSize = (leftProfile) ? leftProfile->getMargin(currentState) + leftProfile->getBorder(currentState) + leftProfile->getPadding(currentState) : 0;
    S32 rightSize = (rightProfile) ? rightProfile->getMargin(currentState) + rightProfile->getBorder(currentState) + rightProfile->getPadding(currentState) : 0;

    return innerWidth + leftSize + rightSize;
}

S32 GuiControl::getOuterHeight(S32 innerHeight, GuiControlState currentState, GuiControlProfile* profile)
{
    //Get the border profiles
    GuiBorderProfile* topProfile = profile->getTopBorder();
    GuiBorderProfile* bottomProfile = profile->getBottomBorder();

    S32 topSize = (topProfile) ? topProfile->getMargin(currentState) + topProfile->getBorder(currentState) + topProfile->getPadding(currentState) : 0;
    S32 bottomSize = (bottomProfile) ? bottomProfile->getMargin(currentState) + bottomProfile->getBorder(currentState) + bottomProfile->getPadding(currentState) : 0;

    return innerHeight + topSize + bottomSize;
}

GuiControlProfile* GuiControl::resolveDefaultTooltipProfile()
{
	// A themed control's tips belong to that theme, not to whichever module
	// happened to define a profile called GuiTooltipProfile.
	GuiProfileTheme* theme = (mProfile != NULL) ? mProfile->getTheme() : NULL;
	if (theme != NULL)
	{
		GuiControlProfile* themed = theme->getProfile(StringTable->insert("Tooltip"));
		if (themed != NULL)
			return themed;
	}

	GuiControlProfile* global = dynamic_cast<GuiControlProfile*>(Sim::findObject("GuiTooltipProfile"));
	if (global != NULL)
		return global;

	return dynamic_cast<GuiControlProfile*>(Sim::findObject("GuiDefaultProfile"));
}

// Wrap one paragraph of tooltip text to maxWidth, appending to lines and
// returning the new line count. This is the word wrapping renderTooltip has
// always done, lifted out so it can run once per paragraph -- a tooltip splits
// on line breaks first now, which is what lets one carry a heading and an
// explanation of what it means on separate lines.
//
// Not GuiControl::getLineList: that wraps only when the CONTROL wraps, and a
// tooltip has to wrap whatever the control it belongs to does.
static S32 wrapTooltipParagraph(const char* paragraph, GFont* font, const S32 maxWidth,
    const S32 spaceWidth, FrameTemp<StringBuffer>& lines, S32 lineCount,
    const S32 maxLines, S32& widestLine)
{
    const S32 wordCount = StringUnit::getUnitCount( paragraph, " " );
    S32 lineWidth = 0;
    S32 wordStartIndex = 0;
    S32 wordEndIndex = 0;

    while( true )
    {
        // Do we have any words left?
        if ( wordEndIndex < wordCount )
        {
            // Yes, so fetch the word.
            const char* pWord = StringUnit::getUnit( paragraph, wordEndIndex, " " );

            // Add word length.
            const S32 wordLength = (S32)font->getStrWidth( pWord ) + spaceWidth;

            // Do we still have room?
            if ( (lineWidth + wordLength) < maxWidth )
            {
                // Yes, so add word length.
                lineWidth += wordLength;

                // Next word.
                wordEndIndex++;

                continue;
            }

            // Do we have any lines left?
            if ( lineCount < maxLines )
            {
                // Yes, so insert line.
                lines[lineCount++] = StringUnit::getUnits( paragraph, wordStartIndex, wordEndIndex-1, " " );

                // Update horizontal text bounds.
                if ( lineWidth > widestLine )
                    widestLine = lineWidth;
            }

            // Set new line length.
            lineWidth = wordLength;

            // Set word start.
            wordStartIndex = wordEndIndex;

            // Next word.
            wordEndIndex++;

            continue;
        }

        // Do we have any words left?
        if ( wordStartIndex < wordCount )
        {
            // Yes, so do we have any lines left?
            if ( lineCount < maxLines )
            {
                // Yes, so insert line.
                lines[lineCount++] = StringUnit::getUnits( paragraph, wordStartIndex, wordCount-1, " " );

                // Update horizontal text bounds.
                if ( lineWidth > widestLine )
                    widestLine = lineWidth;
            }
        }

        break;
    }

    return lineCount;
}

bool GuiControl::renderTooltip(Point2I &cursorPos, const char* tipText )
{
#if !defined(TORQUE_OS_IOS) && !defined(TORQUE_OS_ANDROID) && !defined(TORQUE_OS_EMSCRIPTEN)
    // Short Circuit.
    if (!mAwake) 
        return false;

    if ( dStrlen( mTooltip ) == 0 && ( tipText == NULL || dStrlen( tipText ) == 0 ) )
        return false;

    const char* renderTip = mTooltip;
    if( tipText != NULL )
        renderTip = tipText;

    // Finish if no root.
    GuiCanvas *root = getRoot();
    if ( !root )
        return false;

    // A control that was never given a tooltip profile picks one for itself the
    // first time it actually draws a tip, and keeps checking that the pick is
    // still right: re-theming a control rewrites its Profile field directly, so
    // there is no setter to hook -- resolving here, per draw, is what makes a tip
    // follow its control from one theme to the next. Only our own pick is
    // revisited; a profile someone set deliberately is left alone.
    //
    // The reference has to be taken here rather than left to onWake, which has
    // already run by now (see the mAwake check above) with nothing to count.
    // onSleep decrements whatever it finds, so a control that acquired a profile
    // mid-wake without this would sleep one reference short and trip the
    // zero-ref-count assert. TypeGuiProfile deliberately leaves the book-keeping
    // to GuiControl.
    if (mTooltipProfile == NULL || mTooltipProfile == mLazyTooltipProfile)
    {
		GuiControlProfile* wanted = resolveDefaultTooltipProfile();

		if (wanted != mTooltipProfile)
		{
			if (mTooltipProfile != NULL)
				mTooltipProfile->decRefCount();

			mTooltipProfile = wanted;

			if (mTooltipProfile != NULL)
				mTooltipProfile->incRefCount();
		}

		mLazyTooltipProfile = mTooltipProfile;
    }

    // Nothing to draw a tip with, which is a missing profile rather than an
    // error worth stopping for.
    if (mTooltipProfile == NULL)
        return false;

    GFont *font = mTooltipProfile->getFont();
   
    // Set text bounds.
    Point2I textBounds( 0, 0 );

    // Fetch the width of a space.
    const S32 spaceWidth = (S32)font->getStrWidth(" ");

    // Fetch the maximum allowed tooltip extent.
    const S32 maxTooltipWidth = mTooltipWidth;

    // Reset line storage.
    const S32 tooltipLineStride = (S32)font->getHeight() + 4;
    const S32 maxTooltipLines = 20;
    S32 tooltipLineCount = 0;
    FrameTemp<StringBuffer> tooltipLines( maxTooltipLines );

    // Paragraph by paragraph, wrapping each to the tooltip width. Breaking on
    // newlines first is what lets a tip say what a thing is on one line and
    // what it does on the next.
    const string tip(renderTip);
    string::size_type paragraphStart = 0;
    while ( paragraphStart <= tip.length() )
    {
        const string::size_type breakAt = tip.find('\n', paragraphStart);
        const string paragraph = (breakAt == string::npos)
            ? tip.substr(paragraphStart)
            : tip.substr(paragraphStart, breakAt - paragraphStart);

        tooltipLineCount = wrapTooltipParagraph( paragraph.c_str(), font, maxTooltipWidth,
            spaceWidth, tooltipLines, tooltipLineCount, maxTooltipLines, textBounds.x );

        if ( breakAt == string::npos )
            break;

        paragraphStart = breakAt + 1;
    }

    // Controls the size of the inside (gutter) tooltip region.
    const S32 tooltipGutterSize = 5;

    // Adjust text bounds.
    textBounds.x += tooltipGutterSize * 2;
    textBounds.y = (((S32)font->getHeight() + 4) * tooltipLineCount - 4) + (tooltipGutterSize * 2);

    // Adjust to tooltip is always on-screen.
    Point2I screensize = Platform::getWindowSize();
    Point2I offset = cursorPos; 
    offset.y += 22;
    if (screensize.x < (offset.x + textBounds.x))
        offset.x = screensize.x - textBounds.x;
    if(screensize.y < (offset.y + textBounds.y) )
        offset.y = screensize.y - textBounds.y;

    // Fetch the old clip.
    RectI oldClip = dglGetClipRect();

    // Set rectangle for the box, and set the clip rectangle.
    RectI rect(offset, textBounds);
    dglSetClipRect(rect);

    // Draw body and border of the tool tip
	renderUniversalRect(rect, mTooltipProfile, NormalState);

    // Draw the text centered in the tool tip box
    dglSetBitmapModulation( mTooltipProfile->mFontColor );
    Point2I start( tooltipGutterSize, tooltipGutterSize );
    for ( S32 lineIndex = 0; lineIndex < tooltipLineCount; lineIndex++ )
    {
        dglDrawText( font, start + offset, tooltipLines[lineIndex].getPtr8(), mProfile->mFontColors );
        offset.y += tooltipLineStride;
    }

    dglSetClipRect( oldClip );
#endif
    return true;
}

void GuiControl::renderChildControls(const Point2I& offset, const RectI& content, const RectI& updateRect)
{
   // offset is the upper-left corner of this control in screen coordinates. It should almost always be the same offset passed into the onRender method.
   // updateRect is the area that this control was allowed to draw in. It should almost always be the same as the value in onRender.
   // content is the area that child controls are allowed to draw in.
   RectI clipRect = content;
   if(clipRect.intersect(dglGetClipRect()))
   {
	   S32 size = objectList.size();
	   S32 size_cpy = size;
		//-Mat look through our vector all normal-like, trying to use an iterator sometimes gives us
	   //bad cast on good objects
	   for( S32 count = 0; count < objectList.size(); count++ )
	   {
		  GuiControl *ctrl = (GuiControl *)objectList[count];
		  if( ctrl == NULL ) {
			  Con::errorf( "GuiControl::renderChildControls() object %i is NULL", count );
			continue;
		  }
		  if (ctrl->mVisible && !isHiddenInEditor(ctrl))
		  {
			 renderChild(ctrl, offset, content, clipRect);
		  }
		  size_cpy = objectList.size(); //	CHRIS: i know its wierd but the size of the list changes sometimes during execution of this loop
		  if(size != size_cpy)
		  {
			  size = size_cpy;
			  count--;	//	CHRIS: just to make sure one wasnt skipped.
		  }
	   }
   }
}

void GuiControl::renderChild(GuiControl* ctrl, const Point2I& offset, const RectI& content, const RectI& clipRect)
{
	ctrl->mRenderInsetLT = content.point - offset;
	ctrl->mRenderInsetRB = mBounds.extent - (ctrl->mRenderInsetLT + content.extent);
	Point2I childPosition = content.point + ctrl->getPosition();
	RectI childClip(childPosition, ctrl->getExtent());

	if (childClip.intersect(clipRect))
	{
		RectI old = dglGetClipRect();
		dglSetClipRect(clipRect);
		glDisable(GL_CULL_FACE);
		ctrl->onRender(childPosition, RectI(childPosition, ctrl->getExtent()));
		dglSetClipRect(old);
	}
}

void GuiControl::setUpdateRegion(Point2I pos, Point2I ext)
{
   Point2I upos = localToGlobalCoord(pos);
   GuiCanvas *root = getRoot();
   if (root)
   {
      root->addUpdateRegion(upos, ext);
   }
}

void GuiControl::setUpdate()
{
   setUpdateRegion(Point2I(0,0), mBounds.extent);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiControl::awaken()
{
   AssertFatal(!mAwake, "GuiControl::awaken: control is already awake");
   if(mAwake)
      return;

   iterator i;
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);

      AssertFatal(!ctrl->isAwake(), "GuiControl::awaken: child control is already awake");
      if(!ctrl->isAwake())
         ctrl->awaken();
   }

   AssertFatal(!mAwake, "GuiControl::awaken: should not be awake here");
   if(!mAwake)
   {
      if(!onWake())
      {
         Con::errorf(ConsoleLogEntry::General, "GuiControl::awaken: failed onWake for obj: %s", getName());
         AssertFatal(0, "GuiControl::awaken: failed onWake");
         deleteObject();
      }
	  else
	  {
		  if (mTextID && *mTextID != 0)
			  setTextID(mTextID);
	  }
   }
}

void GuiControl::sleep()
{
   AssertFatal(mAwake, "GuiControl::sleep: control is not awake");
   if(!mAwake)
      return;

   iterator i;
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);

      AssertFatal(ctrl->isAwake(), "GuiControl::sleep: child control is already asleep");
      if(ctrl->isAwake())
         ctrl->sleep();
   }

   AssertFatal(mAwake, "GuiControl::sleep: should not be asleep here");
   if(mAwake)
      onSleep();
}

void GuiControl::preRender()
{
   AssertFatal(mAwake, "GuiControl::preRender: control is not awake");
   if(!mAwake)
      return;

   iterator i;
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      if (ctrl->isVisible())
      {
          ctrl->preRender();
      }
   }
   fitToText();
   onPreRender();
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

bool GuiControl::onWake()
{
   AssertFatal( !mAwake, "GuiControl::onWake: control is already awake" );
   if( mAwake )
      return false;

   // [tom, 4/18/2005] Cause mLangTable to be refreshed in case it was changed
   mLangTable = NULL;

   // Grab the classname of this object
   const char *cName = getClassName();

   //make sure we have a profile
   if( !mProfile )
   {
      // Ensure the classname is a valid name...
      if( cName && cName[0] )
      {
         S32 pos = 0;

         for( pos = 0; pos <= (S32)dStrlen( cName ); pos++ )
            if( !dStrncmp( cName + pos, "Ctrl", 4 ) )
               break;

         if( pos != 0 ) {
            char buff[255];
            dStrncpy( buff, cName, pos );
            buff[pos] = '\0';
            dStrcat( buff, "Profile\0" );

            SimObject *obj = Sim::findObject( buff );

            if( obj )
               mProfile = dynamic_cast<GuiControlProfile*>( obj );
         }
      }

      // Ok lets check to see if that worked
      if( !mProfile ) {
         SimObject *obj = Sim::findObject( "GuiDefaultProfile" );

         if( obj )
            mProfile = dynamic_cast<GuiControlProfile*>(obj);
      }

      AssertFatal( mProfile, avar( "GuiControl: %s created with no profile.", getName() ) );
   }

   //set the flag
   mAwake = true;

   //set the layer
   GuiCanvas *root = getRoot();
   AssertFatal(root, "Unable to get the root Canvas.");
   GuiControl *parent = getParent();
   if (parent && parent != root)
      mLayer = parent->mLayer;

   //make sure the first responder exists
   if (! mFirstResponder)
      mFirstResponder = findFirstTabable();

   //see if we should force this control to be the first responder
   //if (mProfile->mTabable && mProfile->mCanKeyFocus)
   //   setFirstResponder();

   //increment the profile
   mProfile->incRefCount();

   // Now rather than when it is first drawn, which for a control out of view
   // may be never. See fitToText.
   fitToText();

   // Only invoke script callbacks if we have a namespace in which to do so
   // This will suppress warnings
   if( isMethod("onWake") )
      Con::executef(this, 1, "onWake");

   if (mTooltipProfile != NULL)
	   mTooltipProfile->incRefCount();

   return true;
}

void GuiControl::onSleep()
{
   AssertFatal(mAwake, "GuiControl::onSleep: control is not awake");
   if(!mAwake)
      return;

   //decrement the profile referrence
   if( mProfile != NULL )
      mProfile->decRefCount();

   // The profile's last reference takes its fonts with it, and one loaded on
   // the next wake can come back at the same address.
   mFittedWidth = -1;
   clearFirstResponder();
   mouseUnlock();

   // Only invoke script callbacks if we have a namespace in which to do so
   // This will suppress warnings
   if( isMethod("onSleep") )
      Con::executef(this, 1, "onSleep");

   if (mTooltipProfile != NULL)
	   mTooltipProfile->decRefCount();

   // Set Flag
   mAwake = false;
}

void GuiControl::setControlTooltipProfile(GuiControlProfile *prof)
{
   if(prof == mTooltipProfile)
      return;
   if(mAwake && mTooltipProfile != NULL)
      mTooltipProfile->decRefCount();
   mTooltipProfile = prof;

   // A profile handed over deliberately is not ours to revisit when the control
   // is re-themed, so drop any claim renderTooltip had on the old one.
   mLazyTooltipProfile = NULL;

   if(mAwake && mTooltipProfile != NULL)
      mTooltipProfile->incRefCount();
}

void GuiControl::setControlProfile(GuiControlProfile *prof)
{
   AssertFatal(prof, "GuiControl::setControlProfile: invalid profile");
   if(prof == mProfile)
      return;
   if(mAwake)
      mProfile->decRefCount();
   mProfile = prof;
   if(mAwake)
      mProfile->incRefCount();
}

void GuiControl::onPreRender()
{
   // do nothing.
}
//-----------------------------------------------------------------------------
//	checks up the parent hierarchy - if anyone above us is not savable returns false
//	otherwise, returns true.
//-----------------------------------------------------------------------------
bool	GuiControl::getCanSaveParent()
{
   GuiControl *walk = this;
   while(walk)
   {
      if(!walk->getCanSave())
         return false;

      walk = walk->getParent();
   }

   return true;
}

//-----------------------------------------------------------------------------
//	Can we Save to a TorqueScript file?
//-----------------------------------------------------------------------------
bool GuiControl::getCanSave()
{
   return mCanSave;
}

//-----------------------------------------------------------------------------
//	Sets whether we can save out to a file (TorqueScript)
//-----------------------------------------------------------------------------
void GuiControl::setCanSave(bool bCanSave)
{
   mCanSave	=	bCanSave;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//	checks out mCanSave flag, if true just passes along to our parent, 
//	if false, then we return without writing.  Note, also, that 
//	if our parent is not writeable, then we should not be writable...
////////////////////////////////////////////////////////////////////////////////////////////////////
void GuiControl::write(Stream &stream, U32 tabStop, U32 flags)
{
   //note: this will return false if either we, or any of our parents, are non-save controls
   bool bCanSave	=	getCanSaveParent();
   if(bCanSave)
   {
      Parent::write(stream, tabStop, flags);
   }

}




//This is only called if the control is deleted, not when the control is removed from its parent.
void GuiControl::onRemove()
{
	Parent::onRemove();
}

//For GuiControls, this will always just before it is actually removed.
void GuiControl::onGroupRemove()
{
	clearFirstResponder();
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

const char *GuiControl::getScriptValue()
{
   return NULL;
}

void GuiControl::setScriptValue(const char *value)
{
}

void GuiControl::setConsoleVariable(const char *variable)
{
   if (variable)
   {
      mConsoleVariable = StringTable->insert(variable);
   }
   else
   {
      mConsoleVariable = StringTable->EmptyString;
   }
}
  
//-----------------------------------------------------------------------------
//	finds and returns the first immediate child of ours whose
//	internal name matches the passed String. returns Null if not found.
//-----------------------------------------------------------------------------
void GuiControl::setConsoleCommand(const char *newCmd)
{
   if (newCmd)
      mConsoleCommand = StringTable->insert(newCmd);
   else
      mConsoleCommand = StringTable->EmptyString;
}

const char * GuiControl::getConsoleCommand()
{
    return mConsoleCommand;
}

void GuiControl::setSizing(S32 horz, S32 vert)
{
    mHorizSizing = horz;
    mVertSizing = vert;
}


void GuiControl::setVariable(const char *value)
{
   if (mConsoleVariable[0])
      Con::setVariable(mConsoleVariable, value);
}

void GuiControl::setIntVariable(S32 value)
{
   if (mConsoleVariable[0])
      Con::setIntVariable(mConsoleVariable, value);
}

void GuiControl::setFloatVariable(F32 value)
{
   if (mConsoleVariable[0])
      Con::setFloatVariable(mConsoleVariable, value);
}

const char * GuiControl::getVariable()
{
   if (mConsoleVariable[0])
      return Con::getVariable(mConsoleVariable);
   else return NULL;
}

S32 GuiControl::getIntVariable()
{
   if (mConsoleVariable[0])
      return Con::getIntVariable(mConsoleVariable);
   else return 0;
}

F32 GuiControl::getFloatVariable()
{
   if (mConsoleVariable[0])
      return Con::getFloatVariable(mConsoleVariable);
   else return 0.0f;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

bool GuiControl::cursorInControl()
{
   GuiCanvas *root = getRoot();
   if (! root) return false;

   Point2I pt = root->getCursorPos();
   Point2I offset = localToGlobalCoord(Point2I(0, 0));
   if (pt.x >= offset.x && pt.y >= offset.y &&
      pt.x < offset.x + mBounds.extent.x && pt.y < offset.y + mBounds.extent.y)
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool GuiControl::pointInControl(const Point2I& parentCoordPoint)
{
   S32 xt = parentCoordPoint.x - mBounds.point.x;
   S32 yt = parentCoordPoint.y - mBounds.point.y;
   return xt >= 0 && yt >= 0 && xt < mBounds.extent.x && yt < mBounds.extent.y;
}


// A hidden child is skipped here and not descended into, which is what takes its
// whole subtree with it - the same thing renderChildControls does, and it has to
// be the same thing, or the Gui Editor's eye would take a control out of sight
// while leaving it in the way of every click aimed at what is behind it.
GuiControl* GuiControl::findHitControl(const Point2I &pt, S32 initialLayer, const bool ignoreUseInput, const bool ignoreEditSelected)
{
   iterator i = end(); // find in z order (last to first)
   while (i != begin())
   {
      i--;
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      if (initialLayer >= 0 && ctrl->mLayer > initialLayer)
      {
         continue;
      }
      else if (ctrl->pointInControl(pt - ctrl->mRenderInsetLT) &&
		ctrl->mVisible &&
		!isHiddenInEditor(ctrl) &&
		(ignoreUseInput || ctrl->mUseInput) &&
		(ignoreEditSelected || (isEditMode() && !ctrl->isEditSelected())))
      {
         Point2I ptemp = pt - (ctrl->mBounds.point + ctrl->mRenderInsetLT);
         GuiControl *hitCtrl = ctrl->findHitControl(ptemp, -1, ignoreUseInput, ignoreEditSelected);

         if(ignoreUseInput || hitCtrl->mUseInput)
            return hitCtrl;
      }
   }
   return this;
}

bool GuiControl::handleTouchDown(const GuiEvent& event, const Point2I& pt, S32 initialLayer)
{
	bool keepGoing = true;
	iterator i = end(); // find in z order (last to first)
	while (i != begin())
	{
		i--;
		GuiControl* ctrl = static_cast<GuiControl*>(*i);
		if (initialLayer >= 0 && ctrl->mLayer > initialLayer)
		{
			continue;
		}
		else if (ctrl->pointInControl(pt - ctrl->mRenderInsetLT) && ctrl->mVisible && ctrl->mUseInput)
		{
			Point2I ptemp = pt - (ctrl->mBounds.point + ctrl->mRenderInsetLT);
			keepGoing = ctrl->handleTouchDown(event, ptemp) && ctrl->mAllowEventPassThru;
		}

		if (!keepGoing)
		{
			break;
		}
	}
	if (keepGoing)
	{
		mPassEventThru = false;
		onTouchDown(event);
		keepGoing = mPassEventThru;
	}
	return keepGoing;
}

bool GuiControl::handleTouchUp(const GuiEvent& event, const Point2I& pt, S32 initialLayer)
{
	bool keepGoing = true;
	iterator i = end(); // find in z order (last to first)
	while (i != begin())
	{
		i--;
		GuiControl* ctrl = static_cast<GuiControl*>(*i);
		if (initialLayer >= 0 && ctrl->mLayer > initialLayer)
		{
			continue;
		}
		else if (ctrl->pointInControl(pt - ctrl->mRenderInsetLT) && ctrl->mVisible && ctrl->mUseInput)
		{
			Point2I ptemp = pt - (ctrl->mBounds.point + ctrl->mRenderInsetLT);
			keepGoing = ctrl->handleTouchUp(event, ptemp) && ctrl->mAllowEventPassThru;
		}

		if (!keepGoing)
		{
			break;
		}
	}
	if (keepGoing)
	{
		mPassEventThru = false;
		onTouchUp(event);
		keepGoing = mPassEventThru;
	}
	return keepGoing;
}

bool GuiControl::handleTouchMove(const GuiEvent& event, const Point2I& pt, S32 initialLayer)
{
	bool keepGoing = true;
	iterator i = end(); // find in z order (last to first)
	while (i != begin())
	{
		i--;
		GuiControl* ctrl = static_cast<GuiControl*>(*i);
		if (initialLayer >= 0 && ctrl->mLayer > initialLayer)
		{
			continue;
		}
		else if (ctrl->pointInControl(pt - ctrl->mRenderInsetLT) && ctrl->mVisible && ctrl->mUseInput)
		{
			Point2I ptemp = pt - (ctrl->mBounds.point + ctrl->mRenderInsetLT);
			keepGoing = ctrl->handleTouchMove(event, ptemp) && ctrl->mAllowEventPassThru;
		}

		if (!keepGoing)
		{
			break;
		}
	}
	if (keepGoing)
	{
		mPassEventThru = false;
		onTouchMove(event);
		keepGoing = mPassEventThru;
	}
	return keepGoing;
}

bool GuiControl::isMouseLocked()
{
	if (isEditMode())
	{
		return smEditorHandle->editIsMouseLocked(this);
	}

   GuiCanvas *root = getRoot();
   return root ? root->getMouseLockedControl() == this : false;
}

void GuiControl::mouseLock(GuiControl *lockingControl)
{
	if (isEditMode())
	{
		smEditorHandle->editMouseLock(lockingControl);
		return;
	}

   GuiCanvas *root = getRoot();
   if (root)
      root->mouseLock(lockingControl);
}

void GuiControl::mouseLock()
{
   mouseLock(this);
}

void GuiControl::mouseUnlock()
{
	if (isEditMode())
	{
		smEditorHandle->editMouseUnlock();
		return;
	}
   GuiCanvas *root = getRoot();
   if (root)
      root->mouseUnlock(this);
}

bool GuiControl::sendScriptMouseEvent(const char* name, const GuiEvent& event)
{
    bool consumed = false;
    if (isMethod(name))
    {
        char buf[3][32];
        dSprintf(buf[0], 32, "%d", event.modifier);
        dSprintf(buf[1], 32, "%d %d", event.mousePoint.x, event.mousePoint.y);
        dSprintf(buf[2], 32, "%d", event.mouseClickCount);
        consumed = dAtob(Con::executef(this, 4, name, buf[0], buf[1], buf[2]));
    }
    return consumed;
}

bool GuiControl::sendScriptKeyEvent(const char* name, const InputEvent& event)
{
    bool consumed = false;
    if (isMethod(name))
    {
        char buf[2][32];
        dSprintf(buf[0], 32, "%d", event.modifier);
        if (!ActionMap::getKeyString(event.objInst, buf[1]))
            return(false);
        consumed = dAtob(Con::executef(this, 3, name, buf[0], buf[1]));
    }
    return consumed;
}

bool GuiControl::onInputEvent(const InputEvent &event)
{
    if (event.objType == SI_KEY)
    {
        if (event.action == SI_MAKE)
        {
            return sendScriptKeyEvent("onKeyDown", event);
        }
        else if (event.action == SI_BREAK)
        {
            return sendScriptKeyEvent("onKeyUp", event);
        }
        else if (event.action == SI_REPEAT)
        {
            return sendScriptKeyEvent("onKeyRepeat", event);
        }
    }

    return false;
}

void GuiControl::onTouchUp(const GuiEvent &event)
{
    if (!mVisible || !mAwake)
        return;

    bool consumed1 = sendScriptMouseEvent("onTouchUp", event);
    bool consumed2 = sendScriptMouseEvent("onMouseUp", event);

    GuiControl* parent = getParent();
    if (parent && !consumed1 && !consumed2)
        parent->onTouchUp(event);
}

void GuiControl::onTouchDown(const GuiEvent &event)
{
    if (!mVisible || !mAwake)
        return;

    bool consumed1 = sendScriptMouseEvent("onTouchDown", event);
    bool consumed2 = sendScriptMouseEvent("onMouseDown", event);

    GuiControl* parent = getParent();
    if (parent && !consumed1 && !consumed2)
        parent->onTouchDown(event);
}

void GuiControl::onTouchMove(const GuiEvent &event)
{
   if ( !mVisible || !mAwake )
      return;

   bool consumed1 = sendScriptMouseEvent("onTouchMove", event);
   bool consumed2 = sendScriptMouseEvent("onMouseMove", event);

   GuiControl *parent = getParent();
   if (parent && !consumed1 && !consumed2)
      parent->onTouchMove( event );
}

void GuiControl::onTouchDragged(const GuiEvent &event)
{
    if (!mVisible || !mAwake)
        return;

    bool consumed1 = sendScriptMouseEvent("onTouchDragged", event);
    bool consumed2 = sendScriptMouseEvent("onMouseDragged", event);

    GuiControl* parent = getParent();
    if (parent && !consumed1 && !consumed2)
        parent->onTouchDragged(event);
}

void GuiControl::onTouchEnter(const GuiEvent &event)
{
    if (!mVisible || !mAwake)
        return;

    sendScriptMouseEvent("onTouchEnter", event);
    sendScriptMouseEvent("onMouseEnter", event);

    //Entering a child means nothing to a parent
}

void GuiControl::onTouchLeave(const GuiEvent &event)
{
    if (!mVisible || !mAwake)
        return;

    sendScriptMouseEvent("onTouchLeave", event);
    sendScriptMouseEvent("onMouseLeave", event);

    //Leaving a child means nothing to a parent
}

void GuiControl::onMouseWheelUp( const GuiEvent &event )
{
   if ( !mVisible || !mAwake )
      return;

   bool consumed = sendScriptMouseEvent("onMouseWheelUp", event);

   GuiControl *parent = getParent();
   if (parent && !consumed)
      return parent->onMouseWheelUp(event);
}

void GuiControl::onMouseWheelDown( const GuiEvent &event )
{
   if ( !mVisible || !mAwake )
      return;

   bool consumed = sendScriptMouseEvent("onMouseWheelDown", event);

   GuiControl *parent = getParent();
   if (parent && !consumed)
      return parent->onMouseWheelDown(event);
}

void GuiControl::onRightMouseDown(const GuiEvent &event)
{
    if (!mVisible || !mAwake)
        return;

    bool consumed = sendScriptMouseEvent("onRightMouseDown", event);

    GuiControl* parent = getParent();
    if (parent && !consumed)
        parent->onRightMouseDown(event);
}

void GuiControl::onRightMouseUp(const GuiEvent &event)
{
    if (!mVisible || !mAwake)
        return;

    bool consumed = sendScriptMouseEvent("onRightMouseUp", event);

    GuiControl* parent = getParent();
    if (parent && !consumed)
        parent->onRightMouseUp(event);
}

void GuiControl::onRightMouseDragged(const GuiEvent &event)
{
    if (!mVisible || !mAwake)
        return;

    bool consumed = sendScriptMouseEvent("onRightMouseDragged", event);

    GuiControl* parent = getParent();
    if (parent && !consumed)
        parent->onRightMouseDragged(event);
}

void GuiControl::onMiddleMouseDown(const GuiEvent &event)
{
    if (!mVisible || !mAwake)
        return;

    bool consumed = sendScriptMouseEvent("onMiddleMouseDown", event);

    GuiControl* parent = getParent();
    if (parent && !consumed)
        parent->onMiddleMouseDown(event);
}

void GuiControl::onMiddleMouseUp(const GuiEvent &event)
{
    if (!mVisible || !mAwake)
        return;

    bool consumed = sendScriptMouseEvent("onMiddleMouseUp", event);

    GuiControl* parent = getParent();
    if (parent && !consumed)
        parent->onMiddleMouseUp(event);
}

void GuiControl::onMiddleMouseDragged(const GuiEvent &event)
{
    if (!mVisible || !mAwake)
        return;

    bool consumed = sendScriptMouseEvent("onMiddleMouseDragged", event);

    GuiControl* parent = getParent();
    if (parent && !consumed)
        parent->onMiddleMouseDragged(event);
}


GuiControl* GuiControl::findFirstTabable()
{
   GuiControl *tabCtrl = NULL;
   if(mVisible && mAwake)
   {
	   iterator i;
	   for (i = begin(); i != end(); i++)
	   {
		  GuiControl *ctrl = static_cast<GuiControl *>(*i);
		  tabCtrl = ctrl->findFirstTabable();
		  if (tabCtrl)
		  {
			 mFirstResponder = tabCtrl;
			 return tabCtrl;
		  }
	   }
   }

   //nothing was found, therefore, see if this ctrl is tabable
   return ( mProfile != NULL ) ? ( ( mProfile->mTabable && mAwake && mVisible ) ? this : NULL ) : NULL;
}

GuiControl* GuiControl::findLastTabable(bool firstCall)
{
   //if this is the first call, clear the global
   if (firstCall)
      smPrevResponder = NULL;

   //if this control is tabable, set the global
   if (mProfile->mTabable)
      smPrevResponder = this;

	if(mVisible && mAwake)
	{
	   iterator i;
	   for (i = begin(); i != end(); i++)
	   {
		  GuiControl *ctrl = static_cast<GuiControl *>(*i);
		  ctrl->findLastTabable(false);
	   }
   }

   //after the entire tree has been traversed, return the last responder found
   mFirstResponder = smPrevResponder;
   return smPrevResponder;
}

GuiControl *GuiControl::findNextTabable(GuiControl *curResponder, bool firstCall)
{
   //if this is the first call, clear the global
   if (firstCall)
      smCurResponder = NULL;

   //first find the current responder
   if (curResponder == this)
      smCurResponder = this;

   //if the first responder has been found, return the very next *tabable* control
   else if ( smCurResponder && mProfile->mTabable && mAwake && mVisible && mActive )
      return( this );

   //loop through, checking each child to see if it is the one that follows the firstResponder
   GuiControl *tabCtrl = NULL;

   if (mVisible && mAwake)
   {
	   iterator i;
	   for (i = begin(); i != end(); i++)
	   {
		  GuiControl *ctrl = static_cast<GuiControl *>(*i);
		  tabCtrl = ctrl->findNextTabable(curResponder, false);
		  if (tabCtrl) break;
	   }
   }
   mFirstResponder = tabCtrl;
   return tabCtrl;
}

GuiControl *GuiControl::findPrevTabable(GuiControl *curResponder, bool firstCall)
{
   if (firstCall)
      smPrevResponder = NULL;

   //if this is the current reponder, return the previous one
   if (curResponder == this)
      return smPrevResponder;

   //else if this is a responder, store it in case the next found is the current responder
   else if ( mProfile->mTabable && mAwake && mVisible && mActive )
      smPrevResponder = this;

   //loop through, checking each child to see if it is the one that follows the firstResponder
   GuiControl *tabCtrl = NULL;

   if (mVisible && mAwake)
   {
	   iterator i;
	   for (i = begin(); i != end(); i++)
	   {
		  GuiControl *ctrl = static_cast<GuiControl *>(*i);
		  tabCtrl = ctrl->findPrevTabable(curResponder, false);
		  if (tabCtrl) break;
	   }
   }
   mFirstResponder = tabCtrl;
   return tabCtrl;
}

void GuiControl::onLoseFirstResponder()
{
    // Since many controls have visual cues when they are the firstResponder...
    setUpdate();

	if (isMethod("onLoseFirstResponder"))
	{
		Con::executef(this, 2, "onLoseFirstResponder");
	}
	else if (isMethod("onBlur"))
	{
		Con::executef(this, 2, "onBlur");
	}
}

bool GuiControl::ControlIsChild(GuiControl *child)
{
   //function returns true if this control, or one of it's children is the child control
   if (child == this)
      return true;

   //loop through, checking each child to see if it is ,or contains, the firstResponder
   iterator i;
   for (i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      if (ctrl->ControlIsChild(child)) return true;
   }

   //not found, therefore false
   return false;
}

void GuiControl::onFocus(bool foundFirstResponder)
{
	if (!foundFirstResponder && isFirstResponder())
	{
		foundFirstResponder = true;
	}

	//bubble the focus up
	GuiControl *parent = getParent();
	if (parent)
		parent->onFocus(foundFirstResponder);
}

bool GuiControl::isFirstResponder()
{
   GuiCanvas *root = getRoot();
   return root && root->getFirstResponder() == this;
}

void GuiControl::setFirstResponder( GuiControl* firstResponder )
{
   if ( firstResponder && firstResponder->mProfile && firstResponder->mProfile->mCanKeyFocus )
      mFirstResponder = firstResponder;

   GuiControl *parent = getParent();
   if ( parent )
      parent->setFirstResponder( firstResponder );
}

void GuiControl::setFirstResponder()
{
    if ( mAwake && mVisible )
    {
		GuiControl *parent = getParent();
		if (mProfile->mCanKeyFocus && parent )
		{
			parent->setFirstResponder(this);

			// Since many controls have visual cues when they are the firstResponder...
			this->setUpdate();	

			if (isMethod("onGainFirstResponder"))
			{
				Con::executef(this, 2, "onGainFirstResponder");
			}
			else if (isMethod("onFocus"))
			{
				Con::executef(this, 2, "onFocus");
			}
		}
    }
}

void GuiControl::clearFirstResponder()
{
	clearFirstResponder(this);
}

void GuiControl::clearFirstResponder(GuiControl* target)
{
   GuiControl *parent = this;
   while((parent = parent->getParent()) != NULL)
   {
      if(parent->mFirstResponder == target)
         parent->mFirstResponder = NULL;
      else
         break;
   }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiControl::buildAcceleratorMap()
{
   //add my own accel key
   addAcceleratorKey();

   //add all my childrens keys
   iterator i;
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      ctrl->buildAcceleratorMap();
   }
}

void GuiControl::addAcceleratorKey()
{
   //see if we have an accelerator
   if (mAcceleratorKey == StringTable->EmptyString)
      return;

   EventDescriptor accelEvent;
   ActionMap::createEventDescriptor(mAcceleratorKey, &accelEvent);

   //now we have a modifier, and a key, add them to the canvas
   GuiCanvas *root = getRoot();
   if (root)
      root->addAcceleratorKey(this, 0, accelEvent.eventCode, accelEvent.flags);
}

void GuiControl::acceleratorKeyPress(U32 index)
{
   onAction();
}

void GuiControl::acceleratorKeyRelease(U32 index)
{
   //do nothing
}

bool GuiControl::onKeyDown(const GuiEvent &event)
{
   //pass the event to the parent
   GuiControl *parent = getParent();
   if (parent)
      return parent->onKeyDown(event);
   else
      return false;
}

bool GuiControl::onKeyRepeat(const GuiEvent &event)
{
   // default to just another key down.
   return onKeyDown(event);
}

bool GuiControl::onKeyUp(const GuiEvent &event)
{
   //pass the event to the parent
   GuiControl *parent = getParent();
   if (parent)
      return parent->onKeyUp(event);
   else
      return false;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiControl::onAction()
{
	if (! mActive)
		return;

	//execute the console command
	if (mConsoleCommand && mConsoleCommand[0])
	{
		execConsoleCallback();
	}
	else if(isMethod("onAction"))
	{
		Con::executef(this, 1, "onAction");
	}
}

void GuiControl::onMessage(GuiControl *sender, S32 msg)
{
}

void GuiControl::messageSiblings(S32 message)
{
   GuiControl *parent = getParent();
   if (! parent) return;
   GuiControl::iterator i;
   for(i = parent->begin(); i != parent->end(); i++)
   {
      GuiControl *ctrl = dynamic_cast<GuiControl *>(*i);
      if (ctrl != this)
         ctrl->onMessage(this, message);
   }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiControl::onDialogPush()
{
   // Notify Script.
   if( isMethod("onDialogPush") )
      Con::executef(this, 1, "onDialogPush");

}

void GuiControl::onDialogPop()
{
   // Notify Script.
   if( isMethod("onDialogPop") )
      Con::executef(this, 1, "onDialogPop");
}

//------------------------------------------------------------------------------
void GuiControl::setVisible(bool value)
{
    mVisible = value;
   iterator i;
   setUpdate();
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      ctrl->clearFirstResponder();
    }

    GuiControl *parent = getParent();
    if (parent)
       parent->childResized(this);
}


void GuiControl::makeFirstResponder(bool value)
{
   if ( value )
      //setFirstResponder(this);
      setFirstResponder();
   else
      clearFirstResponder();
}

void GuiControl::setActive( bool value )
{
   mActive = value;

   if (value && isMethod("onActive"))
	   Con::executef(this, 1, "onActive");
	else if (!value && isMethod("onInactive"))
		Con::executef(this, 1, "onInactive");

   if ( !mActive )
      clearFirstResponder();

   if ( mVisible && mAwake )
      setUpdate();
}

void GuiControl::getScrollLineSizes(U32 *rowHeight, U32 *columnWidth)
{
    // default to 10 pixels in y, 30 pixels in x
    *columnWidth = 30;
    *rowHeight = 30;
}

void GuiControl::renderText(const Point2I& offset, const Point2I& extent, const char* text, GuiControlProfile* profile, TextRotationOptions rot)
{
    RectI old = dglGetClipRect();
    RectI clipRect = RectI(offset, extent);
    if (clipRect.intersect(old))
    {
        dglSetClipRect(clipRect);

        const S32 textHeight = profile->getFont(mFontSizeAdjust)->getHeight();
        S32 totalWidth = (rot == tRotateNone) ? extent.x : extent.y;
        S32 totalHeight = (rot == tRotateNone) ? extent.y : extent.x;

        S32 startOffsetY = 0;

        vector<string> lineList = getLineList(text, profile, totalWidth);

        //first align vertical
        S32 blockHeight = textHeight * lineList.size();

        if (mTextExtend)
        {
            setExtent(getTextExtendedExtent(blockHeight, text, profile));
        }

        if (blockHeight < totalHeight)
        {
            startOffsetY = getTextVerticalOffset(blockHeight, totalHeight, getVertAlignmentType(profile));
        }
        else if (!mTextWrap)
        {
            startOffsetY = getTextVerticalOffset(blockHeight, totalHeight, VertAlignmentType::MiddleVAlign);
        }
        else
        {
            startOffsetY = getTextVerticalOffset(blockHeight, totalHeight, VertAlignmentType::TopVAlign);
        }

        renderLineList(offset, extent, startOffsetY, lineList, profile, rot);
        dglSetClipRect(old);
    }
}

Point2I GuiControl::getTextExtendedExtent(S32 blockHeight, const char* text, GuiControlProfile* profile)
{
    Point2I extent = getExtent();
    if (mTextWrap)
    {
        extent.y = getOuterHeight(blockHeight, NormalState, profile);
    }
    else
    {
        extent.x = getOuterWidth(profile->getFont(mFontSizeAdjust)->getStrWidth(text), NormalState, profile);
    }
    return extent;
}

// textExtend used to be applied only by renderText, and renderChild does not
// render a child that falls outside its parent's clip rect. So a wrapped label
// scrolled out of view kept the height it was authored with until it came back:
// a column of them in a scroller was too short until every row had been seen
// once, its thumb shrank as it was scrolled, and scrollToBottom stopped short of
// rows that grew after it had measured.
//
// This sizes the control from the rect GuiControl::onRender hands renderText.
// It is called where what it depends on changes -- the text, the width, waking
// -- so that script sees the new size straight away, and from preRender once a
// frame, which reaches every visible control whether or not it is on screen,
// for anything else: a new profile, fontSizeAdjust, textWrap.
//
// renderText still sizes a control as it draws it, and for one that draws its
// text somewhere else -- a check box beside its box, a window in its title bar
// -- its answer is the one that sticks. That is why this does nothing unless
// what it measured from has changed, rather than measuring every frame: two
// measurements of different rects would take turns at the control's height.
//
// Only an awake control is measured. Asking a profile for a font registers a
// texture, which the C++ unit tests, having no GL context, cannot do.
void GuiControl::fitToText()
{
    if (!mTextExtend)
    {
        // So that turning it back on measures even if nothing else changed.
        mFittedWidth = -1;
        return;
    }

    if (!mAwake || mProfile == NULL)
        return;

    GFont* font = mProfile->getFont(mFontSizeAdjust);
    if (font == NULL)
        return;

    Point2I offset = Point2I::Zero;
    Point2I extent = mBounds.extent;
    RectI ctrlRect = applyMargins(offset, extent, NormalState, mProfile);
    RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, NormalState, mProfile);
    RectI contentRect = applyPadding(fillRect.point, fillRect.extent, NormalState, mProfile);

    // onRender draws no text into a rect like this, so renderText never sizes
    // from one either.
    if (!contentRect.isValidRect())
        return;

    if (mText == mFittedText && contentRect.extent.x == mFittedWidth && font == mFittedFont &&
        mProfile == mFittedProfile && mTextWrap == mFittedWrap)
        return;

    mFittedText = mText;
    mFittedWidth = contentRect.extent.x;
    mFittedFont = font;
    mFittedProfile = mProfile;
    mFittedWrap = mTextWrap;

    const S32 blockHeight = font->getHeight() * (S32)getLineList(mText, mProfile, contentRect.extent.x).size();
    setExtent(getTextExtendedExtent(blockHeight, mText, mProfile));
}

void GuiControl::renderLineList(const Point2I& offset, const Point2I& extent, const S32 startOffsetY, const vector<string> lineList, GuiControlProfile* profile, const TextRotationOptions rot)
{
    const S32 textHeight = profile->getFont(mFontSizeAdjust)->getHeight();
    S32 totalWidth = (rot == tRotateNone) ? extent.x : extent.y;

	//Now print each line
    U32 ibeamPos = 0;
    U32 lineNumber = 0;
    S32 offsetX = 0;
    S32 offsetY = startOffsetY;
	for(string line : lineList)
	{
		// align the horizontal
        string trimmedLine = Utility::trim_copy(line);
		U32 textWidth = profile->getFont(mFontSizeAdjust)->getStrWidth(trimmedLine.c_str());
		if(textWidth < totalWidth)
		{
            offsetX = getTextHorizontalOffset(textWidth, totalWidth, getAlignmentType(profile));
		}

		Point2I start = Point2I(0, 0);
		F32 rotation = 0.0f;
		if (rot == tRotateNone)
		{
            start.x += offsetX;
            start.y += offsetY;
			rotation = 0.0f;
		}
		else if (rot == tRotateLeft)
		{
			start.x = offsetY;
			start.y = extent.y + offsetX;
			rotation = 90.0f;
		}
		else if (rot == tRotateRight)
		{
			start.x = extent.x - offsetY;
			start.y = offsetX;
			rotation = -90.0f;
		}

        renderTextLine(start + offset + profile->mTextOffset, trimmedLine, profile, rotation, ibeamPos, lineNumber);
			
		offsetY += textHeight;
        ibeamPos += line.length();
        lineNumber++;
	}
}

// Split on newlines by hand rather than with getline, which cannot tell an
// empty string from no string: it fails immediately on "", so an empty control
// produced NO lines at all. A line block is what draws a GuiTextEditCtrl's
// caret, which is why an empty multi-line box had no cursor in it. getline also
// drops the empty paragraph a trailing newline creates, so pressing return at
// the end of the text left the caret with no line to sit on.
//
// The newline stays on the end of its paragraph, for the same reason the
// wrapping in getLineList re-appends the space it consumed: GuiTextEditCtrl
// finds the character offset of a line by summing the lengths of the lines
// above it (renderLineList), so a character dropped here moves the caret.
// Nothing draws or measures it -- dglDrawText and GFont::getStrWidth both skip
// a line break, which GFont::isValidChar answers false for.
//
// Kept apart from the wrapping below because it needs no font: measuring text
// loads one, and a font registers a texture, which cannot be done in the C++
// unit tests -- they run with no canvas. See guiTextEditTests.cc.
vector<string> GuiControl::splitParagraphs(const char* text)
{
    vector<string> paragraphList = vector<string>();
    string paragraphBuffer;
    for (const char* c = text; *c != '\0'; c++)
    {
        paragraphBuffer += *c;
        if (*c == '\n')
        {
            paragraphList.push_back(paragraphBuffer);
            paragraphBuffer.clear();
        }
    }
    paragraphList.push_back(paragraphBuffer);

    return paragraphList;
}

// The measurer getLineList hands to wrapParagraph: the real font.
static U32 fontStrWidth(void* context, const char* text)
{
    return static_cast<GFont*>(context)->getStrWidth(text);
}

vector<string> GuiControl::wrapParagraph(const string& paragraph, S32 totalWidth, TextWidthFn measure, void* context)
{
    vector<string> lineList = vector<string>();

    vector<string> wordList = vector<string>();
    istringstream f2(paragraph);
    string s2;
    while (getline(f2, s2, ' ')) {
        wordList.push_back(s2);
    }

    //now process the word list
    string line;
    bool newLine = true;
    line.clear();
    for (string& word : wordList)
    {
        if (measure(context, word.c_str()) >= totalWidth)
        {
            if (line.size() > 0)
            {
                lineList.push_back(string(line + " "));
                line.clear();
            }
            lineList.push_back(word + " ");
            newLine = true;
            continue;
        }

        string prevLine = string(line);
        line += (!newLine) ? " " + word : word;
        newLine = false;
        if (measure(context, line.c_str()) >= totalWidth && word.length() != 0)
        {
            lineList.push_back(prevLine + " ");
            line = word;
        }
    }
    // back() on an empty string is undefined behaviour, and an empty
    // paragraph is ordinary now: it is what a blank line between two
    // others is made of.
    if (!paragraph.empty() && paragraph.back() == ' ')
    {
        line += " ";
    }

    // A word too long to fit is pushed as a line of its own and leaves nothing
    // behind it, so pushing again here would put an empty line after it -- and
    // that line is not free. It counts into blockHeight, which is what both
    // vertical alignment and mTextExtend are measured from, so a one-word
    // caption came out a whole line high under BottomVAlign, half a line high
    // under MiddleVAlign, and pushed a control that fitted into the "too tall to
    // fit" branch of renderText.
    //
    // An empty paragraph must still produce its one empty line: that is what a
    // blank line between two others is made of, and what gives an empty
    // multi-line box somewhere to put its caret.
    if (!line.empty() || lineList.empty())
    {
        lineList.push_back(string(line));
    }

    return lineList;
}

vector<string> GuiControl::getLineList(const char* text, GuiControlProfile* profile, S32 totalWidth)
{
    GFont* font = profile->getFont(mFontSizeAdjust);
    vector<string> lineList = vector<string>();

    if (!mTextWrap)
    {
        lineList.push_back(text);
    }
    else
    {
        vector<string> paragraphList = splitParagraphs(text);

        for (string& paragraph : paragraphList)
        {
            vector<string> paragraphLines = wrapParagraph(paragraph, totalWidth, &fontStrWidth, font);
            lineList.insert(lineList.end(), paragraphLines.begin(), paragraphLines.end());
        }
    }

    return lineList;
}

void GuiControl::renderTextLine(const Point2I& startPoint, const string line, GuiControlProfile* profile, F32 rotationInDegrees, U32, U32)
{
    dglDrawText(profile->getFont(mFontSizeAdjust), startPoint, line.c_str(), profile->mFontColors, 9, rotationInDegrees);
}

S32 GuiControl::getTextHorizontalOffset(S32 textWidth, S32 totalWidth, AlignmentType align)
{
	if (align == RightAlign)
	{
		return totalWidth - textWidth;
	}
	else if (align == CenterAlign)
	{
		return (totalWidth - textWidth) / 2;
	}
	return 0;//left aligned
}

S32 GuiControl::getTextVerticalOffset(S32 textHeight, S32 totalHeight, VertAlignmentType align)
{
	if (align == MiddleVAlign)
	{
		return (totalHeight - textHeight) / 2;
	}
	else if (align == BottomVAlign)
	{
		return totalHeight - textHeight;
	}
	return 0;
}

void GuiControl::getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent)
{
   lastGuiEvent;

   if(GuiControl::smCursorChanged != -1 && !isMouseLocked())
   {
      // We've already changed the cursor, 
      // so set it back before we change it again.
      Input::popCursor();

      // We haven't changed it
      GuiControl::smCursorChanged = -1;
   }
}

const char* GuiControl::execConsoleCallback()
{
   if (mConsoleCommand && mConsoleCommand[0])
   {
      Con::setVariable("$ThisControl", avar("%d",getId()));
      return Con::evaluate(mConsoleCommand, false);
   }
   return "";
}

const char* GuiControl::execAltConsoleCallback()
{
   if(mAltConsoleCommand && mAltConsoleCommand[0])
   {
      Con::setVariable("$ThisControl", avar("%d",getId()));
      return Con::evaluate(mAltConsoleCommand, false);
   }
   return "";
}

void GuiControl::setText(const char *text)
{
	mText = StringTable->insert(text, true);
	fitToText();
}

void GuiControl::setTextID(const char *id)
{
	S32 n = Con::getIntVariable(id, -1);
	if (n != -1)
	{
		mTextID = StringTable->insert(id);
		setTextID(n);
	}
}
void GuiControl::setTextID(S32 id)
{
	const UTF8 *str = getGUIString(id);
	if (str)
		setText((const char*)str);
}
const char *GuiControl::getText()
{
	return mText;
}

void GuiControl::setDataField(StringTableEntry slotName, const char* array, const char* value)
{
	this->findField(slotName);
	const AbstractClassRep::Field* fld = this->findField(slotName);
	if(fld)
	{
		if (fld->type == AbstractClassRep::DepricatedFieldType ||
			fld->type == AbstractClassRep::StartGroupFieldType ||
			fld->type == AbstractClassRep::EndGroupFieldType)
			return;

		ConsoleBaseType* cbt = ConsoleBaseType::getType(fld->type);
		bool isProfile = strcmp(cbt->getTypeName(), "TypeGuiProfile") == 0;

		if(isProfile && mAwake)
		{
			//Decrease the ref count on the old profile
			void* dptr = (void*)(((const char*)this) + fld->offset);
			GuiControlProfile** obj = (GuiControlProfile**)dptr;
			if((*obj))
				(*obj)->decRefCount();
		}

		SimObject::setDataField(slotName, array, value);

		if (isProfile && mAwake)
		{
			//Increase the ref count on the new profile
			void* dptr = (void*)(((const char*)this) + fld->offset);
			GuiControlProfile** obj = (GuiControlProfile**)dptr;
			if ((*obj))
				(*obj)->incRefCount();
		}
	}
	else 
	{
		SimObject::setDataField(slotName, array, value);
	}
}

AlignmentType GuiControl::getAlignmentType()
{
    return getAlignmentType(mProfile);
}

AlignmentType GuiControl::getAlignmentType(GuiControlProfile* profile)
{
    return mAlignment == AlignmentType::DefaultAlign ? profile->mAlignment : mAlignment;
}

VertAlignmentType GuiControl::getVertAlignmentType()
{
    return getVertAlignmentType(mProfile);
}

VertAlignmentType GuiControl::getVertAlignmentType(GuiControlProfile* profile)
{
    return mVAlignment == VertAlignmentType::DefaultVAlign ? profile->mVAlignment : mVAlignment;
}

const ColorI& GuiControl::getFontColor(GuiControlProfile* profile, const GuiControlState state)
{
    return mOverrideFontColor ? mFontColor : profile->getFontColor(state);
}

bool GuiControl::isEditMode()
{
	if (smDesignTime && smEditorHandle)
	{
		GuiEditCtrl* edit = GuiControl::smEditorHandle;
		if (this == edit->getRoot())
		{
			return true;
		}

		//work up the parent chain to see if one of the parents is the edit root
		GuiControl* parent = getParent();
		if (parent)
		{
			return parent->isEditMode();
		}
	}
	return false;
}

bool GuiControl::isEditSelected()
{
	if (smDesignTime && smEditorHandle)
	{
		GuiEditCtrl* edit = GuiControl::smEditorHandle;
		
		bool selected = false;
		auto list = edit->getSelected();
		for (auto i = list->begin(); i < list->end(); i++)
		{
			GuiControl* ctrl = dynamic_cast<GuiControl*>(*i);
			if (ctrl && ctrl == this)
			{
				return true;
			}
		}
	}
	return false;
}

bool GuiControl::onMouseDownEditor(const GuiEvent& event, const Point2I& offset)
{
	GuiEditCtrl* edit = GuiControl::smEditorHandle;
	GuiControl* parent = getParent();
	if (this != edit->getRoot() && parent)
	{
		return parent->onMouseDownEditor(event, offset);
	}
	return false;
}

//--------------------------------------------------------------------

GuiEasingSupport::GuiEasingSupport()
{
	//fill color
	mEaseFillColorHL = EasingFunction::Linear;
	mEaseFillColorSL = EasingFunction::Linear;
	mEaseTimeFillColorHL = 500;
	mEaseTimeFillColorSL = 0;

	//control state
	mPreviousState = GuiControlState::DisabledState;
	mCurrentState = GuiControlState::DisabledState;

	mFluidFillColor = FluidColorI(); //The actual fill color as it moves fluidly from one color to another.
}

void GuiEasingSupport::initPersistFields()
{
	Parent::initPersistFields();

	addGroup("Gui Easing Settings");
	addField("easeFillColorHL", TypeEnum, Offset(mEaseFillColorHL, GuiEasingSupport), 1, &gEasingTable);
	addField("easeFillColorSL", TypeEnum, Offset(mEaseFillColorSL, GuiEasingSupport), 1, &gEasingTable);
	addField("easeTimeFillColorHL", TypeS32, Offset(mEaseTimeFillColorHL, GuiEasingSupport));
	addField("easeTimeFillColorSL", TypeS32, Offset(mEaseTimeFillColorSL, GuiEasingSupport));
	endGroup("Gui Easing Settings");
}

const ColorI& GuiEasingSupport::getFillColor(const GuiControlState state)
{
	if (state != mCurrentState)
	{
		//We have just switched states!
		mPreviousState = mCurrentState;
		mCurrentState = state;
		if (mCurrentState == GuiControlState::DisabledState || mPreviousState == GuiControlState::DisabledState)
		{
			mFluidFillColor.stopFluidAnimation();
			mFluidFillColor.set(mProfile->getFillColor(state));
		}
		else if (mCurrentState == GuiControlState::SelectedState || mPreviousState == GuiControlState::SelectedState)
		{
			mFluidFillColor.setEasingFunction(mEaseFillColorSL);
			mFluidFillColor.setAnimationLength(mEaseTimeFillColorSL);
			mFluidFillColor.startFluidAnimation(mProfile->getFillColor(state));
		}
		else if (mCurrentState == GuiControlState::HighlightState || mPreviousState == GuiControlState::HighlightState)
		{
			mFluidFillColor.setEasingFunction(mEaseFillColorHL);
			mFluidFillColor.setAnimationLength(mEaseTimeFillColorHL);
			mFluidFillColor.startFluidAnimation(mProfile->getFillColor(state));
		}
		else
		{
			//we should never get here...
			mFluidFillColor.stopFluidAnimation();
			mFluidFillColor.set(mProfile->getFillColor(state));
		}
	}

	if (mFluidFillColor.isAnimating() && !isProcessingTicks())
	{
		setProcessTicks(true);
	}

	if (!mFluidFillColor.isAnimating())
	{
		mFluidFillColor.set(mProfile->getFillColor(state));
	}

	return mFluidFillColor;
}

void GuiEasingSupport::processTick()
{
	bool shouldWeContinue = false;

	shouldWeContinue |= mFluidFillColor.processTick();

	if (!shouldWeContinue)
	{
		setProcessTicks(false);
	}
}

void GuiEasingSupport::setControlProfile(GuiControlProfile* prof)
{
	Parent::setControlProfile(prof);
	mCurrentState = mCurrentState == DisabledState ? NormalState : DisabledState;
}