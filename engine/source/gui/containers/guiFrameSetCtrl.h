#ifndef _GUIFRAMESETCTRL_H_
#define _GUIFRAMESETCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

#include "graphics/dgl.h"
#include "console/console.h"
#include "console/consoleTypes.h"

class GuiFrameSetCtrl : public GuiEasingSupport
{
private:
	typedef GuiControl Parent;
	U32 mNextFrameID;
	bool mResizeGuard;
	bool mDepressed;
	S32 mFrameDragAnchor;
	
	// Sizing Cursors
	GuiCursor* mLeftRightCursor;
	GuiCursor* mUpDownCursor;

public:
	class Frame
	{
	public:
		Frame() : owner(nullptr), parent(nullptr), child1(nullptr), child2(nullptr),
			control(nullptr), isVertical(true), id(1), extent(Point2I(100, 100)), dividerRect(RectI()) { }
		Frame(GuiFrameSetCtrl* theOwner, Frame* theParent) : owner(theOwner), parent(theParent), child1(nullptr), child2(nullptr), 
			control(nullptr), isVertical(true), id(1), extent(Point2I(100, 100)), dividerRect(RectI()) { }
		virtual ~Frame() { }

		bool isVertical;
		U32 id;
		Point2I extent;
		bool isAnchored;
		RectI dividerRect;

		GuiFrameSetCtrl* owner;
		Frame* parent;
		Frame* child1;
		Frame* child2;

		GuiControl* control;

		void resize(const Point2I& newPosition, const Point2I& newExtent);
		Frame* findFrame(const S32 frameID);
		Frame* findEmptyFrame();
		Frame* twin();
		Frame* findHitDivider(const Point2I& position);
	};
	Frame mRootFrame;
	Frame* mHitDivider;
	Frame* mOldHitDivider;
	U8 mDividerThickness; 
	const S32 minSize = 20;


private:

public:
	GuiFrameSetCtrl();
	static void initPersistFields();
	
	bool onAdd();
	virtual void parentResized(const Point2I& oldParentExtent, const Point2I& newParentExtent);
	void resize(const Point2I& newPosition, const Point2I& newExtent);
	//void childResized(GuiControl *child);
	void inspectPostApply();
	bool onWake();
	void onSleep();
	void onChildAdded(GuiControl* child);
	void onChildRemoved(SimObject* child);
	void childResized(GuiControl* child);
	void childMoved(GuiControl* child);
	void childrenReordered();

	Point2I splitFrame(S32 frameID, bool isVertical);
	void anchorFrame(S32 frameID);
	void setFrameSize(S32 frameID, S32 size);
	void onRender(Point2I offset, const RectI& updateRect);
	void onTouchMove(const GuiEvent& event);
	void onTouchDragged(const GuiEvent& event);
	void onTouchDown(const GuiEvent& event);
	void onTouchUp(const GuiEvent& event);
	void getCursor(GuiCursor*& cursor, bool& showCursor, const GuiEvent& lastGuiEvent);

	DECLARE_CONOBJECT(GuiFrameSetCtrl);
};

#endif // _GUIFRAMESETCTRL_H_