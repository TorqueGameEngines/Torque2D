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

$toyAllCategoryIndex = 1;
$defaultToySelected = false;

//-----------------------------------------------------------------------------

function ToyCategorySelectList::onSelect(%this)
{
    // Fetch the index.
    %index = %this.currentToyCategory;

    $defaultToySelected = false;

    ToyListArray.initialize(%index);
    ToyListScroller.computeSizes();
    ToyListScroller.scrollToTop();

    // Unload the active toy.
    unloadToy();

    // Flag as the default toy selected.
    if ($defaultToySelected)
    {
        loadToy($defaultModuleID);
    }
    else
    {
        if ( ToyListArray.getCount() > 0 )
        {
            %firstToyButton = ToyListArray.getObject(0);
            if (isObject(%firstToyButton))
                %firstToyButton.performSelect();
        }
    }
    if ($platform $= "Android")
    	hideSplashScreen();
}

//-----------------------------------------------------------------------------

function ToyCategorySelectList::initialize(%this)
{
     %this.toyCategories[$toyAllCategoryIndex] = "All";
     %this.toyCategories[$toyAllCategoryIndex+1] = "Physics";
     %this.toyCategories[$toyAllCategoryIndex+2] = "Features";
     %this.toyCategories[$toyAllCategoryIndex+3] = "Stress Testing";
     %this.toyCategories[$toyAllCategoryIndex+4] = "Fun and Games";
     %this.toyCategories[$toyAllCategoryIndex+5] = "Custom";
     %this.toyCategories[$toyAllCategoryIndex+6] = "Experiments";
     %this.maxToyCategories = $toyAllCategoryIndex + 7;

     // Set the "All" category as the default.
     // NOTE:    This is important to use so that the user-configurable default toy
     //          can be selected at start-up.
     %this.currentToyCategory = $toyAllCategoryIndex;

     %this.setText("All");
     %this.onSelect();
}

//-----------------------------------------------------------------------------

function ToyCategorySelectList::nextCategory(%this)
{
    if (%this.currentToyCategory >= %this.maxToyCategories)
        return;

    %this.currentToyCategory++;

    %text = %this.toyCategories[%this.currentToyCategory];
    %this.setText(%text);
    %this.onSelect();
}

//-----------------------------------------------------------------------------

function ToyCategorySelectList::previousCategory(%this)
{
    if (%this.currentToyCategory <= $toyAllCategoryIndex)
        return;

    %this.currentToyCategory--;

    %text = %this.toyCategories[%this.currentToyCategory];
    %this.setText(%text);
    %this.onSelect();
}

//-----------------------------------------------------------------------------

function initializeToolbox()
{
    // Populate the stock colors.
    %colorCount = getStockColorCount();
    for ( %i = 0; %i < %colorCount; %i++ )
    {
        // Fetch stock color name.
        %colorName = getStockColorName(%i);

        // Add to the list.
		%colorName = getStockColorName(%i);
        BackgroundColorSelectList.addItem( %colorName, getStockColorI(%colorName) );
		BackgroundColorSelectList.setItemID(%i, %i+1);

        // Select the color if it's the default one.
        if ( %colorName $= $pref::Sandbox::defaultBackgroundColor )
            BackgroundColorSelectList.setSelected( %i );
    }
	BackgroundColorSelectList.sortByText();

    ToyCategorySelectList.initialize();

    // Is this on the desktop?
    if ( $platform $= "windows" || $platform $= "macos" )
    {
        // Yes, so make the controls screen controls visible.
        ResolutionSelectLabel.Visible = true;
        ResolutionSelectList.Visible = true;
        FullscreenOptionLabel.Visible = true;
        FullscreenOptionButton.Visible = true;

        // Fetch the active resolution.
        %activeResolution = getRes();
        %activeWidth = getWord(%activeResolution, 0);
        %activeHeight = getWord(%activeResolution, 1);
        %activeBpp = getWord(%activeResolution, 2);

        // Fetch the resolutions.
        %resolutionList = getResolutionList( $pref::Video::displayDevice );
        %resolutionCount = getWordCount( %resolutionList ) / 3;
        %inputIndex = 0;
        %outputIndex = 0;
        for( %i = 0; %i < %resolutionCount; %i++ )
        {
            // Fetch the resolution entry.
            %width = getWord( %resolutionList, %inputIndex );
            %height = getWord( %resolutionList, %inputIndex+1 );
            %bpp = getWord( %resolutionList, %inputIndex+2 );
            %inputIndex += 3;

            // Skip the 16-bit ones.
            if ( %bpp == 16 )
                continue;

            // Store the resolution.
            $sandboxResolutions[%outputIndex] = %width SPC %height SPC %bpp;

            // Add to the list.
            ResolutionSelectList.addItem( %width @ "x" @ %height SPC "(" @ %bpp @ ")" );

            // Select the resolution if it's the default one.
            if ( %width == %activeWidth && %height == %activeHeight && %bpp == %activeBpp )
                ResolutionSelectList.setSelected( %outputIndex );

            %outputIndex++;
        }
    }

    // Configure the main overlay.
    SandboxWindow.add(MainOverlay);
    %horizPosition = getWord(SandboxWindow.Extent, 0) - getWord(MainOverlay.Extent, 0);
    %verticalPosition = getWord(SandboxWindow.Extent, 1) - getWord(MainOverlay.Extent, 1);
    MainOverlay.position = %horizPosition SPC %verticalPosition;
}

