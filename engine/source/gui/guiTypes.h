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

#ifndef _GUITYPES_H_
#define _GUITYPES_H_

#ifndef _ASSET_FIELD_TYPES_H_
#include "assets/assetFieldTypes.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _COLOR_H_
#include "graphics/gColor.h"
#endif

#ifndef _FLUID_H_
#include "math/mFluid.h"
#endif

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _PLATFORMAUDIO_H_
#include "platform/platformAudio.h"
#endif

#ifndef _AUDIO_ASSET_H_
#include "audio/AudioAsset.h"
#endif

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/ImageAsset.h"
#endif

#include "graphics/gFont.h"

class GBitmap;

/// Represents a single GUI event.
///
/// This is passed around to all the relevant controls so they know what's going on.
struct GuiEvent
{
   U16      ascii;             ///< ascii character code 'a', 'A', 'b', '*', etc (if device==keyboard) - possibly a uchar or something
   U8       modifier;          ///< SI_LSHIFT, etc
   U8       keyCode;           ///< for unprintables, 'tab', 'return', ...
   Point2I  mousePoint;        ///< for mouse events
   U8       mouseClickCount;   ///< to determine double clicks, etc...
   S32		eventID;		   ///< assigns mouse or touch ID to the event
};

enum GuiControlState
{
	NormalState = 0,				//Control renders with default look
	HighlightState,				//Control is highlighted
	SelectedState,				//Control has been selected
	DisabledState,				//Control cannot be used
	NormalStateOn,				//Used by controls like checkboxes to denote the normal state while checked
	HighlightStateOn,			//The highlight state while on
	SelectedStateOn,			//The selected state while on
	DisabledStateOn,			//The disabled state while on
	StateCount					//Not an actual state! Should always be at the end of the list.
};

enum class GuiDirection
{
	Up,				
	Down,				
	Left,				
	Right			
};

class GuiCursor : public SimObject
{
private:
   typedef SimObject Parent;
   StringTableEntry mBitmapName;

   Point2I mHotSpot;
   Point2F mRenderOffset;
   Point2I mExtent;
   TextureHandle mTextureHandle;

public:
   Point2I getHotSpot() { return mHotSpot; }
   Point2I getExtent() { return mExtent; }

   DECLARE_CONOBJECT(GuiCursor);
   GuiCursor(void);
   ~GuiCursor(void);
   static void initPersistFields();

   bool onAdd(void);
   void onRemove();
   void render(const Point2I &pos);
};

/// A GuiBorderProfile holds on the information needed for a single border of a GuiControl.
/// GuiBorderProfiles can be assigned to a GuiControlProfile to cover one or all of the borders.
class GuiBorderProfile : public SimObject
{
private:
	typedef SimObject Parent;
	inline U8 getStateIndex(const GuiControlState state) { return state >= 4 ? state - 4 : state; }

public:
	S32 mMargin[static_cast<S32>(4)];					//The distance between the edge and the start of the border. Margin is outside of the control.
	S32 mBorder[static_cast<S32>(4)];					//Width of the border.
	ColorI mBorderColor[static_cast<S32>(4)];			//The color of the border.
	S32 mPadding[static_cast<S32>(4)];					//The distance between the border and content of the control.
	bool mUnderfill;									//True if the control's fill color should appear under the border.
public:
	DECLARE_CONOBJECT(GuiBorderProfile);
	GuiBorderProfile();
	~GuiBorderProfile();
	static void initPersistFields();
	bool onAdd();
	void onRemove();

	S32 getMargin(const GuiControlState state); //Returns the margin based on the control's state.
	S32 getBorder(const GuiControlState state); //Returns the size of the border based on the control's state.
	const ColorI& getBorderColor(const GuiControlState state); //Returns the correct border color based on the control's state.
	S32 getPadding(const GuiControlState state); //Returns the padding based on the control's state.
};
DefineConsoleType(TypeGuiBorderProfile)
/// A GuiControlProfile is used by every GuiObject and is akin to a
/// datablock. It is used to control information that does not change
/// or is unlikely to change during execution of a program. It is also
/// a level of abstraction between script and GUI control so that you can
/// use the same control, say a button, and have it look completly different
/// just with a different profile.
class GuiControlProfile : public SimObject
{
private:
   typedef SimObject Parent;

public:
   S32  mRefCount;                                 ///< Used to determine if any controls are using this profile
   bool mTabable;                                  ///< True if this object is accessable from using the tab key

   static StringTableEntry  sFontCacheDirectory;
   bool mCanKeyFocus;                              ///< True if the object can be given keyboard focus (in other words, made a first responder @see GuiControl)
   bool mUseInput;                                 ///< True if input events like a click can be passed to this object. False will pass events to the parent and this object and its children will not be evaluated.

   ColorI mFillColor; //Normal fill color used to fill the control area inside (and possibly under) the border.
   ColorI mFillColorHL; //The highlight fill color used when the cursor enters the control.
   ColorI mFillColorSL;	//Color used when the control is selected.
   ColorI mFillColorNA; //Used if the object is not active or disabled.

