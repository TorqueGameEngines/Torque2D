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

#include "graphics/dgl.h"
#include "gui/guiTypes.h"
#include "gui/guiCanvas.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "math/mMathFn.h"
#include "2d/sceneobject/SceneObject.h"
#include "2d/core/Utility.h"
#include "2d/gui/SceneWindow.h"
#include "gui/containers/guiSceneScrollCtrl.h"

#ifndef _ASSET_MANAGER_H_
#include "assets/assetManager.h"
#endif

// Script bindings.
#include "SceneWindow_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

// Input event names.
static StringTableEntry inputEventEnterName            = StringTable->insert("onTouchEnter");
static StringTableEntry inputEventLeaveName            = StringTable->insert("onTouchLeave");
static StringTableEntry inputEventDownName             = StringTable->insert("onTouchDown");
static StringTableEntry inputEventUpName               = StringTable->insert("onTouchUp");
static StringTableEntry inputEventMovedName            = StringTable->insert("onTouchMoved");
static StringTableEntry inputEventDraggedName          = StringTable->insert("onTouchDragged");

static StringTableEntry mouseEventMiddleMouseDownName   = StringTable->insert("onMiddleMouseDown");
static StringTableEntry mouseEventMiddleMouseUpName     = StringTable->insert("onMiddleMouseUp");
static StringTableEntry mouseEventMiddleMouseDraggedName= StringTable->insert("onMiddleMouseDragged");

static StringTableEntry mouseEventRightMouseDownName   = StringTable->insert("onRightMouseDown");
static StringTableEntry mouseEventRightMouseUpName     = StringTable->insert("onRightMouseUp");
static StringTableEntry mouseEventRightMouseDraggedName= StringTable->insert("onRightMouseDragged");

static StringTableEntry mouseEventWheelUpName          = StringTable->insert("onMouseWheelUp");
static StringTableEntry mouseEventWheelDownName        = StringTable->insert("onMouseWheelDown");

static StringTableEntry mouseEventEnterName            = StringTable->insert("onTouchEnter");
static StringTableEntry mouseEventLeaveName            = StringTable->insert("onTouchLeave");

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SceneWindow);

//-----------------------------------------------------------------------------

static EnumTable::Enums interpolationModeLookup[] =
                {
                { SceneWindow::LINEAR,   "LINEAR" },
                { SceneWindow::SIGMOID,  "SIGMOID" },
                };

static EnumTable interpolationModeTable(sizeof(interpolationModeLookup) / sizeof(EnumTable::Enums), &interpolationModeLookup[0]);

//-----------------------------------------------------------------------------

SceneWindow::CameraInterpolationMode SceneWindow::getInterpolationModeEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(interpolationModeLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(interpolationModeLookup[i].label, label) == 0)
            return((CameraInterpolationMode)interpolationModeLookup[i].index);

    // Warn.
    Con::warnf( "SceneWindow::getInterpolationModeEnum() - Invalid interpolation mode '%s'.", label );

    return SceneWindow::INVALID_INTERPOLATION_MODE;
}

//-----------------------------------------------------------------------------

SceneWindow::SceneWindow() :    mpScene(NULL),
                                mLockMouse(false),
                                mWindowDirty(true),
                                mRenderLayerMask(MASK_ALL),
                                mRenderGroupMask(MASK_ALL),
                                mBackgroundColor( "Black" ),
                                mUseBackgroundColor(false),   
                                mCameraInterpolationMode(SIGMOID),
                                mMaxQueueItems(64),
                                mCameraTransitionTime(2.0f),
                                mMovingCamera(false),
                                mpMountedTo(NULL),
                                mCameraMounted(false),
                                mCameraShaking(false),
                                mCameraShakeOffset(0.0f,0.0f),
                                mViewLimitActive(false),
                                mUseWindowInputEvents(true),
                                mUseObjectInputEvents(false),
                                mInputEventGroupMaskFilter(MASK_ALL),
                                mInputEventLayerMaskFilter(MASK_ALL),
                                mInputEventInvisibleFilter( true ),
                                mProcessAudioListener(false),
								mShowScrollBar(false),
								mMouseWheelScrolls(false)
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mCameraQueue );
    VECTOR_SET_ASSOCIATION( mInputEventQuery );
    VECTOR_SET_ASSOCIATION( mInputEventEntering );
    VECTOR_SET_ASSOCIATION( mInputEventLeaving );    

    // Turn-on Tick Processing.
    setProcessTicks( true );

	mThumbProfile = NULL;
	mTrackProfile = NULL;
	mArrowProfile = NULL;

	mScrollBar = new GuiSceneScrollCtrl(this);
	mScrollBar->mForceVScrollBar = GuiScrollCtrl::ScrollBarDynamic;
	mScrollBar->mForceHScrollBar = GuiScrollCtrl::ScrollBarDynamic;
	mUseConstantHeightThumb = false;
	mScrollBarThickness = 16;
	mShowArrowButtons = true;

	mThumbProfile = mScrollBar->mThumbProfile;
	mThumbProfile->incRefCount();
	mTrackProfile = mScrollBar->mTrackProfile;
	mThumbProfile->incRefCount();
	mArrowProfile = mScrollBar->mArrowProfile;
	mThumbProfile->incRefCount();
}

//-----------------------------------------------------------------------------

SceneWindow::~SceneWindow()
{
	delete mScrollBar;
}

//-----------------------------------------------------------------------------

bool SceneWindow::onAdd()
{
    // Call parent.
    if(!Parent::onAdd())
        return false;

    // Register input sets.
    mInputEventWatching.registerObject();
    mInputListeners.registerObject();

    // Reset the camera position.
    setCameraPosition( Vector2::getZero() );

    // Reset the camera size.
    setCameraSize( Vector2( 100.0f, 75.0f ) );

    // Reset the camera zoom.
    setCameraZoom( 1.0f );

    // Zero Camera Time.
    zeroCameraTime();
   
    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void SceneWindow::onRemove()
{
    // Reset Scene.
    resetScene();

    // Unregister input sets.
    mInputEventWatching.unregisterObject();
    mInputListeners.unregisterObject();

    // Call Parent.
    Parent::onRemove();
}

//-----------------------------------------------------------------------------

void SceneWindow::initPersistFields()
{
    // Call Parent.
    Parent::initPersistFields();

    // Add Fields.
    addField( "lockMouse",               TypeBool, Offset(mLockMouse, SceneWindow) );
    addField( "UseWindowInputEvents",    TypeBool, Offset(mUseWindowInputEvents, SceneWindow) );
    addField( "UseObjectInputEvents",    TypeBool, Offset(mUseObjectInputEvents, SceneWindow) );

    // Background color.
    addField("UseBackgroundColor", TypeBool, Offset(mUseBackgroundColor, SceneWindow), &writeUseBackgroundColor, "" );
    addField("BackgroundColor", TypeColorF, Offset(mBackgroundColor, SceneWindow), &writeBackgroundColor, "" );

	//Standard scroll bar settings
	addProtectedField("constantThumbHeight", TypeBool, Offset(mUseConstantHeightThumb, GuiScrollCtrl), &setConstantThumbFn, &defaultProtectedGetFn, &writeScrollSettingFn, "");
	addProtectedField("scrollBarThickness", TypeS32, Offset(mScrollBarThickness, GuiScrollCtrl), &setScrollBarThicknessFn, &defaultProtectedGetFn, &writeScrollSettingFn, "");
	addProtectedField("showArrowButtons", TypeBool, Offset(mShowArrowButtons, GuiScrollCtrl), &setShowArrowButtonsFn, &defaultProtectedGetFn, &writeScrollSettingFn, "");
	addProtectedField("thumbProfile", TypeGuiProfile, Offset(mThumbProfile, GuiScrollCtrl), &setThumbProfileFn, &defaultProtectedGetFn, &writeScrollSettingFn, "");
	addProtectedField("trackProfile", TypeGuiProfile, Offset(mTrackProfile, GuiScrollCtrl), &setTrackProfileFn, &defaultProtectedGetFn, &writeScrollSettingFn, "");
	addProtectedField("arrowProfile", TypeGuiProfile, Offset(mArrowProfile, GuiScrollCtrl), &setArrowProfileFn, &defaultProtectedGetFn, &writeScrollSettingFn, "");
}

bool SceneWindow::onWake()
{
	if (!Parent::onWake())
	{
		return false;
	}

	if (mThumbProfile != NULL)
		mThumbProfile->incRefCount();

	if (mTrackProfile != NULL)
		mTrackProfile->incRefCount();

	if (mArrowProfile != NULL)
		mArrowProfile->incRefCount();

	if(mShowScrollBar)
	{
		mScrollBar->onWake();
	}

	return true;
}

void SceneWindow::onSleep()
{
	Parent::onSleep();

	if (mThumbProfile != NULL)
		mThumbProfile->decRefCount();

	if (mTrackProfile != NULL)
		mTrackProfile->decRefCount();

	if (mArrowProfile != NULL)
		mArrowProfile->decRefCount();

	if(mScrollBar->mAwake)
	{
		mScrollBar->onSleep();
	}
}

void SceneWindow::setUseConstantThumbHeight(const bool setting)
{
	if (setting == mUseConstantHeightThumb)
		return;

	mUseConstantHeightThumb = setting;
	mScrollBar->mUseConstantHeightThumb = setting;
} 

void SceneWindow::setScrollBarThickness(const S32 thickness)
{
	if (thickness == mScrollBarThickness)
		return;

	mScrollBarThickness = thickness;
	mScrollBar->mScrollBarThickness = thickness;
}

void SceneWindow::setShowArrowButtons(const bool setting)
{
	if (setting == mShowArrowButtons)
		return;

	mShowArrowButtons = setting;
	mScrollBar->mShowArrowButtons = setting;
}

void SceneWindow::setControlThumbProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "SceneWindow::setControlThumbProfile: invalid thumb profile");
	if (prof == mThumbProfile)
		return;
	if (mAwake && mThumbProfile->mRefCount > 0)
		mThumbProfile->decRefCount();
	mThumbProfile = prof;
	if (mAwake)
		mThumbProfile->incRefCount();

	mScrollBar->setControlThumbProfile(prof);
}

