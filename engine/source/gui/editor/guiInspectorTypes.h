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
#ifndef _GUI_INSPECTOR_TYPES_H_
#define _GUI_INSPECTOR_TYPES_H_

#ifndef _GUI_INSPECTOR_H_
#include "gui/editor/guiInspector.h"
#endif

#ifndef _ANIMATION_ASSET_H_
#include "2d/assets/AnimationAsset.h"
#endif

#ifndef _FONT_ASSET_H_
#include "2d/assets/FontAsset.h"
#endif // !_FONT_ASSET_H_

#ifndef _PARTICLE_ASSET_H_
#include "2d/assets/ParticleAsset.h"
#endif // !_PARTICLE_ASSET_H_


#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _H_GUIDEFAULTCONTROLRENDER_
#include "gui/guiDefaultControlRender.h"
#endif

#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif

#ifndef _GUITEXTEDITCTRL_H_
#include "gui/guiTextEditCtrl.h"
#endif

#ifndef _GUIDROPDOWNCTRL_H_
#include "gui/buttons/guiDropDownCtrl.h"
#endif

#ifndef _GUIGRIDCTRL_H_
#include "gui/containers/guiGridCtrl.h"
#endif

#ifndef _GUICHECKBOXCTRL_H_
#include "gui/buttons/guiCheckBoxCtrl.h"
#endif

#ifndef _GUICOLORPICKERCTRL_H_
#include "gui/guiColorPicker.h"
#endif // !_GUICOLORPICKERCTRL_H_



//////////////////////////////////////////////////////////////////////////
// TypeEnum GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeEnum : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeEnum);
   static void consoleInit();

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl*        constructEditControl(S32 width);
   virtual void               setData( StringTableEntry data );
   virtual StringTableEntry   getData();
   virtual void               updateValue( StringTableEntry newValue );
};


//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeCheckBox Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeCheckBox : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeCheckBox);
   static void consoleInit();

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields (Both are REQUIRED)
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl* constructEditControl(S32 width);
};

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeGuiProfile Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeGuiProfile : public GuiInspectorTypeEnum
{
private:
   typedef GuiInspectorTypeEnum Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeGuiProfile);
   static void consoleInit();

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields (Both are REQUIRED)
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl* constructEditControl(S32 width);
};

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeGuiBorderProfile Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeGuiBorderProfile : public GuiInspectorTypeEnum
{
private:
   typedef GuiInspectorTypeEnum Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeGuiBorderProfile);
   static void consoleInit();

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields (Both are REQUIRED)
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl* constructEditControl(S32 width);
};


//////////////////////////////////////////////////////////////////////////
// TypeFileName GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeFileName : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeFileName);
   static void consoleInit();

   SimObjectPtr<GuiButtonCtrl>   mBrowseButton;

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl*        constructEditControl(S32 width);
   virtual void               resize(const Point2I &newPosition, const Point2I &newExtent);
};


//////////////////////////////////////////////////////////////////////////
// TypeColor GuiInspectorField Class (Base for ColorI/ColorF)
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeColor : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeColor);

   StringTableEntry  mColorFunction;

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl*        constructEditControl(S32 width);
   virtual void               updateValue(StringTableEntry newValue);
};

//////////////////////////////////////////////////////////////////////////
// TypeColorI GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeColorI : public GuiInspectorTypeColor
{
private:
   typedef GuiInspectorTypeColor Parent;
public:
   GuiInspectorTypeColorI();

   DECLARE_CONOBJECT(GuiInspectorTypeColorI);
   static void consoleInit();
};

//////////////////////////////////////////////////////////////////////////
// TypeColorF GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeColorF : public GuiInspectorTypeColor
{
private:
   typedef GuiInspectorTypeColor Parent;
public:
   GuiInspectorTypeColorF();

   DECLARE_CONOBJECT(GuiInspectorTypeColorF);
   static void consoleInit();
};

//////////////////////////////////////////////////////////////////////////
// TypeSimObjectPtr GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeSimObjectPtr : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeSimObjectPtr);
   static void consoleInit();

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl*        constructEditControl(S32 width);
   virtual StringTableEntry   getData();
};

//////////////////////////////////////////////////////////////////////////
// TypeS32 GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeS32 : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeS32);
   static void consoleInit();

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl*        constructEditControl(S32 width);
   virtual const char*        getData();
};

//////////////////////////////////////////////////////////////////////////
// TypePoint2I GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypePoint2I : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypePoint2I);
   static void consoleInit();

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl*        constructEditControl(S32 width);
   virtual void               updateValue(StringTableEntry newValue);
};

//////////////////////////////////////////////////////////////////////////
// TypePoint2F GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypePoint2F : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypePoint2F);
   static void consoleInit();

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl*        constructEditControl(S32 width);
   virtual void               updateValue(StringTableEntry newValue);
};

//////////////////////////////////////////////////////////////////////////
// TypeVector2 GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeVector2 : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeVector2);
   static void consoleInit();

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl*        constructEditControl(S32 width);
   virtual void               updateValue(StringTableEntry newValue);
};

//////////////////////////////////////////////////////////////////////////
// TypeAsset GuiInspectorField Class Base for Assets
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeAsset : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeAsset);

   StringTableEntry mAssetType;

   SimObjectPtr<GuiButtonCtrl>   mBrowseButton;
   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl*        constructEditControl(S32 width);
};

//////////////////////////////////////////////////////////////////////////
// TypeImageAssetPtr GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeImageAssetPtr : public GuiInspectorTypeAsset
{
private:
   typedef GuiInspectorTypeAsset Parent;
public:
   GuiInspectorTypeImageAssetPtr();

   DECLARE_CONOBJECT(GuiInspectorTypeImageAssetPtr);
   static void consoleInit();
};

//////////////////////////////////////////////////////////////////////////
// TypeAnimationAssetPtr GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeAnimationAssetPtr : public GuiInspectorTypeAsset
{
private:
   typedef GuiInspectorTypeAsset Parent;
public:
   GuiInspectorTypeAnimationAssetPtr();

   DECLARE_CONOBJECT(GuiInspectorTypeAnimationAssetPtr);
   static void consoleInit();
};

//////////////////////////////////////////////////////////////////////////
// TypeAudioAssetPtr GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeAudioAssetPtr : public GuiInspectorTypeAsset
{
private:
   typedef GuiInspectorTypeAsset Parent;
public:
   GuiInspectorTypeAudioAssetPtr();

   DECLARE_CONOBJECT(GuiInspectorTypeAudioAssetPtr);
   static void consoleInit();
};

//////////////////////////////////////////////////////////////////////////
// TypeFontAssetPtr GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeFontAssetPtr : public GuiInspectorTypeAsset
{
private:
   typedef GuiInspectorTypeAsset Parent;
public:
   GuiInspectorTypeFontAssetPtr();

   DECLARE_CONOBJECT(GuiInspectorTypeFontAssetPtr);
   static void consoleInit();
};

//////////////////////////////////////////////////////////////////////////
// TypeParticleAssetPtr GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeParticleAssetPtr : public GuiInspectorTypeAsset
{
private:
   typedef GuiInspectorTypeAsset Parent;
public:
   GuiInspectorTypeParticleAssetPtr();

   DECLARE_CONOBJECT(GuiInspectorTypeParticleAssetPtr);
   static void consoleInit();
};

#endif
