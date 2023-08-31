
function GuiEditorColorWindow::onAdd(%this)
{
    %ext = %this.getExtent();
    %this.grid = new GuiGridCtrl()
    {
        HorizSizing = "width";
        VertSizing = "height";
        Position = "7 10";
        Extent = (%ext.x - 23) SPC (%ext.y - 43);
        CellSizeX = "40";
        CellSizeY = "40";
        CellModeX = "Absolute";
        CellModeY = "Absolute";
        MaxColCount = "4";
        IsExtentDynamic = "1";
        OrderMode = "LRTB";
    };
    ThemeManager.setProfile(%this.grid, "EmptyProfile");
    %this.add(%this.grid);

    //%this.addColorCtrl(1, "Pallet");
    //%this.addColorCtrl(6, "BlendColor");
    //%this.addColorCtrl(2, "HorizColor");
    //%this.addColorCtrl(3, "VertColor");
    //%this.addColorCtrl(4, "HorizBrightnessColor");
    //%this.addColorCtrl(5, "VertBrightnessColor");
    //%this.addColorCtrl(7, "HorizAlpha");
    //%this.addColorCtrl(8, "VertAlpha");
    //%this.addColorCtrl(9, "Dropper");
    %this.addColorCtrl(10, "Popup");
    %this.addColorCtrl(11, "Popup");
    %this.addColorCtrl(12, "Popup");
    %this.addColorCtrl(13, "Popup");
}

function GuiEditorColorWindow::addColorCtrl(%this, %i, %mode)
{
    if(%mode $= "Popup")
    {
        %this.colorCtrl[%i] = new GuiColorPopupCtrl()
        {
            HorizSizing = "width";
            VertSizing = "height";
        };
        ThemeManager.setProfile(%this.colorCtrl[%i], "colorPickerProfile");
        ThemeManager.setProfile(%this.colorCtrl[%i], "emptyProfile", "backgroundProfile");
        ThemeManager.setProfile(%this.colorCtrl[%i], "colorPopupProfile", "popupProfile");
        ThemeManager.setProfile(%this.colorCtrl[%i], "emptyProfile", "pickerProfile");
        ThemeManager.setProfile(%this.colorCtrl[%i], "colorPickerSelectorProfile", "selectorProfile");
        %this.grid.add(%this.colorCtrl[%i]);

        return;
    }

    %this.colorCtrl[%i] = new GuiColorPickerCtrl()
    {
        HorizSizing = "width";
        VertSizing = "height";
        DisplayMode = %mode;
        ShowSelector = "1";
        SelectorGap = "4";
    };
    ThemeManager.setProfile(%this.colorCtrl[%i], "colorPickerProfile");
    ThemeManager.setProfile(%this.colorCtrl[%i], "colorPickerSelectorProfile", "selectorProfile");
    %this.grid.add(%this.colorCtrl[%i]);
}