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

Sandbox.customLabelHeight = "15";
Sandbox.customLabelWidth = "220";
Sandbox.customLabelSpacing = "18";
Sandbox.customOptionSpacing = "15";
Sandbox.customContainerExtent = "250 3";
Sandbox.containerXPosition = "20";
Sandbox.flagOptionExtent = "240 30";
Sandbox.buttonOptionExtent = "240 35";
Sandbox.spinnerExtent = "22 25";
Sandbox.intOptionExtent = "196 25";
Sandbox.listOptionExtent = "240 0";
Sandbox.customControlCount = "0";
Sandbox.lastControlBottom = "0";

//-----------------------------------------------------------------------------

function resetCustomControls()
{
    Sandbox.lastControlBottom = "0";
    Sandbox.customControlCount = 0;

    %customControlExtent = ToyCustomControls.Extent;
    %newExtent = getWord(ToyCustomControls.Extent, 0) SPC 705;
    ToyCustomControls.Extent = %newExtent;
    CustomControlsScroller.computeSizes();
}

//-----------------------------------------------------------------------------

function createCustomLabel(%text)
{
    %labelWidth = Sandbox.customLabelWidth + (%characterCount * 5);
    %labelExtent = %labelWidth SPC Sandbox.customLabelHeight;

    %labelControl = new GuiControl()
    {
        text = %text;
        Extent = %labelExtent;
        HorizSizing = "relative";
        VertSizing = "relative";
        Profile = "GuiLabelProfile";
        canSaveDynamicFields = "0";
        isContainer = "0";
        Position = "3 0";
        MinExtent = "8 2";
        canSave = "0";
        Visible = "1";
        Active = "0";
        tooltipprofile = "GuiToolTipProfile";
        tooltipWidth = "0";
        maxLength = "255";
        truncate = "0";
    };

    return %labelControl;
}

//-----------------------------------------------------------------------------

function nextCustomControlPosition(%index)
{
    %verticalOffset = (Sandbox.customOptionSpacing + %index) + Sandbox.lastControlBottom;
    %position = Sandbox.containerXPosition SPC %verticalOffset;
    return %position;
}

//-----------------------------------------------------------------------------

function addFlagOption( %label, %callback, %startingValue, %shouldReset, %tooltipText)
{
    %containerPosition = nextCustomControlPosition(Sandbox.customControlCount);

    %customX = getWord(Sandbox.customContainerExtent, 0);
    %customY = getWord(Sandbox.customContainerExtent, 1) + getWord(Sandbox.flagOptionExtent, 1);

    %container = new GuiControl()
    {
        isContainer = 1;
        position = %containerPosition;
        extent = %customX SPC %customY;
        Profile = GuiTransparentProfile;
        HorizSizing = "relative";
        VertSizing = "relative";
    };

    %button = new GuiCheckBoxCtrl()
    {
        canSaveDynamicFields = "0";
        HorizSizing = "relative";
        VertSizing = "relative";
        isContainer = "0";
        Profile = "GuiCheckBoxProfile";
        Position = "0 0";
        Extent = Sandbox.flagOptionExtent;
        Visible = "1";
        toy = Sandbox.ActiveToy.ScopeSet;
        shouldResetToy = %shouldReset;
        callback = %callback;
        class = "FlagController";
        isContainer = "0";
        Active = "1";
        hovertime = "1000";
        toolTipProfile = "GuiToolTipProfile";
        toolTip = %tooltipText;
        text = %label;
     };

    %button.setStateOn(%startingValue);
    %button.command = %button @ ".updateToy();";

    %container.add(%button);

    ToyCustomControls.add(%container);

    Sandbox.lastControlBottom = getWord(%container.position, 1) + getWord(%container.extent, 1);

    if (Sandbox.lastControlBottom > getWord(ToyCustomControls.Extent, 1))
    {
        %rootContainerExtent = getWord(ToyCustomControls.Extent, 0) SPC Sandbox.lastControlBottom + 20;

        ToyCustomControls.Extent = %rootContainerExtent;
        CustomControlsScroller.computeSizes();
    }

    Sandbox.customControlCount++;
}

