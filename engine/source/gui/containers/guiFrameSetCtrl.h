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
#include "gui/containers/guiTabBookCtrl.h"

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
			localPosition(Point2I(0, 0)), mLeftButtonRect(RectI()), mRightButtonRect(RectI()), mCenterButtonRect(RectI()),
			mTopButtonRect(RectI()), mBottomButtonRect(RectI()), isAnchored(true), hasLeftRightButtons(false), 
			hasTopBottomButtons(false), spliterRect1(RectI()), spliterRect2(RectI()) { }
		virtual ~Frame() { }
		void deleteChildren();

		bool isVertical;
		U32 id;
		Point2I extent;
		Point2I localPosition;
		bool isAnchored;
		RectI dividerRect;
		RectI spliterRect1;//shown in edit mode
		RectI spliterRect2;//shown in edit mode
		RectI mLeftButtonRect;
		RectI mRightButtonRect;
		RectI mTopButtonRect;
		RectI mBottomButtonRect;
		RectI mCenterButtonRect;
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
		void editRender(const Point2I& cursorPt, const F32 fade);
	};
	Frame mRootFrame;
	Frame* mHitDivider;
	Frame* mOldHitDivider;
	U8 mDividerThickness; 
	const S32 minSize = 20;

	GuiControlProfile* mTabBookProfile;
	GuiControlProfile* mTabProfile;
	GuiControlProfile* mTabPageProfile;


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
	bool hasCenterButton(GuiFrameSetCtrl::Frame* frame);
	void renderDropButton(const GuiFrameSetCtrl::Frame* frame, const RectI& buttonRect, const Point2I& cursorPt, const Point2I& fillPos, const Point2I& fillExt, GuiDirection direction);
	void handleDropButtons(GuiWindowCtrl* window);
	void undockWindowFromBook(GuiWindowCtrl* window, GuiTabBookCtrl* book, GuiTabPageCtrl* page);

	Point2I splitFrame(S32 frameID, bool isVertical);
	void splitFrame(GuiFrameSetCtrl::Frame* frame, GuiDirection direction);
	void anchorFrame(GuiFrameSetCtrl::Frame* frame);
	void anchorFrame(S32 frameID);
	void setFrameSize(S32 frameID, S32 size);
	void onPreRender();
	void onRender(Point2I offset, const RectI& updateRect);
	void onTouchMove(const GuiEvent& event);
	void onTouchDragged(const GuiEvent& event);
	void onTouchDown(const GuiEvent& event);
	void onTouchUp(const GuiEvent& event);
	void getCursor(GuiCursor*& cursor, bool& showCursor, const GuiEvent& lastGuiEvent);

	bool onMouseDownEditor(const GuiEvent& event, const Point2I& offset);
	bool onMouseUpEditor(const GuiEvent& event, const Point2I& offset);
	bool onMouseDraggedEditor(const GuiEvent& event, const Point2I& offset);

	void setDropButtonProfile(GuiControlProfile* prof);
	void setTabBookProfile(GuiControlProfile* prof);
	void setTabProfile(GuiControlProfile* prof);
	void setTabPageProfile(GuiControlProfile* prof);
	void setControlLeftRightCursor(GuiCursor* cursor);
	void setControlUpDownCursor(GuiCursor* cursor);

	static bool setBookProfileFn(void* obj, const char* data) { GuiControlProfile* profile; Sim::findObject(data, profile); static_cast<GuiFrameSetCtrl*>(obj)->setTabBookProfile(profile); return true; }
	static bool setTabProfileFn(void* obj, const char* data) { GuiControlProfile* profile; Sim::findObject(data, profile); static_cast<GuiFrameSetCtrl*>(obj)->setTabProfile(profile); return true; }
	static bool setPageProfileFn(void* obj, const char* data) { GuiControlProfile* profile; Sim::findObject(data, profile); static_cast<GuiFrameSetCtrl*>(obj)->setTabPageProfile(profile); return true; }

	DECLARE_CONOBJECT(GuiFrameSetCtrl);
};

#endif // _GUIFRAMESETCTRL_H_