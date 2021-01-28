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

#include "platform/platform.h"
#include "platform/types.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/codeBlock.h"
#include "graphics/gFont.h"
#include "graphics/dgl.h"
#include "gui/guiTypes.h"
#include "graphics/gBitmap.h"
#include "graphics/TextureManager.h"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
IMPLEMENT_CONOBJECT(GuiCursor);

GuiCursor::GuiCursor()
{
   mHotSpot.set(0,0);
   mRenderOffset.set(0.0f,0.0f);
   mExtent.set(1,1);
}

GuiCursor::~GuiCursor()
{
}

void GuiCursor::initPersistFields()
{
   Parent::initPersistFields();
   addField("hotSpot",     TypePoint2I,   Offset(mHotSpot, GuiCursor));
   addField("renderOffset",TypePoint2F,   Offset(mRenderOffset, GuiCursor));
   addField("bitmapName",  TypeFilename,  Offset(mBitmapName, GuiCursor));
}

bool GuiCursor::onAdd()
{
   if(!Parent::onAdd())
	  return false;

   Sim::getGuiDataGroup()->addObject(this);

   return true;
}

void GuiCursor::onRemove()
{
   Parent::onRemove();
}

void GuiCursor::render(const Point2I &pos)
{
   if (!mTextureHandle && mBitmapName && mBitmapName[0])
   {
	  mTextureHandle = TextureHandle(mBitmapName, TextureHandle::BitmapTexture);
	  if(!mTextureHandle)
		 return;
	  mExtent.set(mTextureHandle.getWidth(), mTextureHandle.getHeight());
   }

   // Render the cursor centered according to dimensions of texture
   S32 texWidth = mTextureHandle.getWidth();
   S32 texHeight = mTextureHandle.getWidth();

   Point2I renderPos = pos;
   renderPos.x -= (S32)( texWidth  * mRenderOffset.x );
   renderPos.y -= (S32)( texHeight * mRenderOffset.y );
   
   dglClearBitmapModulation();
   dglDrawBitmap(mTextureHandle, renderPos);
}

//------------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiBorderProfile);

GuiBorderProfile::GuiBorderProfile()
{
	for(S32 i = 0; i < 4; i++)
	{
		mMargin[i] = 0;
		mBorder[i] = 0;
		mBorderColor[i].set(255, 255, 255, 255);
		mPadding[i] = 0;
	}

	mUnderfill = true;
}

GuiBorderProfile::~GuiBorderProfile()
{
}

void GuiBorderProfile::initPersistFields()
{
	Parent::initPersistFields();

	addField("margin", TypeS32, Offset(mMargin[0], GuiBorderProfile));
	addField("marginHL", TypeS32, Offset(mMargin[1], GuiBorderProfile));
	addField("marginSL", TypeS32, Offset(mMargin[2], GuiBorderProfile));
	addField("marginNA", TypeS32, Offset(mMargin[3], GuiBorderProfile));

	addField("border", TypeS32, Offset(mBorder[0], GuiBorderProfile));
	addField("borderHL", TypeS32, Offset(mBorder[1], GuiBorderProfile));
	addField("borderSL", TypeS32, Offset(mBorder[2], GuiBorderProfile));
	addField("borderNA", TypeS32, Offset(mBorder[3], GuiBorderProfile));

	addField("borderColor", TypeColorI, Offset(mBorderColor[0], GuiBorderProfile));
	addField("borderColorHL", TypeColorI, Offset(mBorderColor[1], GuiBorderProfile));
	addField("borderColorSL", TypeColorI, Offset(mBorderColor[2], GuiBorderProfile));
	addField("borderColorNA", TypeColorI, Offset(mBorderColor[3], GuiBorderProfile));

	addField("padding", TypeS32, Offset(mPadding[0], GuiBorderProfile));
	addField("paddingHL", TypeS32, Offset(mPadding[1], GuiBorderProfile));
	addField("paddingSL", TypeS32, Offset(mPadding[2], GuiBorderProfile));
	addField("paddingNA", TypeS32, Offset(mPadding[3], GuiBorderProfile));

	addField("underfill", TypeBool, Offset(mUnderfill, GuiBorderProfile));
}

bool GuiBorderProfile::onAdd()
{
	if (!Parent::onAdd())
		return false;

	Sim::getGuiDataGroup()->addObject(this);

	return true;
}

void GuiBorderProfile::onRemove()
{
	Parent::onRemove();
}

S32 GuiBorderProfile::getMargin(const GuiControlState state)
{
	return getMax(mMargin[getStateIndex(state)], 0);
}

S32 GuiBorderProfile::getBorder(const GuiControlState state)
{
	return getMax(mBorder[getStateIndex(state)], 0);
}

const ColorI& GuiBorderProfile::getBorderColor(const GuiControlState state)
{
	return mBorderColor[getStateIndex(state)];
}

