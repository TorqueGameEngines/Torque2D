
function AssetImageLayersHeaderRow::onAdd(%this)
{
	%this.imageBox = new GuiControl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="20 0";
		Extent="200 25";
		Text = "Layer Image";
		vAlign = "Bottom";
	};
	ThemeManager.setProfile(%this.imageBox, "LabelProfile");
	%this.add(%this.imageBox);

	%this.offsetXBox = new GuiControl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="224 0";
		Extent="80 25";
		Text = "X";
		Align = "Center";
		vAlign = "Bottom";
	};
	ThemeManager.setProfile(%this.offsetXBox, "LabelProfile");
	%this.add(%this.offsetXBox);

	%this.offsetYBox = new GuiControl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="308 0";
		Extent="80 25";
		Text = "Y";
		Align = "Center";
		vAlign = "Bottom";
	};
	ThemeManager.setProfile(%this.offsetYBox, "LabelProfile");
	%this.add(%this.offsetYBox);

	%this.colorBox = new GuiControl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="392 0";
		Extent="164 25";
		Text = "Color";
		Align = "Center";
		vAlign = "Bottom";
	};
	ThemeManager.setProfile(%this.colorBox, "LabelProfile");
	%this.add(%this.colorBox);
}