void SceneWindow::setControlTrackProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "SceneWindow::setControlTrackProfile: invalid track profile");
	if (prof == mTrackProfile)
		return;
	if (mAwake && mTrackProfile->mRefCount > 0)
		mTrackProfile->decRefCount();
	mTrackProfile = prof;
	if (mAwake)
		mTrackProfile->incRefCount();

	mScrollBar->setControlTrackProfile(prof);
}

void SceneWindow::setControlArrowProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "SceneWindow::setControlArrowProfile: invalid Arrow profile");
	if (prof == mArrowProfile)
		return;
	if (mAwake && mArrowProfile->mRefCount > 0)
		mArrowProfile->decRefCount();
	mArrowProfile = prof;
	if (mAwake)
		mArrowProfile->incRefCount();

	mScrollBar->setControlArrowProfile(prof);
}

//-----------------------------------------------------------------------------

void SceneWindow::setScene( Scene* pScene )
{
    // Detach (if needed)
    resetScene();

    // Attach the Window.
    pScene->attachSceneWindow( this );

    // Set scene.
    mpScene = pScene;
}

//-----------------------------------------------------------------------------

void SceneWindow::resetScene( void )
{
    // Detach from scene (if attached).
    if ( getScene() )
    {
        getScene()->detachSceneWindow( this );
    }

    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so dismount object.
        dismount();
    }

    // Clear input event watched objects.
    mInputEventWatching.clear();

    // Reset scene.
    mpScene = NULL;
}


//-----------------------------------------------------------------------------