//-----------------------------------------------------------------------------

function toggleToolbox(%make)
{
    // Finish if being released.
    if ( !%make )
        return;

    // Finish if the console is awake.
    //if ( ConsoleDialog.isAwake() )
    //    return;

    // Is the toolbox awake?
    if ( ToolboxDialog.isAwake() )
    {
        // Yes, so deactivate it.
        if ( $enableDirectInput )
            activateKeyboard();
        Canvas.popDialog(ToolboxDialog);
        MainOverlay.setVisible(1);
        return;
    }

    // Activate it.
    if ( $enableDirectInput )
        deactivateKeyboard();

    MainOverlay.setVisible(0);
    Canvas.pushDialog(ToolboxDialog);
}

//-----------------------------------------------------------------------------

function BackgroundColorSelectList::onSelect(%this)
{
    // Fetch the index.
    $activeBackgroundColor = %this.getSelectedItem();

    // Finish if the sandbox scene is not available.
    if ( !isObject(SandboxScene) )
        return;

    // Set the scene color.
    Canvas.BackgroundColor = getStockColorName($activeBackgroundColor);
    Canvas.UseBackgroundColor = true;
}

//-----------------------------------------------------------------------------

function ResolutionSelectList::onSelect(%this)
{
    // Finish if the sandbox scene is not available.
    if ( !isObject(SandboxScene) )
        return;

    // Fetch the index.
    %index = %this.getSelectedItem();

    // Fetch resolution.
    %resolution = $sandboxResolutions[%index];

    // Set the screen mode.
    setScreenMode( GetWord( %resolution , 0 ), GetWord( %resolution, 1 ), GetWord( %resolution, 2 ), $pref::Video::fullScreen );
}

//-----------------------------------------------------------------------------

function PauseSceneModeButton::onClick(%this)
{
    // Sanity!
    if ( !isObject(SandboxScene) )
    {
        error( "Cannot pause/unpause the Sandbox scene as it does not exist." );
        return;
    }

    // Toggle the scene pause.
    SandboxScene.setScenePause( !SandboxScene.getScenePause() );
}

//-----------------------------------------------------------------------------

function ReloadToyOverlayButton::onClick(%this)
{
    // Finish if no toy is loaded.
    if ( !isObject(Sandbox.ActiveToy) )
        return;

    // Reset custom controls.
    resetCustomControls();

    // Reload the toy.
    loadToy( Sandbox.ActiveToy );
}

//-----------------------------------------------------------------------------

function RestartToyOverlayButton::onClick(%this)
{
    // Finish if no toy is loaded.
    if ( !isObject(Sandbox.ActiveToy) )
        return;

    // Reset the toy.
    if ( Sandbox.ActiveToy.ScopeSet.isMethod("reset") )
        Sandbox.ActiveToy.ScopeSet.reset();
}

//-----------------------------------------------------------------------------

