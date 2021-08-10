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
#include "console/consoleTypes.h"
#include "graphics/dgl.h"
#include "graphics/TextureManager.h"
#include "gui/guiSliderCtrl.h"
#include "gui/guiDefaultControlRender.h"
#include "platform/event.h"

IMPLEMENT_CONOBJECT(GuiSliderCtrl);

//----------------------------------------------------------------------------
GuiSliderCtrl::GuiSliderCtrl(void)
{
    mActive = true;
    mRange.set(0.0f, 1.0f);
    mTicks = 10;
    mValue = 0.5f;
    mThumbSize.set(8, 20);
    mShiftPoint = 5;
    mShiftExtent = 10;
    mDisplayValue = false;
    mMouseOver = false;
    mDepressed = false;
}

//----------------------------------------------------------------------------
void GuiSliderCtrl::initPersistFields()
{
    Parent::initPersistFields();

    addGroup("Slider");
    addField("range", TypePoint2F, Offset(mRange, GuiSliderCtrl));
    addField("ticks", TypeS32, Offset(mTicks, GuiSliderCtrl));
    addField("value", TypeF32, Offset(mValue, GuiSliderCtrl));
    endGroup("Slider");
}

//----------------------------------------------------------------------------
ConsoleMethod( GuiSliderCtrl, getValue, F32, 2, 2, "() Use the getValue method to get the current value of the slider.\n"
        "@return Returns current value of control (position of slider)"){
    return object->getValue();
}

//----------------------------------------------------------------------------
void GuiSliderCtrl::setScriptValue(const char *val)
{
    mValue = dAtof(val);
    updateThumb(mValue, false);
}

//----------------------------------------------------------------------------
bool GuiSliderCtrl::onWake()
{
    if (!Parent::onWake())
        return false;

    if (mThumbSize.y + mProfile->mFont->getHeight() - 4 <= (U32) mBounds.extent.y)
        mDisplayValue = true;
    else
        mDisplayValue = false;

    updateThumb(mValue, false, true);

    mHasTexture = mProfile->constructBitmapArray() >= NumBitmaps;
    if (mHasTexture)
        mBitmapBounds = mProfile->mBitmapArrayRects.address();

    return true;
}

//----------------------------------------------------------------------------
void GuiSliderCtrl::onTouchDown(const GuiEvent &event)
{
    if (!mActive || !mAwake || !mVisible)
        return;

    mouseLock();
    setFirstResponder();
    mDepressed = true;

    Point2I curMousePos = globalToLocalCoord(event.mousePoint);
    F32 value;
    if (mBounds.extent.x >= mBounds.extent.y)
        value = F32(curMousePos.x - mShiftPoint) / F32(mBounds.extent.x - mShiftExtent) * (mRange.y - mRange.x) + mRange.x;
    else
        value = F32(curMousePos.y) / F32(mBounds.extent.y) * (mRange.y - mRange.x) + mRange.x;

    updateThumb(value, !(event.modifier & SI_SHIFT));
}

//----------------------------------------------------------------------------
void GuiSliderCtrl::onTouchDragged(const GuiEvent &event)
{
    if (!mActive || !mAwake || !mVisible)
        return;

    Point2I curMousePos = globalToLocalCoord(event.mousePoint);
    F32 value;
    if (mBounds.extent.x >= mBounds.extent.y)
        value = F32(curMousePos.x - mShiftPoint) / F32(mBounds.extent.x - mShiftExtent) * (mRange.y - mRange.x) + mRange.x;
    else
        value = F32(curMousePos.y) / F32(mBounds.extent.y) * (mRange.y - mRange.x) + mRange.x;

    if (value > mRange.y)
        value = mRange.y;
    else if (value < mRange.x)
        value = mRange.x;

    if (!(event.modifier & SI_SHIFT) && mTicks > 2)
    {
        // If the shift key is held, snap to the nearest tick, if any are being drawn

        F32 tickStep = (mRange.y - mRange.x) / F32(mTicks + 1);

        F32 tickSteps = (value - mRange.x) / tickStep;
        S32 actualTick = S32(tickSteps + 0.5);

        value = actualTick * tickStep + mRange.x;
        AssertFatal(value <= mRange.y && value >= mRange.x, "Error, out of bounds value generated from shift-snap of slider");
    }

    Con::executef(this, 1, "onMouseDragged");

    updateThumb(value, !(event.modifier & SI_SHIFT));
}

