
function AssetParticleGraphUnit::onAdd(%this)
{
	%this.graph = new GuiParticleGraphInspector()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="30 18";
		Extent= (getWord(%this.extent, 0) - 40) SPC (getWord(%this.extent, 1) - 60);
	};
	ThemeManager.setProfile(%this.graph, "graphProfile");
	%this.add(%this.graph);

	//Value zoom buttons
	%center = 6 + mRound(getWord(%this.graph.extent, 1) / 2);
	%this.valueZoomInButton = new GuiButtonCtrl()
	{
		Position = "2" SPC (%center + 13);
		Extent = "24 24";
		Text = "+";
		Command = %this.getId() @ ".valueZoomIn();";
	};
	ThemeManager.setProfile(%this.valueZoomInButton, "buttonProfile");
	%this.add(%this.valueZoomInButton);

	%this.valueZoomOutButton = new GuiButtonCtrl()
	{
		Position = "2" SPC (%center - 13);
		Extent = "24 24";
		Text = "-";
		Command = %this.getId() @ ".valueZoomOut();";
	};
	ThemeManager.setProfile(%this.valueZoomOutButton, "buttonProfile");
	%this.add(%this.valueZoomOutButton);

	//Value move buttons
	%this.valueMoveUpButton = new GuiButtonCtrl()
	{
		Position = "2 18";
		Extent = "24 24";
		Text = "^";
		Command = %this.getId() @ ".valueMoveUp();";
	};
	ThemeManager.setProfile(%this.valueMoveUpButton, "buttonProfile");
	%this.add(%this.valueMoveUpButton);

	%this.valueMoveDownButton = new GuiButtonCtrl()
	{
		Position = "2" SPC (getWord(%this.extent, 1) - 66);
		Extent = "24 24";
		Text = "v";
		Command = %this.getId() @ ".valueMoveDown();";
	};
	ThemeManager.setProfile(%this.valueMoveDownButton, "buttonProfile");
	%this.add(%this.valueMoveDownButton);

	//time zoom buttons
	%center = 18 + mRound(getWord(%this.graph.extent, 0));
	%bottom = getWord(%this.extent, 1) - 38;
	%this.timeZoomContainer = new GuiControl()
	{
		HorizSizing = "Center";
		Position = "0" SPC %bottom;
		Extent = "50 24";
	};
	ThemeManager.setProfile(%this.timeZoomContainer, "emptyProfile");
	%this.add(%this.timeZoomContainer);

	%this.timeZoomInButton = new GuiButtonCtrl()
	{
		Position = "0 0";
		Extent = "24 24";
		Text = "+";
		Command = %this.getId() @ ".timeZoomIn();";
	};
	ThemeManager.setProfile(%this.timeZoomInButton, "buttonProfile");
	%this.timeZoomContainer.add(%this.timeZoomInButton);

	%this.timeZoomOutButton = new GuiButtonCtrl()
	{
		Position = "26 0";
		Extent = "24 24";
		Text = "-";
		Command = %this.getId() @ ".timeZoomOut();";
	};
	ThemeManager.setProfile(%this.timeZoomOutButton, "buttonProfile");
	%this.timeZoomContainer.add(%this.timeZoomOutButton);

	//Time move buttons
	%this.timeMoveBackButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		Position = "30" SPC %bottom;
		Extent = "24 24";
		Text = "<";
		Command = %this.getId() @ ".timeMoveBack();";
	};
	ThemeManager.setProfile(%this.timeMoveBackButton, "buttonProfile");
	%this.add(%this.timeMoveBackButton);

	%this.timeMoveForwardButton = new GuiButtonCtrl()
	{
		HorizSizing = "left";
		Position = (getWord(%this.graph.extent, 0) + 6) SPC %bottom;
		Extent = "24 24";
		Text = ">";
		Command = %this.getId() @ ".timeMoveForward();";
	};
	ThemeManager.setProfile(%this.timeMoveForwardButton, "buttonProfile");
	%this.add(%this.timeMoveForwardButton);
}

function AssetParticleGraphUnit::setToScale(%this, %scaleName)
{
	%this.graph.setDisplayLabels("Time", "Scale");
	%this.graph.setDisplayField(%scaleName);
}

