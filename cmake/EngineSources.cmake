# -----------------------------------------------------------------------------
# EngineSources.cmake
#
# Authoritative, explicit list of the cross-platform Torque2D engine translation
# units (the build's source of truth). When you add or remove an engine source
# file, edit THIS list, then re-generate the per-platform project files.
#
# Platform-specific sources live in PlatformSources.cmake.
# This list was bootstrapped from the on-disk source tree and reconciled against
# the VS2022 project; maintain it by hand going forward.
# -----------------------------------------------------------------------------

set(TORQUE_ENGINE_SOURCES
    # ---- 2d/assets ----
    ${TORQUE_SRC}/2d/assets/AnimationAsset.cc
    ${TORQUE_SRC}/2d/assets/FontAsset.cc
    ${TORQUE_SRC}/2d/assets/ImageAsset.cc
    ${TORQUE_SRC}/2d/assets/ParticleAsset.cc
    ${TORQUE_SRC}/2d/assets/ParticleAssetEmitter.cc
    ${TORQUE_SRC}/2d/assets/ParticleAssetField.cc
    ${TORQUE_SRC}/2d/assets/ParticleAssetFieldCollection.cc
    # ---- 2d/controllers ----
    ${TORQUE_SRC}/2d/controllers/AmbientForceController.cc
    ${TORQUE_SRC}/2d/controllers/BuoyancyController.cc
    ${TORQUE_SRC}/2d/controllers/PointForceController.cc
    # ---- 2d/controllers/core ----
    ${TORQUE_SRC}/2d/controllers/core/GroupedSceneController.cc
    ${TORQUE_SRC}/2d/controllers/core/PickingSceneController.cc
    # ---- 2d/core ----
    ${TORQUE_SRC}/2d/core/BatchRender.cc
    ${TORQUE_SRC}/2d/core/CoreMath.cc
    ${TORQUE_SRC}/2d/core/ImageFrameProvider.cc
    ${TORQUE_SRC}/2d/core/ImageFrameProviderCore.cc
    ${TORQUE_SRC}/2d/core/ParticleSystem.cc
    ${TORQUE_SRC}/2d/core/RenderProxy.cc
    ${TORQUE_SRC}/2d/core/SpriteBase.cc
    ${TORQUE_SRC}/2d/core/SpriteBatch.cc
    ${TORQUE_SRC}/2d/core/SpriteBatchItem.cc
    ${TORQUE_SRC}/2d/core/SpriteBatchQuery.cc
    ${TORQUE_SRC}/2d/core/Utility.cc
    ${TORQUE_SRC}/2d/core/Vector2.cc
    # ---- 2d/editorToy ----
    ${TORQUE_SRC}/2d/editorToy/EditorToySceneWindow.cc
    ${TORQUE_SRC}/2d/editorToy/EditorToyTool.cc
    # ---- 2d/experimental/composites ----
    ${TORQUE_SRC}/2d/experimental/composites/WaveComposite.cc
    # ---- 2d/gui ----
    ${TORQUE_SRC}/2d/gui/SceneWindow.cc
    ${TORQUE_SRC}/2d/gui/guiSceneObjectCtrl.cc
    ${TORQUE_SRC}/2d/gui/guiSpriteCtrl.cc
    # ---- 2d/scene ----
    ${TORQUE_SRC}/2d/scene/ContactFilter.cc
    ${TORQUE_SRC}/2d/scene/DebugDraw.cc
    ${TORQUE_SRC}/2d/scene/Scene.cc
    ${TORQUE_SRC}/2d/scene/SceneRenderFactories.cpp
    ${TORQUE_SRC}/2d/scene/SceneRenderQueue.cpp
    ${TORQUE_SRC}/2d/scene/WorldQuery.cc
    # ---- 2d/sceneobject ----
    ${TORQUE_SRC}/2d/sceneobject/CompositeSprite.cc
    ${TORQUE_SRC}/2d/sceneobject/LightObject.cc
    ${TORQUE_SRC}/2d/sceneobject/ParticlePlayer.cc
    ${TORQUE_SRC}/2d/sceneobject/Path.cc
    ${TORQUE_SRC}/2d/sceneobject/SceneObject.cc
    ${TORQUE_SRC}/2d/sceneobject/SceneObjectList.cc
    ${TORQUE_SRC}/2d/sceneobject/SceneObjectSet.cc
    ${TORQUE_SRC}/2d/sceneobject/Scroller.cc
    ${TORQUE_SRC}/2d/sceneobject/ShapeVector.cc
    ${TORQUE_SRC}/2d/sceneobject/Sprite.cc
    ${TORQUE_SRC}/2d/sceneobject/TextSprite.cc
    ${TORQUE_SRC}/2d/sceneobject/Trigger.cc
    # ---- algorithm ----
    ${TORQUE_SRC}/algorithm/Perlin.cc
    ${TORQUE_SRC}/algorithm/crc.cc
    ${TORQUE_SRC}/algorithm/hashFunction.cc
    ${TORQUE_SRC}/algorithm/pcg_basic.c
    # ---- assets ----
    ${TORQUE_SRC}/assets/assetBase.cc
    ${TORQUE_SRC}/assets/assetFieldTypes.cc
    ${TORQUE_SRC}/assets/assetManager.cc
    ${TORQUE_SRC}/assets/assetQuery.cc
    ${TORQUE_SRC}/assets/assetTagsManifest.cc
    ${TORQUE_SRC}/assets/declaredAssets.cc
    ${TORQUE_SRC}/assets/referencedAssets.cc
    # ---- audio ----
    ${TORQUE_SRC}/audio/AudioAsset.cc
    ${TORQUE_SRC}/audio/audio.cc
    ${TORQUE_SRC}/audio/audioBuffer.cc
    ${TORQUE_SRC}/audio/audioDataBlock.cc
    ${TORQUE_SRC}/audio/audioDescriptions.cc
    ${TORQUE_SRC}/audio/audioStreamSourceFactory.cc
    ${TORQUE_SRC}/audio/audio_ScriptBinding.cc
    ${TORQUE_SRC}/audio/vorbisStreamSource.cc
    ${TORQUE_SRC}/audio/wavStreamSource.cc
    # ---- bitmapFont ----
    ${TORQUE_SRC}/bitmapFont/BitmapFont.cc
    ${TORQUE_SRC}/bitmapFont/BitmapFontCharacter.cc
    # ---- collection ----
    ${TORQUE_SRC}/collection/bitTables.cc
    ${TORQUE_SRC}/collection/hashTable.cc
    ${TORQUE_SRC}/collection/nameTags.cpp
    ${TORQUE_SRC}/collection/undo.cc
    ${TORQUE_SRC}/collection/vector.cc
    # ---- component ----
    ${TORQUE_SRC}/component/dynamicConsoleMethodComponent.cpp
    ${TORQUE_SRC}/component/simComponent.cpp
    # ---- component/behaviors ----
    ${TORQUE_SRC}/component/behaviors/behaviorComponent.cpp
    ${TORQUE_SRC}/component/behaviors/behaviorInstance.cpp
    ${TORQUE_SRC}/component/behaviors/behaviorTemplate.cpp
    # ---- console ----
    ${TORQUE_SRC}/console/CMDscan.cc
    ${TORQUE_SRC}/console/ConsoleTypeValidators.cc
    ${TORQUE_SRC}/console/Package.cc
    ${TORQUE_SRC}/console/arrayObject.cpp
    ${TORQUE_SRC}/console/astAlloc.cc
    ${TORQUE_SRC}/console/astNodes.cc
    ${TORQUE_SRC}/console/cmdgram.cc
    ${TORQUE_SRC}/console/codeBlock.cc
    ${TORQUE_SRC}/console/compiledEval.cc
    ${TORQUE_SRC}/console/compiler.cc
    ${TORQUE_SRC}/console/console.cc
    ${TORQUE_SRC}/console/consoleBaseType.cc
    ${TORQUE_SRC}/console/consoleDictionary.cc
    ${TORQUE_SRC}/console/consoleDoc.cc
    ${TORQUE_SRC}/console/consoleExprEvalState.cc
    ${TORQUE_SRC}/console/consoleFunctions.cc
    ${TORQUE_SRC}/console/consoleLogger.cc
    ${TORQUE_SRC}/console/consoleNamespace.cc
    ${TORQUE_SRC}/console/consoleObject.cc
    ${TORQUE_SRC}/console/consoleParser.cc
    ${TORQUE_SRC}/console/consoleTypes.cc
    ${TORQUE_SRC}/console/metaScripting_ScriptBinding.cc
    # ---- debug ----
    ${TORQUE_SRC}/debug/profiler.cc
    ${TORQUE_SRC}/debug/telnetDebugger.cc
    # ---- debug/remote ----
    ${TORQUE_SRC}/debug/remote/RemoteDebugger1.cc
    ${TORQUE_SRC}/debug/remote/RemoteDebuggerBase.cc
    ${TORQUE_SRC}/debug/remote/RemoteDebuggerBridge.cc
    # ---- delegates ----
    ${TORQUE_SRC}/delegates/delegateSignal.cpp
    # ---- game ----
    ${TORQUE_SRC}/game/defaultGame.cc
    ${TORQUE_SRC}/game/gameConnection.cc
    ${TORQUE_SRC}/game/gameInterface.cc
    ${TORQUE_SRC}/game/version.cc
    # ---- graphics ----
    ${TORQUE_SRC}/graphics/DynamicTexture.cc
    ${TORQUE_SRC}/graphics/PNGImage.cpp
    ${TORQUE_SRC}/graphics/TextureDictionary.cc
    ${TORQUE_SRC}/graphics/TextureHandle.cc
    ${TORQUE_SRC}/graphics/TextureManager.cc
    ${TORQUE_SRC}/graphics/bitmapBmp.cc
    ${TORQUE_SRC}/graphics/bitmapJpeg.cc
    ${TORQUE_SRC}/graphics/bitmapPng.cc
    # excluded (mobile/other-platform): graphics/bitmapPvr.cc
    ${TORQUE_SRC}/graphics/dgl.cc
    ${TORQUE_SRC}/graphics/dglMatrix.cc
    ${TORQUE_SRC}/graphics/gBitmap.cc
    ${TORQUE_SRC}/graphics/gColor.cc
    ${TORQUE_SRC}/graphics/gFont.cc
    ${TORQUE_SRC}/graphics/gPalette.cc
    ${TORQUE_SRC}/graphics/splineUtil.cc
    # ---- gui ----
    ${TORQUE_SRC}/gui/guiArrayCtrl.cc
    ${TORQUE_SRC}/gui/guiCanvas.cc
    ${TORQUE_SRC}/gui/guiColorPickerCtrl.cc
    ${TORQUE_SRC}/gui/guiColorPopupCtrl.cc
    ${TORQUE_SRC}/gui/guiConsole.cc
    ${TORQUE_SRC}/gui/guiConsoleEditCtrl.cc
    ${TORQUE_SRC}/gui/guiControl.cc
    ${TORQUE_SRC}/gui/guiDefaultControlRender.cc
    ${TORQUE_SRC}/gui/guiInputCtrl.cc
    ${TORQUE_SRC}/gui/guiListBoxCtrl.cc
    ${TORQUE_SRC}/gui/guiMessageVectorCtrl.cc
    ${TORQUE_SRC}/gui/guiProfileTheme.cc
    ${TORQUE_SRC}/gui/guiProgressCtrl.cc
    ${TORQUE_SRC}/gui/guiSliderCtrl.cc
    ${TORQUE_SRC}/gui/guiTextEditCtrl.cc
    ${TORQUE_SRC}/gui/guiTextEditSliderCtrl.cc
    ${TORQUE_SRC}/gui/guiTreeViewCtrl.cc
    ${TORQUE_SRC}/gui/guiTypes.cc
    ${TORQUE_SRC}/gui/messageVector.cc
    # ---- gui/buttons ----
    ${TORQUE_SRC}/gui/buttons/guiButtonCtrl.cc
    ${TORQUE_SRC}/gui/buttons/guiCheckBoxCtrl.cc
    ${TORQUE_SRC}/gui/buttons/guiDropDownCtrl.cc
    ${TORQUE_SRC}/gui/buttons/guiRadioCtrl.cc
    # ---- gui/containers ----
    ${TORQUE_SRC}/gui/containers/guiChainCtrl.cc
    ${TORQUE_SRC}/gui/containers/guiDragAndDropCtrl.cc
    ${TORQUE_SRC}/gui/containers/guiExpandCtrl.cc
    ${TORQUE_SRC}/gui/containers/guiFrameSetCtrl.cc
    ${TORQUE_SRC}/gui/containers/guiGridCtrl.cc
    ${TORQUE_SRC}/gui/containers/guiPanelCtrl.cc
    ${TORQUE_SRC}/gui/containers/guiSceneScrollCtrl.cc
    ${TORQUE_SRC}/gui/containers/guiScrollCtrl.cc
    ${TORQUE_SRC}/gui/containers/guiTabBookCtrl.cc
    ${TORQUE_SRC}/gui/containers/guiTabPageCtrl.cc
    ${TORQUE_SRC}/gui/containers/guiWindowCtrl.cc
    # ---- gui/editor ----
    ${TORQUE_SRC}/gui/editor/guiEditorCursorCtrl.cc
    ${TORQUE_SRC}/gui/editor/guiDebugger.cc
    ${TORQUE_SRC}/gui/editor/guiEditCtrl.cc
    ${TORQUE_SRC}/gui/editor/guiEditFramePaletteCtrl.cc
    ${TORQUE_SRC}/gui/editor/guiEditFrameStripCtrl.cc
    ${TORQUE_SRC}/gui/editor/guiEditFrameTimelineCtrl.cc
    ${TORQUE_SRC}/gui/editor/guiEditParticleColorGraph.cc
    ${TORQUE_SRC}/gui/editor/guiEditorExplorerTree.cc
    ${TORQUE_SRC}/gui/editor/guiGraphCtrl.cc
    ${TORQUE_SRC}/gui/editor/guiInspector.cc
    ${TORQUE_SRC}/gui/editor/guiInspectorTypes.cc
    ${TORQUE_SRC}/gui/editor/guiMenuBarCtrl.cc
    ${TORQUE_SRC}/gui/editor/guiParticleGraphInspector.cc
    # ---- gui/language ----
    ${TORQUE_SRC}/gui/language/lang.cc
    # ---- input ----
    ${TORQUE_SRC}/input/actionMap.cc
    # ---- io ----
    ${TORQUE_SRC}/io/bitStream.cc
    ${TORQUE_SRC}/io/bufferStream.cc
    ${TORQUE_SRC}/io/byteBuffer.cpp
    ${TORQUE_SRC}/io/fileObject.cc
    ${TORQUE_SRC}/io/fileStream.cc
    ${TORQUE_SRC}/io/fileStreamObject.cc
    ${TORQUE_SRC}/io/fileSystem_ScriptBinding.cc
    ${TORQUE_SRC}/io/filterStream.cc
    ${TORQUE_SRC}/io/memStream.cc
    ${TORQUE_SRC}/io/nStream.cc
    ${TORQUE_SRC}/io/resizeStream.cc
    ${TORQUE_SRC}/io/streamObject.cc
    # ---- io/resource ----
    ${TORQUE_SRC}/io/resource/resourceDictionary.cc
    ${TORQUE_SRC}/io/resource/resourceManager.cc
    # ---- io/zip ----
    ${TORQUE_SRC}/io/zip/centralDir.cc
    ${TORQUE_SRC}/io/zip/compressor.cc
    ${TORQUE_SRC}/io/zip/deflate.cc
    ${TORQUE_SRC}/io/zip/extraField.cc
    ${TORQUE_SRC}/io/zip/fileHeader.cc
    ${TORQUE_SRC}/io/zip/stored.cc
    ${TORQUE_SRC}/io/zip/zipArchive.cc
    ${TORQUE_SRC}/io/zip/zipCryptStream.cc
    ${TORQUE_SRC}/io/zip/zipObject.cc
    ${TORQUE_SRC}/io/zip/zipSubStream.cc
    ${TORQUE_SRC}/io/zip/zipTempStream.cc
    # ---- math ----
    ${TORQUE_SRC}/math/mBox.cc
    ${TORQUE_SRC}/math/mFluid.cpp
    ${TORQUE_SRC}/math/mMathAMD.cc
    ${TORQUE_SRC}/math/mMathAltivec.cc
    ${TORQUE_SRC}/math/mMathFn.cc
    ${TORQUE_SRC}/math/mMathSSE.cc
    ${TORQUE_SRC}/math/mMath_C.cc
    ${TORQUE_SRC}/math/mMatrix.cc
    ${TORQUE_SRC}/math/mPlaneTransformer.cc
    ${TORQUE_SRC}/math/mPoint.cpp
    ${TORQUE_SRC}/math/mQuadPatch.cc
    ${TORQUE_SRC}/math/mQuat.cc
    ${TORQUE_SRC}/math/mRandom.cc
    ${TORQUE_SRC}/math/mSolver.cc
    ${TORQUE_SRC}/math/mSplinePatch.cc
    ${TORQUE_SRC}/math/mathTypes.cc
    ${TORQUE_SRC}/math/mathUtils.cc
    ${TORQUE_SRC}/math/math_ScriptBinding.cc
    ${TORQUE_SRC}/math/rectClipper.cpp
    # ---- math/noise ----
    ${TORQUE_SRC}/math/noise/NoiseGenerator.cc
    ${TORQUE_SRC}/math/noise/RandomNumberGenerator.cc
    # ---- memory ----
    ${TORQUE_SRC}/memory/dataChunker.cc
    ${TORQUE_SRC}/memory/frameAllocator_ScriptBinding.cc
    # ---- messaging ----
    ${TORQUE_SRC}/messaging/dispatcher.cc
    ${TORQUE_SRC}/messaging/eventManager.cc
    ${TORQUE_SRC}/messaging/message.cc
    ${TORQUE_SRC}/messaging/messageForwarder.cc
    ${TORQUE_SRC}/messaging/scriptMsgListener.cc
    # ---- module ----
    ${TORQUE_SRC}/module/moduleDefinition.cc
    ${TORQUE_SRC}/module/moduleManager.cc
    ${TORQUE_SRC}/module/moduleMergeDefinition.cc
    # ---- network ----
    ${TORQUE_SRC}/network/RemoteCommandEvent.cc
    ${TORQUE_SRC}/network/connectionProtocol.cc
    ${TORQUE_SRC}/network/connectionStringTable.cc
    ${TORQUE_SRC}/network/httpObject.cc
    ${TORQUE_SRC}/network/netConnection.cc
    ${TORQUE_SRC}/network/netDownload.cc
    ${TORQUE_SRC}/network/netEvent.cc
    ${TORQUE_SRC}/network/netGhost.cc
    ${TORQUE_SRC}/network/netInterface.cc
    ${TORQUE_SRC}/network/netObject.cc
    ${TORQUE_SRC}/network/netStringTable.cc
    ${TORQUE_SRC}/network/netTest.cc
    ${TORQUE_SRC}/network/networkProcessList.cc
    ${TORQUE_SRC}/network/serverQuery.cc
    ${TORQUE_SRC}/network/tcpObject.cc
    ${TORQUE_SRC}/network/telnetConsole.cc
    # ---- persistence ----
    ${TORQUE_SRC}/persistence/SimXMLDocument.cpp
    # ---- persistence/taml ----
    ${TORQUE_SRC}/persistence/taml/taml.cc
    ${TORQUE_SRC}/persistence/taml/tamlCustom.cc
    ${TORQUE_SRC}/persistence/taml/tamlWriteNode.cc
    # ---- persistence/taml/binary ----
    ${TORQUE_SRC}/persistence/taml/binary/tamlBinaryReader.cc
    ${TORQUE_SRC}/persistence/taml/binary/tamlBinaryWriter.cc
    # ---- persistence/taml/json ----
    ${TORQUE_SRC}/persistence/taml/json/tamlJSONParser.cc
    ${TORQUE_SRC}/persistence/taml/json/tamlJSONReader.cc
    ${TORQUE_SRC}/persistence/taml/json/tamlJSONWriter.cc
    # ---- persistence/taml/xml ----
    ${TORQUE_SRC}/persistence/taml/xml/tamlXmlParser.cc
    ${TORQUE_SRC}/persistence/taml/xml/tamlXmlReader.cc
    ${TORQUE_SRC}/persistence/taml/xml/tamlXmlWriter.cc
    # ---- persistence/tinyXML ----
    ${TORQUE_SRC}/persistence/tinyXML/tinystr.cpp
    ${TORQUE_SRC}/persistence/tinyXML/tinyxml.cpp
    ${TORQUE_SRC}/persistence/tinyXML/tinyxmlerror.cpp
    ${TORQUE_SRC}/persistence/tinyXML/tinyxmlparser.cpp
    # ---- sim ----
    ${TORQUE_SRC}/sim/SimObjectList.cc
    ${TORQUE_SRC}/sim/scriptGroup.cc
    ${TORQUE_SRC}/sim/scriptObject.cc
    ${TORQUE_SRC}/sim/simBase.cc
    ${TORQUE_SRC}/sim/simConsoleEvent.cc
    ${TORQUE_SRC}/sim/simConsoleThreadExecEvent.cc
    ${TORQUE_SRC}/sim/simDatablock.cc
    ${TORQUE_SRC}/sim/simDictionary.cc
    ${TORQUE_SRC}/sim/simFieldDictionary.cc
    ${TORQUE_SRC}/sim/simManager.cc
    ${TORQUE_SRC}/sim/simObject.cc
    ${TORQUE_SRC}/sim/simSerialize.cpp
    ${TORQUE_SRC}/sim/simSet.cc
    # ---- string ----
    ${TORQUE_SRC}/string/findMatch.cc
    ${TORQUE_SRC}/string/stringBuffer.cc
    ${TORQUE_SRC}/string/stringStack.cc
    ${TORQUE_SRC}/string/stringTable.cc
    ${TORQUE_SRC}/string/stringUnit.cpp
    ${TORQUE_SRC}/string/unicode.cc
    # ---- testing ----
    ${TORQUE_SRC}/testing/unitTesting.cc
    # ---- testing/tests ----
    ${TORQUE_SRC}/testing/tests/animationFrameConversionTests.cc
    ${TORQUE_SRC}/testing/tests/assetStateCopyTests.cc
    ${TORQUE_SRC}/testing/tests/bitmapFontParseTests.cc
    ${TORQUE_SRC}/testing/tests/declaredPathCaseTests.cc
    ${TORQUE_SRC}/testing/tests/directoryScanCaseTests.cc
    ${TORQUE_SRC}/testing/tests/guiControlReparentTests.cc
    ${TORQUE_SRC}/testing/tests/guiCursorHotSpotTests.cc
    ${TORQUE_SRC}/testing/tests/guiFrameStripLayoutTests.cc
    ${TORQUE_SRC}/testing/tests/guiHitTestTests.cc
    ${TORQUE_SRC}/testing/tests/guiParticleColorGraphTests.cc
    ${TORQUE_SRC}/testing/tests/guiProfileThemeTests.cc
    ${TORQUE_SRC}/testing/tests/guiScrollLayoutTests.cc
    ${TORQUE_SRC}/testing/tests/guiTextEditTests.cc
    ${TORQUE_SRC}/testing/tests/guiTextWrapTests.cc
    ${TORQUE_SRC}/testing/tests/guiTreeRowLayoutTests.cc
    ${TORQUE_SRC}/testing/tests/imageAssetCellNameTests.cc
    ${TORQUE_SRC}/testing/tests/namespaceLinkTests.cc
    ${TORQUE_SRC}/testing/tests/particleEmissionAngleTests.cc
    ${TORQUE_SRC}/testing/tests/platformFileIoTests.cc
    ${TORQUE_SRC}/testing/tests/platformMemoryTests.cc
    ${TORQUE_SRC}/testing/tests/platformStringTests.cc
    ${TORQUE_SRC}/testing/tests/resourceManagerDeleteTests.cc
    ${TORQUE_SRC}/testing/tests/simObjectCloneTests.cc
    ${TORQUE_SRC}/testing/tests/stringTableCaseTests.cc
    # ---- platform ----
    ${TORQUE_SRC}/platform/CursorManager.cc
    ${TORQUE_SRC}/platform/Tickable.cc
    ${TORQUE_SRC}/platform/platform.cc
    ${TORQUE_SRC}/platform/platformAssert.cc
    ${TORQUE_SRC}/platform/platformCPU.cc
    ${TORQUE_SRC}/platform/platformFileIO.cc
    ${TORQUE_SRC}/platform/platformFont.cc
    ${TORQUE_SRC}/platform/platformMemory.cc
    ${TORQUE_SRC}/platform/platformNet.cpp
    ${TORQUE_SRC}/platform/platformNetAsync.cpp
    # excluded (mobile/other-platform): platform/platformNet_Emscripten.cpp
    ${TORQUE_SRC}/platform/platformNet_ScriptBinding.cc
    ${TORQUE_SRC}/platform/platformString.cc
    ${TORQUE_SRC}/platform/platformVideo.cc
    # ---- platform/menus ----
    ${TORQUE_SRC}/platform/menus/popupMenu.cc
    # ---- platform/nativeDialogs ----
    ${TORQUE_SRC}/platform/nativeDialogs/fileDialog.cc
    ${TORQUE_SRC}/platform/nativeDialogs/msgBox.cpp
    # ---- Box2D (third-party physics, full tree) ----
    ${TORQUE_SRC}/Box2D/Collision/Shapes/b2ChainShape.cpp
    ${TORQUE_SRC}/Box2D/Collision/Shapes/b2CircleShape.cpp
    ${TORQUE_SRC}/Box2D/Collision/Shapes/b2EdgeShape.cpp
    ${TORQUE_SRC}/Box2D/Collision/Shapes/b2PolygonShape.cpp
    ${TORQUE_SRC}/Box2D/Collision/b2BroadPhase.cpp
    ${TORQUE_SRC}/Box2D/Collision/b2CollideCircle.cpp
    ${TORQUE_SRC}/Box2D/Collision/b2CollideEdge.cpp
    ${TORQUE_SRC}/Box2D/Collision/b2CollidePolygon.cpp
    ${TORQUE_SRC}/Box2D/Collision/b2Collision.cpp
    ${TORQUE_SRC}/Box2D/Collision/b2Distance.cpp
    ${TORQUE_SRC}/Box2D/Collision/b2DynamicTree.cpp
    ${TORQUE_SRC}/Box2D/Collision/b2TimeOfImpact.cpp
    ${TORQUE_SRC}/Box2D/Common/b2BlockAllocator.cpp
    ${TORQUE_SRC}/Box2D/Common/b2Draw.cpp
    ${TORQUE_SRC}/Box2D/Common/b2FreeList.cpp
    ${TORQUE_SRC}/Box2D/Common/b2Math.cpp
    ${TORQUE_SRC}/Box2D/Common/b2Settings.cpp
    ${TORQUE_SRC}/Box2D/Common/b2StackAllocator.cpp
    ${TORQUE_SRC}/Box2D/Common/b2Stat.cpp
    ${TORQUE_SRC}/Box2D/Common/b2Timer.cpp
    ${TORQUE_SRC}/Box2D/Common/b2TrackedBlock.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Contacts/b2CircleContact.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Contacts/b2Contact.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Contacts/b2ContactSolver.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Contacts/b2PolygonContact.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Joints/b2DistanceJoint.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Joints/b2FrictionJoint.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Joints/b2GearJoint.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Joints/b2Joint.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Joints/b2MotorJoint.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Joints/b2MouseJoint.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Joints/b2PulleyJoint.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Joints/b2RopeJoint.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Joints/b2WeldJoint.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/Joints/b2WheelJoint.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/b2Body.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/b2ContactManager.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/b2Fixture.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/b2Island.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/b2World.cpp
    ${TORQUE_SRC}/Box2D/Dynamics/b2WorldCallbacks.cpp
    ${TORQUE_SRC}/Box2D/Particle/b2Particle.cpp
    ${TORQUE_SRC}/Box2D/Particle/b2ParticleAssembly.cpp
    ${TORQUE_SRC}/Box2D/Particle/b2ParticleGroup.cpp
    ${TORQUE_SRC}/Box2D/Particle/b2ParticleSystem.cpp
    ${TORQUE_SRC}/Box2D/Particle/b2VoronoiDiagram.cpp
    ${TORQUE_SRC}/Box2D/Rope/b2Rope.cpp
)
