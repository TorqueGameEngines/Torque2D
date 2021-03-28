$currentTarget = "";
// Doing this in script might end up being a waste of time
// but because of the way the current inspector is messing about
// it is needed.

function GuiEditorCtrlProperties::update(%this, %target)
{
	// We can only get here from the guiEdit so its definitely a 
	// gui control, no need to check.
	if(%target == 0)
	{
		GuiEditorCtrlProperties.clear();
		$currentTarget = "";
	}
	else
	{
		if($currentTarget != %target)
		{
			%this.build(%target);
		}
		else
		{
			// if we are locked update the gui to reflect it.
			if($currentTarget.getFieldValue("locked"))
			{
				$targetprevLocked = true;
				%this.build($currentTarget);
			}
			// if we were prev locked we need to rebuild
			else if($targetprevLocked)
			{
				// just so this isn't always happening
				// sometimes we only want to updateValues
				%this.build($currentTarget);
				$targetprevLocked = false;
			}
			// update values of fields.
			%this.updateValues(%target);
		}
	}
}

function GuiEditorCtrlProperties::updateValues(%this, %target)
{
	// go through every field and updates.
	%count = %target.getFieldCount();
	for(%i = 0; %i < %count; %i++)
	{
		%field = %target.getField(%i);
		%fieldType = %target.getFieldType(%field);
		if(%fieldType $= "Point2I")
		{
			%fieldVal = %target.getFieldValue(%field);
			%fieldEditX = %field @ "editX";
			%fieldEditY = %field @ "editY";
			%fieldEditX.setText(%fieldVal.x);
			%fieldEditY.setText(%fieldVal.y);
			
		}
		else if(%fieldType $= "bool")
		{
			%fieldEdit = %field @ "edit";
			%fieldEdit.setStateOn(%target.getFieldValue(%field));
		}
		else
		{
			%fieldEdit = %field @ "edit";
			%fieldEdit.setText(%target.getFieldValue(%field));
		}
	}
}

function GuiEditorCtrlProperties::build(%this, %target)
{
	// assign target so we can keep track.
	$currentTarget = %target;
	GuiEditorCtrlProperties.clear();
	
	// every field gets a gui
	%count = %target.getFieldCount();
	for(%i = 0; %i < %count; %i++)
	{
		%field = %target.getField(%i);
		// we only want the locked field.
		if($currentTarget.getFieldValue("locked"))
		{
			if(%field !$= "locked")
			{
				continue;
			}
		}
		
		%fieldCtrl = new GuiControl()
		{
			extent = "320 30";
			horizSizing = "right";
			vertSizing = "bottom";
			profile = "GuiDefaultProfile";
			visible = "1";
		};
		
		%fieldLabel = new GuiControl()
		{
			text = %field @ ": ";
			profile = "GuiTextRightProfile";
			Position = "10 0";
			extent = "150 30";
			horizSizing = "right";
			vertSizing = "bottom";
			visible = "1";
		};
		
		%fieldType = %target.getFieldType(%field);
		%fieldVal = %target.getFieldValue(%field);
		%fieldEdit = %this.buildEdit(%field,%fieldType,%fieldVal);
		%fieldCtrl.add(%fieldLabel);
		%fieldCtrl.add(%fieldEdit);

		GuiEditorCtrlProperties.add(%fieldCtrl);
	}
	
	// we don't want dynamic fields either.
	if($currentTarget.getFieldValue("locked"))
	{
		return;
	}
	
	// build dynamic fields
	%dynCtrl = new GuiPanelCtrl()
	{
		position = "10 0";
		extent = "320 20";
		horizSizing = "right";
		vertSizing = "bottom";
		profile = "GuiPanelProfile";
		visible = "1";
		Text = "Dynamic Fields";
	};
	
	GuiEditorCtrlProperties.add(%dynCtrl);
	
	%addDyn = new GuiControl()
	{
		Position = "0 20";
		extent = "320 30";
		horizSizing = "right";
		vertSizing = "bottom";
		profile = "GuiDefaultProfile";
		visible = "1";
	};
	
	%label = new GuiControl()
	{
		text = "Add Dynamic Field";
		profile = "GuiTextProfile";
		Position = "10 0";
		extent = "150 30";
		horizSizing = "right";
		vertSizing = "bottom";
		visible = "1";
	};
	
	%bttn = new GuiButtonCtrl()
	{
		text = "+";
		profile = "GuiButtonDynProfile";
		Position = "280 0";
		extent = "30 30";
		Command = "GuiEditorCtrlProperties.addDynamic();";
	};
	
	%addDyn.add(%label);
	%addDyn.add(%bttn);
	%dynCtrl.add(%addDyn);
	if(isObject(DynamicContainer))
	{
		DynamicContainer.clear();
		DynamicContainer.delete();
	}
	
	%dynChain = new GuiChainCtrl(DynamicContainer)
	{
		position = "0 50";
		extent = "320 30";
	};
		
	%dynCtrl.add(%dynChain);
	
	// dynamic fields need to be updated separately.
	// eventually should separate fields by group.
	
	GuiEditorCtrlProperties.updateDynamicFields();
}