S32 GuiBorderProfile::getPadding(const GuiControlState state)
{
	return getMax(mPadding[getStateIndex(state)], 0);
}

//------------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiControlProfile);

static EnumTable::Enums alignEnums[] =
{
   { GuiControlProfile::LeftAlign,          "left"      },
   { GuiControlProfile::CenterAlign,        "center"    },
   { GuiControlProfile::RightAlign,         "right"     }
};
static EnumTable gAlignTable(3, &alignEnums[0]);

static EnumTable::Enums vAlignEnums[] =
{
   { GuiControlProfile::TopVAlign,          "top"      },
   { GuiControlProfile::MiddleVAlign,        "middle"    },
   { GuiControlProfile::BottomVAlign,         "bottom"     }
};
static EnumTable gVAlignTable(3, &vAlignEnums[0]);

static EnumTable::Enums charsetEnums[]=
{
	{ TGE_ANSI_CHARSET,         "ANSI" },
	{ TGE_SYMBOL_CHARSET,       "SYMBOL" },
	{ TGE_SHIFTJIS_CHARSET,     "SHIFTJIS" },
	{ TGE_HANGEUL_CHARSET,      "HANGEUL" },
	{ TGE_HANGUL_CHARSET,       "HANGUL" },
	{ TGE_GB2312_CHARSET,       "GB2312" },
	{ TGE_CHINESEBIG5_CHARSET,  "CHINESEBIG5" },
	{ TGE_OEM_CHARSET,          "OEM" },
	{ TGE_JOHAB_CHARSET,        "JOHAB" },
	{ TGE_HEBREW_CHARSET,       "HEBREW" },
	{ TGE_ARABIC_CHARSET,       "ARABIC" },
	{ TGE_GREEK_CHARSET,        "GREEK" },
	{ TGE_TURKISH_CHARSET,      "TURKISH" },
	{ TGE_VIETNAMESE_CHARSET,   "VIETNAMESE" },
	{ TGE_THAI_CHARSET,         "THAI" },
	{ TGE_EASTEUROPE_CHARSET,   "EASTEUROPE" },
	{ TGE_RUSSIAN_CHARSET,      "RUSSIAN" },
	{ TGE_MAC_CHARSET,          "MAC" },
	{ TGE_BALTIC_CHARSET,       "BALTIC" },
};

#define NUMCHARSETENUMS     (sizeof(charsetEnums) / sizeof(EnumTable::Enums))

static EnumTable gCharsetTable(NUMCHARSETENUMS, &charsetEnums[0]);

StringTableEntry GuiControlProfile::sFontCacheDirectory = "";

GuiControlProfile::GuiControlProfile(void) :
   mFontColor(mFontColors[BaseColor]),
   mFontColorHL(mFontColors[ColorHL]),
   mFontColorNA(mFontColors[ColorNA]),
   mFontColorSL(mFontColors[ColorSL]),
   mImageAssetID( StringTable->EmptyString )
{
	mRefCount = 0;
	mBitmapArrayRects.clear();
	mMouseOverSelected = false;
	
	mTabable       = false;
	mCanKeyFocus   = false;
	mUseInput      = true;

	mBorderDefault = NULL;
	mBorderLeft = NULL;
	mBorderRight = NULL;
	mBorderTop = NULL;
	mBorderBottom = NULL;
	
	// default font
	mFontType      = StringTable->EmptyString;
	mFontSize      = 12;
	mFontCharset   = TGE_ANSI_CHARSET;
	mFontColors[BaseColor].set(255,255,255,255);
	
	// default bitmap
	mBitmapName    = NULL;
	mTextOffset.set(0,0);

	// default image asset
	mImageAsset = NULL;
	
	mAlignment     = LeftAlign;
	mVAlignment    = MiddleVAlign;
	mReturnTab     = false;
	mNumbersOnly   = false;
	mProfileForChildren = NULL;

	//fill color
	mFillColor.set(0, 0, 0, 0);
	mFillColorHL.set(0, 0, 0, 0);
	mFillColorSL.set(0, 0, 0, 0);
	mFillColorNA.set(0, 0, 0, 0);
}

GuiControlProfile::~GuiControlProfile()
{
}


