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

#ifndef _GUIPARTICLEGRAPHINSPECTOR_H_
#define _GUIPARTICLEGRAPHINSPECTOR_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

class GuiParticleGraphInspector : public GuiControl
{
private:
   typedef GuiControl Parent;
   StringTableEntry mTargetField;
   ParticleAsset* mTargetAsset;
   U32 mEmitterIndex;

   F32 mMinX, mMinY, mMaxX, mMaxY; //Display settings
   StringTableEntry mLabelX, mLabelY; 
   StringTableEntry mMaxYLabel, mMinYLabel, mMaxXLabel, mMinXLabel;

   const F32 mRadius = 7; //size of a point

   RectI mGridRect;
   Point2I mCalculationOffset;

public:
	struct GraphPoint
	{
		GraphPoint() {}
		GraphPoint(Point2I p, F32 time, F32 value, U32 index) { mTime = time; mValue = value; mPoint = p; mIndex = index; }

		F32 mTime;
		F32 mValue;
		Point2I mPoint;
		U32 mIndex;
	};
	S32 mSelectedIndex;
	bool mDirty;
	Vector<GraphPoint> mPointList;

   //creation methods
   DECLARE_CONOBJECT(GuiParticleGraphInspector);
   GuiParticleGraphInspector();
   static void initPersistFields();

   virtual void inspectObject(ParticleAsset* object);
   virtual void setDisplayField(const char* fieldName);
   virtual void setDisplayField(const char* fieldName, U16 index);
   virtual void setDisplayArea(StringTableEntry minX, StringTableEntry minY, StringTableEntry maxX, StringTableEntry maxY);
   virtual void setDisplayLabels(const char* labelX, const char* labelY);

   virtual void resize(const Point2I &newPosition, const Point2I &newExtent);
   virtual void setControlProfile(GuiControlProfile *prof);

   virtual void onTouchUp(const GuiEvent &event);
   virtual void onTouchDown(const GuiEvent &event);
   virtual void onTouchDragged(const GuiEvent &event);

   void onRender(Point2I offset, const RectI &updateRect);

protected:
	U32 findHitGraphPoint(const Point2I &point);
	F32 getGraphValue(const F32 y);
	F32 getGraphTime(const F32 x);

	void calculatePoints(const RectI &contentRect);
	void renderLabels(const RectI &contentRect, const ColorI &labelColor);
	void renderGrid(const RectI &contentRect, const ColorI &gridColor);
	void renderPoints(const RectI &contentRect, const ColorI &lineColor);
	void renderDot(const RectI &contentRect, const Point2I &point, const Point2I &cursorPt, bool isSelected);
	void renderLine(const RectI &contentRect, const Point2I &point1, const Point2I &point2, const ColorI &lineColor);

	ParticleAssetField* getTargetField();
};

#endif