function updateToolboxOptions()
{
    // Finish if the sandbox scene is not available.
    if ( !isObject(SandboxScene) )
        return;

    // Set the scene color.
    Canvas.BackgroundColor = getStockColorName($activeBackgroundColor);
    Canvas.UseBackgroundColor = true;

    // Set option.
    if ( $pref::Sandbox::metricsOption )
        SandboxScene.setDebugOn( "metrics" );
    else
        SandboxScene.setDebugOff( "metrics" );

    // Set option.
    if ( $pref::Sandbox::fpsmetricsOption )
        SandboxScene.setDebugOn( "fps" );
    else
        SandboxScene.setDebugOff( "fps" );

    // Set option.
    if ( $pref::Sandbox::controllersOption )
        SandboxScene.setDebugOn( "controllers" );
    else
        SandboxScene.setDebugOff( "controllers" );

    // Set option.
    if ( $pref::Sandbox::jointsOption )
        SandboxScene.setDebugOn( "joints" );
    else
        SandboxScene.setDebugOff( "joints" );

    // Set option.
    if ( $pref::Sandbox::wireframeOption )
        SandboxScene.setDebugOn( "wireframe" );
    else
        SandboxScene.setDebugOff( "wireframe" );

    // Set option.
    if ( $pref::Sandbox::aabbOption )
        SandboxScene.setDebugOn( "aabb" );
    else
        SandboxScene.setDebugOff( "aabb" );

    // Set option.
    if ( $pref::Sandbox::oobbOption )
        SandboxScene.setDebugOn( "oobb" );
    else
        SandboxScene.setDebugOff( "oobb" );

    // Set option.
    if ( $pref::Sandbox::sleepOption )
        SandboxScene.setDebugOn( "sleep" );
    else
        SandboxScene.setDebugOff( "sleep" );

    // Set option.
    if ( $pref::Sandbox::collisionOption )
        SandboxScene.setDebugOn( "collision" );
    else
        SandboxScene.setDebugOff( "collision" );

    // Set option.
    if ( $pref::Sandbox::positionOption )
        SandboxScene.setDebugOn( "position" );
    else
        SandboxScene.setDebugOff( "position" );

    // Set option.
    if ( $pref::Sandbox::sortOption )
        SandboxScene.setDebugOn( "sort" );
    else
        SandboxScene.setDebugOff( "sort" );

    // Set the options check-boxe.
    MetricsOptionCheckBox.setStateOn( $pref::Sandbox::metricsOption );
    FpsMetricsOptionCheckBox.setStateOn( $pref::Sandbox::fpsmetricsOption );
    ControllersOptionCheckBox.setStateOn( $pref::Sandbox::controllersOption );
    JointsOptionCheckBox.setStateOn( $pref::Sandbox::jointsOption );
    WireframeOptionCheckBox.setStateOn( $pref::Sandbox::wireframeOption );
    AABBOptionCheckBox.setStateOn( $pref::Sandbox::aabbOption );
    OOBBOptionCheckBox.setStateOn( $pref::Sandbox::oobbOption );
    SleepOptionCheckBox.setStateOn( $pref::Sandbox::sleepOption );
    CollisionOptionCheckBox.setStateOn( $pref::Sandbox::collisionOption );
    PositionOptionCheckBox.setStateOn( $pref::Sandbox::positionOption );
    SortOptionCheckBox.setStateOn( $pref::Sandbox::sortOption );
    BatchOptionCheckBox.setStateOn( SandboxScene.getBatchingEnabled() );

    // Is this on the desktop?
    //if ( $platform $= "windows" || $platform $= "macos" )
    if ( $platform !$= "iOS" && $platform !$= "Android" )
    {
        // Set the fullscreen check-box.
        FullscreenOptionButton.setStateOn( $pref::Video::fullScreen );

        // Set the full-screen mode appropriately.
        if ( isFullScreen() != $pref::Video::fullScreen )
            toggleFullScreen();
    }
}

//-----------------------------------------------------------------------------