void SceneWindow::setCameraPosition( const Vector2& position )
{
    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so cannot use this command.
        Con::warnf("SceneWindow::setCameraPosition() - Cannot use this command when camera is mounted!");
        return;
    }

    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Fetch the camera size.
    const Vector2 cameraSize = getCameraSize();

    // Set Camera Target.
    mCameraCurrent.mSourceArea = RectF( position.x - (cameraSize.x * 0.5f), position.y - (cameraSize.y * 0.5f), cameraSize.x, cameraSize.y );

    // Set Camera Target to Current.
    mCameraTarget = mCameraCurrent;

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::setCameraSize( const Vector2& size )
{
    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Fetch the current position.
    const Vector2 position = getCameraPosition();

    // Set Camera Target.
    mCameraCurrent.mSourceArea = RectF( position.x - (size.x * 0.5f), position.y - (size.y * 0.5f), size.x, size.y );

    // Set Camera Target to Current.
    mCameraTarget = mCameraCurrent;

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::setCameraArea( const RectF& cameraWindow )
{
    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so cannot use this command.
        Con::warnf("SceneWindow::setCameraArea - Cannot use this command when camera is mounted!");
        return;
    }

    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Target.
    mCameraCurrent.mSourceArea = cameraWindow;
    mCameraCurrent.mCameraZoom = 1.0f;
    mCameraCurrent.mCameraAngle = 0.0f;

    // Set Camera Target to Current.
    mCameraTarget = mCameraCurrent;

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::setCameraZoom( const F32 zoomFactor )
{
    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Target.
    mCameraCurrent.mCameraZoom = getMax( zoomFactor, 0.000001f  );

    // Set Camera Target to Current.
    mCameraTarget = mCameraCurrent;

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::setCameraAngle( const F32 cameraAngle )
{
    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Target.
    mCameraCurrent.mCameraAngle = mFmod( cameraAngle, b2_pi2 );

    // Set Camera Target to Current.
    mCameraTarget = mCameraCurrent;

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::setTargetCameraPosition( const Vector2& position )
{
    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so cannot use this command.
        Con::warnf("SceneWindow::setTargetCameraPosition - Cannot use this command when camera is mounted!");
        return;
    }

    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Fetch the camera size.
    const Vector2 cameraSize = getTargetCameraSize();

    // Set Camera Target.
    mCameraTarget.mSourceArea = RectF( position.x - (cameraSize.x*0.5f), position.y - (cameraSize.y*0.5f), cameraSize.x, cameraSize.y );

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::setTargetCameraSize( const Vector2& size )
{
    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Fetch the current position.
    const Vector2 position = getTargetCameraPosition();

    // Set Camera Target.
    mCameraTarget.mSourceArea = RectF( position.x - (size.x * 0.5f), position.y - (size.y * 0.5f), size.x, size.y );

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::setTargetCameraArea( const RectF& cameraWindow )
{
    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so cannot use this command.
        Con::warnf("SceneWindow::setTargetCameraArea - Cannot use this command when camera is mounted!");
        return;
    }

    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Target.
    mCameraTarget.mSourceArea = cameraWindow;

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::setTargetCameraZoom( const F32 zoomFactor )
{
    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Target.
    mCameraTarget.mCameraZoom = getMax( zoomFactor, 0.000001f );

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::setTargetCameraAngle( const F32 cameraAngle )
{
    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Target.
    mCameraTarget.mCameraAngle = mFmod( cameraAngle, b2_pi2 );

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::setCameraInterpolationTime( const F32 interpolationTime )
{
    // Set Interpolation Time.
    mCameraTransitionTime = interpolationTime;
}

//-----------------------------------------------------------------------------

void SceneWindow::setCameraInterpolationMode( const CameraInterpolationMode interpolationMode )
{
    // Set Interpolation Mode.
    mCameraInterpolationMode = interpolationMode;
}

//-----------------------------------------------------------------------------

void SceneWindow::setProcessAudioListener(bool mval)
{
    mProcessAudioListener = mval;
}

//-----------------------------------------------------------------------------

void SceneWindow::startCameraMove( const F32 interpolationTime )
{
    // Are we mounted to an object and trying to move?
    if ( isCameraMounted() )
    {
        if ( ( mCameraCurrent.mSourceArea.point  != mCameraTarget.mSourceArea.point ) ||
             ( mCameraCurrent.mSourceArea.extent != mCameraTarget.mSourceArea.extent ) )
        {
           // Yes, so cannot use this command.
           Con::warnf("SceneWindow::startCameraMove - Cannot use this command when camera is mounted!");
           return;
        }
    }

    // Stop move if we're at target already.
    if (    mCameraCurrent.mSourceArea.point  == mCameraTarget.mSourceArea.point &&
            mCameraCurrent.mSourceArea.extent == mCameraTarget.mSourceArea.extent &&
            mIsEqual( mCameraCurrent.mCameraZoom, mCameraTarget.mCameraZoom ) &&
            mIsEqual( mCameraTarget.mCameraAngle, mCameraTarget.mCameraAngle ) )
    {
        // Reset Camera Move.
        mMovingCamera = false;

        // Return here.
        return;
    }
    else
    {
        // Stop Camera Move ( if any ).
        if ( mMovingCamera ) stopCameraMove();
    }

    // Set Camera Interpolation Time.
    setCameraInterpolationTime( interpolationTime );

    // Zero Camera Time.
    zeroCameraTime();

    // Set Source Camera.
    mCameraSource = mCameraCurrent;

    // Set Camera Move.
    mMovingCamera = true;

    // Complete camera move if interpolate time is zero.
    if ( mIsZero(mCameraTransitionTime) ) completeCameraMove();

    // Queue Current Camera.
    mCameraQueue.push_back( mCameraCurrent );
    // Clamp Queue Size.
    if ( mCameraQueue.size() > mMaxQueueItems ) mCameraQueue.pop_front();

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::stopCameraMove( void )
{
    // Quit if we're not moving.
    if ( !mMovingCamera ) return;

    // Reset Tick Camera Time.
    resetTickCameraTime();

    // Set target to Current.
    mCameraTarget = mCameraCurrent;

    // Reset Camera Move.
    mMovingCamera = false;

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::completeCameraMove( void )
{
    // Quit if we're not moving.
    if ( !mMovingCamera ) return;

    // Reset Tick Camera Time.
    resetTickCameraTime();

    // Move straight to target.
    mCameraCurrent = mCameraTarget;

    // Reset Camera Move.
    mMovingCamera = false;

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::undoCameraMove( const F32 interpolationTime )
{
    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so cannot use this command.
        Con::warnf("SceneWindow::undoCameraMove - Cannot use this command when camera is mounted!");
        return;
    }

    // Quit if we've got no queued targets.
    if ( mCameraQueue.size() == 0 ) return;

    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Interpolation Time.
    setCameraInterpolationTime( interpolationTime );

    // Zero Camera Time.
    zeroCameraTime();

    // Set Source Camera.
    mCameraSource = mCameraCurrent;

    // Set Camera Move.
    mMovingCamera = true;

    // Fetch Last Queued Camera Target.
    mCameraTarget = mCameraQueue.last();
    // Remove Last Target.
    mCameraQueue.pop_back();

    // Complete camera move if interpolate time is zero.
    if ( mIsZero(mCameraTransitionTime) ) completeCameraMove();

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::updateCamera( void )
{
    // Calculate Normalised Time.
    const F32 normCameraTime = mRenderCameraTime / mCameraTransitionTime;

    // Have we finished the interpolation?
    if ( mGreaterThanOrEqual(normCameraTime, 1.0f) )
    {
        // Yes, so complete camera move.
        completeCameraMove();
        // Finish here.
        return;
    }

    // Interpolate Camera Window/Zoom.
    mCameraCurrent.mSourceArea.point.x    = interpolate( mCameraSource.mSourceArea.point.x, mCameraTarget.mSourceArea.point.x, normCameraTime );
    mCameraCurrent.mSourceArea.point.y    = interpolate( mCameraSource.mSourceArea.point.y, mCameraTarget.mSourceArea.point.y, normCameraTime );
    mCameraCurrent.mSourceArea.extent.x   = interpolate( mCameraSource.mSourceArea.extent.x, mCameraTarget.mSourceArea.extent.x, normCameraTime );
    mCameraCurrent.mSourceArea.extent.y   = interpolate( mCameraSource.mSourceArea.extent.y, mCameraTarget.mSourceArea.extent.y, normCameraTime );
    mCameraCurrent.mCameraZoom            = interpolate( mCameraSource.mCameraZoom, mCameraTarget.mCameraZoom, normCameraTime );
    mCameraCurrent.mCameraAngle           = interpolate( mCameraSource.mCameraAngle, mCameraTarget.mCameraAngle, normCameraTime );

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

F32 SceneWindow::interpolate( F32 from, F32 to, F32 delta )
{
    // Linear.
    if ( mCameraInterpolationMode == LINEAR )
        return mLerp( from, to, delta );
    // Sigmoid.
    else if ( mCameraInterpolationMode == SIGMOID )
        return mSmoothStep( from, to, delta );
    // Hmmm...
    else
        return from;
}


//-----------------------------------------------------------------------------

void SceneWindow::startCameraShake( const F32 magnitude, const F32 time )
{
    // Is the time zero?
    if ( mIsZero( time ) && mIsZero( magnitude ) )
    {
        // Yes, so simply stop the camera shaking.
        stopCameraShake();

        // Finish here.
        return;
    }

    // Set Current Shake.
    mCurrentShake = mFabs(magnitude);

    // Set Shake Life.
    mShakeLife = time;

    // Calculate Shake Ramp.
    mShakeRamp = mCurrentShake / mShakeLife;

    // Flag camera shaking.
    mCameraShaking = true;
}

//-----------------------------------------------------------------------------

void SceneWindow::stopCameraShake( void )
{
    // Flag camera not shaking.
    mCameraShaking = false;

    // Reset Shake Offset.
    mCameraShakeOffset.setZero();
}

//-----------------------------------------------------------------------------

void SceneWindow::mount( SceneObject* pSceneObject, const Vector2& mountOffset, const F32 mountForce, const bool sendToMount, const bool mountAngle )
{
    // Sanity!
    AssertFatal( pSceneObject != NULL, "Scene object cannot be NULL." );

    // Cannot mount if not in a scene.
    if ( !mpScene )
    {
        // Warn!
        Con::warnf("Cannot mount scene window (%d) to a scene object (%d) if scene window is not attached to a scene.", getId(), pSceneObject->getId() );
        return;
    }

    // Fetch objects' scene.
    const Scene* pScene = pSceneObject->getScene();

    // Scene object must be in a scene.
    if ( !pScene )
    {
        // Warn!
        Con::warnf("Cannot mount scene window (%d) to a scene object (%d) that is not in a scene.", getId(), pSceneObject->getId() );
        return;
    }

    // Scene object must be in same scene as the one the scene window is attached to.
    if ( pScene != mpScene )
    {
        // Warn!
        Con::warnf("Cannot mount scene window (%d) to a scene object (%d) that are not using the same scene.", getId(), pSceneObject->getId() );
        return;
    }

    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so dismount object.
        dismount();
    }
    else
    {
        // No, so stop any Camera Move.
        if ( mMovingCamera ) stopCameraMove();
    }

	//Are we using scroll bars? If so that's done now.
	mShowScrollBar = false;

    // Set Mount Object Reference.
    mpMountedTo = pSceneObject;

    // Store Mount Offset.
    mMountOffset = mountOffset;

    // Set Mount Force.
    mMountForce = mountForce;

    // Set Mount Angle.
    mMountAngle = mountAngle;

    // Add Camera Mount Reference.
    pSceneObject->addCameraMountReference( this );

    // Flag Camera mounted.
    mCameraMounted = true;

    // Send directly to mount (if selected).
    if ( sendToMount )
    {
        // Fetch Mount Position.
        const Vector2& mountPos = mpMountedTo->getBody()->GetWorldPoint( mountOffset );

        // Calculate Window Half-Dimensions.
        const F32 halfWidth = mCameraCurrent.mSourceArea.len_x() * 0.5f;
        const F32 halfHeight = mCameraCurrent.mSourceArea.len_y() * 0.5f;

        // Set Current View to Object Position.
        mCameraCurrent.mSourceArea.point.set( mountPos.x - halfWidth, mountPos.y - halfHeight );
    }

    // Reset Tick Camera Position.
    resetTickCameraPosition();
}

//-----------------------------------------------------------------------------

void SceneWindow::dismount( void )
{
    // Nothing to do if we're not mounted!
    if (!isCameraMounted() )
        return;

    // Remove Camera Mount Reference.
    mpMountedTo->removeCameraMountReference();

    // Reset Camera Object Mount.
    mpMountedTo = NULL;

    // Flag Camera not mounted.
    mCameraMounted = false;

    // Reset Tick Camera Position.
    resetTickCameraPosition();
}

//-----------------------------------------------------------------------------

void SceneWindow::dismountMe(SceneObject* pSceneObject)
{
	// Are we mounted to the specified object?
	if (isCameraMounted() && pSceneObject != mpMountedTo)
	{
		// No, so warn.
		Con::warnf("SceneWindow::dismountMe() - Object is not mounted by the camera!");
		return;
	}

	// Dismount Object.
	dismount();
}

//-----------------------------------------------------------------------------

void SceneWindow::setViewLimitOn( const Vector2& limitMin, const Vector2& limitMax )
{
    // Activate View Limit.
    mViewLimitActive = true;

    // Set View Limit Min/Max.
    mViewLimitMin = limitMin;
    mViewLimitMax = limitMax;

    // Calculate Camera Area.
    mViewLimitArea = mViewLimitMax - mViewLimitMin;

	//Update the Scroll Bar
	updateScrollBar();
}

//-----------------------------------------------------------------------------

void SceneWindow::clampCameraViewLimit( void )
{
    // Finish if the view-limit is not on or the camera is moving.
    if ( !mViewLimitActive || mMovingCamera )
        return;

    // Calculate Current Camera View.
    calculateCameraView( &mCameraCurrent );

    // Calculate the source and destination centres.
    const Point2F sourceCentre = mCameraCurrent.mSourceArea.centre();
    const Point2F destinationCentre = mCameraCurrent.mDestinationArea.centre();

    // Are the source and destination areas the same?
    if ( sourceCentre != destinationCentre )
    {
        // No, so adjust the source position to be at the same position as the destination i.e. don't change the source area.
        mCameraCurrent.mSourceArea.point += destinationCentre - sourceCentre;
    }
}

//-----------------------------------------------------------------------------

void SceneWindow::setShowScrollBar(bool setting)
{
	//Warn if we don't have a view limit
	if(setting && !mViewLimitActive)
	{
		Con::warnf("SceneWindow::setShowScrollBar - View Limit must be turned off for scroll bars to appear!");
	}

	// Unmount the camera if it is mounted
	if(setting && isCameraMounted())
	{
		dismount();
	}

	mShowScrollBar = setting;

	if(setting)
	{
		//now wake the scrollbar
		if(mAwake)
		{
			mScrollBar->onWake();
		}

		updateScrollBar();
	}
}

void SceneWindow::updateScrollBar()
{
	if (mShowScrollBar)
	{
		mScrollBar->computeSizes();
	}
}

//-----------------------------------------------------------------------------

void SceneWindow::setObjectInputEventFilter( const U32 groupMask, const U32 layerMask, const bool useInvisible )
{
    // Set Object Mouse Event Filter.
    mInputEventGroupMaskFilter = groupMask;
    mInputEventLayerMaskFilter = layerMask;
    mInputEventInvisibleFilter = useInvisible;
}

//-----------------------------------------------------------------------------

void SceneWindow::setObjectInputEventGroupFilter( const U32 groupMask )
{
    mInputEventGroupMaskFilter = groupMask;

    // Clear existing watched input events.
    clearWatchedInputEvents();
}

//-----------------------------------------------------------------------------

void SceneWindow::setObjectInputEventLayerFilter( const U32 layerMask )
{
    mInputEventLayerMaskFilter = layerMask;

    // Clear existing watched input events.
    clearWatchedInputEvents();
}

//-----------------------------------------------------------------------------

void SceneWindow::setObjectInputEventInvisibleFilter( const bool useInvisible )
{
    mInputEventInvisibleFilter = useInvisible;

    // Clear existing watched input events.
    clearWatchedInputEvents();
}

//-----------------------------------------------------------------------------

void SceneWindow::addInputListener( SimObject* pSimObject )
{
    // Sanity!
    AssertFatal( pSimObject != NULL, "SceneWindow::addInputEventListener() - Cannot add NULL object as input event listener." );

    // Ignore if the object is already a listener.
    if ( mInputListeners.find( pSimObject ) != mInputListeners.end() )
        return;

    // Add as listener.
    mInputListeners.addObject( pSimObject );
}

//-----------------------------------------------------------------------------

void SceneWindow::removeInputListener( SimObject* pSimObject )
{
    mInputListeners.removeObject( pSimObject );
}

//-----------------------------------------------------------------------------

void SceneWindow::setMousePosition( const Vector2& mousePosition )
{
    // Fetch Canvas.
    GuiCanvas* pCanvas = getRoot();

    // Canvas available?
    if ( pCanvas )
    {
        // Are we bound to a scene?
        if ( getScene() )
        {
            Vector2 windowMousePosition;

            // Yes, so convert window into scene coordinates...
            sceneToWindowPoint( mousePosition, windowMousePosition );

            // Copy into a compatible format for TGE.
            Point2I localWindowPosition( S32(windowMousePosition.x), S32(windowMousePosition.y) );

            // Set Cursor Position.
            pCanvas->setCursorPos( localToGlobalCoord(localWindowPosition) );
        }
        else
        {
            // No, so error.
            Con::warnf("SceneObject::setMousePosition() - No scene attached to window!");
            return;
        }
    }
}

//-----------------------------------------------------------------------------

Vector2 SceneWindow::getMousePosition( void )
{
    // Calculate Current Camera View.
    calculateCameraView( &mCameraCurrent );

    // Fetch Canvas.
    GuiCanvas* pCanvas = getRoot();

    // World Mouse Position.
    Vector2 worldMousePoint(0, 0);

    // Canvas available?
    if ( pCanvas )
    {
        // Yes, so fetch local GUI coordinates.
        const Vector2 localGuiPoint = globalToLocalCoord( pCanvas->getCursorPos() );

        // Are we bound to a scene?
        if ( getScene() )
        {
            // Yes, so convert window into scene coordinates...
            windowToScenePoint(localGuiPoint, worldMousePoint);
        }
        else
        {
            // No, so use window screen coordinates.
            worldMousePoint = localGuiPoint;
        }
    }
#if 0
    else
    {
        //  No, so warn
        Con::warnf("SceneWindow::getMousePosition() - Window not attached to canvas!" );
    }
#endif
    // Return World Mouse Position.
    return worldMousePoint;
}

//-----------------------------------------------------------------------------

void SceneWindow::windowToScenePoint( const Vector2& srcPoint, Vector2& dstPoint ) const
{
    // Return Conversion.
    dstPoint.Set( (srcPoint.x * mCameraCurrent.mSceneWindowScale.x) + mCameraCurrent.mSceneMin.x, mCameraCurrent.mSceneMax.y - (srcPoint.y * mCameraCurrent.mSceneWindowScale.y) );
}

//-----------------------------------------------------------------------------

void SceneWindow::sceneToWindowPoint( const Vector2& srcPoint, Vector2& dstPoint ) const
{
    // Return Conversion.
    dstPoint.Set( (srcPoint.x - mCameraCurrent.mSceneMin.x) / mCameraCurrent.mSceneWindowScale.x, (mCameraCurrent.mSceneMax.y - srcPoint.y) / mCameraCurrent.mSceneWindowScale.y );
}

//-----------------------------------------------------------------------------

void SceneWindow::dispatchInputEvent( StringTableEntry name, const GuiEvent& event )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneWindow_DispatchInputEvent);

    // Dispatch input event to window if appropriate.
    if ( getUseWindowInputEvents() )
        sendWindowInputEvent( name, event );

    // Dispatch input event to scene objects if appropriate.
    if ( getUseObjectInputEvents() )
        sendObjectInputEvent( name, event );
}

//-----------------------------------------------------------------------------

void SceneWindow::sendWindowInputEvent( StringTableEntry name, const GuiEvent& event )
{       
    // Debug Profiling.
    PROFILE_SCOPE(SceneWindow_SendWindowInputEvent);

    Vector2   worldMousePoint;

    // Calculate Current Camera View.
    calculateCameraView( &mCameraCurrent );

    // Convert to local gui coordinates.
    const Vector2 localGuiPoint = globalToLocalCoord(event.mousePoint);

    // Are we bound to a scene?
    if ( getScene() )
    {
        // Yes, so convert window into scene coordinates...
        windowToScenePoint(localGuiPoint, worldMousePoint);
    }
    else
    {
        // No, so use window screen coordinates.
        worldMousePoint = localGuiPoint;
    }


    // Argument Buffers.
    char argBuffer[3][64];

     // Format Event-Modifier Buffer.
    dSprintf(argBuffer[0], 64, "%d", event.eventID);

    // Format Mouse-Position Buffer.
    dSprintf(argBuffer[1], 64, "%g %g", worldMousePoint.x, worldMousePoint.y);

    // Format Mouse-Click Count Buffer.
    dSprintf(argBuffer[2], 64, "%d", event.mouseClickCount);

    // Call Scripts.
    Con::executef(this, 4, name, argBuffer[0], argBuffer[1], argBuffer[2]);

    // Iterate listeners.
    for( SimSet::iterator listenerItr = mInputListeners.begin(); listenerItr != mInputListeners.end(); ++listenerItr )
    {
        // Call scripts on listener.
        Con::executef( *listenerItr, 4, name, argBuffer[0], argBuffer[1], argBuffer[2] );
    }
}

//-----------------------------------------------------------------------------

void SceneWindow::sendObjectInputEvent( StringTableEntry name, const GuiEvent& event )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneWindow_SendObjectInputEvent);

    // Finish if we're not bound to a scene?
    if ( !getScene() ) return;

    // Only process appropriate input events.
    if ( !( name == inputEventDownName ||
            name == inputEventUpName ||
            name == inputEventMovedName ||
            name == inputEventDraggedName ) )
        return;

    // Convert Event-Position into scene coordinates.
    Vector2 worldMousePoint;
    windowToScenePoint(Vector2(globalToLocalCoord(event.mousePoint)), worldMousePoint);

    // Fetch old pick count.
    const U32 oldPickCount = (U32)mInputEventWatching.size();

    // Fetch world query and clear results.
    WorldQuery* pWorldQuery = getScene()->getWorldQuery( true );

    // Set filter.
    WorldQueryFilter queryFilter( mInputEventLayerMaskFilter, mInputEventGroupMaskFilter, true, mInputEventInvisibleFilter, true, true );
    pWorldQuery->setQueryFilter( queryFilter );

    // Perform world query.
    const U32 newPickCount = pWorldQuery->anyQueryPoint( worldMousePoint );

    // Early-out if nothing to do.
    if ( newPickCount == 0 && oldPickCount == 0 )
        return;

    // Fetch results.
    mInputEventQuery = pWorldQuery->getQueryResults();
    pWorldQuery->clearQuery();

    // Determine "enter" events.
    for( U32 newIndex = 0; newIndex < newPickCount; ++newIndex )
    {
        // Fetch new scene object.
        SceneObject* pNewSceneObject = mInputEventQuery[newIndex].mpSceneObject;

        // Ignore object if it's not using input events.
        // NOTE:-   We only check this for "enter" events in-case the option is
        //          changed whilst it's currently picked.  We want to guarantee
        //          that any "enter" event is paired with a "leave" event.
        if ( !pNewSceneObject->getUseInputEvents() )
            continue;

        bool alreadyPresent = false;

        for ( U32 oldIndex = 0; oldIndex < oldPickCount; ++oldIndex )
        {
            // Skip if scene object is not present...
            if ( mInputEventWatching[oldIndex] != pNewSceneObject )
                continue;

            // Flag as already present.
            alreadyPresent = true;
            break;
        }

        // Add scene object as entering if not already present.
        if ( !alreadyPresent )
            mInputEventEntering.push_back( pNewSceneObject );
    }

    // Determine "leave" events.
    for ( U32 oldIndex = 0; oldIndex < oldPickCount; ++oldIndex )
    {
        // Fetch old scene object.
        SceneObject* pOldSceneObject = dynamic_cast<SceneObject*>( mInputEventWatching[oldIndex] );

        // Sanity!
        AssertFatal( pOldSceneObject != NULL, "Invalid object found in mouse-event pick vector." );

        bool stillPresent = false;

        for( U32 newIndex = 0; newIndex < newPickCount; ++newIndex )
        {
            // Skip if scene object is not present.
            if ( mInputEventQuery[newIndex].mpSceneObject != pOldSceneObject )
                continue;

            // Flag as still present.
            stillPresent = true;
            break;
        }

        // Add scene object as leaving if not still present.
        if ( !stillPresent )
            mInputEventLeaving.push_back( pOldSceneObject );
    }

    for ( U32 index = 0; index < (U32)mInputEventQuery.size(); ++index )
    {
        // Fetch scene object.
        SceneObject* pSceneObject = mInputEventQuery[index].mpSceneObject;

        // Ignore object if it's not using input events.
        if ( !pSceneObject->getUseInputEvents() )
            continue;

        // Emit event.
        pSceneObject->onInputEvent( name, event, worldMousePoint );
    }

    // Process "leave" events.
    for ( U32 index = 0; index < (U32)mInputEventLeaving.size(); ++index )
    {
        // Fetch scene object.
        SceneObject* pSceneObject = mInputEventLeaving[index];

        // Emit event.
        pSceneObject->onInputEvent( inputEventLeaveName, event, worldMousePoint );

        // Remove scene object.
        mInputEventWatching.removeObject( pSceneObject );
    }

    // Process "enter" events.
    for ( U32 index = 0; index < (U32)mInputEventEntering.size(); ++index )
    {
        // Fetch scene object.
        SceneObject* pSceneObject = mInputEventEntering[index];

        // Emit event.
        pSceneObject->onInputEvent( inputEventEnterName, event, worldMousePoint );

        // Process "moved" or "dragged" events.
        if ( name == inputEventMovedName || name == inputEventDraggedName )
            pSceneObject->onInputEvent( name, event, worldMousePoint );

        // Add scene object.
        mInputEventWatching.addObject( pSceneObject );
    }

    // Clear input event vectors.
    mInputEventQuery.clear();
    mInputEventEntering.clear();
    mInputEventLeaving.clear();
}

//-----------------------------------------------------------------------------

void SceneWindow::onTouchEnter( const GuiEvent& event )
{
    // Dispatch input event.
    dispatchInputEvent(mouseEventEnterName, event);
}

//-----------------------------------------------------------------------------

void SceneWindow::onTouchLeave( const GuiEvent& event )
{
	if (mShowScrollBar)
	{
		mScrollBar->onTouchLeave(event);
	}

    // Dispatch input event.
    dispatchInputEvent(mouseEventLeaveName, event);
}

//-----------------------------------------------------------------------------

void SceneWindow::onTouchDown( const GuiEvent& event )
{
	if (mShowScrollBar)
	{
		mScrollBar->curHitRegion = mScrollBar->findHitRegion(mScrollBar->globalToLocalCoord(event.mousePoint));
		if (mScrollBar->curHitRegion != GuiScrollCtrl::None)
		{
			setUpdate();
			mScrollBar->onTouchDown(event);
			return;
		}
	}

    // Lock Mouse (if necessary).
    if(mLockMouse)
        mouseLock();

    // Dispatch input event.
    dispatchInputEvent( inputEventDownName, event);
}

//-----------------------------------------------------------------------------

void SceneWindow::onTouchUp( const GuiEvent& event )
{
    // Lock Mouse (if necessary).
    if(mLockMouse)
        mouseUnlock();

    // Dispatch input event.
    dispatchInputEvent(inputEventUpName, event);
}

//-----------------------------------------------------------------------------

void SceneWindow::onTouchMove( const GuiEvent& event )
{
	if (mShowScrollBar)
	{
		mScrollBar->onTouchMove(event);
	}

    // Dispatch input event.
    dispatchInputEvent(inputEventMovedName, event);
}

//-----------------------------------------------------------------------------

void SceneWindow::onTouchDragged( const GuiEvent& event )
{
    // Dispatch input event.
    dispatchInputEvent(inputEventDraggedName, event);
}

//-----------------------------------------------------------------------------

void SceneWindow::onMiddleMouseDown( const GuiEvent& event )
{
    // Lock Mouse (if necessary).
    if(mLockMouse)
        mouseLock();

    // Dispatch input event.
    dispatchInputEvent(mouseEventMiddleMouseDownName, event);
}

//-----------------------------------------------------------------------------

void SceneWindow::onMiddleMouseUp( const GuiEvent& event )
{
    // Lock Mouse (if necessary).
    if(mLockMouse)
        mouseUnlock();

    // Dispatch input event.
    dispatchInputEvent(mouseEventMiddleMouseUpName, event);
}

//-----------------------------------------------------------------------------

void SceneWindow::onMiddleMouseDragged( const GuiEvent& event )
{
    // Dispatch input event.
    dispatchInputEvent(mouseEventMiddleMouseDraggedName, event);
}

//-----------------------------------------------------------------------------

void SceneWindow::onRightMouseDown( const GuiEvent& event )
{
    // Lock Mouse (if necessary).
    if(mLockMouse)
        mouseLock();

    // Dispatch input event.
    dispatchInputEvent(mouseEventRightMouseDownName, event);
}

//-----------------------------------------------------------------------------

void SceneWindow::onRightMouseUp( const GuiEvent& event )
{
    // Lock Mouse (if necessary).
    if(mLockMouse)
        mouseUnlock();

    // Dispatch input event.
    dispatchInputEvent(mouseEventRightMouseUpName, event);
}

//-----------------------------------------------------------------------------

void SceneWindow::onRightMouseDragged( const GuiEvent& event )
{
    // Dispatch input event.
    dispatchInputEvent(mouseEventRightMouseDraggedName, event);
}

//-----------------------------------------------------------------------------

bool SceneWindow::onMouseWheelUp( const GuiEvent& event )
{
	if (mShowScrollBar && ((mMouseWheelScrolls && !(event.modifier & SI_SHIFT)) || (!mMouseWheelScrolls && (event.modifier & SI_SHIFT))))
	{
		mScrollBar->onMouseWheelUp(event);
		return true;
	}

   // Call Parent.
   Parent::onMouseWheelUp( event );

   // Dispatch input event.
   dispatchInputEvent(mouseEventWheelUpName, event);

   // Return Success.
   return true;
}

//-----------------------------------------------------------------------------

bool SceneWindow::onMouseWheelDown( const GuiEvent& event )
{
	if (mShowScrollBar && ((mMouseWheelScrolls && !(event.modifier & SI_SHIFT)) || (!mMouseWheelScrolls && (event.modifier & SI_SHIFT))))
	{
		mScrollBar->onMouseWheelDown(event);
		return true;
	}

   // Call Parent.
   Parent::onMouseWheelDown( event );

   // Dispatch input event.
   dispatchInputEvent(mouseEventWheelDownName, event);

   // Return Success.
   return true;
}

//-----------------------------------------------------------------------------

void SceneWindow::calculateCameraMount( const F32 elapsedTime )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneWindow_CalculateCameraMount);

    // Fetch Mount Position.
    const Vector2& mountPos = mpMountedTo->getBody()->GetWorldPoint( mMountOffset );

    // Set Pre-Tick Position.
    mPreTickPosition = mPostTickPosition;

    // Set Current Camera Position.
    mCameraCurrent.mSourceArea.point = mPreTickPosition;

    // Calculate Window Half-Dimensions.
    const F32 halfWidth = mCameraCurrent.mSourceArea.len_x() * 0.5f;
    const F32 halfHeight = mCameraCurrent.mSourceArea.len_y() * 0.5f;

    // Calculate Target Position.
    const Point2F targetPos = Point2F( mountPos.x - halfWidth, mountPos.y - halfHeight );

    // Mount the angle?
    if ( mMountAngle )
        mCameraCurrent.mCameraAngle = -mpMountedTo->getAngle();

    // Rigid Mount?
    if ( mIsZero( mMountForce ) )
    {
        // Yes, so easy post-tick position.
        mPostTickPosition = targetPos;
        return;
    }

    // Calculate Time/Force Product.
    const F32 timeForce = elapsedTime * mMountForce;

    // Will we exceed our step?
    if ( timeForce > 1.0f )
    {
        // Yes, so clamp at step.
        mPostTickPosition = targetPos;
        return;
    }
    else
    {
        // No, so calculate Direction to move.
        const Point2F direction = (targetPos - mPreTickPosition) * timeForce;

        // Calculate post-tick position.
        mPostTickPosition = mPreTickPosition + direction;
        return;
    }
}

//-----------------------------------------------------------------------------

void SceneWindow::calculateCameraView( CameraView* pCameraView )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneWindow_CalculateCameraView);

    // Calculate Zoom Reciprocal.
    const F32 zoomRecip = pCameraView->mCameraZoom > 0.0f ? 1.0f / pCameraView->mCameraZoom : pCameraView->mCameraZoom;

    // Calculate Zoom X/Y Factors.
    const F32 zoomFactorX = (pCameraView->mSourceArea.len_x() - (pCameraView->mSourceArea.len_x() * zoomRecip))/2;
    const F32 zoomFactorY = (pCameraView->mSourceArea.len_y() - (pCameraView->mSourceArea.len_y() * zoomRecip))/2;

    // Fetch Camera View.
    pCameraView->mDestinationArea = pCameraView->mSourceArea;

    // Inset Window by Zoom Factor (if it's big enough to do so).
    if (    pCameraView->mDestinationArea.extent.x > (zoomFactorX*2.0f) &&
            pCameraView->mDestinationArea.extent.y > (zoomFactorY*2.0f) )
    {
        pCameraView->mDestinationArea.inset( zoomFactorX, zoomFactorY );
    }
        
    // Ensure we've got a valid window.
    if ( !pCameraView->mDestinationArea.isValidRect() )
        // Make it real!
        pCameraView->mDestinationArea.extent = Point2F(1,1);

    // Calculate Scene Min/Max.
    pCameraView->mSceneMin.x = pCameraView->mDestinationArea.point.x;
    pCameraView->mSceneMin.y = pCameraView->mDestinationArea.point.y;
    pCameraView->mSceneMax.x = pCameraView->mDestinationArea.point.x + pCameraView->mDestinationArea.len_x();
    pCameraView->mSceneMax.y = pCameraView->mDestinationArea.point.y + pCameraView->mDestinationArea.len_y();

    // Is View Limit Active?
    if ( mViewLimitActive )
    {
        // Yes, so is the limit area X less than the current view X?
        if ( mViewLimitArea.x < pCameraView->mDestinationArea.len_x() )
        {
            // Yes, so calculate center of view.
            const F32 viewCenterX = mViewLimitMin.x + ( mViewLimitArea.x * 0.5f );

            // Half Camera Width.
            const F32 halfCameraX = pCameraView->mDestinationArea.len_x() * 0.5f;

            // Calculate Min/Max X.
            pCameraView->mSceneMin.x = viewCenterX - halfCameraX;
            pCameraView->mSceneMax.x = viewCenterX + halfCameraX;
        }
        else
        {
            // No, so calculate window min overlap.
            const F32 windowMinOverlapX = getMax(0.0f, mViewLimitMin.x - pCameraView->mSceneMin.x);

            // Calculate window max overlap.
            const F32 windowMaxOverlapX = getMin(0.0f, mViewLimitMax.x - pCameraView->mSceneMax.x);

            // Adjust Window.
            pCameraView->mSceneMin.x += windowMinOverlapX + windowMaxOverlapX;
            pCameraView->mSceneMax.x += windowMinOverlapX + windowMaxOverlapX;
        }

        // Is the limit area Y less than the current view Y?
        if ( mViewLimitArea.y < pCameraView->mDestinationArea.len_y() )
        {
            // Yes, so calculate center of view.
            const F32 viewCenterY = mViewLimitMin.y + ( mViewLimitArea.y * 0.5f );

            // Half Camera Height.
            const F32 halfCameraY = pCameraView->mDestinationArea.len_y() * 0.5f;

            // Calculate Min/Max Y.
            pCameraView->mSceneMin.y = viewCenterY - halfCameraY;
            pCameraView->mSceneMax.y = viewCenterY + halfCameraY;
        }
        else
        {
            // No, so calculate window min overlap.
            const F32 windowMinOverlapY = getMax(0.0f, mViewLimitMin.y - pCameraView->mSceneMin.y);

            // Calculate window max overlap.
            const F32 windowMaxOverlapY = getMin(0.0f, mViewLimitMax.y - pCameraView->mSceneMax.y);

            // Adjust Window.
            pCameraView->mSceneMin.y += windowMinOverlapY + windowMaxOverlapY;
            pCameraView->mSceneMax.y += windowMinOverlapY + windowMaxOverlapY;
        }

        // Recalculate destination area.
        pCameraView->mDestinationArea.point  = pCameraView->mSceneMin;
        pCameraView->mDestinationArea.extent = pCameraView->mSceneMax - pCameraView->mSceneMin;
    }

    // Calculate Scene Window Scale.
    pCameraView->mSceneWindowScale.x = (pCameraView->mSceneMax.x - pCameraView->mSceneMin.x) / mBounds.len_x();
    pCameraView->mSceneWindowScale.y = (pCameraView->mSceneMax.y - pCameraView->mSceneMin.y) / mBounds.len_y();
}

//-----------------------------------------------------------------------------

void SceneWindow::resize(const Point2I &newPosition, const Point2I &newExtent)
{
    // Resize Parent.
    Parent::resize( newPosition, newExtent);

	//Update the Scroll Bar
	updateScrollBar();

    // Argument Buffer.
    char argBuffer[64];

    // Format Buffer.
    dSprintf( argBuffer, 64, "%d %d %d %d", newPosition.x, newPosition.y, newExtent.x, newExtent.y );

    // Resize Callback.
	if(this->getNamespace())
		Con::executef( this, 2, "onExtentChange", argBuffer );
}

//-----------------------------------------------------------------------------

void SceneWindow::processTick( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneWindow_ProcessTick);

    Point2F PreMove_CameraPos = mCameraCurrent.mSourceArea.centre();
    F32 PreMove_CameraZoom = mCameraCurrent.mCameraZoom;
    // Are we moving the camera.
    if ( mMovingCamera )
    {
        // Yes, so add Elapsed Time (scaled appropriately).
        mCurrentCameraTime += Tickable::smTickSec;

        // Update Tick Camera Time.
        updateTickCameraTime();

        // Update Camera.
        updateCamera();
    } 

    // Is the Camera Shaking?
    if ( mCameraShaking )
    {
        // Reduce Shake Life.
        mShakeLife -= Tickable::smTickSec;

        // Is the Shake still active?
        if ( mShakeLife > 0.0f )
        {
            // Calculate Current Shake.
            mCurrentShake -= mShakeRamp * Tickable::smTickSec;

            // Clamp Shake.
            mCurrentShake = getMax(mCurrentShake, 0.0f);

            // Calculate the Screen Shake-Ratio.
            const Point2F shakeRatio( mCameraCurrent.mDestinationArea.len_x() / F32(mBounds.len_x()), mCameraCurrent.mDestinationArea.len_y() / F32(mBounds.len_y()) );

            // Calculate the Camera Shake Magnitude based upon the Screen Shake-Ratio.
            const F32 shakeMagnitudeX = mCurrentShake * shakeRatio.x * 0.5f;
            const F32 shakeMagnitudeY = mCurrentShake * shakeRatio.y * 0.5f;

            // Choose a random Shake.
            mCameraShakeOffset.Set( CoreMath::mGetRandomF( -shakeMagnitudeX, shakeMagnitudeX ), CoreMath::mGetRandomF( -shakeMagnitudeY, shakeMagnitudeY ) );
        }
        else
        {
            // No, so stop shake.
            stopCameraShake();
        }
    }
    if (mProcessAudioListener)
    {
        F32 listenervelocity[] = { 0.f, 0.f, 0.f };

        Point2F campos = mCameraCurrent.mSourceArea.centre();

        if (campos != PreMove_CameraPos)
        {
            listenervelocity[0] = (campos.x - PreMove_CameraPos.x) * Tickable::smTickSec;
            listenervelocity[1] = (campos.y - PreMove_CameraPos.y) * Tickable::smTickSec;
            listenervelocity[2] = (mCameraCurrent.mCameraZoom - PreMove_CameraZoom) * Tickable::smTickSec;
        }

        F32 listenerpos[] = { campos.x, campos.y, 5.f };

        alListenerfv(AL_POSITION, listenerpos);
        alListenerfv(AL_VELOCITY, listenervelocity);
    }
}