function GuiEditorCtrlProperties::addDynamic(%this)
{
	%name = "dynamicField" @ $currentTarget.getDynamicFieldCount();
	$currentTarget.setFieldValue(%name,"defaultValue");
	
	GuiEditorCtrlProperties.updateDynamicFields();
}

function GuiEditorCtrlProperties::removeDynamic(%this, %field)
{
	// giving a dynamic field a null value deletes it.
	$currentTarget.setFieldValue(%field,"");
	GuiEditorCtrlProperties.updateDynamicFields();
}

function GuiEditorCtrlProperties::renameDynamic(%this, %field, %newName, %fieldVal)
{
	$currentTarget.setFieldValue(%newName, %fieldVal);
	$currentTarget.setFieldValue(%field,"");
	GuiEditorCtrlProperties.updateDynamicFields();
}

function GuiEditorCtrlProperties::updateDynamicFields(%this)
{
	%count = $currentTarget.getDynamicFieldCount();
	DynamicContainer.clear();
	for(%i = 0; %i < %count; %i++)
	{
		%field = $currentTarget.getDynamicField(%i);
		%fieldVal = $currentTarget.getFieldValue(%field);
		
		
		%dynCtrl = new GuiControl()
		{
			extent = "320 30";
			horizSizing = "right";
			vertSizing = "bottom";
			profile = "GuiDefaultProfile";
			visible = "1";
		};
		
		%label = new GuiTextEditCtrl()
		{
			text = %field;
			profile = "GuiTextEditProfile";
			Position = "10 0";
			extent = "130 30";
			horizSizing = "right";
			vertSizing = "bottom";
			visible = "1";
		};
		
		%val = new GuiTextEditCtrl()
		{
			text = %fieldVal;
			profile = "GuiTextEditProfile";
			Position = "140 0";
			extent = "130 30";
			horizSizing = "right";
			vertSizing = "bottom";
			visible = "1";
		};
		%label.setFieldValue("AltCommand", "GuiEditorCtrlProperties.renameDynamic(" @ %field @ "," @ %label.getId()  @ ".getText()," @ %val.getId()  @ ".getText());");
		%label.setFieldValue("Validate", "GuiEditorCtrlProperties.renameDynamic(" @ %field @ "," @ %label.getId()  @ ".getText()," @ %val.getId()  @ ".getText());");
		
		%val.setFieldValue("AltCommand", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %val.getId() @ ".getText());");
		%val.setFieldValue("Validate", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %val.getId() @ ".getText());");
		
		%bttn = new GuiButtonCtrl()
		{
			text = "X";
			profile = "GuiButtonDynProfile";
			Position = "280 0";
			extent = "30 30";
		};
		
		%bttn.setFieldValue("Command", "GuiEditorCtrlProperties.removeDynamic(" @ %field @ ");");
		%dynCtrl.add(%label);
		%dynCtrl.add(%val);
		%dynCtrl.add(%bttn);
		DynamicContainer.add(%dynCtrl);
		DynamicContainer.getParent().setExpanded(0);
		DynamicContainer.getParent().setExpanded(1);
	}
}

