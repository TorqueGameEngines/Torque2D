
function AssetImageFrameHeaderRow::onAdd(%this)
{
	%this.nameBox = new GuiControl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="20 0";
		Extent="200 25";
		Text = "Cell Name";
		vAlign = "Bottom";
	};
	ThemeManager.setProfile(%this.nameBox, "LabelProfile");
	%this.add(%this.nameBox);

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

	%this.widthBox = new GuiControl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="392 0";
		Extent="80 25";
		Text = "Width";
		Align = "Center";
		vAlign = "Bottom";
	};
	ThemeManager.setProfile(%this.widthBox, "LabelProfile");
	%this.add(%this.widthBox);

	%this.heightBox = new GuiControl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="476 0";
		Extent="80 25";
		Text = "Height";
		Align = "Center";
		vAlign = "Bottom";
	};
	ThemeManager.setProfile(%this.heightBox, "LabelProfile");
	%this.add(%this.heightBox);
}