void GuiControlProfile::initPersistFields()
{
   Parent::initPersistFields();

   addField("tab",           TypeBool,       Offset(mTabable, GuiControlProfile));
   addField("canKeyFocus",   TypeBool,       Offset(mCanKeyFocus, GuiControlProfile));
   addField("useInput",      TypeBool,       Offset(mUseInput, GuiControlProfile));
   addField("mouseOverSelected", TypeBool,   Offset(mMouseOverSelected, GuiControlProfile));

   addField("fillColor",     TypeColorI,     Offset(mFillColor, GuiControlProfile));
   addField("fillColorHL",   TypeColorI,     Offset(mFillColorHL, GuiControlProfile));
   addField("fillColorSL",   TypeColorI,     Offset(mFillColorSL, GuiControlProfile));
   addField("fillColorNA",   TypeColorI,     Offset(mFillColorNA, GuiControlProfile));

   addField("borderDefault", TypeSimObjectPtr, Offset(mBorderDefault, GuiControlProfile));
   addField("borderLeft",    TypeSimObjectPtr, Offset(mBorderLeft, GuiControlProfile));
   addField("borderRight",   TypeSimObjectPtr, Offset(mBorderRight, GuiControlProfile));
   addField("borderTop",     TypeSimObjectPtr, Offset(mBorderTop, GuiControlProfile));
   addField("borderBottom",  TypeSimObjectPtr, Offset(mBorderBottom, GuiControlProfile));

   addField("fontType",      TypeString,     Offset(mFontType, GuiControlProfile));
   addField("fontSize",      TypeS32,        Offset(mFontSize, GuiControlProfile));
   addField("fontCharset",   TypeEnum,       Offset(mFontCharset, GuiControlProfile), 1, &gCharsetTable);
   addField("fontColors",    TypeColorI,     Offset(mFontColors, GuiControlProfile), 10);
   addField("fontColor",     TypeColorI,     Offset(mFontColors[BaseColor], GuiControlProfile));
   addField("fontColorHL",   TypeColorI,     Offset(mFontColors[ColorHL], GuiControlProfile));
   addField("fontColorNA",   TypeColorI,     Offset(mFontColors[ColorNA], GuiControlProfile));
   addField("fontColorSL",  TypeColorI,     Offset(mFontColors[ColorSL], GuiControlProfile));
   addField("fontColorLink", TypeColorI,     Offset(mFontColors[ColorUser0], GuiControlProfile));
   addField("fontColorLinkHL", TypeColorI,     Offset(mFontColors[ColorUser1], GuiControlProfile));

   addField("align", TypeEnum, Offset(mAlignment, GuiControlProfile), 1, &gAlignTable);
   addField("vAlign", TypeEnum, Offset(mVAlignment, GuiControlProfile), 1, &gVAlignTable);
   addField("textOffset",    TypePoint2I,    Offset(mTextOffset, GuiControlProfile));
   addField("returnTab",     TypeBool,       Offset(mReturnTab, GuiControlProfile));
   addField("numbersOnly",   TypeBool,       Offset(mNumbersOnly, GuiControlProfile));
   addField("cursorColor",   TypeColorI,     Offset(mCursorColor, GuiControlProfile));

   addField("bitmap",        TypeFilename,   Offset(mBitmapName, GuiControlProfile));
   addProtectedField("imageAsset", TypeAssetId, Offset(mImageAssetID, GuiControlProfile), &setImageAsset, &getImageAsset, "The image asset ID used to render the control");

   addField("soundButtonDown", TypeAudioAssetPtr,  Offset(mSoundButtonDown, GuiControlProfile));
   addField("soundButtonOver", TypeAudioAssetPtr,  Offset(mSoundButtonOver, GuiControlProfile));
   addField("profileForChildren", TypeSimObjectPtr,  Offset(mProfileForChildren, GuiControlProfile));
}

bool GuiControlProfile::onAdd()
{
   if(!Parent::onAdd())
	  return false;

   Sim::getGuiDataGroup()->addObject(this);

   return true;
}

S32 GuiControlProfile::constructBitmapArray()
{
   if(mBitmapArrayRects.size())
	  return mBitmapArrayRects.size();

   GBitmap *bmp = mTextureHandle.getBitmap();

   // Make sure the texture exists.
   if( !bmp )
	  return 0;
  
   //get the separator color
   ColorI sepColor;
   if ( !bmp || !bmp->getColor( 0, 0, sepColor ) )
	{
	  Con::errorf("Failed to create bitmap array from %s for profile %s - couldn't ascertain seperator color!", mBitmapName, getName());
	  AssertFatal( false, avar("Failed to create bitmap array from %s for profile %s - couldn't ascertain seperator color!", mBitmapName, getName()));
	  return 0;
	}

   //now loop through all the scroll pieces, and find the bounding rectangle for each piece in each state
   S32 curY = 0;

   // ascertain the height of this row...
   ColorI color;
   mBitmapArrayRects.clear();
   while(curY < (S32)bmp->getHeight())
   {
	  // skip any sep colors
	  bmp->getColor( 0, curY, color);
	  if(color == sepColor)
	  {
		 curY++;
		 continue;
	  }
	  // ok, process left to right, grabbing bitmaps as we go...
	  S32 curX = 0;
	  while(curX < (S32)bmp->getWidth())
	  {
		 bmp->getColor(curX, curY, color);
		 if(color == sepColor)
		 {
			curX++;
			continue;
		 }
		 S32 startX = curX;
		 while(curX < (S32)bmp->getWidth())
		 {
			bmp->getColor(curX, curY, color);
			if(color == sepColor)
			   break;
			curX++;
		 }
		 S32 stepY = curY;
		 while(stepY < (S32)bmp->getHeight())
		 {
			bmp->getColor(startX, stepY, color);
			if(color == sepColor)
			   break;
			stepY++;
		 }
		 mBitmapArrayRects.push_back(RectI(startX, curY, curX - startX, stepY - curY));
	  }
	  // ok, now skip to the next separation color on column 0
	  while(curY < (S32)bmp->getHeight())
	  {
		 bmp->getColor(0, curY, color);
		 if(color == sepColor)
			break;
		 curY++;
	  }
   }
   return mBitmapArrayRects.size();
}

