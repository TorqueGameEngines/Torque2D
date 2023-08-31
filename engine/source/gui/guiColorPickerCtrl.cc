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
#include "console/consoleTypes.h"
#include "platform/platformAudio.h"
#include "gui/guiCanvas.h"
#include "gui/buttons/guiButtonCtrl.h"
#include "gui/guiDefaultControlRender.h"
#include "gui/guiColorPickerCtrl.h"

#include "gui/guiColorPickerCtrl_ScriptBinding.h"

/// @name Common colors we use
/// @{
ColorF colorWhite(1., 1., 1.);
ColorF colorWhiteBlend(1., 1., 1., .75);
ColorF colorBlack(0.0f, 0.0f, 0.0f);
ColorF colorAlpha(0.0f, 0.0f, 0.0f, 0.0f);
ColorF colorAlphaW(1.0f, 1.0f, 1.0f, 0.0f);

ColorI GuiColorPickerCtrl::mColorRange[9] = {
   ColorI(255,255,255), // White
   ColorI(255,0,0),     // Red
	ColorI(255,255,0),   // Yellow
	ColorI(0,255,0),     // Green
	ColorI(0,255,255),   // Light blue
	ColorI(0,0,255),     // Blue
	ColorI(255,0,255),   // Pink
	ColorI(255,0,0),     // Red
   ColorI(0,0,0)        // Black
};
/// @}

IMPLEMENT_CONOBJECT(GuiColorPickerCtrl);

//--------------------------------------------------------------------------
GuiColorPickerCtrl::GuiColorPickerCtrl()
{
	mBounds.extent.set(60, 30);
	mDisplayMode = pPallet;
	mBaseColor = ColorF(0.475, 0.494, 0.965); //Cornflower Blue ;)
	mPickColor = ColorF(0.475, 0.494, 0.965);
	mSelectorPos = Point2I(0, 0);
	mMouseDown = mMouseOver = false;
	mActive = true;
	mPositionChanged = true;
	mActionOnMove = false;
	mShowSelector = false;
	mIsContainer = false;

	mProfile = NULL;
	mSelectorProfile = NULL;

	setField("profile", "GuiDefaultProfile");
	setField("SelectorProfile", "GuiColorSelectorProfile");
}

//--------------------------------------------------------------------------
static EnumTable::Enums gColorPickerModeEnums[] =
{
   { GuiColorPickerCtrl::pPallet,		"Pallet"   },
   { GuiColorPickerCtrl::pHorizColorRange,	"HorizColor"},
   { GuiColorPickerCtrl::pVertColorRange,	"VertColor" },
   { GuiColorPickerCtrl::pHorizColorBrightnessRange,	"HorizBrightnessColor"},
   { GuiColorPickerCtrl::pVertColorBrightnessRange,	"VertBrightnessColor" },
   { GuiColorPickerCtrl::pBlendColorRange,	"BlendColor"},
   { GuiColorPickerCtrl::pHorizAlphaRange,	"HorizAlpha"},
   { GuiColorPickerCtrl::pVertAlphaRange,	"VertAlpha" },
   { GuiColorPickerCtrl::pDropperBackground,	"Dropper" },
};

static EnumTable gColorPickerModeTable(9, gColorPickerModeEnums);

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addGroup("ColorPicker");
	addField("BaseColor", TypeColorF, Offset(mBaseColor, GuiColorPickerCtrl));
	addField("PickColor", TypeColorF, Offset(mPickColor, GuiColorPickerCtrl));
	addField("DisplayMode", TypeEnum, Offset(mDisplayMode, GuiColorPickerCtrl), 1, &gColorPickerModeTable);
	addField("ActionOnMove", TypeBool, Offset(mActionOnMove, GuiColorPickerCtrl));
	addField("ShowSelector", TypeBool, Offset(mShowSelector, GuiColorPickerCtrl));
	addField("SelectorProfile", TypeGuiProfile, Offset(mSelectorProfile, GuiColorPickerCtrl));
	endGroup("ColorPicker");
}


