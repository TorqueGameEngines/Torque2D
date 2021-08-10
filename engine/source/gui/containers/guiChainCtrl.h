#ifndef _GUICHAINCTRL_H_
#define _GUICHAINCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

#include "graphics/dgl.h"
#include "console/console.h"
#include "console/consoleTypes.h"

class GuiChainCtrl : public GuiControl
{
private:
	typedef GuiControl Parent;

protected:
	S32 mChildSpacing;
	bool mIsVertical;

	virtual void calculateExtent();
	virtual S32 positionChildren(RectI &innerRect);

public:
	GuiChainCtrl();

	void childResized(GuiControl *child);
	void resize(const Point2I &newPosition, const Point2I &newExtent);
	void inspectPostApply();
	void onChildAdded(GuiControl *child);
	void onChildRemoved(SimObject *child);

	inline void setChildSpacing(S32 space) { mChildSpacing = space; }
	inline S32 getChildSpacing() { return mChildSpacing; }
	inline bool getIsVertical() { return mIsVertical; }

	static void initPersistFields();
	DECLARE_CONOBJECT(GuiChainCtrl);
};

#endif // _GUICHAINCTRL_H_