//-----------------------------------------------------------------------------

void SceneWindow::interpolateTick( F32 timeDelta )
{
    // Are we moving the camera.
    if ( mMovingCamera )
    {
        // Calculate Render Tick Position.
        mRenderCameraTime = (mPreCameraTime * timeDelta) + ((1.0f-timeDelta) * mPostCameraTime);

        // Update Camera.
        updateCamera();
    }
}

//-----------------------------------------------------------------------------

void SceneWindow::interpolateCameraMount( const F32 timeDelta )
{
    // Camera Mounted?
    if ( !isCameraMounted() ) return;

    // Calculate position.
    mCameraCurrent.mSourceArea.point = (mPreTickPosition * timeDelta) + ((1.0f-timeDelta) * mPostTickPosition);  
}

//-----------------------------------------------------------------------------

void SceneWindow::zeroCameraTime( void )
{
    // Reset Camera Time.
    mRenderCameraTime = mPreCameraTime = mPostCameraTime = mCurrentCameraTime = 0.0f;
}

//-----------------------------------------------------------------------------

void SceneWindow::resetTickCameraTime( void )
{
    // Reset Camera Time.
    mRenderCameraTime = mPreCameraTime = mPostCameraTime = mCurrentCameraTime;
}

//-----------------------------------------------------------------------------