bool GuiColorPickerCtrl::onWake()
{
	if (!Parent::onWake())
		return false;

	//increment the tab profile
	if (mSelectorProfile != NULL)
		mSelectorProfile->incRefCount();

	return true;
}

void GuiColorPickerCtrl::onSleep()
{
	Parent::onSleep();

	//decrement the tab profile referrence
	if (mSelectorProfile != NULL)
		mSelectorProfile->decRefCount();
}

//--------------------------------------------------------------------------
/// Function to invoke calls to draw the picker box and selector
void GuiColorPickerCtrl::renderColorBox(const RectI& pickerBounds)
{
	RectI oldClip = dglGetClipRect();
	RectI clipRect = RectI(pickerBounds);
	clipRect.intersect(oldClip);
	dglSetClipRect(clipRect);

	ColorF baseAlpha = ColorF(mBaseColor);
	baseAlpha.alpha = 0.0f;

	// Draw color box differently depending on mode
	RectI blendRect;
	switch (mDisplayMode)
	{
	case pHorizColorRange:
		dglDrawBlendRangeBox(pickerBounds, false, 7, mColorRange + 1);
		break;
	case pVertColorRange:
		dglDrawBlendRangeBox(pickerBounds, true, 7, mColorRange + 1);
		break;
	case pHorizColorBrightnessRange:
		dglDrawBlendRangeBox(pickerBounds, false, 7, mColorRange + 1);
		blendRect = pickerBounds;
		blendRect.point.y++;
		blendRect.extent.y--;
		dglDrawBlendBox(blendRect, colorAlpha, colorAlpha, colorBlack, colorBlack);
		break;
	case pVertColorBrightnessRange:
		dglDrawBlendRangeBox(pickerBounds, true, 7, mColorRange + 1);
		blendRect = pickerBounds;
		blendRect.point.x++;
		blendRect.extent.x--;
		dglDrawBlendBox(pickerBounds, colorAlpha, colorBlack, colorBlack, colorAlpha);
		break;
	case pHorizAlphaRange:
		dglRenderCheckers(pickerBounds);
		dglDrawBlendBox(pickerBounds, baseAlpha, mBaseColor, mBaseColor, baseAlpha);
		break;
	case pVertAlphaRange:
		dglRenderCheckers(pickerBounds);
		dglDrawBlendBox(pickerBounds, baseAlpha, baseAlpha, mBaseColor, mBaseColor);
		break;
	case pBlendColorRange:
		dglDrawBlendBox(pickerBounds, colorWhite, mBaseColor, mBaseColor, colorWhite);
		blendRect = pickerBounds;
		blendRect.point.y++;
		blendRect.extent.y--;
		dglDrawBlendBox(blendRect, colorAlpha, colorAlpha, colorBlack, colorBlack);
		break;
	case pDropperBackground:
		break;
	case pPallet:
	default:
		dglDrawRectFill(pickerBounds, mBaseColor);
		break;
	}
	dglSetClipRect(oldClip);
}

void GuiColorPickerCtrl::renderSelector(const RectI& pickerBounds)
{
	RectI oldClip = dglGetClipRect();
	RectI clipRect = RectI(pickerBounds);
	clipRect.intersect(oldClip);
	dglSetClipRect(clipRect);

	Point2I selectorPos = localToGlobalCoord(mSelectorPos);

	if (mDisplayMode != pPallet && mDisplayMode != pDropperBackground)
	{
		if (mDisplayMode == pHorizColorBrightnessRange ||
			mDisplayMode == pVertColorBrightnessRange ||
			mDisplayMode == pBlendColorRange)
		{
			drawSelector(pickerBounds, selectorPos, sHorizontal);
			drawSelector(pickerBounds, selectorPos, sVertical);
		}
		drawSelector(pickerBounds, selectorPos, sCircle);
	}
	dglSetClipRect(oldClip);
}

