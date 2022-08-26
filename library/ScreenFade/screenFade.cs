function ScreenFade::create(%this)
{
	exec("./scripts/ScreenFadeBackground.cs");

	%this.background = new GuiSpriteCtrl() {
		class = "ScreenFadeBackground";
		profile = "GuiDefaultProfile";
		HorizSizing = "relative";
		VertSizing = "relative";
		Position = "0 0";
		Image = "ScreenFade:background";
		FullSize = 1;
		ConstrainProportions = 0;
		ImageColor = "255 255 255 0";
		Owner = %this;
	};
}

function ScreenFade::destroy(%this)
{
	if(isObject(%this.background))
	{
		%this.background.delete();
	}
}

//Switches the canvas by fading in to color and back out to the new content. The process takes the given time in milliseconds.
//Color and time are optional.
//ScreenFade will post event: onSwapComplete().
function ScreenFade::swapCanvas(%this, %content, %color, %time)
{
	if(%color $= "")
	{
		%color = "0 0 0 0";
	}

	if(%time $= "")
	{
		%time = 1400;
	}

	%base = getWord(%color, 0) SPC getWord(%color, 1) SPC getWord(%color, 2);
	%this.background.solidColor = %base SPC "255";
	%this.background.transparentColor = %base SPC "0";
	%this.background.swapContent = %content;
	%this.background.swapTime = mRound((%time / 5) * 2);
	%this.background.startSwap();
}

//Fades the screen to color and then puts the dialog on top of it.
//Color and time are optional.
//When your dialog closes, call %this.post("dialogClose"); to inform ScreenFade where %this is the same object passed to openDialog().
//ScreenFade will post events: onOpenComplete() and onCloseComplete().
function ScreenFade::openDialog(%this, %dialog, %color, %time)
{
	if(%color $= "")
	{
		%color = "0 0 0 230";
	}

	if(%time $= "")
	{
		%time = 300;
	}

	%this.background.solidColor = %color;
	%this.background.transparentColor = getWord(%color, 0) SPC getWord(%color, 1) SPC getWord(%color, 2) SPC "0";
	%this.background.dialog = %dialog;
	%this.background.dialogTime = %time;
	%this.background.openDialog();
}
