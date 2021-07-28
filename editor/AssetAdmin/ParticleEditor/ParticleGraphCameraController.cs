
function ParticleGraphCameraController::onAdd(%this)
{
	if(strstr(%this.fieldName, "Scale") > 0)
	{
		%this.asset.selectField(%this.fieldName);
		if(%this.isTime)
		{
			%this.min = %this.asset.getMinTime();
			%this.max = %this.asset.getMaxTime();
		}
		else
		{
			%this.min = %this.asset.getMinValue();
			%this.max = %this.asset.getMaxValue();
		}
	}
	else
	{
		%emitter = %this.asset.getEmitter(0);
		%emitter.selectField(%this.fieldName);
		if(%this.isTime)
		{
			%this.min = %emitter.getMinTime();
			%this.max = %emitter.getMaxTime();
		}
		else
		{
			%this.min = %emitter.getMinValue();
			%this.max = %emitter.getMaxValue();
		}

	}
	%this.currentPosition = 0;

	%this.zoomLevel[1] = 1;
	%this.zoomCount = 1;
	%this.currentZoomLevel = 1;

	if(%this.max > 1)
	{
		%this.zoomLevel[2] = 10;
		%this.zoomCount = 2;
		%this.currentZoomLevel = 2;
	}

	if(%this.max > 10)
	{
		%this.zoomLevel[3] = 100;
		%this.zoomCount = 3;
	}

	if(%this.max > 100)
	{
		%this.zoomLevel[4] = 1000;
		%this.zoomCount = 4;
	}

	if(%this.max == 180 || %this.max == 360 || %this.max == 720)
	{
		%this.setupDegreeValue();
	}
}

function ParticleGraphCameraController::setupDegreeValue(%this)
{
	%this.currentPosition = %this.min;

	%this.zoomLevel[1] = 5;
	%this.zoomLevel[2] = 30;
	%this.zoomLevel[3] = 90;
	%this.zoomLevel[4] = 360;
	%this.zoomCount = 4;
	%this.currentZoomLevel = 4;
}

function ParticleGraphCameraController::getCameraMin(%this)
{
	return %this.currentPosition;
}

function ParticleGraphCameraController::getCameraMax(%this)
{
	return %this.currentPosition + %this.zoomLevel[%this.currentZoomLevel];
}

function ParticleGraphCameraController::zoomIn(%this)
{
	%this.currentZoomLevel = mGetMax(%this.currentZoomLevel - 1, 1);
}

function ParticleGraphCameraController::zoomOut(%this)
{
	%this.currentZoomLevel = mGetMin(%this.currentZoomLevel + 1, %this.zoomCount);
	%zoom = %this.zoomLevel[%this.currentZoomLevel];
	%this.currentPosition = mFloor(%this.currentPosition / %zoom) * %zoom;

	if((%this.currentPosition + %zoom) > %this.max)
	{
		%this.currentPosition = %this.max - %zoom;
	}

	if(%this.currentPosition < %this.min)
	{
		%this.currentPosition = %this.min;
	}
}

function ParticleGraphCameraController::moveUp(%this)
{
	%zoom = %this.zoomLevel[%this.currentZoomLevel];
	%this.currentPosition = mGetMin(%this.currentPosition + %zoom, %this.max - %zoom);
}

function ParticleGraphCameraController::moveDown(%this)
{
	%this.currentPosition = mGetMax(%this.currentPosition - %this.zoomLevel[%this.currentZoomLevel], %this.min);
}

function ParticleGraphCameraController::getMoveUpEnabled(%this)
{
	%zoom = %this.zoomLevel[%this.currentZoomLevel];
	return (%this.currentPosition + %zoom) < %this.max;
}

function ParticleGraphCameraController::getMoveDownEnabled(%this)
{
	return %this.currentPosition > %this.min;
}

function ParticleGraphCameraController::getZoomInEnabled(%this)
{
	return %this.currentZoomLevel > 1;
}

function ParticleGraphCameraController::getZoomOutEnabled(%this)
{
	return %this.currentZoomLevel < %this.zoomCount;
}
