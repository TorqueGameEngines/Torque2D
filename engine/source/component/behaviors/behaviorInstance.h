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
#ifndef _BEHAVIORINSTANCE_H_
#define _BEHAVIORINSTANCE_H_

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _HASHTABLE_H_
#include "collection/hashTable.h"
#endif

#ifndef _SIMCOMPONENT_H_
#include "component/simComponent.h"
#endif

//-----------------------------------------------------------------------------

class BehaviorTemplate;
class BehaviorComponent;

//-----------------------------------------------------------------------------

class BehaviorInstance : public SimObject
{
    typedef SimObject Parent;

public:
    BehaviorInstance( BehaviorTemplate* pTemplate = NULL );
    virtual ~BehaviorInstance() {}

    virtual bool onAdd();
    virtual void onRemove();
    static void initPersistFields();

    /// Template name.
    inline BehaviorTemplate* getTemplate( void ) { return mTemplate; }
    const char* getTemplateName( void );

    /// Owner.
    inline void setBehaviorOwner( BehaviorComponent* pOwner ) { mBehaviorOwner = pOwner; }
    inline BehaviorComponent* getBehaviorOwner( void ) const { return mBehaviorOwner ? mBehaviorOwner : NULL; }

    /// Behavior Id.
    inline void setBehaviorId( const U32 id ) { mBehaviorId = id; }
    inline U32 getBehaviorId( void ) const { return mBehaviorId; }

    DECLARE_CONOBJECT(BehaviorInstance);

protected:
    BehaviorTemplate*   mTemplate;
    BehaviorComponent*  mBehaviorOwner;
    U32                 mBehaviorId;

    // Set "Owner" via the field does nothing.
    static bool setOwner( void* obj, const char* data ) { return true; }

    // Get template.
    static const char* getTemplate(void* obj, const char* data);
};

#endif // _BEHAVIORINSTANCE_H_
