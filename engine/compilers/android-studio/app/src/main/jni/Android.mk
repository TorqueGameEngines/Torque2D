# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

#required for compiling on windows
NDK_APP_SHORT_COMMANDS  := true
LOCAL_SHORT_COMMANDS  := true

MY_LOCAL_PATH := $(call my-dir)

LOCAL_PATH := $(MY_LOCAL_PATH)
# OpenAL Soft library must be a shared library since license is LGPLv3
include $(CLEAR_VARS)

LOCAL_MODULE    := libopenal-prebuilt
LOCAL_SRC_FILES := ../../../../../../lib/openal/Android/$(TARGET_ARCH_ABI)/libopenal.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../../../lib/openal/Android/openal-soft-master/jni/OpenAL/include

include $(PREBUILT_SHARED_LIBRARY)

#freetype2 lib for generating fonts on device
LOCAL_PATH := $(MY_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE := freetype-prebuilt
LOCAL_SRC_FILES := ../../../../../../lib/freetype/android/lib/$(TARGET_ARCH_ABI)/libfreetype.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../../../lib/freetype/android/include $(LOCAL_PATH)/../../../../../../lib/freetype/android/include/freetype2

include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(MY_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE    := torque2d
LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../../../../../../lib/ljpeg \
					$(LOCAL_PATH)/../../../../../../lib/lpng \
					$(LOCAL_PATH)/../../../../../../lib/libogg \
					$(LOCAL_PATH)/../../../../../../lib/libogg/include \
					$(LOCAL_PATH)/../../../../../../lib/libvorbis \
					$(LOCAL_PATH)/../../../../../../lib/libvorbis/include \
					$(LOCAL_PATH)/../../../../../../lib/libvorbis/lib \
    				$(LOCAL_PATH)/../../../../../../source \
    				$(LOCAL_PATH)/../../../../../../source/2d \
    				$(LOCAL_PATH)/../../../../../../source/2d/assets \
    				$(LOCAL_PATH)/../../../../../../source/2d/controllers \
    				$(LOCAL_PATH)/../../../../../../source/2d/core \
    				$(LOCAL_PATH)/../../../../../../source/2d/experimental/composites \
    				$(LOCAL_PATH)/../../../../../../source/2d/gui \
    				$(LOCAL_PATH)/../../../../../../source/2d/sceneobject \
    				$(LOCAL_PATH)/../../../../../../source/2d/scene \
    				$(LOCAL_PATH)/../../../../../../source/algorithm \
    				$(LOCAL_PATH)/../../../../../../source/assets \
    				$(LOCAL_PATH)/../../../../../../source/audio \
    				$(LOCAL_PATH)/../../../../../../source/Box2D \
    				$(LOCAL_PATH)/../../../../../../source/Box2D/Collision \
					$(LOCAL_PATH)/../../../../../../source/Box2D/Collision/Shapes \
					$(LOCAL_PATH)/../../../../../../source/Box2D/Common \
					$(LOCAL_PATH)/../../../../../../source/Box2D/Dynamics \
					$(LOCAL_PATH)/../../../../../../source/Box2D/Dynamics/Contacts \
					$(LOCAL_PATH)/../../../../../../source/Box2D/Dynamics/Joints \
					$(LOCAL_PATH)/../../../../../../source/Box2D/Rope \
    				$(LOCAL_PATH)/../../../../../../source/collection \
    				$(LOCAL_PATH)/../../../../../../source/component \
    				$(LOCAL_PATH)/../../../../../../source/component/behaviors \
    				$(LOCAL_PATH)/../../../../../../source/console \
    				$(LOCAL_PATH)/../../../../../../source/debug \
    				$(LOCAL_PATH)/../../../../../../source/debug/remote \
    				$(LOCAL_PATH)/../../../../../../source/delegates \
    				$(LOCAL_PATH)/../../../../../../source/game \
    				$(LOCAL_PATH)/../../../../../../source/graphics \
    				$(LOCAL_PATH)/../../../../../../source/gui \
    				$(LOCAL_PATH)/../../../../../../source/gui/buttons \
    				$(LOCAL_PATH)/../../../../../../source/gui/containers \
    				$(LOCAL_PATH)/../../../../../../source/gui/editor \
    				$(LOCAL_PATH)/../../../../../../source/gui/language \
    				$(LOCAL_PATH)/../../../../../../source/input \
    				$(LOCAL_PATH)/../../../../../../source/io \
    				$(LOCAL_PATH)/../../../../../../source/io/resource \
    				$(LOCAL_PATH)/../../../../../../source/io/zip \
    				$(LOCAL_PATH)/../../../../../../source/math \
    				$(LOCAL_PATH)/../../../../../../source/memory \
    				$(LOCAL_PATH)/../../../../../../source/messaging \
    				$(LOCAL_PATH)/../../../../../../source/module \
    				$(LOCAL_PATH)/../../../../../../source/network \
    				$(LOCAL_PATH)/../../../../../../source/persistence \
    				$(LOCAL_PATH)/../../../../../../source/persistence/taml \
    				$(LOCAL_PATH)/../../../../../../source/persistence/taml/binary \
    				$(LOCAL_PATH)/../../../../../../source/persistence/taml/json \
    				$(LOCAL_PATH)/../../../../../../source/persistence/taml/xml \
    				$(LOCAL_PATH)/../../../../../../source/persistence/rapidjson/include \
    				$(LOCAL_PATH)/../../../../../../source/platform \
    				$(LOCAL_PATH)/../../../../../../source/platform/nativeDialogs \
    				$(LOCAL_PATH)/../../../../../../source/platformAndroid \
    				$(LOCAL_PATH)/../../../../../../source/sim \
    				$(LOCAL_PATH)/../../../../../../source/spine \
    				$(LOCAL_PATH)/../../../../../../source/string
#    				$(LOCAL_PATH)/../../../../../../source/testing \
#    				$(LOCAL_PATH)/../../../../../../source/testing/tests \


LOCAL_SRC_FILES :=  ../../../../../../lib/ljpeg/jcapimin.c \
					../../../../../../lib/ljpeg/jcapistd.c \
					../../../../../../lib/ljpeg/jccoefct.c \
					../../../../../../lib/ljpeg/jccolor.c \
					../../../../../../lib/ljpeg/jcdctmgr.c \
					../../../../../../lib/ljpeg/jchuff.c \
					../../../../../../lib/ljpeg/jcinit.c \
					../../../../../../lib/ljpeg/jcmainct.c \
					../../../../../../lib/ljpeg/jcmarker.c \
					../../../../../../lib/ljpeg/jcmaster.c \
					../../../../../../lib/ljpeg/jcomapi.c \
					../../../../../../lib/ljpeg/jcparam.c \
					../../../../../../lib/ljpeg/jcphuff.c \
					../../../../../../lib/ljpeg/jcprepct.c \
					../../../../../../lib/ljpeg/jcsample.c \
					../../../../../../lib/ljpeg/jctrans.c \
					../../../../../../lib/ljpeg/jdapimin.c \
					../../../../../../lib/ljpeg/jdapistd.c \
					../../../../../../lib/ljpeg/jdatadst.c \
					../../../../../../lib/ljpeg/jdatasrc.c \
					../../../../../../lib/ljpeg/jdcoefct.c \
					../../../../../../lib/ljpeg/jdcolor.c \
					../../../../../../lib/ljpeg/jddctmgr.c \
					../../../../../../lib/ljpeg/jdhuff.c \
					../../../../../../lib/ljpeg/jdinput.c \
					../../../../../../lib/ljpeg/jdmainct.c \
					../../../../../../lib/ljpeg/jdmarker.c \
					../../../../../../lib/ljpeg/jdmaster.c \
					../../../../../../lib/ljpeg/jdmerge.c \
					../../../../../../lib/ljpeg/jdphuff.c \
					../../../../../../lib/ljpeg/jdpostct.c \
					../../../../../../lib/ljpeg/jdsample.c \
					../../../../../../lib/ljpeg/jdtrans.c \
					../../../../../../lib/ljpeg/jerror.c \
					../../../../../../lib/ljpeg/jfdctflt.c \
					../../../../../../lib/ljpeg/jfdctfst.c \
					../../../../../../lib/ljpeg/jfdctint.c \
					../../../../../../lib/ljpeg/jidctflt.c \
					../../../../../../lib/ljpeg/jidctfst.c \
					../../../../../../lib/ljpeg/jidctint.c \
					../../../../../../lib/ljpeg/jidctred.c \
					../../../../../../lib/ljpeg/jmemansi.c \
					../../../../../../lib/ljpeg/jmemmgr.c \
					../../../../../../lib/ljpeg/jquant1.c \
					../../../../../../lib/ljpeg/jquant2.c \
					../../../../../../lib/ljpeg/jutils.c \
					../../../../../../lib/libogg/src/bitwise.c \
					../../../../../../lib/libogg/src/framing.c \
					../../../../../../lib/lpng/png.c \
					../../../../../../lib/lpng/pngerror.c \
					../../../../../../lib/lpng/pngget.c \
					../../../../../../lib/lpng/pngmem.c \
					../../../../../../lib/lpng/pngpread.c \
					../../../../../../lib/lpng/pngread.c \
					../../../../../../lib/lpng/pngrio.c \
					../../../../../../lib/lpng/pngrtran.c \
					../../../../../../lib/lpng/pngrutil.c \
					../../../../../../lib/lpng/pngset.c \
					../../../../../../lib/lpng/pngtrans.c \
					../../../../../../lib/lpng/pngwio.c \
					../../../../../../lib/lpng/pngwrite.c \
					../../../../../../lib/lpng/pngwtran.c \
					../../../../../../lib/lpng/pngwutil.c \
					../../../../../../lib/libvorbis/analysis.c \
                    ../../../../../../lib/libvorbis/barkmel.c \
                    ../../../../../../lib/libvorbis/bitrate.c \
                    ../../../../../../lib/libvorbis/block.c \
                    ../../../../../../lib/libvorbis/codebook.c \
                    ../../../../../../lib/libvorbis/envelope.c \
                    ../../../../../../lib/libvorbis/floor0.c \
                    ../../../../../../lib/libvorbis/floor1.c \
                    ../../../../../../lib/libvorbis/info.c \
                    ../../../../../../lib/libvorbis/lookup.c \
                    ../../../../../../lib/libvorbis/lpc.c \
                    ../../../../../../lib/libvorbis/lsp.c \
                    ../../../../../../lib/libvorbis/mapping0.c \
                    ../../../../../../lib/libvorbis/mdct.c \
                    ../../../../../../lib/libvorbis/psy.c \
                    ../../../../../../lib/libvorbis/registry.c \
                    ../../../../../../lib/libvorbis/res0.c \
                    ../../../../../../lib/libvorbis/sharedbook.c \
                    ../../../../../../lib/libvorbis/smallft.c \
                    ../../../../../../lib/libvorbis/synthesis.c \
                    ../../../../../../lib/libvorbis/vorbisfile.c \
                    ../../../../../../lib/libvorbis/window.c \
					../../../../../../source/2d/assets/AnimationAsset.cc \
					../../../../../../source/2d/assets/FontAsset.cc \
					../../../../../../source/2d/assets/ImageAsset.cc \
					../../../../../../source/2d/assets/ParticleAsset.cc \
					../../../../../../source/2d/assets/ParticleAssetEmitter.cc \
					../../../../../../source/2d/assets/ParticleAssetField.cc \
					../../../../../../source/2d/assets/ParticleAssetFieldCollection.cc \
					../../../../../../source/2d/assets/SkeletonAsset.cc \
					../../../../../../source/2d/controllers/AmbientForceController.cc \
					../../../../../../source/2d/controllers/BuoyancyController.cc \
					../../../../../../source/2d/controllers/core/GroupedSceneController.cc \
					../../../../../../source/2d/controllers/core/PickingSceneController.cc \
					../../../../../../source/2d/controllers/PointForceController.cc \
					../../../../../../source/2d/core/BatchRender.cc \
					../../../../../../source/2d/core/CoreMath.cc \
					../../../../../../source/2d/core/ImageFrameProvider.cc \
					../../../../../../source/2d/core/ImageFrameProviderCore.cc \
					../../../../../../source/2d/core/ParticleSystem.cc \
					../../../../../../source/2d/core/RenderProxy.cc \
					../../../../../../source/2d/core/SpriteBase.cc \
					../../../../../../source/2d/core/SpriteBatch.cc \
					../../../../../../source/2d/core/SpriteBatchItem.cc \
					../../../../../../source/2d/core/SpriteBatchQuery.cc \
					../../../../../../source/2d/core/Utility.cc \
					../../../../../../source/2d/core/Vector2.cc \
					../../../../../../source/2d/experimental/composites/WaveComposite.cc \
					../../../../../../source/2d/gui/guiImageButtonCtrl.cc \
					../../../../../../source/2d/gui/guiSceneObjectCtrl.cc \
					../../../../../../source/2d/gui/guiSpriteCtrl.cc \
					../../../../../../source/2d/gui/SceneWindow.cc \
					../../../../../../source/2d/sceneobject/CompositeSprite.cc \
					../../../../../../source/2d/sceneobject/ParticlePlayer.cc \
					../../../../../../source/2d/sceneobject/SceneObject.cc \
					../../../../../../source/2d/sceneobject/SceneObjectList.cc \
					../../../../../../source/2d/sceneobject/SceneObjectSet.cc \
					../../../../../../source/2d/sceneobject/Scroller.cc \
					../../../../../../source/2d/sceneobject/ShapeVector.cc \
					../../../../../../source/2d/sceneobject/SkeletonObject.cc \
					../../../../../../source/2d/sceneobject/Sprite.cc \
					../../../../../../source/2d/sceneobject/TextSprite.cc \
					../../../../../../source/2d/sceneobject/Trigger.cc \
					../../../../../../source/2d/scene/ContactFilter.cc \
					../../../../../../source/2d/scene/DebugDraw.cc \
					../../../../../../source/2d/scene/Scene.cc \
					../../../../../../source/2d/scene/SceneRenderFactories.cpp \
					../../../../../../source/2d/scene/SceneRenderQueue.cpp \
					../../../../../../source/2d/scene/WorldQuery.cc \
					../../../../../../source/algorithm/crc.cc \
					../../../../../../source/algorithm/hashFunction.cc \
					../../../../../../source/assets/assetBase.cc \
					../../../../../../source/assets/assetFieldTypes.cc \
					../../../../../../source/assets/assetManager.cc \
					../../../../../../source/assets/assetQuery.cc \
					../../../../../../source/assets/assetTagsManifest.cc \
					../../../../../../source/assets/declaredAssets.cc \
					../../../../../../source/assets/referencedAssets.cc \
                    ../../../../../../source/audio/audio.cc \
                    ../../../../../../source/audio/audioDataBlock.cc \
					../../../../../../source/audio/audioDescriptions.cc \
                    ../../../../../../source/audio/audio_ScriptBinding.cc \
                    ../../../../../../source/audio/audioStreamSourceFactory.cc \
                    ../../../../../../source/audio/wavStreamSource.cc \
					../../../../../../source/audio/AudioAsset.cc \
					../../../../../../source/audio/audioBuffer.cc \
					../../../../../../source/audio/vorbisStreamSource.cc \
					../../../../../../source/bitmapFont/BitmapFont.cc \
					../../../../../../source/bitmapFont/BitmapFontCharacter.cc \
					../../../../../../source/Box2D/Collision/b2BroadPhase.cpp \
					../../../../../../source/Box2D/Collision/b2CollideCircle.cpp \
					../../../../../../source/Box2D/Collision/b2CollideEdge.cpp \
					../../../../../../source/Box2D/Collision/b2CollidePolygon.cpp \
					../../../../../../source/Box2D/Collision/b2Collision.cpp \
					../../../../../../source/Box2D/Collision/b2Distance.cpp \
					../../../../../../source/Box2D/Collision/b2DynamicTree.cpp \
					../../../../../../source/Box2D/Collision/b2TimeOfImpact.cpp \
					../../../../../../source/Box2D/Collision/Shapes/b2ChainShape.cpp \
					../../../../../../source/Box2D/Collision/Shapes/b2CircleShape.cpp \
					../../../../../../source/Box2D/Collision/Shapes/b2EdgeShape.cpp \
					../../../../../../source/Box2D/Collision/Shapes/b2PolygonShape.cpp \
					../../../../../../source/Box2D/Common/b2BlockAllocator.cpp \
					../../../../../../source/Box2D/Common/b2Draw.cpp \
					../../../../../../source/Box2D/Common/b2FreeList.cpp \
					../../../../../../source/Box2D/Common/b2Math.cpp \
					../../../../../../source/Box2D/Common/b2Settings.cpp \
					../../../../../../source/Box2D/Common/b2TrackedBlock.cpp \
					../../../../../../source/Box2D/Common/b2StackAllocator.cpp \
					../../../../../../source/Box2D/Common/b2Timer.cpp \
					../../../../../../source/Box2D/Dynamics/b2Body.cpp \
					../../../../../../source/Box2D/Dynamics/b2ContactManager.cpp \
					../../../../../../source/Box2D/Dynamics/b2Fixture.cpp \
					../../../../../../source/Box2D/Dynamics/b2Island.cpp \
					../../../../../../source/Box2D/Dynamics/b2World.cpp \
					../../../../../../source/Box2D/Dynamics/b2WorldCallbacks.cpp \
					../../../../../../source/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp \
					../../../../../../source/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp \
					../../../../../../source/Box2D/Dynamics/Contacts/b2CircleContact.cpp \
					../../../../../../source/Box2D/Dynamics/Contacts/b2Contact.cpp \
					../../../../../../source/Box2D/Dynamics/Contacts/b2ContactSolver.cpp \
					../../../../../../source/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp \
					../../../../../../source/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp \
					../../../../../../source/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp \
					../../../../../../source/Box2D/Dynamics/Contacts/b2PolygonContact.cpp \
					../../../../../../source/Box2D/Dynamics/Joints/b2DistanceJoint.cpp \
					../../../../../../source/Box2D/Dynamics/Joints/b2FrictionJoint.cpp \
					../../../../../../source/Box2D/Dynamics/Joints/b2GearJoint.cpp \
					../../../../../../source/Box2D/Dynamics/Joints/b2Joint.cpp \
					../../../../../../source/Box2D/Dynamics/Joints/b2MotorJoint.cpp \
					../../../../../../source/Box2D/Dynamics/Joints/b2MouseJoint.cpp \
					../../../../../../source/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp \
					../../../../../../source/Box2D/Dynamics/Joints/b2PulleyJoint.cpp \
					../../../../../../source/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp \
					../../../../../../source/Box2D/Dynamics/Joints/b2RopeJoint.cpp \
					../../../../../../source/Box2D/Dynamics/Joints/b2WeldJoint.cpp \
					../../../../../../source/Box2D/Dynamics/Joints/b2WheelJoint.cpp \
					../../../../../../source/Box2D/Particle/b2Particle.cpp \
					../../../../../../source/Box2D/Particle/b2ParticleAssembly.cpp \
					../../../../../../source/Box2D/Particle/b2ParticleGroup.cpp \
					../../../../../../source/Box2D/Particle/b2ParticleSystem.cpp \
					../../../../../../source/Box2D/Particle/b2VoronoiDiagram.cpp \
					../../../../../../source/Box2D/Rope/b2Rope.cpp \
					../../../../../../source/collection/bitTables.cc \
					../../../../../../source/collection/hashTable.cc \
					../../../../../../source/collection/nameTags.cpp \
					../../../../../../source/collection/undo.cc \
					../../../../../../source/collection/vector.cc \
					../../../../../../source/console/consoleBaseType.cc \
					../../../../../../source/console/consoleDictionary.cc \
					../../../../../../source/console/consoleExprEvalState.cc \
					../../../../../../source/console/consoleNamespace.cc \
					../../../../../../source/console/ConsoleTypeValidators.cc \
					../../../../../../source/console/metaScripting_ScriptBinding.cc \
					../../../../../../source/debug/profiler.cc \
					../../../../../../source/debug/remote/RemoteDebugger1.cc \
					../../../../../../source/debug/remote/RemoteDebuggerBase.cc \
					../../../../../../source/debug/remote/RemoteDebuggerBridge.cc \
					../../../../../../source/debug/telnetDebugger.cc \
					../../../../../../source/delegates/delegateSignal.cpp \
					../../../../../../source/game/defaultGame.cc \
					../../../../../../source/game/gameInterface.cc \
					../../../../../../source/graphics/bitmapBmp.cc \
					../../../../../../source/graphics/bitmapJpeg.cc \
					../../../../../../source/graphics/bitmapPng.cc \
					../../../../../../source/graphics/color.cc \
					../../../../../../source/graphics/dgl.cc \
					../../../../../../source/graphics/dglMatrix.cc \
					../../../../../../source/graphics/DynamicTexture.cc \
					../../../../../../source/graphics/gBitmap.cc \
					../../../../../../source/graphics/gFont.cc \
					../../../../../../source/graphics/gPalette.cc \
					../../../../../../source/graphics/PNGImage.cpp \
					../../../../../../source/graphics/splineUtil.cc \
					../../../../../../source/graphics/TextureDictionary.cc \
					../../../../../../source/graphics/TextureHandle.cc \
					../../../../../../source/graphics/TextureManager.cc \
					../../../../../../source/gui/containers/guiGridCtrl.cc \
					../../../../../../source/gui/guiArrayCtrl.cc \
					../../../../../../source/gui/guiBackgroundCtrl.cc \
					../../../../../../source/gui/guiBitmapBorderCtrl.cc \
					../../../../../../source/gui/guiBitmapCtrl.cc \
					../../../../../../source/gui/guiBubbleTextCtrl.cc \
					../../../../../../source/gui/guiCanvas.cc \
					../../../../../../source/gui/guiColorPicker.cc \
					../../../../../../source/gui/guiConsole.cc \
					../../../../../../source/gui/guiConsoleEditCtrl.cc \
					../../../../../../source/gui/guiConsoleTextCtrl.cc \
					../../../../../../source/gui/guiControl.cc \
					../../../../../../source/gui/guiDefaultControlRender.cc \
					../../../../../../source/gui/guiFadeinBitmapCtrl.cc \
					../../../../../../source/gui/guiInputCtrl.cc \
					../../../../../../source/gui/guiListBoxCtrl.cc \
					../../../../../../source/gui/guiMessageVectorCtrl.cc \
					../../../../../../source/gui/guiMLTextCtrl.cc \
					../../../../../../source/gui/guiMLTextEditCtrl.cc \
					../../../../../../source/gui/guiMouseEventCtrl.cc \
					../../../../../../source/gui/guiPopUpCtrl.cc \
					../../../../../../source/gui/guiPopUpCtrlEx.cc \
					../../../../../../source/gui/guiProgressCtrl.cc \
					../../../../../../source/gui/guiScriptNotifyControl.cc \
					../../../../../../source/gui/guiSliderCtrl.cc \
					../../../../../../source/gui/guiTabPageCtrl.cc \
					../../../../../../source/gui/guiTextCtrl.cc \
					../../../../../../source/gui/guiTextEditCtrl.cc \
					../../../../../../source/gui/guiTextEditSliderCtrl.cc \
					../../../../../../source/gui/guiTextListCtrl.cc \
					../../../../../../source/gui/guiTickCtrl.cc \
					../../../../../../source/gui/guiTreeViewCtrl.cc \
					../../../../../../source/gui/guiTypes.cc \
					../../../../../../source/gui/language/lang.cc \
					../../../../../../source/gui/messageVector.cc \
					../../../../../../source/input/actionMap.cc \
					../../../../../../source/io/byteBuffer.cpp \
					../../../../../../source/io/bitStream.cc \
					../../../../../../source/io/bufferStream.cc \
					../../../../../../source/io/fileObject.cc \
					../../../../../../source/io/fileStream.cc \
					../../../../../../source/io/fileStreamObject.cc \
					../../../../../../source/io/fileSystem_ScriptBinding.cc \
					../../../../../../source/io/filterStream.cc \
					../../../../../../source/io/memStream.cc \
					../../../../../../source/io/nStream.cc \
					../../../../../../source/io/resizeStream.cc \
					../../../../../../source/io/resource/resourceDictionary.cc \
					../../../../../../source/io/resource/resourceManager.cc \
					../../../../../../source/io/streamObject.cc \
					../../../../../../source/io/zip/centralDir.cc \
					../../../../../../source/io/zip/compressor.cc \
					../../../../../../source/io/zip/deflate.cc \
					../../../../../../source/io/zip/extraField.cc \
					../../../../../../source/io/zip/fileHeader.cc \
					../../../../../../source/io/zip/stored.cc \
					../../../../../../source/io/zip/zipArchive.cc \
					../../../../../../source/io/zip/zipCryptStream.cc \
					../../../../../../source/io/zip/zipObject.cc \
					../../../../../../source/io/zip/zipSubStream.cc \
					../../../../../../source/io/zip/zipTempStream.cc \
					../../../../../../source/math/rectClipper.cpp \
					../../../../../../source/memory/dataChunker.cc \
					../../../../../../source/memory/frameAllocator_ScriptBinding.cc \
					../../../../../../source/messaging/dispatcher.cc \
					../../../../../../source/messaging/eventManager.cc \
					../../../../../../source/messaging/message.cc \
					../../../../../../source/messaging/messageForwarder.cc \
					../../../../../../source/messaging/scriptMsgListener.cc \
					../../../../../../source/module/moduleDefinition.cc \
					../../../../../../source/module/moduleManager.cc \
					../../../../../../source/module/moduleMergeDefinition.cc \
					../../../../../../source/network/connectionProtocol.cc \
					../../../../../../source/network/connectionStringTable.cc \
					../../../../../../source/network/httpObject.cc \
					../../../../../../source/network/netConnection.cc \
					../../../../../../source/network/netDownload.cc \
					../../../../../../source/network/netEvent.cc \
					../../../../../../source/network/netGhost.cc \
					../../../../../../source/network/netInterface.cc \
					../../../../../../source/network/netObject.cc \
					../../../../../../source/network/netStringTable.cc \
					../../../../../../source/network/netTest.cc \
					../../../../../../source/network/networkProcessList.cc \
					../../../../../../source/network/RemoteCommandEvent.cc \
					../../../../../../source/network/serverQuery.cc \
					../../../../../../source/network/tcpObject.cc \
					../../../../../../source/network/telnetConsole.cc \
					../../../../../../source/persistence/taml/binary/tamlBinaryReader.cc \
					../../../../../../source/persistence/taml/binary/tamlBinaryWriter.cc \
					../../../../../../source/persistence/taml/json/tamlJSONParser.cc \
					../../../../../../source/persistence/taml/json/tamlJSONReader.cc \
					../../../../../../source/persistence/taml/json/tamlJSONWriter.cc \
					../../../../../../source/persistence/taml/taml.cc \
					../../../../../../source/persistence/taml/tamlCustom.cc \
					../../../../../../source/persistence/taml/tamlWriteNode.cc \
					../../../../../../source/persistence/taml/xml/tamlXmlParser.cc \
					../../../../../../source/persistence/taml/xml/tamlXmlReader.cc \
					../../../../../../source/persistence/taml/xml/tamlXmlWriter.cc \
					../../../../../../source/persistence/tinyXML/tinystr.cpp \
					../../../../../../source/persistence/tinyXML/tinyxml.cpp \
					../../../../../../source/persistence/tinyXML/tinyxmlerror.cpp \
					../../../../../../source/persistence/tinyXML/tinyxmlparser.cpp \
					../../../../../../source/component/dynamicConsoleMethodComponent.cpp \
					../../../../../../source/component/simComponent.cpp \
					../../../../../../source/component/behaviors/behaviorComponent.cpp \
					../../../../../../source/component/behaviors/behaviorInstance.cpp \
					../../../../../../source/component/behaviors/behaviorTemplate.cpp \
					../../../../../../source/console/astAlloc.cc \
					../../../../../../source/console/astNodes.cc \
					../../../../../../source/console/cmdgram.cc \
					../../../../../../source/console/CMDscan.cc \
					../../../../../../source/console/codeBlock.cc \
					../../../../../../source/console/compiledEval.cc \
					../../../../../../source/console/compiler.cc \
					../../../../../../source/console/console.cc \
					../../../../../../source/console/consoleDoc.cc \
					../../../../../../source/console/consoleFunctions.cc \
					../../../../../../source/console/consoleLogger.cc \
					../../../../../../source/console/consoleObject.cc \
					../../../../../../source/console/consoleParser.cc \
					../../../../../../source/console/consoleTypes.cc \
					../../../../../../source/game/gameConnection.cc \
					../../../../../../source/game/version.cc \
					../../../../../../source/math/math_ScriptBinding.cc \
					../../../../../../source/math/mathTypes.cc \
					../../../../../../source/math/mathUtils.cc \
					../../../../../../source/math/mBox.cc \
					../../../../../../source/math/mMath_C.cc \
					../../../../../../source/math/mMathAltivec.cc \
					../../../../../../source/math/mMathAMD.cc \
					../../../../../../source/math/mMathFn.cc \
					../../../../../../source/math/mMathSSE.cc \
					../../../../../../source/math/mMatrix.cc \
					../../../../../../source/math/mPlaneTransformer.cc \
					../../../../../../source/math/mQuadPatch.cc \
					../../../../../../source/math/mQuat.cc \
					../../../../../../source/math/mRandom.cc \
					../../../../../../source/math/mSolver.cc \
					../../../../../../source/math/mSplinePatch.cc \
					../../../../../../source/math/mPoint.cpp \
					../../../../../../source/persistence/SimXMLDocument.cpp \
					../../../../../../source/platform/CursorManager.cc \
					../../../../../../source/platform/nativeDialogs/fileDialog.cc \
					../../../../../../source/platform/platform.cc \
					../../../../../../source/platform/platformAssert.cc \
					../../../../../../source/platform/platformCPU.cc \
					../../../../../../source/platform/platformFileIO.cc \
					../../../../../../source/platform/platformFont.cc \
					../../../../../../source/platform/platformMemory.cc \
					../../../../../../source/platform/platformNet.cpp \
					../../../../../../source/platform/platformNetAsync.cpp \
					../../../../../../source/platform/platformNet_ScriptBinding.cc \
					../../../../../../source/platform/platformString.cc \
					../../../../../../source/platform/platformVideo.cc \
					../../../../../../source/platform/menus/popupMenu.cc \
					../../../../../../source/platform/nativeDialogs/msgBox.cpp \
					../../../../../../source/platform/Tickable.cc \
					../../../../../../source/platformAndroid/android_native_app_glue.c \
					../../../../../../source/platformAndroid/AndroidAlerts.cpp \
					../../../../../../source/platformAndroid/AndroidAudio.cpp \
					../../../../../../source/platformAndroid/AndroidConsole.cpp \
					../../../../../../source/platformAndroid/AndroidCPUInfo.cpp \
					../../../../../../source/platformAndroid/AndroidDialogs.cpp \
					../../../../../../source/platformAndroid/AndroidEvents.cpp \
					../../../../../../source/platformAndroid/AndroidFileio.cpp \
					../../../../../../source/platformAndroid/AndroidFont.cpp \
					../../../../../../source/platformAndroid/AndroidGL.cpp \
					../../../../../../source/platformAndroid/AndroidGL2ES.cpp \
					../../../../../../source/platformAndroid/AndroidInput.cpp \
					../../../../../../source/platformAndroid/AndroidMath.cpp \
					../../../../../../source/platformAndroid/AndroidMemory.cpp \
					../../../../../../source/platformAndroid/AndroidMutex.cpp \
					../../../../../../source/platformAndroid/AndroidOGLVideo.cpp \
					../../../../../../source/platformAndroid/AndroidOutlineGL.cpp \
					../../../../../../source/platformAndroid/AndroidPlatform.cpp \
					../../../../../../source/platformAndroid/AndroidProcessControl.cpp \
					../../../../../../source/platformAndroid/AndroidProfiler.cpp \
					../../../../../../source/platformAndroid/AndroidSemaphore.cpp \
					../../../../../../source/platformAndroid/AndroidStreamSource.cc \
					../../../../../../source/platformAndroid/AndroidStrings.cpp \
					../../../../../../source/platformAndroid/AndroidThread.cpp \
					../../../../../../source/platformAndroid/AndroidTime.cpp \
					../../../../../../source/platformAndroid/AndroidUtil.cpp \
					../../../../../../source/platformAndroid/AndroidWindow.cpp \
					../../../../../../source/platformAndroid/main.cpp \
					../../../../../../source/platformAndroid/T2DActivity.cpp \
					../../../../../../source/platformAndroid/menus/popupMenu.cpp \
					../../../../../../source/sim/scriptGroup.cc \
					../../../../../../source/sim/scriptObject.cc \
					../../../../../../source/sim/simBase.cc \
					../../../../../../source/sim/simConsoleEvent.cc \
					../../../../../../source/sim/simConsoleThreadExecEvent.cc \
					../../../../../../source/sim/simDatablock.cc \
					../../../../../../source/sim/simDictionary.cc \
					../../../../../../source/sim/simFieldDictionary.cc \
					../../../../../../source/sim/simManager.cc \
					../../../../../../source/sim/simObject.cc \
					../../../../../../source/sim/SimObjectList.cc \
					../../../../../../source/sim/simSerialize.cpp \
					../../../../../../source/sim/simSet.cc \
					../../../../../../source/spine/Animation.c \
					../../../../../../source/spine/AnimationState.c \
					../../../../../../source/spine/AnimationStateData.c \
					../../../../../../source/spine/Atlas.c \
					../../../../../../source/spine/AtlasAttachmentLoader.c \
					../../../../../../source/spine/Attachment.c \
					../../../../../../source/spine/AttachmentLoader.c \
					../../../../../../source/spine/Bone.c \
					../../../../../../source/spine/BoneData.c \
					../../../../../../source/spine/BoundingBoxAttachment.c \
					../../../../../../source/spine/Event.c \
					../../../../../../source/spine/EventData.c \
					../../../../../../source/spine/extension.c \
					../../../../../../source/spine/Json.c \
					../../../../../../source/spine/RegionAttachment.c \
					../../../../../../source/spine/Skeleton.c \
					../../../../../../source/spine/SkeletonBounds.c \
					../../../../../../source/spine/SkeletonData.c \
					../../../../../../source/spine/SkeletonJson.c \
					../../../../../../source/spine/Skin.c \
					../../../../../../source/spine/Slot.c \
					../../../../../../source/spine/SlotData.c \
					../../../../../../source/string/findMatch.cc \
					../../../../../../source/string/stringBuffer.cc \
					../../../../../../source/string/stringStack.cc \
					../../../../../../source/string/stringTable.cc \
					../../../../../../source/string/stringUnit.cpp \
					../../../../../../source/string/unicode.cc \
					../../../../../../source/gui/buttons/guiButtonCtrl.cc \
					../../../../../../source/gui/buttons/guiCheckBoxCtrl.cc \
					../../../../../../source/gui/buttons/guiRadioCtrl.cc \
					../../../../../../source/gui/containers/guiAutoScrollCtrl.cc \
					../../../../../../source/gui/containers/guiCtrlArrayCtrl.cc \
					../../../../../../source/gui/containers/guiDragAndDropCtrl.cc \
					../../../../../../source/gui/containers/guiDynamicCtrlArrayCtrl.cc \
					../../../../../../source/gui/containers/guiFormCtrl.cc \
					../../../../../../source/gui/containers/guiFrameCtrl.cc \
					../../../../../../source/gui/containers/guiPaneCtrl.cc \
					../../../../../../source/gui/containers/guiRolloutCtrl.cc \
					../../../../../../source/gui/containers/guiScrollCtrl.cc \
					../../../../../../source/gui/containers/guiStackCtrl.cc \
					../../../../../../source/gui/containers/guiTabBookCtrl.cc \
					../../../../../../source/gui/containers/guiWindowCtrl.cc \
					../../../../../../source/gui/editor/guiControlListPopup.cc \
					../../../../../../source/gui/editor/guiDebugger.cc \
					../../../../../../source/gui/editor/guiEditCtrl.cc \
					../../../../../../source/gui/editor/guiFilterCtrl.cc \
					../../../../../../source/gui/editor/guiGraphCtrl.cc \
					../../../../../../source/gui/editor/guiImageList.cc \
					../../../../../../source/gui/editor/guiInspector.cc \
					../../../../../../source/gui/editor/guiInspectorTypes.cc \
					../../../../../../source/gui/editor/guiMenuBar.cc \
					../../../../../../source/gui/editor/guiSeparatorCtrl.cc
#					../../../../../../source/testing/tests/platformFileIoTests.cc \
#					../../../../../../source/testing/tests/platformMemoryTests.cc \
#					../../../../../../source/testing/tests/platformStringTests.cc \
#					../../../../../../source/testing/unitTesting.cc

ifeq ($(APP_OPTIM),debug)
	LOCAL_CFLAGS := -DENABLE_CONSOLE_MSGS -D__ANDROID__ -DTORQUE_DEBUG -DTORQUE_OS_ANDROID -DGL_GLEXT_PROTOTYPES -O0 -fsigned-char
	LOCAL_CPPFLAGS := -std=gnu++11 -frtti $(LOCAL_CFLAGS)
else
	LOCAL_CFLAGS := -DENABLE_CONSOLE_MSGS -D__ANDROID__ -DTORQUE_OS_ANDROID -DGL_GLEXT_PROTOTYPES -O3 -fsigned-char
	LOCAL_CPPFLAGS := -std=gnu++11 -frtti $(LOCAL_CFLAGS)
endif
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM -lz -lOpenSLES -L../../../../../../lib/openal/Android/$(TARGET_ARCH_ABI)
LOCAL_STATIC_LIBRARIES := freetype-prebuilt
LOCAL_SHARED_LIBRARIES := libopenal-prebuilt

ifeq ($(TARGET_ARCH_ABI),$(filter $(TARGET_ARCH_ABI), arm64-v8a))
    LOCAL_CFLAGS := -DHAVE_NEON=1
    LOCAL_ARM_NEON := true

endif

LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)
