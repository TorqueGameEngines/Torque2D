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

#ifndef _BATCH_RENDER_H_
#define _BATCH_RENDER_H_

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

#ifndef _DEBUG_STATS_H_
#include "2d/scene/DebugStats.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _HASHTABLE_H_
#include "collection/hashTable.h"
#endif

#ifndef _COLOR_H_
#include "graphics/gColor.h"
#endif

//-----------------------------------------------------------------------------

#define BATCHRENDER_BUFFERSIZE      (65535)
#define BATCHRENDER_MAXTRIANGLES    (BATCHRENDER_BUFFERSIZE/3)

//-----------------------------------------------------------------------------

class SceneRenderRequest;

//-----------------------------------------------------------------------------

class BatchRender
{
private:
    struct TriangleRun
    {
        enum PrimitiveMode
        {
            TRIANGLE,
            QUAD,
        };

        TriangleRun( const PrimitiveMode primitive, const U32 primitiveCount, const U32 startIndex ) :
            mPrimitiveMode( primitive ),
            mPrimitiveCount( primitiveCount ),
            mStartIndex( startIndex )
        { }

        PrimitiveMode mPrimitiveMode;
        U32 mPrimitiveCount;
        U32 mStartIndex;
    };

    typedef Vector<TriangleRun> indexVectorType;
    typedef HashMap<U32, indexVectorType*> textureBatchType;

    VectorPtr< indexVectorType* > mIndexVectorPool;
    textureBatchType    mTextureBatchMap;

    const ColorF        NoColor;

    Vector2             mVertexBuffer[ BATCHRENDER_BUFFERSIZE ];
    Vector2             mTextureBuffer[ BATCHRENDER_BUFFERSIZE ];
    U16                 mIndexBuffer[ BATCHRENDER_BUFFERSIZE ];
    ColorF              mColorBuffer[ BATCHRENDER_BUFFERSIZE ];
   
    U32                 mTriangleCount;
    U32                 mVertexCount;
    U32                 mTextureCoordCount;
    U32                 mIndexCount;
    U32                 mColorCount;

    bool                mBlendMode;
    GLenum              mSrcBlendFactor;
    GLenum              mDstBlendFactor;
    ColorF              mBlendColor;
    F32                 mAlphaTestMode;

    bool                mStrictOrderMode;
    TextureHandle       mStrictOrderTextureHandle;
    DebugStats*         mpDebugStats;

    bool                mWireframeMode;
    bool                mBatchEnabled;

public:
    BatchRender();
    virtual ~BatchRender();

	static const U32 maxVertexCount = BATCHRENDER_BUFFERSIZE; 

    /// Set the strict order mode.
    inline void setStrictOrderMode( const bool strictOrder, const bool forceFlush = false )
    {
        // Ignore if no change.
        if ( !forceFlush && strictOrder == mStrictOrderMode )
            return;

        // Flush.
        flushInternal();

        // Update strict order mode.
        mStrictOrderMode = strictOrder;
    }

    /// Gets the strict order mode.
    inline bool getStrictOrderMode( void ) const { return mStrictOrderMode; }

    /// Turns-on blend mode with the specified blend factors and color.
    inline void setBlendMode( GLenum srcFactor, GLenum dstFactor, const ColorF& blendColor = ColorF(1.0f, 1.0f, 1.0f, 1.0f))
    {
        // Ignore no change.
        if (    mBlendMode &&
                mSrcBlendFactor == srcFactor &&
                mDstBlendFactor == dstFactor &&
                mBlendColor == blendColor )
                return;

        // Flush.
        flush( mpDebugStats->batchBlendStateFlush );

        mBlendMode = true;
        mSrcBlendFactor = srcFactor;
        mDstBlendFactor = dstFactor;
        mBlendColor = blendColor;
    }

    /// Turns-off blend mode.
    inline void setBlendOff( void )
    {
        // Ignore no change,
        if ( !mBlendMode )
            return;

        // Flush.
        flush( mpDebugStats->batchBlendStateFlush );

        mBlendMode = false;
    }

    // Set blend mode using a specific scene render request.
    void setBlendMode( const SceneRenderRequest* pSceneRenderRequest );