void GuiControlProfile::incRefCount()
{
	if(!mRefCount++)
	{
		sFontCacheDirectory = Con::getVariable("$GUI::fontCacheDirectory");

		//verify the font
		mFont = GFont::create(mFontType, mFontSize, sFontCacheDirectory);
		if (mFont.isNull())
			Con::errorf("Failed to load/create profile font (%s/%d)", mFontType, mFontSize);

		//Set the bitmap
		if ( mBitmapName != NULL && mBitmapName != StringTable->EmptyString )
		{
			mTextureHandle = TextureHandle(mBitmapName, TextureHandle::BitmapKeepTexture);
			if (!(bool)mTextureHandle)
				Con::errorf("Failed to load profile bitmap (%s)",mBitmapName);
		}

		//set the image asset
		if (mImageAssetID != NULL && mImageAssetID != StringTable->EmptyString)
		{
			mImageAsset = mImageAssetID;
		}
	}
}

void GuiControlProfile::decRefCount()
{
   AssertFatal(mRefCount, "GuiControlProfile::decRefCount: zero ref count");
   if(!mRefCount)
	  return;

   if(!--mRefCount)
   {
	  mFont = NULL;
	  mTextureHandle = NULL;
	  mImageAsset.clear();
   }
}

void GuiControlProfile::setImageAsset(const char* pImageAssetID)
{
	// Sanity!
	AssertFatal(pImageAssetID != NULL, "Cannot use a NULL asset ID.");

	// Fetch the asset ID
	mImageAsset = StringTable->insert(pImageAssetID);

	// Assign asset if this profile is being used.
	if (mRefCount != 0)
		mImageAsset = pImageAssetID;
}

const ColorI& GuiControlProfile::getFillColor(const GuiControlState state)
{
	switch (state)
	{
	default:
	case NormalState:
	case NormalStateOn:
		return mFillColor;
		break;
	case HighlightState:
	case HighlightStateOn:
		return mFillColorHL;
		break;
	case SelectedState:
	case SelectedStateOn:
		return mFillColorSL;
		break;
	case DisabledState:
	case DisabledStateOn:
		return mFillColorNA;
		break;
	}
}

const ColorI& GuiControlProfile::getFontColor(const GuiControlState state)
{
	switch (state)
	{
	default:
	case NormalState:
	case NormalStateOn:
		return mFontColor;
		break;
	case HighlightState:
	case HighlightStateOn:
		return mFontColorHL;
		break;
	case SelectedState:
	case SelectedStateOn:
		return mFontColorSL;
		break;
	case DisabledState:
	case DisabledStateOn:
		return mFontColorNA;
		break;
	}
}

ConsoleType( GuiProfile, TypeGuiProfile, sizeof(GuiControlProfile*), "" )

ConsoleSetType( TypeGuiProfile )
{
   GuiControlProfile *profile = NULL;
   if(argc == 1)
	  Sim::findObject(argv[0], profile);

   AssertWarn(profile != NULL, avar("GuiControlProfile: requested gui profile (%s) does not exist.", argv[0]));
   if(!profile)
	  profile = dynamic_cast<GuiControlProfile*>(Sim::findObject("GuiDefaultProfile"));

   AssertFatal(profile != NULL, avar("GuiControlProfile: unable to find specified profile (%s) and GuiDefaultProfile does not exist!", argv[0]));

   GuiControlProfile **obj = (GuiControlProfile **)dptr;
   if((*obj) == profile)
	  return;

   if(*obj)
	  (*obj)->decRefCount();

   *obj = profile;
   (*obj)->incRefCount();
}

ConsoleGetType( TypeGuiProfile )
{
   static char returnBuffer[256];

   GuiControlProfile **obj = (GuiControlProfile**)dptr;
   dSprintf(returnBuffer, sizeof(returnBuffer), "%s", *obj ? (*obj)->getName() ? (*obj)->getName() : (*obj)->getIdString() : "");
   return returnBuffer;
}