function setFullscreenOption( %flag )
{
    $pref::Video::fullScreen = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function setMetricsOption( %flag )
{
    $pref::Sandbox::metricsOption = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function setFPSMetricsOption( %flag )
{
    $pref::Sandbox::fpsmetricsOption = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function setMetricsOption( %flag )
{
    $pref::Sandbox::metricsOption = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function setControllersOption( %flag )
{
    $pref::Sandbox::controllersOption = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function setJointsOption( %flag )
{
    $pref::Sandbox::jointsOption = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function setWireframeOption( %flag )
{
    $pref::Sandbox::wireframeOption = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function setAABBOption( %flag )
{
    $pref::Sandbox::aabbOption = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function setOOBBOption( %flag )
{
    $pref::Sandbox::oobbOption = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function setSleepOption( %flag )
{
    $pref::Sandbox::sleepOption = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function setCollisionOption( %flag )
{
    $pref::Sandbox::collisionOption = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function setPositionOption( %flag )
{
    $pref::Sandbox::positionOption = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function setSortOption( %flag )
{
    $pref::Sandbox::sortOption = %flag;
    updateToolboxOptions();
}

//-----------------------------------------------------------------------------

function ToyListScroller::scrollToNext(%this)
{
    %currentScroll = %this.getScrollPositionY();
    %currentScroll += 85;
    %this.setScrollPosition(0, %currentScroll);
}

//-----------------------------------------------------------------------------

function ToyListScroller::scrollToPrevious(%this)
{
    %currentScroll = %this.getScrollPositionY();
    %currentScroll -= 85;
    %this.setScrollPosition(0, %currentScroll);
}

//-----------------------------------------------------------------------------

function ToyListArray::initialize(%this, %index)
{
    %this.clear();
    %currentExtent = %this.extent;
    %newExtent = getWord(%currentExtent, 0) SPC "20";
    %this.Extent = %newExtent;

    // Fetch the toy count.
    %toyCount = SandboxToys.getCount();

    // Populate toys in the selected category.
    for ( %toyIndex = 0; %toyIndex < %toyCount; %toyIndex++ )
    {
        // Fetch the toy module.
        %moduleDefinition = SandboxToys.getObject( %toyIndex );

        // Skip the toy module if the "all" category is not selected and if the toy is not in the selected category.
        if ( %index != $toyAllCategoryIndex && %moduleDefinition.ToyCategoryIndex != %index )
            continue;

        // Fetch the module version.
        %versionId = %moduleDefinition.versionId;

        // Format module title so that version#1 doesn't show version but all other versions do.
        if ( %versionId == 1 )
            %moduleTitle = %moduleDefinition.moduleId;
        else
            %moduleTitle = %moduleDefinition.moduleId SPC "(v" @ %moduleDefinition.versionId @ ")";

        // Add the toy GUI list.
        %this.addToyButton(%moduleTitle, %moduleDefinition);

        // Select the toy if it's the default and we've not selected a toy yet.
        if (!$defaultToySelected && %moduleDefinition.moduleId $= $pref::Sandbox::defaultToyId && %moduleDefinition.versionId == $pref::Sandbox::defaultToyVersionId )
        {
            $defaultToySelected = true;
            $defaultModuleID = %moduleDefinition.getId();
        }
    }
}

//-----------------------------------------------------------------------------

function ToyListArray::addToyButton(%this, %moduleTitle, %moduleDefintion)
{
    %button = new GuiButtonCtrl()
    {
        canSaveDynamicFields = "0";
        HorizSizing = "relative";
        class = "ToySelectButton";
        VertSizing = "relative";
        isContainer = "0";
        Profile = "BlueButtonProfile";
        toolTipProfile = "GuiToolTipProfile";
        toolTip = %moduleDefintion.Description;
        Position = "0 0";
        Extent = "160 80";
        Visible = "1";
        toy = %moduleDefintion.getId();
        isContainer = "0";
        Active = "1";
        text = %moduleTitle;
        groupNum = "-1";
        buttonType = "PushButton";
        useMouseEvents = "0";
    };

    %button.command = %button @ ".performSelect();";

    %this.add(%button);
}

//-----------------------------------------------------------------------------

function ToySelectButton::performSelect(%this)
{
    // Finish if already selected.
    if ( %this.toy == Sandbox.ActiveToy )
        return;

    // Load the selected toy.
    loadToy( %this.toy );
}