//-----------------------------------------------------------------------------

function FlagController::updateToy(%this)
{
    if (%this.toy $= "")
        return;

    if (%this.callback !$= "")
    {
        %setter = "%this.toy." @ %this.callback @ "(" @ %this.getStateOn() @ ");";

        eval(%setter);
    }

    if (%this.shouldResetToy && %this.toy.isMethod("reset"))
        %this.toy.reset();
}

//-----------------------------------------------------------------------------

function addButtonOption( %label, %callback, %shouldReset, %tooltipText)
{
    %containerPosition = nextCustomControlPosition(Sandbox.customControlCount);

    %customX = getWord(Sandbox.customContainerExtent, 0);
    %customY = getWord(Sandbox.customContainerExtent, 1) + getWord(Sandbox.buttonOptionExtent, 1);

    %container = new GuiControl()
    {
        isContainer = 1;
        HorizSizing = "relative";
        VertSizing = "relative";
        position = %containerPosition;
        extent = %customX SPC %customY;
        Profile = GuiTransparentProfile;
    };

    %button = new GuiButtonCtrl()
    {
        canSaveDynamicFields = "0";
        HorizSizing = "relative";
        VertSizing = "relative";
        isContainer = "0";
        Profile = "BlueButtonProfile";
        Position = "0 0";
        Extent = Sandbox.buttonOptionExtent;
        Visible = "1";
        toy = Sandbox.ActiveToy.ScopeSet;
        shouldResetToy = %shouldReset;
        callback = %callback;
        class = "ButtonController";
        isContainer = "0";
        Active = "1";
        hovertime = "1000";
        toolTipProfile = "GuiToolTipProfile";
        toolTip = %tooltipText;
        text = %label;
        groupNum = "-1";
        buttonType = "PushButton";
        useMouseEvents = "0";
     };

    %button.command = %button @ ".updateToy();";

    %container.add(%button);

    ToyCustomControls.add(%container);

    Sandbox.lastControlBottom = getWord(%container.position, 1) + getWord(%container.extent, 1);

    if (Sandbox.lastControlBottom > getWord(ToyCustomControls.Extent, 1))
    {
        %rootContainerExtent = getWord(ToyCustomControls.Extent, 0) SPC Sandbox.lastControlBottom + 20;

        ToyCustomControls.Extent = %rootContainerExtent;
        CustomControlsScroller.computeSizes();
    }

    Sandbox.customControlCount++;
}

//-----------------------------------------------------------------------------

function ButtonController::updateToy(%this)
{
    if (%this.toy $= "")
        return;

    if (%this.callback !$= "")
    {
        %setter = "%this.toy." @ %this.callback @ "();";
        eval(%setter);
    }

    if (%this.shouldResetToy && %this.toy.isMethod("reset"))
        %this.toy.reset();
}

//-----------------------------------------------------------------------------