void GuiColorPickerCtrl::drawSelector(const RectI& bounds, Point2I& selectorPos, SelectorMode mode)
{
	//bounds is a global rect. The clip rect has been set to the bounds.
	//selectorPos is a global position.
	if (!mShowSelector || !mSelectorProfile)
		return;


	GuiBorderProfile* selProfile = mSelectorProfile->getTopProfile();
	S32 padding = selProfile->getPadding(NormalState);
	S32 margin = selProfile->getMargin(NormalState);
	S32 inner = (margin * 2) + 1;
	S32 diameter = (padding * 2) + inner;

	switch (mode)
	{
	case sVertical:
		// Up -> Pos
		if (selectorPos.y != bounds.point.y)
			dglDrawLine(selectorPos.x, bounds.point.y, selectorPos.x, selectorPos.y - diameter, colorWhiteBlend);
		// Down -> Pos
		if (selectorPos.y != bounds.point.y + bounds.extent.y)
			dglDrawLine(selectorPos.x, selectorPos.y + diameter, selectorPos.x, bounds.point.y + bounds.extent.y, colorWhiteBlend);
		break;
	case sHorizontal:
		// Left -> Pos
		if (selectorPos.x != bounds.point.x)
			dglDrawLine(bounds.point.x, selectorPos.y, selectorPos.x - diameter, selectorPos.y, colorWhiteBlend);
		// Right -> Pos
		if (selectorPos.x != bounds.point.x + bounds.extent.x)
			dglDrawLine(selectorPos.x + diameter, selectorPos.y, bounds.point.x + bounds.extent.x, selectorPos.y, colorWhiteBlend);
		break;
	case sCircle:
		GuiControlState currentState = NormalState;
		if (mMouseDown)
		{
			currentState = SelectedState;
		}
		else
		{
			GuiCanvas* canvas = getRoot();
			if (canvas)
			{
				Point2I curPos = canvas->getCursorPos();
				S32 dist = (curPos - selectorPos).len();
				if (dist <= diameter)
				{
					currentState = HighlightState;
				}
			}
		}
		renderBorderedRing(selectorPos, diameter, inner, mSelectorProfile, currentState);
		break;
	}
}

void GuiColorPickerCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	if(mDisplayMode != pDropperBackground)
	{
		RectI ctrlRect = applyMargins(offset, mBounds.extent, NormalState, mProfile);

		renderUniversalRect(ctrlRect, mProfile, NormalState);

		RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, NormalState, mProfile);
		RectI contentRect = applyPadding(fillRect.point, fillRect.extent, NormalState, mProfile);

		renderColorBox(contentRect);
		updatePickColor(offset, contentRect);
		renderText(offset, updateRect.extent, mText, mProfile);
		renderSelector(contentRect);
	}
	else
	{
		updatePickColor(offset, updateRect);
	}
}

