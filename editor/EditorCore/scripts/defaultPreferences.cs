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

/// Game
$Game::CompanyName              = "Torque Game Engines";
$Game::ProductName              = "Torque2D";

/// iOS
$pref::iOS::ScreenOrientation   = $iOS::constant::Landscape;
$pref::iOS::ScreenDepth		    = 32;
$pref::iOS::UseGameKit          = 0;
$pref::iOS::UseMusic            = 0;
$pref::iOS::UseMoviePlayer      = 0;
$pref::iOS::UseAutoRotate       = 1;
$pref::iOS::EnableOrientationRotation = 1;
$pref::iOS::EnableOtherOrientationRotation = 1;
$pref::iOS::StatusBarType       = 0;

/// T2D
$pref::T2D::ParticlePlayerEmissionRateScale = 1.0;
$pref::T2D::ParticlePlayerSizeScale = 1.0;
$pref::T2D::ParticlePlayerForceScale = 1.0;
$pref::T2D::ParticlePlayerTimeScale = 1.0;
$pref::T2D::warnFileDeprecated = 1;
$pref::T2D::warnSceneOccupancy = 1;
$pref::T2D::imageAssetGlobalFilterMode = Bilinear;
$pref::T2D::TAMLSchema="";
$pref::T2D::JSONStrict = 1;

/// Video
$pref::Video::appliedPref = 0;
$pref::Video::displayDevice = "OpenGL";
$pref::Video::preferOpenGL = 1;
$pref::Video::fullScreen = 0;
$pref::Video::defaultResolution = "1024 768";
/// Empty: no preferred window size, so a tiling window manager is free to tile
/// the window. A size here is one wanted exactly; a tiling window manager then
/// opens the window floating at that size.
$pref::Video::windowedRes = "";
$pref::OpenGL::gammaCorrection = 0.5;

/// Fonts.
$Gui::fontCacheDirectory = expandPath( "^EditorCore/gui/fonts" );

/// Generic fallback font (a .ttf rasterized by FreeType) for platforms with no
/// system fonts -- currently the web (Emscripten) build. The editor keeps its OWN
/// copy (separate from the app's in AppCore), so editor fonts stay independent and
/// the editor never reaches into AppCore. This overrides AppCore's value while the
/// editor is loaded; a shipped game (editor/ removed) falls back to AppCore's.
$pref::Web::fallbackFont = expandPath( "^EditorCore/gui/fonts/Roboto-Regular.ttf" );
