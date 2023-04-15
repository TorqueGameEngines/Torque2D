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

#ifndef _TRIGGER_H_
#define _TRIGGER_H_

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _HASHTABLE_H_
#include "collection/hashTable.h"
#endif

///-----------------------------------------------------------------------------
/// Trigger 2D.
///-----------------------------------------------------------------------------
class Trigger : public SceneObject
{
   typedef SceneObject Parent;

private:
    /// Callback Options.
    bool                    mEnterCallback;
    bool                    mStayCallback;
    bool                    mLeaveCallback;

    /// Object Mapping Database.
    typedef VectorPtr<SceneObject*> collideCallbackType;

    collideCallbackType     mEnterColliders;
    collideCallbackType     mLeaveColliders;

public:
    Trigger();
    virtual ~Trigger() {};

    /// Engine.
    virtual bool onAdd();
    static void initPersistFields();

    /// Integration.
    virtual void            preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats *pDebugStats );
    virtual void            integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );

    /// Rendering.
    virtual bool            shouldRender( void ) const { return false; }

    /// Contact processing.
    virtual void            onBeginCollision( const TickContact& tickContact );
    virtual void            onEndCollision( const TickContact& tickContact );
    virtual void            setGatherContacts( const bool gatherContacts ) { } // Suppress changing contact gathering.

    /// Cloning.
    virtual void            copyTo(SimObject* object);

    /// Callback Management.
    inline void             setEnterCallback(bool enter = true)         { mEnterCallback = enter; };
    inline void             setStayCallback(bool stay = true)           { mStayCallback = stay; };
    inline void             setLeaveCallback(bool leave = true)         { mLeaveCallback = leave; };
    inline bool             getEnterCallback()                          { return mEnterCallback; };
    inline bool             getStayCallback()                           { return mStayCallback; };
    inline bool             getLeaveCallback()                          { return mLeaveCallback; };
    
    /// Declare Console Object.
    DECLARE_CONOBJECT( Trigger );

protected:
    /// Callback Management.
    static bool             setEnterCallback(void* obj, const char* data) { static_cast<Trigger*>(obj)->setEnterCallback(dAtob(data)); return false; };
    static bool             writeEnterCallback( void* obj, StringTableEntry pFieldName ) {return static_cast<Trigger*>(obj)->mEnterCallback == false; }
    static bool             setStayCallback(void* obj, const char* data)  { static_cast<Trigger*>(obj)->setStayCallback(dAtob(data)); return false; };
    static bool             writeStayCallback( void* obj, StringTableEntry pFieldName ) { return  static_cast<Trigger*>(obj)->mStayCallback == true; }
    static bool             setLeaveCallback(void* obj, const char* data) { static_cast<Trigger*>(obj)->setLeaveCallback(dAtob(data)); return false; };
    static bool             writeLeaveCallback( void* obj, StringTableEntry pFieldName ) {return  static_cast<Trigger*>(obj)->mLeaveCallback == false; }
};

#endif // _TRIGGER_H_