void GuiColorPickerCtrl::updatePickColor(const Point2I& offset,  const RectI& contentRect)
{
	if (mPositionChanged && mDisplayMode != pPallet)
	{
		mPositionChanged = false;
		Point2I extent = Canvas->getExtent();
		U32 buf_x = offset.x + mSelectorPos.x;
		U32 buf_y = extent.y - (offset.y + mSelectorPos.y + 1);

		S32 xDelta = (contentRect.point.x - offset.x);
		S32 yDelta = (contentRect.point.y - offset.y);
		F32 numeratorX = mSelectorPos.x - xDelta;
		F32 denominatorX = contentRect.extent.x - 1;
		F32 numeratorY = mSelectorPos.y - yDelta;
		F32 denominatorY = contentRect.extent.y - 1;
		F32 varX = numeratorX / getMax(denominatorX, 1.0f);
		F32 varY = numeratorY / getMax(denominatorY, 1.0f);

		if (mDisplayMode == pHorizAlphaRange || mDisplayMode == pVertAlphaRange)
		{
			mPickColor = mBaseColor;
			mPickColor.alpha = mDisplayMode == pHorizAlphaRange ? varX : varY;
		}
		else if ((mDisplayMode == pHorizColorRange && (varX == 1.0f || varX == 0.0f)) ||
			(mDisplayMode == pVertColorRange && (varY == 1.0f || varY == 0.0f)) ||
			(mDisplayMode == pHorizColorBrightnessRange && varY == 0.0f && (varX == 1.0f || varX == 0.0f)) ||
			(mDisplayMode == pVertColorBrightnessRange && varX == 0.0f && (varY == 1.0f || varY == 0.0f)))
		{
			mPickColor = mColorRange[1];
		}
		else if ((mDisplayMode == pBlendColorRange && varY == 1.0f) ||
			(mDisplayMode == pHorizColorBrightnessRange && varY == 1.0f) ||
			(mDisplayMode == pVertColorBrightnessRange && varX == 1.0f))
		{
			mPickColor = colorBlack;
		}
		else if ((mDisplayMode == pHorizColorBrightnessRange && (varX == 0.0f || varX == 1.0f)) ||
			(mDisplayMode == pVertColorBrightnessRange && (varY == 0.0f || varY == 1.0f)))
		{
			mPickColor.red = mDisplayMode == pHorizColorBrightnessRange ? (mColorRange[1].red / 255) * (1.0f - varY) : (mColorRange[1].red / 255) * (1.0f - varX);
			mPickColor.green = mDisplayMode == pHorizColorBrightnessRange ? (mColorRange[1].green / 255) * (1.0f - varY) : (mColorRange[1].green / 255) * (1.0f - varX);
			mPickColor.blue = mDisplayMode == pHorizColorBrightnessRange ? (mColorRange[1].blue / 255) * (1.0f - varY) : (mColorRange[1].blue / 255) * (1.0f - varX);
		}
		else if (mDisplayMode == pBlendColorRange && varY == 0.0f)
		{
			if(varX == 0.0f)
			{
				mPickColor = colorWhite;
			}
			else if (varX == 1.0f)
			{
				mPickColor = mBaseColor;
			}
			else
			{
				mPickColor.red = 1.0f - ((1.0f - mBaseColor.red) * varX);
				mPickColor.green = 1.0f - ((1.0f - mBaseColor.green) * varX);
				mPickColor.blue = 1.0f - ((1.0f - mBaseColor.blue) * varX);
			}
		}
		else
		{
			GLubyte rBuffer[4] = { 255, 255, 255, 255 };
			glReadBuffer(GL_BACK);
			glReadPixels(buf_x, buf_y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, rBuffer);
			mPickColor = ColorI(rBuffer[0], rBuffer[1], rBuffer[2], 255);
		}

		if (mActionOnMove)
			onAction();

		//char buffer[1024];
		//dSprintf(buffer, 1024, "%f, %f, %f, %f, local:(%d %d) global:(%d %d)", mPickColor.red, mPickColor.green, mPickColor.blue, mPickColor.alpha, mSelectorPos.x, mSelectorPos.y, buf_x, buf_y);

		//setText(buffer);
	}
}

