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
#include "SpineCollisionProxy.h"
#endif

//------------------------------------------------------------------------------
// Want these proxies usable from torque script
IMPLEMENT_CONOBJECT(SpineCollisionProxy);

//------------------------------------------------------------------------------

SpineCollisionProxy::SpineCollisionProxy() :
	mAttachmentName{NULL},
	mSlotName{NULL},
	mSkinName{NULL},
	mWidthSizer{1.0f},
	mHeightSizer{1.0f},
	mObjectName{NULL},
	mActive{true},
	mRotation{0.0f}
{}

//------------------------------------------------------------------------------

SpineCollisionProxy::SpineCollisionProxy(const char *name, const char *slot, const char *skin, F32 wSizer, F32 hSizer, const char *objectName) :
	mAttachmentName{ name },
	mSlotName{ slot },
	mSkinName{ skin },
	mWidthSizer{ wSizer },
	mHeightSizer{ hSizer },
	mObjectName{ objectName },
	mActive{ true },
	mRotation{ 0.0f }
{}