function GuiEditorCtrlProperties::setData(%this,%field, %val)
{
	$currentTarget.setEditFieldValue(%field, %val);
	%this.update($currentTarget);
}

function GuiEditorCtrlProperties::buildEdit(%this,%field,%fieldType,%fieldVal)
{
	// items yet to be added:
	// filename
	// RectI
	// assetIdString
	// audioAssetPtr
	
	%fieldEdit = %field @ "edit";
	echo(%field TAB %fieldType TAB %fieldVal);
	
	switch$(%fieldType)
	{
		case "float":
			%ctrl = new GuiTextEditCtrl(%fieldEdit)
				{
					Text = %fieldVal;
					Position = "160 0";
					extent = "160 30";
					horizSizing = "right";
					vertSizing = "bottom";
					visible = "1";
					Profile = "GuiTextEditProfile";
				};
			%ctrl.setFieldValue("AltCommand", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getText());");
			%ctrl.setFieldValue("Validate", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getText());");
			return %ctrl;
			
		case "ColorF":
			%ctrl = new GuiTextEditCtrl(%fieldEdit)
				{
					Text = %fieldVal;
					Position = "160 0";
					extent = "160 30";
					horizSizing = "right";
					vertSizing = "bottom";
					visible = "1";
					Profile = "GuiTextEditProfile";
				};
			%ctrl.setFieldValue("AltCommand", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getText());");
			%ctrl.setFieldValue("Validate", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getText());");
			return %ctrl;
			
		case "ColorI":
			%ctrl = new GuiTextEditCtrl(%fieldEdit)
				{
					Text = %fieldVal;
					Position = "160 0";
					extent = "160 30";
					horizSizing = "right";
					vertSizing = "bottom";
					visible = "1";
					Profile = "GuiTextEditProfile";
				};
			%ctrl.setFieldValue("AltCommand", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getText());");
			%ctrl.setFieldValue("Validate", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getText());");
			return %ctrl;
			
		case "SimObjectPtr":
		return	%ctrl = new GuiControl(%fieldEdit)
				{
					Text = %fieldVal;
					Position = "160 0";
					extent = "160 30";
					horizSizing = "right";
					vertSizing = "bottom";
					visible = "1";
					Profile = "GuiTextProfile";
				};
			
		case "int":
			%ctrl = new GuiTextEditCtrl(%fieldEdit)
				{
					Text = %fieldVal;
					Position = "160 0";
					extent = "160 30";
					horizSizing = "right";
					vertSizing = "bottom";
					visible = "1";
					Profile = "GuiNumberEditProfile";
				};
			%ctrl.setFieldValue("AltCommand", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getText());");
			%ctrl.setFieldValue("Validate", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getText());");
			return %ctrl;
			
		case "bool":
			%ctrl = new GuiCheckBoxCtrl(%fieldEdit)
				{
					text = "";
					stateOn = %fieldVal;
					Position = "160 0";
					extent = "160 30";
					horizSizing = "relative";
					vertSizing = "relative";
					visible = "1";
					Profile = "GuiCheckBoxProfile";
				};
			%ctrl.setFieldValue("Command", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getStateOn());");
			return %ctrl;
			
		case "Point2I":
			return	%this.buildPoint2ICtrl(%field,%fieldVal);
			
		case "Point2F":
			return	%this.buildPoint2ICtrl(%field,%fieldVal);
			
		case "enumval":
			%ctrl = new GuiPopUpMenuCtrl(%fieldEdit)
				{
					text = %fieldVal;
					Position = "160 6";
					extent = "160 18";
					Profile="GuiPopUpMenuProfile2";
					HorizSizing="relative";
					VertSizing="relative";
					maxLength="1024";
					maxPopupHeight="200";
					bitmapBounds="16 16";
				};
			%cl = $currentTarget.getClassName();
			%ctrl.setEnumContent(%cl, %field);
			%ctrl.sort();
			%ctrl.setFieldValue("Command", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getText());");
			return %ctrl;
			
		case "GuiProfile":
			return %ctrl = %this.buildGuiProfileCtrl(%field, %fieldVal);
			
		default:
			// better lazy than complicated.
			%ctrl = new GuiTextEditCtrl(%fieldEdit)
			{
				Text = %fieldVal;
				Position = "160 0";
				extent = "160 30";
				horizSizing = "right";
				vertSizing = "bottom";
				visible = "1";
				Profile = "GuiTextEditProfile";
				
			};
			%ctrl.setFieldValue("AltCommand", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getText());");
			%ctrl.setFieldValue("Validate", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getText());");
			return %ctrl;
	}
	
}