function addNumericOption( %label, %min, %max, %step, %callback, %startingValue, %shouldReset, %tooltipText)
{
    %customLabel = createCustomLabel(%label);

    %containerPosition = nextCustomControlPosition(Sandbox.customControlCount);

    %customX = getWord(Sandbox.customContainerExtent, 0);
    %customY = getWord(Sandbox.customContainerExtent, 1) + getWord(Sandbox.intOptionExtent, 1) + Sandbox.customLabelHeight;

    %container = new GuiControl()
    {
        isContainer = 1;
        position = %containerPosition;
        HorizSizing = "relative";
        VertSizing = "relative";
        extent = %customX SPC %customY;
        Profile = GuiTransparentProfile;
    };

    %container.add(%customLabel);

    %spinnerPosition = "1" SPC Sandbox.customLabelSpacing;

    %spinnerDown = new GuiButtonCtrl()
    {
        Action = "decrease";
        Class = "SpinnerController";
        Step = %step;
        HorizSizing = "relative";
        VertSizing = "relative";
        canSaveDynamicFields = "0";
        isContainer = "0";
        Profile = "GuiMinusButtonProfile";
        Position = %spinnerPosition;
        Extent = Sandbox.spinnerExtent;
        MinExtent = "22 25";
        canSave = "1";
        Visible = "1";
        Active = "1";
        hovertime = "1000";
        toolTipProfile = "GuiToolTipProfile";
        toolTip = %tooltipText;
        text = "";
    };

    %controlPosition = (getWord(Sandbox.spinnerExtent, 0) + 1) SPC Sandbox.customLabelSpacing;

    %textEdit = new GuiTextEditCtrl()
    {
        Position = %controlPosition;
        HorizSizing = "relative";
        VertSizing = "relative";
        min = %min;
        max = %max;
        Text = %startingValue;
        Extent = Sandbox.intOptionExtent;
        toy = Sandbox.ActiveToy.ScopeSet;
        shouldResetToy = %shouldReset;
        callback = %callback;
        class = "TextEditController";
        isContainer = "0";
        Profile = "GuiSpinnerProfile";
        toolTipProfile = "GuiToolTipProfile";
        toolTip = %tooltipText;
        hovertime = "1000";
    };

    %spinnerPosition = (getWord(%textEdit.Extent, 0) + getWord(%textEdit.position, 0)) SPC Sandbox.customLabelSpacing;

    %spinnerUp = new GuiButtonCtrl()
    {
        Action = "increase";
        HorizSizing = "relative";
        VertSizing = "relative";
        Class = "SpinnerController";
        Step = %step;
        canSaveDynamicFields = "0";
        isContainer = "0";
        Profile = "GuiPlusButtonProfile";
        Position = %spinnerPosition;
        Extent = Sandbox.spinnerExtent;
        MinExtent = "22 25";
        canSave = "1";
        Visible = "1";
        Active = "1";
        hovertime = "1000";
        toolTipProfile = "GuiToolTipProfile";
        toolTip = %tooltipText;
        text = "";
    };

    %textEdit.validate = %textEdit @ ".updateToy();";
    %spinnerDown.target = %textEdit;
    %spinnerUp.target = %textEdit;
    %spinnerDown.command = %spinnerDown @ ".updateTarget();";
    %spinnerUp.command = %spinnerUp @ ".updateTarget();";

    %container.add(%spinnerDown);
    %container.add(%textEdit);
    %container.add(%spinnerUp);

    ToyCustomControls.add(%container);

    Sandbox.lastControlBottom = getWord(%container.position, 1) + getWord(%container.extent, 1);

    if (Sandbox.lastControlBottom > getWord(ToyCustomControls.Extent, 1))
    {
        %rootContainerExtent = getWord(ToyCustomControls.Extent, 0) SPC Sandbox.lastControlBottom + 20;

        ToyCustomControls.Extent = %rootContainerExtent;
        CustomControlsScroller.computeSizes();
    }

    Sandbox.customControlCount++;
}

//-----------------------------------------------------------------------------

function SpinnerController::updateTarget(%this)
{
    %target = %this.target;

    if (%this.action $= "increase")
    {
        %value = %target.getText();
        %value += %this.step;

        if (%value > %target.max)
            %value = %target.max;

        %target.setText(%value);
    }
    else if (%this.action $= "decrease")
    {
        %value = %target.getText();
        %value -= %this.step;

        if (%value < %target.min)
            %value = %target.min;

        %target.setText(%value);
    }

    %target.updateToy();
}

//-----------------------------------------------------------------------------

function TextEditController::updateToy(%this)
{
    if (%this.toy $= "")
        return;

    if (%this.getText() > %this.max)
        %this.setText(%this.max);
    else if (%this.getText() < %this.min)
        %this.setText(%this.min);

    if (%this.callback !$= "" && %this.getValue() !$= "")
    {
        %setter = "%this.toy." @ %this.callback @ "(" @ %this.getValue() @ ");";
        eval(%setter);
    }

    if (%this.shouldResetToy && %this.toy.isMethod("reset"))
        %this.toy.reset();
}

//-----------------------------------------------------------------------------

