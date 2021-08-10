
function AssetAudioPlayButton::onClick(%this)
{
	if(alxIsPlaying(%this.sound))
	{
		%this.resetSound();
	}
	else
	{
		%this.sound = alxPlay(%this.assetID);
		%this.setText("Stop");

		if(!%this.asset.Looping)
		{
			%this.soundSchedule = %this.schedule(100, "testSound");
		}
	}
}

function AssetAudioPlayButton::testSound(%this)
{
	if(alxIsPlaying(%this.sound))
	{
		%this.soundSchedule = %this.schedule(100, "testSound");
	}
	else
	{
		%this.setText("Play");
	}
}

function AssetAudioPlayButton::resetSound(%this)
{
	if(alxIsPlaying(%this.sound))
	{
		alxStop(%this.sound);
		%this.setText("Play");
		cancel(%this.soundSchedule);
	}
}
