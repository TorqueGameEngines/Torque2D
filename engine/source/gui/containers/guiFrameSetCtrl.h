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
#include "gui/containers/guiWindowCtrl.h"

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

	GuiControlProfile* mDropButtonProfile; //Used to render a drop button (the buttons that appear on frames when dragging a window).

public:
	class Frame
	{
	public:
		Frame() : Frame(nullptr, nullptr) { }
		Frame(GuiFrameSetCtrl* theOwner, Frame* theParent) : owner(theOwner), parent(theParent), child1(nullptr), child2(nullptr), 
			control(nullptr), isVertical(true), id(1), extent(Point2I(100, 100)), dividerRect(RectI()),
			localPosition(Point2I(0, 0)), mLeftButtonRect(RectI()), mRightButtonRect(RectI()),
			mTopButtonRect(RectI()), mBottomButtonRect(RectI()), isAnchored(true), hasLeftRightButtons(false), hasTopBottomButtons(false) { }
		virtual ~Frame() { }
		void deleteChildren();

		bool isVertical;
		U32 id;
		Point2I extent;
		Point2I localPosition;
		bool isAnchored;
		RectI dividerRect;
		RectI mLeftButtonRect;
		RectI mRightButtonRect;
		RectI mTopButtonRect;
		RectI mBottomButtonRect;
		bool hasLeftRightButtons;
		bool hasTopBottomButtons;

		GuiFrameSetCtrl* owner;
		Frame* parent;
		Frame* child1;
		Frame* child2;

		GuiControl* control;

		void resize(const Point2I& newPosition, const Point2I& newExtent);
		void sizeInsertButtons(const Point2I& newPosition, const Point2I& newExtent);
		Frame* findFrame(const S32 frameID);
		Frame* findEmptyFrame();
		Frame* twin();
		Frame* findHitDivider(const Point2I& position);
		Frame* findFrameWithCtrl(GuiControl* ctrl);
		Frame* findFrameWithPoint(const Point2I& point);
	};
	Frame mRootFrame;
	Frame* mHitDivider;
	Frame* mOldHitDivider;
	U8 mDividerThickness; 
	const S32 minSize = 20;


private:

public:
	GuiFrameSetCtrl();
	~GuiFrameSetCtrl();
	static void initPersistFields();
	
	bool onAdd();
	virtual void parentResized(const Point2I& oldParentExtent, const Point2I& newParentExtent);
	void resize(const Point2I& newPosition, const Point2I& newExtent);
	void inspectPostApply();
	bool onWake();
	void onSleep();
	void onChildAdded(GuiControl* child);
	void onChildRemoved(GuiControl* child);
	void childResized(GuiControl* child);
	void childMoved(GuiControl* child);
	void childrenReordered();
	void renderDropOptions(GuiWindowCtrl* window);
	void renderDropButton(const GuiFrameSetCtrl::Frame* frame, const RectI& buttonRect, const Point2I& cursorPt, const Point2I& fillPos, const Point2I& fillExt, GuiDirection direction);
	void handleDropButtons(GuiWindowCtrl* window);

	Point2I splitFrame(S32 frameID, bool isVertical);
	void splitFrame(GuiFrameSetCtrl::Frame* frame, GuiDirection direction);
	void anchorFrame(GuiFrameSetCtrl::Frame* frame);
	void anchorFrame(S32 frameID);
	void setFrameSize(S32 frameID, S32 size);
	void onRender(Point2I offset, const RectI& updateRect);
	void onTouchMove(const GuiEvent& event);
	void onTouchDragged(const GuiEvent& event);
	void onTouchDown(const GuiEvent& event);
	void onTouchUp(const GuiEvent& event);
	void getCursor(GuiCursor*& cursor, bool& showCursor, const GuiEvent& lastGuiEvent);

	void setDropButtonProfile(GuiControlProfile* prof);
	void setControlLeftRightCursor(GuiCursor* cursor);
	void setControlUpDownCursor(GuiCursor* cursor);

	DECLARE_CONOBJECT(GuiFrameSetCtrl);
};

#endif // _GUIFRAMESETCTRL_H_