function AssetParticleGraphUnit::setToBase(%this, %baseName, %variName, %emitterID)
{
	%this.graph.setDisplayLabels("Time", "Base Value");
	%this.graph.setDisplayField(%baseName, %emitterID);
}

function AssetParticleGraphUnit::setToVari(%this, %variName, %emitterID)
{
	if(%variName $= "")
	{
		if(%this.Tool.isMember(%this))
		{
			%this.Tool.remove(%this);
		}
		return;
	}

	if(!%this.Tool.isMember(%this))
	{
		%this.Tool.add(%this);
	}
	%this.graph.setDisplayLabels("Time", "Variation");
	%this.graph.setDisplayField(%variName, %emitterID);
}

function AssetParticleGraphUnit::setToLife(%this, %lifeName, %emitterID)
{
	if(%lifeName $= "")
	{
		if(%this.Tool.isMember(%this))
		{
			%this.Tool.remove(%this);
		}
		return;
	}

	if(!%this.Tool.isMember(%this))
	{
		%this.Tool.add(%this);
	}
	%this.graph.setDisplayLabels("Time", "Scale");
	%this.graph.setDisplayField(%lifeName, %emitterID);
}

function AssetParticleGraphUnit::setValueController(%this, %controller)
{
	if(!isObject(%controller))
	{
		return;
	}
	if(isObject(%this.valueController))
	{
		%this.valueController.delete();
	}

	%this.valueController = %controller;
	%this.refreshCamera();
}

function AssetParticleGraphUnit::setTimeController(%this, %controller)
{
	if(!isObject(%controller))
	{
		return;
	}
	if(isObject(%this.timeController))
	{
		%this.timeController.delete();
	}

	%this.timeController = %controller;
	%this.refreshCamera();
}

function AssetParticleGraphUnit::refreshCamera(%this)
{
	%xMin = %this.timeController.getCameraMin();
	%xMax = %this.timeController.getCameraMax();
	%yMin = %this.valueController.getCameraMin();
	%yMax = %this.valueController.getCameraMax();

	%this.graph.setDisplayArea(%xMin SPC %yMin SPC %xMax SPC %yMax);

	%this.valueMoveUpButton.setActive(%this.valueController.getMoveUpEnabled());
	%this.valueMoveDownButton.setActive(%this.valueController.getMoveDownEnabled());
	%this.valueZoomInButton.setActive(%this.valueController.getZoomInEnabled());
	%this.valueZoomOutButton.setActive(%this.valueController.getZoomOutEnabled());

	%this.timeMoveForwardButton.setActive(%this.timeController.getMoveUpEnabled());
	%this.timeMoveBackButton.setActive(%this.timeController.getMoveDownEnabled());
	%this.timeZoomInButton.setActive(%this.timeController.getZoomInEnabled());
	%this.timeZoomOutButton.setActive(%this.timeController.getZoomOutEnabled());
}

function AssetParticleGraphUnit::valueZoomIn(%this)
{
	%this.valueController.zoomIn();
	%this.refreshCamera();
}

function AssetParticleGraphUnit::valueZoomOut(%this)
{
	%this.valueController.zoomOut();
	%this.refreshCamera();
}

function AssetParticleGraphUnit::valueMoveUp(%this)
{
	%this.valueController.moveUp();
	%this.refreshCamera();
}

function AssetParticleGraphUnit::valueMoveDown(%this)
{
	%this.valueController.moveDown();
	%this.refreshCamera();
}

function AssetParticleGraphUnit::timeZoomIn(%this)
{
	%this.timeController.zoomIn();
	%this.refreshCamera();
}

function AssetParticleGraphUnit::timeZoomOut(%this)
{
	%this.timeController.zoomOut();
	%this.refreshCamera();
}

function AssetParticleGraphUnit::timeMoveBack(%this)
{
	%this.timeController.moveDown();
	%this.refreshCamera();
}

function AssetParticleGraphUnit::timeMoveForward(%this)
{
	%this.timeController.moveUp();
	%this.refreshCamera();
}