void GuiColorPickerCtrl::resize(const Point2I& newPosition, const Point2I& newExtent)
{
	Point2F pos = Point2F((F32)mSelectorPos.x / (F32)mBounds.extent.x, (F32)mSelectorPos.y / (F32)mBounds.extent.y);

	Parent::resize(newPosition, newExtent);

	mSelectorPos.x = mRound(newExtent.x * pos.x);
	mSelectorPos.y = mRound(newExtent.y * pos.y);
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::setSelectorPos(const Point2I& pos)
{
	Point2I zero = Point2I::Zero;
	RectI rect = getInnerRect(zero);

	mSelectorPos = pos;
	if (pos.x < rect.point.x)
	{
		mSelectorPos.x = rect.point.x;
	}
	else if (pos.x >= (rect.point.x + rect.extent.x))
	{
		mSelectorPos.x = rect.point.x + rect.extent.x - 1;
	}

	if (pos.y < rect.point.y)
	{
		mSelectorPos.y = rect.point.y;
	}
	else if (pos.y >= (rect.point.y + rect.extent.y))
	{
		mSelectorPos.y = rect.point.y + rect.extent.y - 1;
	}

	if (mDisplayMode == pHorizColorRange || mDisplayMode == pHorizAlphaRange)
	{
		mSelectorPos.y = rect.centre().y;
	}
	else if (mDisplayMode == pVertColorRange || mDisplayMode == pVertAlphaRange)
	{
		mSelectorPos.x = rect.centre().x;
	}

	mPositionChanged = true;
	setUpdate();
}

Point2I GuiColorPickerCtrl::getRangeBoxColorPos(bool vertical, const ColorF& targetColor)
{
	// Calculate hue
	F32 hue = targetColor.getHue();
	RectI contentRect = getInnerRect();

	// Transform the hue value to [0-1].
	F32 hueFraction = hue / 360;

	// Calculate position in range box
	Point2I position;
	if (vertical)
	{
		position.x = (S32)(0.5f * (contentRect.extent.x));
		position.y = (S32)(hueFraction * (contentRect.extent.y));
	}
	else
	{
		position.x = (S32)(hueFraction * (contentRect.extent.x));
		position.y = (S32)(0.5f * (contentRect.extent.y));
	}

	return position;
}

Point2I GuiColorPickerCtrl::getBlendBoxColorPos(const ColorF& targetColor)
{
	// Calculate hue
	F32 hue = targetColor.getHue();
	RectI contentRect = getInnerRect();

	// Calculate the largest, smallest RGB components of the hue
	F32 largest, smallest, baseLargest, baseSmallest;
	if (hue >= 0 && hue < 60)
	{
		largest = targetColor.red;
		smallest = targetColor.blue;
		baseLargest = mBaseColor.red;
		baseSmallest = mBaseColor.blue;
	}
	else if (hue >= 60 && hue < 120)
	{
		largest = targetColor.green;
		smallest = targetColor.blue;
		baseLargest = mBaseColor.green;
		baseSmallest = mBaseColor.blue;
	}
	else if (hue >= 120 && hue < 180)
	{
		largest = targetColor.green;
		smallest = targetColor.red;
		baseLargest = mBaseColor.green;
		baseSmallest = mBaseColor.red;
	}
	else if (hue >= 180 && hue < 240)
	{
		largest = targetColor.blue;
		smallest = targetColor.red;
		baseLargest = mBaseColor.blue;
		baseSmallest = mBaseColor.red;
	}
	else if (hue >= 240 && hue < 300)
	{
		largest = targetColor.blue;
		smallest = targetColor.green;
		baseLargest = mBaseColor.blue;
		baseSmallest = mBaseColor.green;
	}
	else if (hue >= 300 && hue < 360)
	{
		largest = targetColor.red;
		smallest = targetColor.green;
		baseLargest = mBaseColor.red;
		baseSmallest = mBaseColor.green;
	}
	else
	{
		// Color hue must be between 0 and 360.
		Con::errorf("getBlendBoxColorPos - Color hue is outside allowable range.");
		return Point2I(0, 0);
	}

	// Calculate position in range box
	Point2I position;

	F32 h = 1.0f - (smallest / largest);
	F32 v = 1.0f - largest;

	position.x = (S32)(h * (contentRect.extent.x - 1));
	position.y = (S32)(v * (contentRect.extent.y - 1));

	return position;
}

Point2I GuiColorPickerCtrl::getAlphaBoxColorPos(bool vertical, const ColorF& targetColor)
{
	RectI contentRect = getInnerRect();
	Point2I position;

	if (vertical)
	{
		position.x = contentRect.centre().x;
		position.y = (S32)(targetColor.alpha * (contentRect.extent.y - 1));
	}
	else
	{
		position.x = (S32)(targetColor.alpha * (contentRect.extent.x - 1));
		position.y = contentRect.centre().y;
	}

	return position;
}

Point2I GuiColorPickerCtrl::getSelectorPositionForColor(const ColorF& targetColor)
{
	Point2I position(0, 0);

	switch (mDisplayMode)
	{
	case pHorizColorRange:
		position = getRangeBoxColorPos(false, targetColor);
		break;
	case pVertColorRange:
		position = getRangeBoxColorPos(true, targetColor);
		break;

	case pBlendColorRange:
		position = getBlendBoxColorPos(targetColor);
		break;

	case pHorizAlphaRange:
		position = getAlphaBoxColorPos(false, targetColor);
		break;
	case pVertAlphaRange:
		position = getAlphaBoxColorPos(true, targetColor);
		break;

	default:
		Con::errorf("Display mode does not support the function: getSelectorPositionForColor().");
		break;
	}

	return position;
}


//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onTouchDown(const GuiEvent& event)
{
	if (!mActive)
		return;

	if (mDisplayMode == pDropperBackground)
		return;

	mouseLock(this);

	if (mProfile->mCanKeyFocus)
		setFirstResponder();

	// Update the picker cross position
	if (mDisplayMode != pPallet)
	{
		setSelectorPos(globalToLocalCoord(event.mousePoint));
		Canvas->showCursor(false);
	}

	mMouseDown = true;

	Parent::onTouchDown(event);
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onTouchDragged(const GuiEvent& event)
{
	if ((mActive && mMouseDown) || (mActive && (mDisplayMode == pDropperBackground)))
	{
		// Update the picker cross position
		if (mDisplayMode != pPallet)
			setSelectorPos(globalToLocalCoord(event.mousePoint));
	}

	if (!mActionOnMove && mAltConsoleCommand[0])
		Con::evaluate(mAltConsoleCommand, false);

	Parent::onTouchDragged(event);
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onTouchMove(const GuiEvent& event)
{
	// Only for dropper mode
	if (mActive && (mDisplayMode == pDropperBackground))
		setSelectorPos(globalToLocalCoord(event.mousePoint));

	Parent::onTouchMove(event);
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onTouchEnter(const GuiEvent& event)
{
	mMouseOver = true;

	Parent::onTouchEnter(event);
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onTouchLeave(const GuiEvent& event)
{
	// Reset state
	mMouseOver = false;

	Parent::onTouchLeave(event);
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onTouchUp(const GuiEvent& event)
{
	//if we released the mouse within this control, perform the action
	if (mActive && mMouseDown && (mDisplayMode != pDropperBackground))
	{
		onAction();
		mMouseDown = false;
	}
	else if (mActive && (mDisplayMode == pDropperBackground))
	{
		// In a dropper, the alt command executes the mouse up action (to signal stopping)
		if (mAltConsoleCommand[0])
			Con::evaluate(mAltConsoleCommand, false);
	}

	mouseUnlock();
	Canvas->showCursor(true);

	Parent::onTouchUp(event);
}

//--------------------------------------------------------------------------
const char* GuiColorPickerCtrl::getScriptValue()
{
	static char temp[256];
	ColorF color = getValue();
	dSprintf(temp, 256, "%g %g %g %g", color.red, color.green, color.blue, color.alpha);
	return temp;
}

//--------------------------------------------------------------------------    
void GuiColorPickerCtrl::setScriptValue(const char* value)
{
	ColorF newValue;
	dSscanf(value, "%g %g %g %g", &newValue.red, &newValue.green, &newValue.blue, &newValue.alpha);
	setValue(newValue);
}

void GuiColorPickerCtrl::setControlSelectorProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiColorPickerCtrl::setControlSelectorProfile: invalid selector profile");
	if (prof == mSelectorProfile)
		return;
	if (mAwake)
		mSelectorProfile->decRefCount();
	mSelectorProfile = prof;
	if (mAwake)
		mSelectorProfile->incRefCount();
}

