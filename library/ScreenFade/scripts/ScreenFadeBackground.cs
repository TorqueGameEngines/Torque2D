function ScreenFadeBackground::resetColor(%this)
{
	if(getWord(%this.getImageColor(), 3) == 0)
	{
		%this.setImageColor(%this.transparentColor);
		%extent = Canvas.getExtent();
		%this.setExtent(getWord(%extent, 0), getWord(%extent, 1));
	}
}

function ScreenFadeBackground::startSwap(%this)
{
	%this.resetColor();
	Canvas.pushDialog(%this);
	%this.fadeTo(%this.solidColor, %this.swapTime, "EaseInOut");
	%this.schedule(%this.swapTime, "doSwap");
}

function ScreenFadeBackground::doSwap(%this)
{
	Canvas.setContent(%this.swapContent);
	Canvas.pushDialog(%this);
	%this.schedule(%this.swapTime/2, "fadeSwap");
}

function ScreenFadeBackground::fadeSwap(%this)
{
	%this.fadeTo(%this.transparentColor, %this.swapTime, "EaseInOut");
	%this.schedule(%this.swapTime, "finishSwap");
}

function ScreenFadeBackground::finishSwap(%this)
{
	Canvas.popDialog(%this);
	%this.Owner.postEvent("SwapComplete");
}

function ScreenFadeBackground::openDialog(%this)
{
	%this.resetColor();
	%this.add(%this.dialog);
	Canvas.pushDialog(%this);
	%this.fadeTo(%this.solidColor, %this.dialogTime, "EaseInOut");
	%this.schedule(%this.dialogTime, "openDialogComplete");
}

function ScreenFadeBackground::openDialogComplete(%this)
{
	%this.Owner.postEvent("OpenComplete");
	%this.startListening(%this.dialog);
}

function ScreenFadeBackground::onDialogClose(%this)
{
	if(%this.isAwake())
	{
		if(isEventPending(%this.hideSchedule))
		{
			cancel(%this.hideSchedule);
		}
		%this.fadeTo(%this.transparentColor, %this.dialogTime, "EaseIn");
		%this.hideSchedule = %this.schedule(%this.dialogTime, "onCloseComplete");
	}
}

function ScreenFadeBackground::onCloseComplete(%this)
{
	if(isObject(%this.dialog))
	{
		%this.stopListening(%this.dialog);
		%this.removeIfMember(%this.dialog);
	}
	Canvas.popDialog(%this);
	%this.Owner.postEvent("CloseComplete");
}