//----------------------------------------------------------------------------
void GuiSliderCtrl::onTouchUp(const GuiEvent &)
{
    if (!mActive || !mAwake || !mVisible)
        return;
    mDepressed = false;
    mouseUnlock();
    execConsoleCallback();
}

void GuiSliderCtrl::onTouchEnter(const GuiEvent &event)
{
    setUpdate();
    if (isMouseLocked())
    {
        mDepressed = true;
        mMouseOver = true;
    }
    else
    {
        if (mActive && mProfile->mSoundButtonOver)
        {
            //F32 pan = (F32(event.mousePoint.x)/F32(Canvas->mBounds.extent.x)*2.0f-1.0f)*0.8f;
            AUDIOHANDLE handle = alxCreateSource(mProfile->mSoundButtonOver);
            alxPlay(handle);
        }
        mMouseOver = true;
    }
}

void GuiSliderCtrl::onTouchLeave(const GuiEvent &)
{
    setUpdate();
    if (isMouseLocked())
        mDepressed = false;
    mMouseOver = false;
}

//----------------------------------------------------------------------------
void GuiSliderCtrl::updateThumb(F32 _value, bool snap, bool onWake)
{
    if (snap && mTicks > 1)
    {
        // If the shift key is held, snap to the nearest tick, if any are being drawn

        F32 tickStep = (mRange.y - mRange.x) / F32(mTicks + 1);

        F32 tickSteps = (_value - mRange.x) / tickStep;
        S32 actualTick = S32(tickSteps + 0.5);

        _value = actualTick * tickStep + mRange.x;
        AssertFatal(_value <= mRange.y && _value >= mRange.x, "Error, out of bounds value generated from shift-snap of slider");
    }


    mValue = _value;
    // clamp the thumb to legal values
    if (mValue < mRange.x) mValue = mRange.x;
    if (mValue > mRange.y) mValue = mRange.y;

    Point2I ext = mBounds.extent;
    ext.x -= (mShiftExtent + mThumbSize.x) / 2;
    // update the bounding thumb rect
    if (mBounds.extent.x >= mBounds.extent.y)
    {  // HORZ thumb
        S32 mx = (S32) ((F32(ext.x) * (mValue - mRange.x) / (mRange.y - mRange.x)));
        S32 my = ext.y / 2;
        if (mDisplayValue)
            my = mThumbSize.y / 2;

        mThumb.point.x = mx - (mThumbSize.x / 2);
        mThumb.point.y = my - (mThumbSize.y / 2);
        mThumb.extent = mThumbSize;
    }
    else
    {  // VERT thumb
        S32 mx = ext.x / 2;
        S32 my = (S32) ((F32(ext.y) * (mValue - mRange.x) / (mRange.y - mRange.x)));
        mThumb.point.x = mx - (mThumbSize.y / 2);
        mThumb.point.y = my - (mThumbSize.x / 2);
        mThumb.extent.x = mThumbSize.y;
        mThumb.extent.y = mThumbSize.x;
    }
    setFloatVariable(mValue);
    setUpdate();

    // Use the alt console command if you want to continually update:
    if (!onWake)
        execAltConsoleCallback();
}