void SceneWindow::updateTickCameraTime( void )
{
    // Store Pre Camera Time.
    mPreCameraTime = mPostCameraTime;

    // Store Current Camera Time.
    mPostCameraTime = mCurrentCameraTime;

    // Render Camera Time is at Pre-Tick Time.
    mRenderCameraTime = mPreCameraTime;
}

//-----------------------------------------------------------------------------

void SceneWindow::resetTickCameraPosition( void )
{
    mPreTickPosition = mPostTickPosition = mCameraCurrent.mSourceArea.point;
}

//-----------------------------------------------------------------------------

void SceneWindow::onRender( Point2I offset, const RectI& updateRect )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneWindow_onRender);

	//save the old clip
	RectI oldClipRect = dglGetClipRect();

	//clip to the updateRect
	dglSetClipRect(updateRect);

    // Fetch scene.
    Scene* pScene = getScene();

    // Cannot render without scene!
    if ( !pScene )
        return;

    // Calculate current camera View ( if needed ).
    calculateCameraView( &mCameraCurrent );

    // Fetch current camera.
    const Point2F& sceneWindowScale = mCameraCurrent.mSceneWindowScale;
    Point2F sceneMin = mCameraCurrent.mSceneMin;
    Point2F sceneMax = mCameraCurrent.mSceneMax;

    // Fetch bounds.
    const RectI& bounds = getBounds();  
    const Point2I globalTopLeft( updateRect.point.x, updateRect.point.y );
    const Point2I globalBottomRight( updateRect.point.x + updateRect.extent.x, updateRect.point.y + updateRect.extent.y );
    const Point2I localTopLeft = globalToLocalCoord( globalTopLeft );
    const Point2I localBottomRight = globalToLocalCoord( globalBottomRight );

    // Clip top?
    if ( localTopLeft.y > 0 )
    {
        sceneMax.y -= localTopLeft.y * sceneWindowScale.y;
    }

    // Clip left?
    if ( localTopLeft.x > 0 )
    {
        sceneMin.x += localTopLeft.x * sceneWindowScale.x;
    }

    // Clip bottom?
    if ( localBottomRight.y < bounds.extent.y )
    {
        sceneMin.y += (bounds.extent.y - localBottomRight.y ) * sceneWindowScale.y;
    }

    // Clip right?
    if ( localBottomRight.x < bounds.extent.x )
    {
        sceneMax.x -= (bounds.extent.x - localBottomRight.x ) * sceneWindowScale.x;
    }

    // Add camera shake offset if active.
    if ( mCameraShaking )
    {
        sceneMin += mCameraShakeOffset;
        sceneMax += mCameraShakeOffset;
    }

    // Setup new logical coordinate system.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // Set orthographic projection.
    glOrtho( sceneMin.x, sceneMax.x, sceneMin.y, sceneMax.y, 0.0f, MAX_LAYERS_SUPPORTED );

    // Set ModelView.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Disable Alpha Test by default
    glDisable( GL_ALPHA_TEST );    
    glDisable( GL_DEPTH_TEST );

    // Get Debug Stats.
    DebugStats& debugStats = pScene->getDebugStats();

    // Create a scene render state.
    SceneRenderState sceneRenderState(
        mCameraCurrent.mDestinationArea,
        mCameraCurrent.mDestinationArea.centre(),
        mCameraCurrent.mCameraAngle,
        mRenderLayerMask,
        mRenderGroupMask,
        Vector2( mCameraCurrent.mSceneWindowScale ),
        &debugStats,
        this );

    // Clear the background color if requested.
    if ( mUseBackgroundColor )
    {
        // Enable the scissor.
        const RectI& clipRect = dglGetClipRect();
        glEnable(GL_SCISSOR_TEST );
        glScissor( clipRect.point.x, Platform::getWindowSize().y - (clipRect.point.y + clipRect.extent.y), clipRect.len_x(), clipRect.len_y() );

        // Clear the background.
        glClearColor( mBackgroundColor.red, mBackgroundColor.green, mBackgroundColor.blue, mBackgroundColor.alpha );
        glClear(GL_COLOR_BUFFER_BIT);

        // Disable the scissor.
        glDisable( GL_SCISSOR_TEST );
    }

    // Render View.
    pScene->sceneRender( &sceneRenderState );

    // Restore Matrices.
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Render the metrics.
    renderMetricsOverlay( offset, updateRect );

	// Render scrollBar.
	if (mShowScrollBar)
	{
		mScrollBar->onRender(offset, updateRect);
	}

    // Render Children.
    renderChildControls( offset, mBounds, updateRect );

    // Update Window.
    setUpdate();

	//return the clip rect
	dglSetClipRect(oldClipRect);
}