function addSelectionOption( %entries, %label, %maxDisplay, %callback, %shouldReset, %tooltipText)
{
    // Combined Y extent of the up/down buttons
    %buttonExtentAddition = 46;

    // Extra padding
    %buffer = 15;

    // Size of each button added to the list
    %buttonSize = 50;
    %buttonSpacing = %maxDisplay;// * 8;

    // Starting location of the main container
    %containerPosition = nextCustomControlPosition(Sandbox.customControlCount);

    // Main container base width
    %containerWidth = getWord(Sandbox.customContainerExtent, 0);

    // Main container base height
    %containerHeight = getWord(Sandbox.customContainerExtent, 1) + getWord(Sandbox.listOptionExtent, 1) + Sandbox.customLabelHeight;

    // Main container buffer (accounts for size of list, up/down buttons, and buffer)
    %containerHeight += ((%maxDisplay+1) * %buttonSize) + %buttonExtentAddition + %buffer;

    // X position of buttons
    %buttonX = "90";

    // Y position for up button
    %upButtonY = Sandbox.customLabelSpacing;

    // List container
    %listContainerPosition = "0" SPC %upButtonY + 22;
    %listContainerWidth = %containerWidth;
    %listContainerHeight = ((%maxDisplay+1) * %buttonSize) + %buttonSpacing;

    %scrollContainerWidth = %listContainerWidth - 5;
    %scrollContainerHeight = %listContainerHeight;

    // Array control
    %arrayListWidth = %scrollContainerWidth - 25;
    %arrayListHeight = 0;

    // Y position for the down button
    %downButtonY = getWord(%listContainerPosition, 1) + %listContainerHeight;

    // Create the base container
    %container = new GuiControl()
    {
        isContainer = 1;
        HorizSizing = "relative";
        VertSizing = "relative";
        position = %containerPosition;
        extent = %containerWidth SPC %containerHeight;
        Profile = GuiTransparentProfile;
    };

    // Create and add the text label
    %customLabel = createCustomLabel(%label);
    %container.add(%customLabel);

    %listContainer = new GuiControl()
    {
        isContainer = 1;
        HorizSizing = "relative";
        VertSizing = "relative";
        position = %listContainerPosition;
        extent = %listContainerWidth SPC %listContainerHeight;
        Profile = GuiSunkenContainerProfile;
    };

    %scrollControl = new GuiScrollCtrl()
    {
        canSaveDynamicFields = "1";
        isContainer = "1";
        class = "CustomScrollControl";
        Profile = "GuiScrollProfile";
		ThumbProfile = "GuiScrollThumbProfile";
		TrackProfile = "GuiScrollTrackProfile";
		ArrowProfile = "GuiScrollArrowProfile";
        HorizSizing = "relative";
        VertSizing = "relative";
        Position = "3 3";
        Extent = %scrollContainerWidth SPC %scrollContainerHeight;
        MinExtent = %scrollContainerWidth SPC %scrollContainerHeight;
        canSave = "1";
        Visible = "1";
        Active = "1";
        hovertime = "1000";
        willFirstRespond = "1";
        hScrollBar = "alwaysOn";
        vScrollBar = "alwaysOn";
        constantThumbHeight = "0";
        childMargin = "2 3";
        toolTipProfile = "GuiToolTipProfile";
        toolTip = %tooltipText;
    };

    %arrayList = new GuiGridCtrl()
    {
        canSaveDynamicFields = "0";
        isContainer = "1";
        class = "CustomOptionArrayClass";
        Profile = "GuiTransparentProfile";
        HorizSizing = "relative";
        VertSizing = "relative";
        Position = "0 1";
        Extent = %arrayListWidth SPC %arrayListHeight;
        MinExtent = "1 2";
        canSave = "1";
        Visible = "1";
        Active = "1";
        tooltipprofile = "GuiToolTipProfile";
        hovertime = "1000";
        MaxColCount = "1";
        CellSizeX = %arrayListWidth-10;
        CellSizeY = "50";
        CellSpacingX = "8";
        CellSpacingY = "8";
		CellModeX = "variable";
        toolTipProfile = "GuiToolTipProfile";
        toolTip = %tooltipText;
    };

    %scrollControl.add(%arrayList);

    %listContainer.add(%scrollControl);

    %container.add(%listContainer);

    // Populate the list

    for (%i = 0; %i < getUnitCount(%entries, ","); %i++)
    {
        %button = new GuiButtonCtrl()
        {
            canSaveDynamicFields = "0";
            class = "SelectionController";
            toy = Sandbox.ActiveToy.ScopeSet;
            shouldResetToy = %shouldReset;
            callback = %callback;
            HorizSizing = "relative";
            VertSizing = "relative";
            isContainer = "0";
            Profile = "BlueButtonProfile";
            Position = "0 0";
            Extent = "160 80";
            Visible = "1";
            isContainer = "0";
            Active = "1";
            text = getUnit(%entries, %i, ",");
            groupNum = "1";
            buttonType = "ToggleButton";
            useMouseEvents = "0";
            toolTipProfile = "GuiToolTipProfile";
            toolTip = %tooltipText;
        };

        %button.command = %arrayList @ ".clearSelections();" @ %button @ ".updateToy();";

        %arrayList.add(%button);
    }

    // Create and add the up button
    %upButton = new GuiButtonCtrl()
    {
        canSaveDynamicFields = "0";
        isContainer = "0";
        Profile = "GuiNorthButtonProfile";
        HorizSizing = "relative";
        VertSizing = "relative";
        Position = %buttonX SPC %upButtonY;
        Extent = "69 23";
        MinExtent = "12 4";
        canSave = "1";
        Visible = "1";
        Active = "1";
		text = "";
        toolTipProfile = "GuiToolTipProfile";
        toolTip = %tooltipText;
    };

    %upButton.command = %scrollControl @ ".scrollToPrevious();";

    // Create and add the down button
    %downButton = new GuiButtonCtrl()
    {
        canSaveDynamicFields = "0";
        isContainer = "0";
        Profile = "GuiSouthButtonProfile";
        HorizSizing = "relative";
        VertSizing = "relative";
        Position = %buttonX SPC %downButtonY;
        Extent = "69 23";
        MinExtent = "12 4";
        canSave = "1";
        Visible = "1";
        Active = "1";
        text = "";
        toolTipProfile = "GuiToolTipProfile";
        toolTip = %tooltipText;
    };

    %downButton.command = %scrollControl @ ".scrollToNext();";

    %container.add(%upButton);
    %container.add(%downButton);

    ToyCustomControls.add(%container);

    Sandbox.lastControlBottom = getWord(%container.position, 1) + getWord(%container.extent, 1);

    if (Sandbox.lastControlBottom > getWord(ToyCustomControls.Extent, 1))
    {
        %rootContainerExtent = getWord(ToyCustomControls.Extent, 0) SPC Sandbox.lastControlBottom + 20;

        ToyCustomControls.Extent = %rootContainerExtent;
        CustomControlsScroller.computeSizes();
    }

    Sandbox.customControlCount++;
}

function CustomOptionArrayClass::clearSelections(%this)
{
    %count = %this.getCount();

    for (%i = 0; %i < %count; %i++)
    {
        %button = %this.getObject(%i);

        %button.setStateOn(false);
    }
}
//-----------------------------------------------------------------------------

function CustomScrollControl::scrollToNext(%this)
{
    %currentScroll = %this.getScrollPositionY();
    %currentScroll += 55;
    %this.setScrollPosition(0, %currentScroll);
}

//-----------------------------------------------------------------------------

function CustomScrollControl::scrollToPrevious(%this)
{
    %currentScroll = %this.getScrollPositionY();
    %currentScroll -= 55;
    %this.setScrollPosition(0, %currentScroll);
}

//-----------------------------------------------------------------------------

function SelectionController::updateToy(%this)
{
    if (%this.toy $= "")
        return;

    %this.setStateOn(true);

    if (%this.callback !$= "")
    {
        %value = %this.getText();
        %setter = "%this.toy." @ %this.callback @ "(\"" @ %value @ "\");";
        eval(%setter);
    }

    if (%this.shouldResetToy && %this.toy.isMethod("reset"))
        %this.toy.reset();
}