//----------------------------------------------------------------------------
void GuiSliderCtrl::onRender(Point2I offset, const RectI &updateRect)
{
    Point2I pos(offset.x + mShiftPoint, offset.y);
    Point2I ext(mBounds.extent.x - mShiftExtent, mBounds.extent.y);
    RectI thumb = mThumb;

    if (mHasTexture)
    {
        if (mTicks > 0)
        {
            // TODO: tick marks should be positioned based on the bitmap dimentions.
            Point2I mid(ext.x, ext.y / 2);
            Point2I oldpos = pos;
            pos += Point2I(1, 0);
            glColor4f(0, 0, 0, 1);

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
            // tick marks
            for (U32 t = 0; t <= (mTicks + 1); t++)
            {
                S32 x = (S32) (F32(mid.x + 1) / F32(mTicks + 1) * F32(t)) + pos.x;
                S32 y = pos.y + mid.y;

                GLfloat shiftVert[] = {
                        (GLfloat) (x),
                        (GLfloat) (y + mShiftPoint),
                        (GLfloat) (x),
                        (GLfloat) (y + mShiftPoint * 2.0f + 2.0f),
                };

                glVertexPointer(2, GL_FLOAT, 0, shiftVert);
                glDrawArrays(GL_LINES, 0, 2);
            }

            glColor4f(0.9f, 0.9f, 0.9f, 1.0f);
            // tick marks
            for (U32 t = 0; t <= (mTicks + 1); t++)
            {
                S32 x = (S32) (F32(mid.x + 1) / F32(mTicks + 1) * F32(t)) + pos.x + 1;
                S32 y = pos.y + mid.y + 1;

                GLfloat shiftVert[] = {
                        (GLfloat) (x),
                        (GLfloat) (y + mShiftPoint),
                        (GLfloat) (x),
                        (GLfloat) (y + mShiftPoint * 2 + 3),
                };

                glVertexPointer(2, GL_FLOAT, 0, shiftVert);
                glDrawArrays(GL_LINES, 0, 2);

            }
#else
         glBegin(GL_LINES);
         // tick marks
         for (U32 t = 0; t <= (mTicks+1); t++)
         {
            S32 x = (S32)(F32(mid.x+1)/F32(mTicks+1)*F32(t)) + pos.x;
            S32 y = pos.y + mid.y;
            glVertex2i(x, y + mShiftPoint);
            glVertex2i(x, y + mShiftPoint*2 + 2);
         }
         glEnd();
         glColor4f(0.9f, 0.9f, 0.9f, 1.0f);
         glBegin(GL_LINES);
         // tick marks
         for (U32 t = 0; t <= (mTicks+1); t++)
         {
            S32 x = (S32)(F32(mid.x+1)/F32(mTicks+1)*F32(t)) + pos.x + 1;
            S32 y = pos.y + mid.y + 1;
            glVertex2i(x, y + mShiftPoint );
            glVertex2i(x, y + mShiftPoint*2 + 3);
         }
         glEnd();
#endif
            pos = oldpos;
        }

        S32 index = SliderButtonNormal;
        if (mMouseOver)
            index = SliderButtonHighlight;
        dglClearBitmapModulation();

        //left border
        dglDrawBitmapSR(mProfile->mTextureHandle, Point2I(offset.x, offset.y + (mBounds.extent.y / 4)), mBitmapBounds[SliderLineLeft]);
        //right border
        dglDrawBitmapSR(mProfile->mTextureHandle, Point2I(offset.x + mBounds.extent.x - mBitmapBounds[SliderLineRight].extent.x, offset.y + (mBounds.extent.y / 4)), mBitmapBounds[SliderLineRight]);


        //draw our center piece to our slider control's border and stretch it
        RectI destRect;
        destRect.point.x = offset.x + mBitmapBounds[SliderLineLeft].extent.x;
        destRect.extent.x = mBounds.extent.x - mBitmapBounds[SliderLineLeft].extent.x - mBitmapBounds[SliderLineRight].extent.x;
        destRect.point.y = offset.y + (mBounds.extent.y / 4);
        destRect.extent.y = mBitmapBounds[SliderLineCenter].extent.y;

        RectI stretchRect;
        stretchRect = mBitmapBounds[SliderLineCenter];
        stretchRect.inset(1, 0);

        dglDrawBitmapStretchSR(mProfile->mTextureHandle, destRect, stretchRect);

        //draw our control slider button
        thumb.point += pos;
        dglDrawBitmapSR(mProfile->mTextureHandle, Point2I(thumb.point.x, offset.y), mBitmapBounds[index]);
    }
    else
    {
        // we're not usina a bitmap, draw procedurally.
        if (mBounds.extent.x >= mBounds.extent.y)
        {
            Point2I mid(ext.x, ext.y / 2);
            if (mDisplayValue)
                mid.set(ext.x, mThumbSize.y / 2);

            glColor4f(0, 0, 0, 1);
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
            // tick marks
            for (U32 t = 0; t <= (mTicks + 1); t++)
            {
                S32 x = (S32) (F32(mid.x - 1) / F32(mTicks + 1) * F32(t));
                GLfloat verts[] = {
                        static_cast<GLfloat>(pos.x + x), static_cast<GLfloat>(pos.y + mid.y - mShiftPoint),
                        static_cast<GLfloat>(pos.x + x), static_cast<GLfloat>(pos.y + mid.y + mShiftPoint)
                };

                glVertexPointer(2, GL_FLOAT, 0, verts);
                glDrawArrays(GL_LINES, 0, 2);
            }

            GLfloat verts[] = {
                    static_cast<GLfloat>(pos.x), static_cast<GLfloat>(pos.y + mid.y),
                    static_cast<GLfloat>(pos.x + mid.x), static_cast<GLfloat>(pos.y + mid.y)
            };

            glVertexPointer(2, GL_FLOAT, 0, verts);
            glDrawArrays(GL_LINES, 0, 2);

        }
        else
        {
            Point2I mid(ext.x / 2, ext.y);
            glColor4f(0, 0, 0, 1);
            // tick marks
            for (U32 t = 0; t <= (mTicks + 1); t++)
            {
                S32 y = (S32) (F32(mid.y - 1) / F32(mTicks + 1) * F32(t));
                GLfloat verts[] = {
                        static_cast<GLfloat>(pos.x + mid.x - mShiftPoint), static_cast<GLfloat>(pos.y + y),
                        static_cast<GLfloat>(pos.x + mid.x + mShiftPoint), static_cast<GLfloat>(pos.y + y)
                };

                glVertexPointer(2, GL_FLOAT, 0, verts);
                glDrawArrays(GL_LINES, 0, 2);
            }

            GLfloat verts[] = {
                    static_cast<GLfloat>(pos.x + mid.x), static_cast<GLfloat>(pos.y),
                    static_cast<GLfloat>(pos.x + mid.x), static_cast<GLfloat>(pos.y + mid.y)
            };
            //for the horizontal line
            glVertexPointer(2, GL_FLOAT, 0, verts);
            glDrawArrays(GL_LINES, 0, 2);
#else
         glBegin(GL_LINES);
            // horz rule
            glVertex2i(pos.x,       pos.y+mid.y);
            glVertex2i(pos.x+mid.x, pos.y+mid.y);

            // tick marks
            for (U32 t = 0; t <= (mTicks+1); t++)
            {
               S32 x = (S32)(F32(mid.x-1)/F32(mTicks+1)*F32(t));
               glVertex2i(pos.x+x, pos.y+mid.y-mShiftPoint);
               glVertex2i(pos.x+x, pos.y+mid.y+mShiftPoint);
            }
         glEnd();
      }
      else
      {
         Point2I mid(ext.x/2, ext.y);

         glColor4f(0, 0, 0, 1);
         glBegin(GL_LINES);
            // horz rule
            glVertex2i(pos.x+mid.x, pos.y);
            glVertex2i(pos.x+mid.x, pos.y+mid.y);

            // tick marks
            for (U32 t = 0; t <= (mTicks+1); t++)
            {
               S32 y = (S32)(F32(mid.y-1)/F32(mTicks+1)*F32(t));
               glVertex2i(pos.x+mid.x-mShiftPoint, pos.y+y);
               glVertex2i(pos.x+mid.x+mShiftPoint, pos.y+y);
            }
         glEnd();
#endif
            mDisplayValue = false;
        }

        // draw the thumb
        thumb.point += pos;
		renderUniversalRect(thumb, mProfile, NormalState);
    }

    if (mDisplayValue)
    {
        char buf[20];
        dSprintf(buf, sizeof(buf), "%0.3g", mValue);

        Point2I textStart = thumb.point;

        S32 txt_w = mProfile->mFont->getStrWidth((const UTF8 *) buf);

        textStart.x += (S32) ((thumb.extent.x / 2.0f));
        textStart.y += thumb.extent.y - 2; //19
        textStart.x -= (txt_w / 2);
        if (textStart.x < offset.x)
            textStart.x = offset.x;
        else if (textStart.x + txt_w > offset.x + mBounds.extent.x)
            textStart.x -= ((textStart.x + txt_w) - (offset.x + mBounds.extent.x));

        dglSetBitmapModulation(mProfile->mFontColor);
        dglDrawText(mProfile->mFont, textStart, buf, mProfile->mFontColors);
    }
    renderChildControls(offset, mBounds, updateRect);
}

