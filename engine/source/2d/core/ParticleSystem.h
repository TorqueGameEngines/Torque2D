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

#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

#ifndef _IMAGE_FRAME_PROVIDER_H
#include "2d/core/ImageFrameProvider.h"
#endif

//-----------------------------------------------------------------------------

class ParticleSystem
{
public:
    /// Particle node.
    struct ParticleNode : public IFactoryObjectReset
    {
        /// Particle Node Linkages.
        ParticleNode*           mPreviousNode;
        ParticleNode*           mNextNode;

        /// Suppress Movement.
        bool                    mSuppressMovement;

        /// Particle Components.
        F32                     mParticleLifetime;
        F32                     mParticleAge;
        Vector2                 mPosition;
        Vector2                 mVelocity;
        F32                     mOrientationAngle;
        Vector2                 mRenderOOBB[4];
        b2Transform             mTransform;
        ImageFrameProviderCore  mFrameProvider;

        /// Render Properties.
        Vector2                 mRenderSize;
        F32                     mRenderSpeed;
        F32                     mRenderSpin;
        F32                     mRenderFixedForce;
        F32                     mRenderRandomMotion;

        /// Base Properties.
        Vector2                 mSize;
        F32                     mSpeed;
        F32                     mSpin;
        F32                     mFixedForce;
        F32                     mRandomMotion;
        ColorF                  mColor;    

        /// Physics particles
        bool                    mPhysicsParticles;
        b2ParticleGroupDef      mParticleGroup;
        b2ParticleSystem*       mParticleSystem;

        /// Interpolated Tick Position.
        Vector2                 mPreTickPosition;
        Vector2                 mPostTickPosition;
        Vector2                 mRenderTickPosition;

        ParticleNode() { constructInPlace<ImageFrameProviderCore>(&mFrameProvider); resetState(); }

        virtual void resetState( void )
        {
            mFrameProvider.resetState();
        }
    };

private:
    const U32               mParticlePoolBlockSize;
    Vector<ParticleNode*>   mParticlePool;
    ParticleNode*           mpFreeParticleNodes;
    U32                     mActiveParticleCount;

public:
    static void Init( void );
    static void destroy( void );
    static ParticleSystem* Instance;

    ParticleSystem();
    ~ParticleSystem();

    ParticleNode* createParticle( void );
    void freeParticle( ParticleNode* pParticleNode );

    inline U32 getActiveParticleCount( void ) const { return mActiveParticleCount; };
    inline U32 getAllocatedParticleCount( void ) const { return (U32)mParticlePool.size() * mParticlePoolBlockSize; }
};

#endif // _PARTICLE_SYSTEM_H_
