function Audio::create(%this)
{
	if(OpenALInitDriver())
	{
	    %this.MusicOn = true;
	    %this.SoundOn = true;
		%this.setMasterVolume(1);
		%this.SetMusicVolume(1);
		%this.SetSoundVolume(1);
	    %this.CurrentSong = "";
	}
}

function Audio::destroy( %this )
{
	OpenALShutdownDriver();
}

function Audio::setMasterVolume(%this, %volume)
{
	%this.MasterVolume = mClamp(%volume, 0, 1);
	alxListenerf(AL_GAIN_LINEAR, %this.MasterVolume);
}

function Audio::SetMusicVolume(%this, %volume)
{
	%this.MusicVolume = mClamp(%volume, 0, 1);
    alxSetChannelVolume(0, %this.MusicVolume);
}

function Audio::SetSoundVolume(%this, %volume)
{
    %this.SoundVolume = mClamp(%volume, 0,1);
    alxSetChannelVolume(1, %this.SoundVolume);
}

function Audio::SetPitch(%this, %noise, %pitch)
{
	alxSourcef(%noise, AL_PITCH, %pitch);
}

function Audio::StopAllAndPlayMusic(%this, %song)
{
    if(%song !$= %this.CurrentSong)
    {
        alxStopAll();
        %this.CurrentSong = %song;
        if(%this.MusicOn)
        {
            cancel(%this.fadeMusicSchedule);
            %this.Music = alxPlay("Audio:" @ %song);
        }
    }
}

function Audio::PlayMusic(%this, %song)
{
   if(%song !$= %this.CurrentSong)
   {
      %this.CurrentSong = %song;
      if(%this.MusicOn)
      {
         cancel(%this.fadeMusicSchedule);
         alxStop(%this.Music);
         %this.Music = alxPlay("Audio:" @ %song);
      }
   }
}

function Audio::RestartMusic(%this)
{
	cancel(%this.fadeMusicSchedule);
	alxStop(%this.Music);
	%this.Music = alxPlay("Audio:" @ %this.CurrentSong);
}

function Audio::StopMusic(%this)
{
	alxStop(%this.Music);
}

function Audio::PlaySound(%this, %name)
{
    if(%this.SoundOn)
    {
        %sound = alxPlay("Audio:" @ %name);
    }

    return %sound;
}

function Audio::FadeMusicVolumeTo(%this, %time, %volume)
{
	//Time is in milliseconds
	%volume = mClamp(%volume, 0,1);

	if(%volume == %this.MusicVolume)
	{
		return;
	}

	%difference = %volume - %this.MusicVolume;
	%rate = 50;
	%steps = mCeil(%time / %rate);
	%delta = %difference / %steps;
	%this.fadeMusicSchedule = %this.schedule(%rate, "FadeMusicStep", %volume, %rate, %delta);
}

function Audio::FadeMusicStep(%this, %targetVolume, %rate, %delta)
{
	if(mAbs(%targetVolume - %this.MusicVolume) < mAbs(%delta))
	{
		%this.SetMusicVolume(%targetVolume);
		return;
	}
	%this.SetMusicVolume(%this.MusicVolume + %delta);
	%this.fadeMusicSchedule = %this.schedule(%rate, "FadeMusicStep", %targetVolume, %rate, %delta);
}
