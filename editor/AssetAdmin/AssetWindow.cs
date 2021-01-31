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

function AssetWindow::onAdd(%this)
{

}

function AssetWindow::displayImageAsset(%this, %imageAsset, %assetID)
{
	AssetAdmin.AssetScene.clear(true);

	if(!%imageAsset.getExplicitMode() && %imageAsset.getFrameCount() == 1)
	{
		%size = %this.getWorldSize(%imageAsset.getFrameSize(0));
		new Sprite()
		{
			Scene = AssetAdmin.AssetScene;
			Image = %assetID;
			size = %size;
			BlandColor = "1 1 1 1";
			SceneLayer = 1;
			Position = "0 0";
			BodyType = static;
		};
	}
	else if(!%imageAsset.getExplicitMode() && %imageAsset.getFrameCount() > 1)
	{
		%cellCountX = %imageAsset.getCellCountX();
		%cellCountY = %imageAsset.getCellCountY();

		%cellWidth = %imageAsset.getCellWidth();
		%cellHeight = %imageAsset.getCellHeight();

		%imageSizeX = %cellWidth * %cellCountX;
		%imageSizeY = %cellHeight * %cellCountY;

		%size = %this.getWorldSize(%imageSizeX SPC %imageSizeY);
		%sizeX = getWord(%size, 0);
		%sizeY = getWord(%size, 1);

		%worldCellWidth = %sizeX / %cellCountX;
		%worldCellHeight = %sizeY / %cellCountY;

		for(%j = 0; %j < %cellCountY; %j++)
		{
			for(%i = 0; %i < %cellCountX; %i++)
			{
				%sprite = new Sprite()
				{
					Scene = AssetAdmin.AssetScene;
					Image = %assetID;
					Frame = (%j * %imageAsset.getCellCountX()) + %i;
					size = Vector2Scale(%worldCellWidth SPC %worldCellHeight, 2.8);
					position = ((%worldCellWidth * %i) + (%worldCellWidth/2) - (%sizeX/2)) SPC ((-%worldCellHeight * %j) - (%worldCellHeight/2) + (%sizeY/2));
					BlendColor = "1 1 1 0";
					SceneLayer = 1;
					BodyType = static;
				};

				%sprite.growToTime(Vector2Scale(%worldCellWidth SPC %worldCellHeight, 0.94), 225 + (%i * 20));
				%sprite.fadetoTime("1 1 1 1", 225 + (%i * 20));
			}
		}
	}
}

function AssetWindow::displayAnimationAsset(%this, %imageAsset, %animationAsset, %assetID)
{
	AssetAdmin.AssetScene.clear(true);

	%size = %this.getWorldSize(%imageAsset.getFrameSize(0));
	new Sprite()
	{
		Scene = AssetAdmin.AssetScene;
		Animation = %assetID;
		size = %size;
		BlandColor = "1 1 1 1";
		SceneLayer = 1;
		Position = "0 0";
		BodyType = static;
	};
}

function AssetWindow::getWorldSize(%this, %size)
{
	%cameraSize = %this.getCameraSize();
	%cameraX = getWord(%cameraSize, 0);
	%cameraY = getWord(%cameraSize, 1);

	%x = getWord(%size, 0);
	%y = getWord(%size, 1);

	%inset = 10;

	//calculate the finished rect 2 ways
	%finX1 = %cameraX - %inset;
	%finY1 = (%y * %finX1) / %x;

	%finY2 = %cameraY - %inset;
	%finX2 = (%x * %finY2) / %y;

	//Return 1 if it's bigger and fits or if it's smaller and 2 doesn't fit
	if((%finX1 > %finX2 && %finX1 <= (%cameraX - %inset) && %finY1 <= (%cameraY - %inset)) || (%finX1 < %finX2 && (%finX2 > (%cameraX - %inset) || %finY2 > (%cameraY - %inset))))
	{
		return %finX1 SPC %finY1;
	}
	else
	{
		//otherwise use the second finished rect
		return %finX2 SPC %finY2;
	}
}

function AssetWindow::onExtentChange(%this, %d)
{
	%x = getWord(%d, 2);
	%y = getWord(%d, 3);

	if(%x >= %y)
	{
		%mult = 100/%x;
		%areaY =  (%y * %mult);
		%topLeft = "-50" SPC (%areaY / 2);
		%bottomRight = "50" SPC -(%areaY / 2);
	}
	else
	{
		%mult = 100/%y;
		%areaX =  (%x * %mult);
		%topLeft = (%areaX / 2) SPC "-50";
		%bottomRight = -(%areaX / 2) SPC "50";
	}
	%area = %topLeft SPC %bottomRight;
	%this.setCameraArea(%area);
}