   GuiBorderProfile* mBorderDefault;					//The default border settings.
   // top profile
   StringTableEntry mTopProfileName;
   GuiBorderProfile* mBorderTop;
   // Bottom profile
   StringTableEntry mBottomProfileName;
   GuiBorderProfile* mBorderBottom;
   // Left profile
   StringTableEntry mLeftProfileName;
   GuiBorderProfile* mBorderLeft;
   // Left profile
   StringTableEntry mRightProfileName;
   GuiBorderProfile* mBorderRight;

   // font members
   StringTableEntry  mFontType;                    ///< Font face name for the control
   S32               mFontSize;                    ///< Font size for the control
   enum {
      BaseColor = 0,
      ColorHL,
      ColorNA,
      ColorSL,
      ColorUser0,
      ColorUser1,
      ColorUser2,
      ColorUser3,
      ColorUser4,
      ColorUser5,
   };
   ColorI  mFontColors[10];                        ///< Array of font colors used for drawText with escape characters for changing color mid-string
   ColorI& mFontColor;                             ///< Main font color
   ColorI& mFontColorHL;                           ///< Highlited font color
   ColorI& mFontColorNA;                           ///< Font color when object is not active/disabled
   ColorI& mFontColorSL;                          ///< Font color when object/text is selected
   FontCharset mFontCharset;                       ///< Font character set

   Resource<GFont>   mFont;                        ///< Font resource

   enum AlignmentType
   {
      LeftAlign,
      RightAlign,
      CenterAlign
   };
   AlignmentType mAlignment;                       ///< Horizontal text alignment

   enum VertAlignmentType
   {
	   TopVAlign,
	   BottomVAlign,
	   MiddleVAlign
   };
   VertAlignmentType mVAlignment;				   ///< Vertical text alignment

   bool mReturnTab;                                ///< Used in GuiTextEditCtrl to specify if a tab-event should be simulated when return is pressed.
   bool mNumbersOnly;                              ///< For text controls, true if this should only accept numerical data
   bool mMouseOverSelected;                        ///< True if this object should be "selected" while the mouse is over it
   ColorI mCursorColor;                            ///< Color for the blinking cursor in text fields (for example)

   Point2I mTextOffset;                            ///< Text offset for the control
   StringTableEntry  mCategory;                    ///< Category for control in editors.

   // imageAsset members
   StringTableEntry mImageAssetID;
   AssetPtr<ImageAsset> mImageAsset;
   void setImageAsset( const char* pImageAssetID );
   inline StringTableEntry getImageAsset( void ) const { return mImageAssetID; }
protected:
	static bool setImageAsset(void* obj, const char* data) { static_cast<GuiControlProfile*>(obj)->setImageAsset(data); return false; }
	static const char* getImageAsset(void* obj, const char* data) { return static_cast<GuiControlProfile*>(obj)->getImageAsset(); }

public:
   // bitmap members
   StringTableEntry mBitmapName;                   ///< Bitmap file name for the bitmap of the control
   TextureHandle mTextureHandle;                   ///< Texture handle for the control
   Vector<RectI> mBitmapArrayRects;                ///< Used for controls which use an array of bitmaps such as checkboxes

   // sound members
   AssetPtr<AudioAsset> mSoundButtonDown;                 ///< Sound played when the object is "down" ie a button is pushed
   AssetPtr<AudioAsset> mSoundButtonOver;                 ///< Sound played when the mouse is over the object
   StringTableEntry mProfileForChildrenName;
   GuiControlProfile* mProfileForChildren;         ///< Profile used with children controls (such as the scroll bar on a popup menu) when defined.
public:
   DECLARE_CONOBJECT(GuiControlProfile);
   GuiControlProfile();
   ~GuiControlProfile();
   static void initPersistFields();
   bool onAdd();


   GuiBorderProfile* getLeftProfile();
   void setLeftProfile(GuiBorderProfile* prof);

   GuiBorderProfile* getRightProfile();
   void setRightProfile(GuiBorderProfile* prof);

   GuiBorderProfile* getTopProfile();
   void setTopProfile(GuiBorderProfile* prof);

   GuiBorderProfile* getBottomProfile();
   void setBottomProfile(GuiBorderProfile* prof);

   // Get and Set child profile
   GuiControlProfile * getChildrenProfile();
   void setChildrenProfile(GuiControlProfile * prof);

   /// This method creates an array of bitmaps from one single bitmap with
   /// seperator color. The seperator color is whatever color is in pixel 0,0
   /// of the bitmap. For an example see darkWindow.png and some of the other
   /// UI textures. It returns the number of bitmaps in the array it created
   /// It also stores the sizes in the mBitmapArrayRects vector.
   S32 constructBitmapArray();

   void incRefCount();
   void decRefCount();

   const ColorI& getFillColor(const GuiControlState state); //Returns the fill color based on the state.
   const ColorI& getFontColor(const GuiControlState state); //Returns the font color based on the state.

   GuiBorderProfile* getLeftBorder() { return mBorderLeft; }
   GuiBorderProfile* getRightBorder() { return mBorderRight; }
   GuiBorderProfile* getTopBorder() { return mBorderTop; }
   GuiBorderProfile* getBottomBorder() { return mBorderBottom; }
};
DefineConsoleType( TypeGuiProfile)

#endif //_GUITYPES_H
