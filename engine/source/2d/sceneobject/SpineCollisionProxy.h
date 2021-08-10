// Author: Mike Tannel - 7/21/2020
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------
#ifndef _SPINE_COLLISION_PROXY_H_
#define _SPINE_COLLISION_PROXY_H_

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef SPINE_SPINE_H_
#include "spine/spine.h"
#endif

class SpineCollisionProxy :
	public SceneObject
{
protected:
	// For console object
	typedef SceneObject Parent;

public:
	SpineCollisionProxy();
	SpineCollisionProxy(const char *name, const char *slot, const char *skin, F32 wSizer = 1.0f, F32 hSizer = 1.0f, const char *objectName = NULL);

	const char *mAttachmentName;
	const char *mSlotName;
	const char *mSkinName;
	F32			mWidthSizer;
	F32			mHeightSizer;
	const char *mObjectName;

	bool mActive;

	F32	 mRotation;

	/// Declare Console Object.
	DECLARE_CONOBJECT(SpineCollisionProxy);

public:
	inline void deActivate(void) { if (!mActive) return;  mActive = false; setActive(false); }
	inline void activate(void) { if (mActive) return;  mActive = true; setActive(true); }

};

#endif // _SPINE_COLLISION_PROXY_H_