    /// Set alpha-test mode.
    void setAlphaTestMode( const F32 alphaTestMode )
    {
        // Ignore no change.
        if ( mIsEqual( mAlphaTestMode, alphaTestMode ) )
            return;

        // Flush.
        flush( mpDebugStats->batchAlphaStateFlush );

        // Stats.
        mpDebugStats->batchAlphaStateFlush++;

        mAlphaTestMode = alphaTestMode;
    }

    // Set alpha test mode using a specific scene render request.
    void setAlphaTestMode( const SceneRenderRequest* pSceneRenderRequest );

    /// Sets the wireframe mode.
    inline void setWireframeMode( const bool enabled )
    {
        // Ignore no change.
        if ( mWireframeMode == enabled )
            return;

        mWireframeMode = enabled;
    }

    // Gets the wireframe mode.
    inline bool getWireframeMode( void ) const { return mWireframeMode; }

    /// Sets the batch enabled mode.
    inline void setBatchEnabled( const bool enabled )
    {
        // Ignore no change.
        if ( mBatchEnabled == enabled )
            return;

        // Flush.
        flushInternal();

        mBatchEnabled = enabled;
    }

    /// Gets the batch enabled mode.
    inline bool getBatchEnabled( void ) const { return mBatchEnabled; }

    /// Sets the debug stats to use.
    inline void setDebugStats( DebugStats* pDebugStats ) { mpDebugStats = pDebugStats; }

    /// Submit triangles for batching.
    /// Vertex and textures are indexed as:
    ///  2        5
    ///   |\      |\
    ///   | \     | \
    ///  0| _\1  3| _\4
	void SubmitTriangles(
		const U32 vertexCount,
		const Vector2* pVertexArray,
		const Vector2* pTextureArray,
		const ColorF*  pColorArray,
		TextureHandle& texture);

    /// Submit a quad for batching.
    /// Vertex and textures are indexed as:
    ///  3 ___ 2
    ///   |\  |
    ///   | \ |
    ///  0| _\|1
    void SubmitQuad(
            const Vector2& vertexPos0,
            const Vector2& vertexPos1,
            const Vector2& vertexPos2,
            const Vector2& vertexPos3,
            const Vector2& texturePos0,
            const Vector2& texturePos1,
            const Vector2& texturePos2,
            const Vector2& texturePos3,
            TextureHandle& texture,
			const ColorF& color0 = ColorF(-1.0f, -1.0f, -1.0f),
			const ColorF& color1 = ColorF(-1.0f, -1.0f, -1.0f),
			const ColorF& color2 = ColorF(-1.0f, -1.0f, -1.0f),
			const ColorF& color3 = ColorF(-1.0f, -1.0f, -1.0f));

    /// Render a quad immediately without affecting current batch.
    /// All render state should be set beforehand directly.
    /// Vertex and textures are indexed as:
    ///  3 ___ 2
    ///   |\  |
    ///   | \ |
    ///  0| _\|1
    static void RenderQuad(
            const Vector2& vertexPos0,
            const Vector2& vertexPos1,
            const Vector2& vertexPos2,
            const Vector2& vertexPos3,
            const Vector2& texturePos0,
            const Vector2& texturePos1,
            const Vector2& texturePos2,
            const Vector2& texturePos3 )
    {
        glBegin( GL_TRIANGLE_STRIP );
            glTexCoord2f( texturePos0.x, texturePos0.y );
            glVertex2f( vertexPos0.x, vertexPos0.y );
            glTexCoord2f( texturePos1.x, texturePos1.y );
            glVertex2f( vertexPos1.x, vertexPos1.y );
            glTexCoord2f( texturePos3.x, texturePos3.y );
            glVertex2f( vertexPos3.x, vertexPos3.y );
            glTexCoord2f( texturePos2.x, texturePos2.y );
            glVertex2f( vertexPos2.x, vertexPos2.y );
        glEnd();
    }

    /// Flush (render) any pending batches with a reason metric.
    void flush( U32& reasonMetric );

    /// Flush (render) any pending batches.
    void flush( void );

private:
    /// Flush (render) any pending batches.
    void flushInternal( void );

    /// Find texture batch.
    indexVectorType* findTextureBatch( TextureHandle& handle );
};

#endif