//------------------------------------------------------------------------------

void SceneWindow::renderMetricsOverlay( Point2I offset, const RectI& updateRect )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneWindow_RenderMetricsOverlay);

    // Fetch scene.
    Scene* pScene = getScene();

#if 0
    // Force on debug rendering.
    pScene->setDebugOn( 0xFFFFFFFF );
#endif

    // Fetch full metrics mode.
    const bool fullMetrics = pScene->getDebugMask() & Scene::SCENE_DEBUG_METRICS;
    const bool fpsMetrics = pScene->getDebugMask() & Scene::SCENE_DEBUG_FPS_METRICS;

    // Finish if should not or cannot render.
    if (    ( !fullMetrics && !fpsMetrics ) ||
            mProfile == NULL ||
            mProfile->mFont.isNull() )            
            return;

    // Fetch the font.
    Resource<GFont>& font = mProfile->mFont;    

    // Blending for banner background.
    glEnable        ( GL_BLEND );
    glBlendFunc     ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

    // Set banner background color.
    const ColorI& fillColor = mProfile->mFillColor;
    const F32 colorScale = 1.0f / 255.0f;
    glColor4f( fillColor.red * colorScale, fillColor.green * colorScale, fillColor.blue * colorScale, fillColor.alpha * colorScale );

    // Fetch debug scene object.
    SceneObject* pDebugSceneObject = pScene->getDebugSceneObject();

    // Get Debug Stats.
    DebugStats& debugStats = pScene->getDebugStats();

    // Set metrics offset
    const S32 metricsOffset = (S32)font->getStrWidth( "WWWWWWWWWWWW" );

    // Set Banner Height.
    F32 bannerLineHeight = fullMetrics ? 17.0f : 1.0f;

    // Add an extra line if we're monitoring a scene object.
    if ( pDebugSceneObject != NULL )
        bannerLineHeight += 5.0f;

    U32 bannerHeight = (U32)((bannerLineHeight * (F32)font->getHeight()));

    // Calculate Debug Banner Offset.
    Point2I bannerOffset = updateRect.point + Point2I(8,8);

    GLfloat sWindowVertices[] = {
        (GLfloat)updateRect.point.x, (GLfloat)updateRect.point.y,
        (GLfloat)updateRect.point.x + updateRect.extent.x, (GLfloat)updateRect.point.y,
        (GLfloat)updateRect.point.x, (GLfloat)updateRect.point.y + bannerHeight + 16,
        (GLfloat)updateRect.point.x + updateRect.extent.x, (GLfloat)updateRect.point.y + bannerHeight + 16
    };
    
    glVertexPointer(2, GL_FLOAT, 0, sWindowVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    // Disable Banner Blending.
    glDisable       ( GL_BLEND );
        
    // Set Debug Text color.
    dglSetBitmapModulation( mProfile->mFontColor );

    // ****************************************************************
    // Draw Banner Text.
    // ****************************************************************

    F32 linePositionY = 0.25f;
    F32 linePositionOffsetY = font->getHeight() * 1.25f;

    if ( fullMetrics )
    {
        // Rendering.
        dglDrawText( font, bannerOffset + Point2I(0,(S32)linePositionY), "Render", NULL );
        dSprintf( mDebugText, sizeof( mDebugText ), "- FPS=%4.1f<%4.1f/%4.1f>, Frames=%u, Picked=%d<%d>, RenderRequests=%d<%d>, RenderFallbacks=%d<%d>",
            debugStats.fps, debugStats.minFPS, debugStats.maxFPS,
            debugStats.frameCount,
            debugStats.renderPicked, debugStats.maxRenderPicked,
            debugStats.renderRequests, debugStats.maxRenderRequests,
            debugStats.renderFallbacks, debugStats.maxRenderFallbacks );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // Scene.
        dglDrawText( font, bannerOffset + Point2I(0,(S32)linePositionY), "Scene", NULL );
        dSprintf( mDebugText, sizeof( mDebugText ), "- Count=%d, Index=%d, Time=%0.1fs, Objects=%d<%d>(Global=%d), Enabled=%d<%d>, Visible=%d<%d>, Awake=%d<%d>, Controllers=%d",
            Scene::getGlobalSceneCount(), pScene->getSceneIndex(),
            pScene->getSceneTime(),
            debugStats.objectsCount, debugStats.maxObjectsCount, SceneObject::getGlobalSceneObjectCount(),
            debugStats.objectsEnabled, debugStats.maxObjectsEnabled,
            debugStats.objectsVisible, debugStats.maxObjectsVisible,
            debugStats.objectsAwake, debugStats.maxObjectsAwake,
            pScene->getControllers() == NULL ? 0 : pScene->getControllers()->size() );        
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // Camera Window #1.
        dglDrawText( font, bannerOffset + Point2I(0,(S32)linePositionY), "Camera", NULL );
        Vector2 cameraPosition = getCameraPosition();
        dSprintf( mDebugText, sizeof( mDebugText ), "- Pos=(%0.1f,%0.1f), Size=(%0.1f,%0.1f), Zoom=%0.1f, Angle=%0.1f, Lower=(%0.1f,%0.1f), Upper=(%0.1f,%0.1f)", 
            cameraPosition.x,
            cameraPosition.y,
            mCameraCurrent.mSourceArea.extent.x,
            mCameraCurrent.mSourceArea.extent.y,
            mCameraCurrent.mCameraZoom,
            mRadToDeg(mCameraCurrent.mCameraAngle),
            mCameraCurrent.mSourceArea.point.x,
            mCameraCurrent.mSourceArea.point.y,
            mCameraCurrent.mSourceArea.point.x + mCameraCurrent.mSourceArea.extent.x,
            mCameraCurrent.mSourceArea.point.y + mCameraCurrent.mSourceArea.extent.y );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // Camera Window #2.
        Point2I windowExtent = getExtent();
        Vector2 windowScale = getCameraWindowScale();
        dSprintf( mDebugText, sizeof( mDebugText ), "- Window=(%d,%d), WorldScale=(%0.3f,%0.3f), RenderScale=(%0.3f,%0.3f)", 
            windowExtent.x, windowExtent.y,
            windowScale.x, windowScale.y,
            1.0f / windowScale.x, 1.0f / windowScale.y);
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // Batching #1.
        dglDrawText( font, bannerOffset + Point2I(0,(S32)linePositionY), "Batching", NULL );
        dSprintf( mDebugText, sizeof( mDebugText ), "- %sTris=%d<%d>, MaxTriDraw=%d, MaxVerts=%d, Strict=%d<%d>, Sorted=%d<%d>",
            pScene->getBatchingEnabled() ? "" : "(OFF) ",
            debugStats.batchTrianglesSubmitted, debugStats.maxBatchTrianglesSubmitted,
            debugStats.batchMaxTriangleDrawn,
            debugStats.batchMaxVertexBuffer,
            debugStats.batchDrawCallsStrict, debugStats.maxBatchDrawCallsStrict,
            debugStats.batchDrawCallsSorted, debugStats.maxBatchDrawCallsSorted                   
            );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // Batching #2.
        dSprintf( mDebugText, sizeof( mDebugText ), "- Flush=%d<%d>, BlendFlush=%d<%d>, ColorFlush=%d<%d>, AlphaFlush=%d<%d>, TexFlush=%d<%d>",
            debugStats.batchFlushes, debugStats.maxBatchFlushes,
            debugStats.batchBlendStateFlush, debugStats.maxBatchBlendStateFlush,
            debugStats.batchColorStateFlush, debugStats.maxBatchColorStateFlush,
            debugStats.batchAlphaStateFlush, debugStats.maxBatchAlphaStateFlush,
            debugStats.batchTextureChangeFlush, debugStats.maxBatchTextureChangeFlushes
            );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // Batching #3.
        dSprintf( mDebugText, sizeof( mDebugText ), "- IsolatedFlush=%d<%d>, FullFlush=%d<%d>, LayerFlush=%d<%d>, NoBatchFlush=%d<%d>, AnonFlush=%d<%d>",
            debugStats.batchIsolatedFlush, debugStats.maxBatchIsolatedFlush,
            debugStats.batchBufferFullFlush, debugStats.maxBatchBufferFullFlush,
            debugStats.batchLayerFlush, debugStats.maxBatchLayerFlush,
            debugStats.batchNoBatchFlush, debugStats.maxBatchNoBatchFlush,
            debugStats.batchAnonymousFlush, debugStats.maxBatchAnonymousFlush
            );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // Textures.
        dglDrawText( font, bannerOffset + Point2I(0,(S32)linePositionY), "Textures", NULL );
        dSprintf( mDebugText, sizeof( mDebugText ), "- TextureCount=%d, TextureSize=%d, TextureWaste=%d, BitmapSize=%d",
            TextureManager::getTextureResidentCount(),
            TextureManager::getTextureResidentSize(),
            TextureManager::getTextureResidentWasteSize(),
            TextureManager::getBitmapResidentSize()
            );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // Physics.
        dglDrawText( font, bannerOffset + Point2I(0,(S32)linePositionY), "Physics", NULL );
        dSprintf( mDebugText, sizeof( mDebugText ), "- Bodies=%d<%d>, Joints=%d<%d>, Contacts=%d<%d>, Proxies=%d<%d>",
            debugStats.bodyCount, debugStats.maxBodyCount,
            debugStats.jointCount, debugStats.maxJointCount,
            debugStats.contactCount, debugStats.maxContactCount,
            debugStats.proxyCount, debugStats.maxProxyCount );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        const b2Profile& worldProfile = debugStats.worldProfile;
        const b2Profile& maxWorldProfile = debugStats.maxWorldProfile;

        // Physics timings #1.
        dglDrawText( font, bannerOffset + Point2I(0,(S32)linePositionY), "Timings", NULL );
        dSprintf( mDebugText, sizeof( mDebugText ), "- Step=%0.0f<%0.0f>, Collide=%0.0f<%0.0f>, BroadPhase=%0.0f<%0.0f>",
            worldProfile.step, maxWorldProfile.step,
            worldProfile.collide, maxWorldProfile.collide,
            worldProfile.broadphase, maxWorldProfile.broadphase );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // Physics timings #2.
        dSprintf( mDebugText, sizeof( mDebugText ), "- Solve=%0.0f<%0.0f>, SolveInit=%0.0f<%0.0f>, SolveVel=%0.0f<%0.0f>, SolvePos=%0.0f<%0.0f>, SolveTOI=%0.0f<%0.0f>",
            worldProfile.solve, maxWorldProfile.solve,
            worldProfile.solveInit, maxWorldProfile.solveInit,
            worldProfile.solveVelocity, maxWorldProfile.solveVelocity,
            worldProfile.solvePosition, maxWorldProfile.solvePosition,
            worldProfile.solveTOI, maxWorldProfile.solveTOI );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // Physics spatial tree.
        dglDrawText( font, bannerOffset + Point2I(0,(S32)linePositionY), "Partition", NULL );
        const b2World* pWorld = pScene->getWorld();
        const S32 treeBalance = pWorld->GetTreeBalance();
        const S32 treeHeight = pWorld->GetTreeHeight();
        const F32 treeQuality = pWorld->GetTreeQuality();
        dSprintf( mDebugText, sizeof( mDebugText ), "- Balance=%d, Height=%d, Quality=%0.2f",
            treeBalance,
            treeHeight,
            treeQuality );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // Particles.
        dglDrawText( font, bannerOffset + Point2I(0,(S32)linePositionY), "Particles", NULL );
        dSprintf( mDebugText, sizeof( mDebugText ), "- Allocated=%d, Used=%d<%d>, Free=%d",
            debugStats.particlesAlloc,
            debugStats.particlesUsed, debugStats.maxParticlesUsed,
            debugStats.particlesFree );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // Asset Manager.
        dglDrawText( font, bannerOffset + Point2I(0,(S32)linePositionY), "Assets", NULL );
        dSprintf( mDebugText, sizeof( mDebugText ), "- AcquiredRefs=%d, Declared=%d, Referenced=%d, LoadedInternal=%d<%d>, LoadedExternal=%d<%d>, LoadedPrivate=%d<%d>",
            AssetDatabase.getAcquiredReferenceCount(),
            AssetDatabase.getDeclaredAssetCount(),
            AssetDatabase.getReferencedAssetCount(),
            AssetDatabase.getLoadedInternalAssetCount(), AssetDatabase.getMaxLoadedInternalAssetCount(),
            AssetDatabase.getLoadedExternalAssetCount(), AssetDatabase.getMaxLoadedExternalAssetCount(),
            AssetDatabase.getLoadedPrivateAssetCount(), AssetDatabase.getMaxLoadedPrivateAssetCount() );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;
    }
    else if ( fpsMetrics )
    {
        // Rendering.
        dSprintf( mDebugText, sizeof( mDebugText ), "FPS=%4.1f<%4.1f/%4.1f>",
            debugStats.fps, debugStats.minFPS, debugStats.maxFPS );
        dglDrawText( font, bannerOffset + Point2I(0,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;
    }

    // Monitored scene object.
    if ( pDebugSceneObject != NULL )
    {
        // SceneObject #1.
        dglDrawText( font, bannerOffset + Point2I(0,(S32)linePositionY), "SceneObject", NULL );
        const b2Vec2 position = pDebugSceneObject->getRenderPosition();
        const F32 angle = mRadToDeg( pDebugSceneObject->getRenderAngle() );
        const Vector2 size = pDebugSceneObject->getSize();
        const U32 sceneLayer = pDebugSceneObject->getSceneLayer();
        const U32 sceneGroup = pDebugSceneObject->getSceneGroup();
        const U32 serialId = pDebugSceneObject->getSerialId();
        StringTableEntry renderGroup = pDebugSceneObject->getRenderGroup();
        dSprintf( mDebugText, sizeof( mDebugText ), "- Id=%d, Pos=(%0.3f,%0.3f), Angle=%0.3f, Size=(%0.3f,%0.3f), Layer=%d, Group=%d, SerialId=%d, RenderGroup='%s'",
            pDebugSceneObject->getId(),
            position.x, position.y,
            angle,
            size.x, size.y,
            sceneLayer,
            sceneGroup,
            serialId,
            renderGroup
            );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // SceneObject #2.
        const char* pBodyType = SceneObject::getBodyTypeDescription(pDebugSceneObject->getBodyType());
        const bool enabled = pDebugSceneObject->isEnabled();
        const bool active = pDebugSceneObject->getActive();
        const bool visible = pDebugSceneObject->getVisible();
        const bool isBullet = pDebugSceneObject->getBullet();
        const bool awake = pDebugSceneObject->getAwake();
        const bool sleepingAllowed = pDebugSceneObject->getSleepingAllowed();
        dSprintf( mDebugText, sizeof( mDebugText ), "- Body=%s, Enabled=%d, Active=%d, Visible=%d, Bullet=%d, Awake=%d, CanSleep=%d",
            pBodyType,
            enabled,
            active,
            visible,
            isBullet,
            awake,
            sleepingAllowed
            );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;

        // SceneObject #3.
        const Vector2 linearVelocity = pDebugSceneObject->getLinearVelocity();
        const F32 angularVelocity = pDebugSceneObject->getAngularVelocity();
        const F32 linearDamping = pDebugSceneObject->getLinearDamping();
        const F32 angularDamping = pDebugSceneObject->getAngularDamping();
        const Vector2 localCenter = pDebugSceneObject->getLocalCenter();
        dSprintf( mDebugText, sizeof( mDebugText ), "- LinVel=(%0.3f,%0.3f), AngVel=%0.3f, LinDamp=%0.3f, AngDamp=%0.3f, Com=(%0.3f,%0.3f)",
            linearVelocity.x, linearVelocity.y,
            angularVelocity,
            linearDamping,
            angularDamping,
            localCenter.x, localCenter.y
            );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
        linePositionY += linePositionOffsetY;
            
        // SceneObject #4.
        const bool collisionSuppress = pDebugSceneObject->getCollisionSuppress();
        const U32 collisionLayerMask = pDebugSceneObject->getCollisionLayerMask();
        const U32 collisionGroupMask = pDebugSceneObject->getCollisionGroupMask();
        const U32 collisionShapeCount = pDebugSceneObject->getCollisionShapeCount();
        dSprintf( mDebugText, sizeof( mDebugText ), "- Shapes=%d, ColSuppress=%d, ColLayer=%0.8x, ColGroup=%0.8x",
            collisionShapeCount,
            collisionSuppress,
            collisionLayerMask,
            collisionGroupMask
            );
        dglDrawText( font, bannerOffset + Point2I(metricsOffset,(S32)linePositionY), mDebugText, NULL );
    }

    // Clear Bitmap Modulation.
    dglClearBitmapModulation();
}