function GuiEditorCtrlProperties::buildGuiProfileCtrl(%this,%field,%fieldVal)
{
	%fieldEdit = %field @ "edit";
	%ctrl = new GuiPopUpMenuCtrl(%fieldEdit)
	{
		text = %fieldVal;
		Position = "160 6";
		extent = "160 18";
		Profile="GuiPopUpMenuProfile2";
		HorizSizing="relative";
		VertSizing="relative";
		maxLength="1024";
		maxPopupHeight="200";
		bitmapBounds="16 16";
	};
	%count = GuiDataGroup.getCount();
	for(%i = 0; %i < %count; %i++)
	{
		%obj = GuiDataGroup.getObject(%i);
		if(%obj.getClassName() $= "GuiControlProfile")
		{
			%cat = %obj.category;
			if(%cat !$= "")
			{
				echo(%cat);
			}
			
			if(%obj.getName() !$= "")
			{
				%ctrl.add(%obj.getName(), 0);
			}
		}
	}
	%ctrl.sort();
	%ctrl.setFieldValue("Command", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrl.getId() @ ".getText());");
	return %ctrl;
	
}

function GuiEditorCtrlProperties::buildPoint2ICtrl(%this,%field, %fieldVal)
{
	%fieldEditX = %field @ "editX";
	%fieldEditY = %field @ "editY";
	
	%cont = new GuiControl()
	{
		extent = "160 30";
		Position = "160 0";
		horizSizing = "right";
		vertSizing = "bottom";
		profile = "GuiDefaultProfile";
		visible = "1";
	};
	
	%xLabel = new GuiControl()
	{
		text = "x:";
		extent = "30 30";
		Position = "0 0";
		horizSizing = "right";
		vertSizing = "bottom";
		profile = "GuiTextRightProfile";
		visible = "1";
	}; 
	
	
	
	%ctrlX = new GuiTextEditCtrl(%fieldEditX)
	{
		Text = %fieldVal.x;
		Position = "30 0";
		extent = "50 30";
		horizSizing = "right";
		vertSizing = "bottom";
		visible = "1";
		Profile = "GuiTextEditProfile";
	};
	
	
	
	%yLabel = new GuiControl()
	{
		text = "y:";
		extent = "30 30";
		Position = "80 0";
		horizSizing = "right";
		vertSizing = "bottom";
		profile = "GuiTextRightProfile";
		visible = "1";
	}; 
	
	
	
	%ctrlY = new GuiTextEditCtrl(%fieldEditY)
	{
		Text = %fieldVal.y;
		Position = "110 0";
		extent = "50 30";
		horizSizing = "right";
		vertSizing = "bottom";
		visible = "1";
		Profile = "GuiTextEditProfile";
	};
	
	%ctrlX.setFieldValue("AltCommand", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrlX.getId() @ ".getText() SPC " @ %ctrlY.getId() @ ".getText());");
	%ctrlX.setFieldValue("Validate", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrlX.getId() @ ".getText() SPC " @ %ctrlY.getId() @ ".getText());");
	%ctrlY.setFieldValue("AltCommand", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrlX.getId() @ ".getText() SPC " @ %ctrlY.getId() @ ".getText());");
	%ctrlY.setFieldValue("Validate", "GuiEditorCtrlProperties.setData(" @ %field @ "," @ %ctrlX.getId() @ ".getText() SPC " @ %ctrlY.getId() @ ".getText());");
	
	%cont.add(%xLabel);
	%cont.add(%ctrlX);
	%cont.add(%yLabel);
	%cont.add(%ctrlY);
	
	return %cont;
	
}


