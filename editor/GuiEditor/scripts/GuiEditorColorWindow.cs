
function GuiEditorColorWindow::onAdd(%this)
{
    %ext = %this.getExtent();
    %this.grid = new GuiGridCtrl()
    {
        HorizSizing = "width";
        VertSizing = "height";
        Position = "7 10";
        Extent = (%ext.x - 23) SPC (%ext.y - 43);
        CellSizeX = "50";
        CellSizeY = "200";
        CellModeX = "Variable";
        CellModeY = "Absolute";
        MaxColCount = "2";
        IsExtentDynamic = "1";
        OrderMode = "LRTB";
    };
    ThemeManager.setProfile(%this.grid, "EmptyProfile");
    %this.add(%this.grid);

    %this.addColorCtrl(1, "Pallet");
    %this.addColorCtrl(6, "BlendColor");
    %this.addColorCtrl(2, "HorizColor");
    %this.addColorCtrl(3, "VertColor");
    %this.addColorCtrl(4, "HorizBrightnessColor");
    %this.addColorCtrl(5, "VertBrightnessColor");
    %this.addColorCtrl(7, "HorizAlpha");
    %this.addColorCtrl(8, "VertAlpha");
    %this.addColorCtrl(9, "Dropper");
}

function GuiEditorColorWindow::addColorCtrl(%this, %i, %mode)
{
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