#ifndef _MECHBASS_2_
#define _MECHBASS_2_
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/GlslProg.h"
#include "Cinder-DSAPI/src/CiDSAPI.h"

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace CinderDS;


class MBHitBox
{
public:
	MBHitBox();
	MBHitBox(vec3 pPos, float pSize, int pCooldown);
	~MBHitBox();

	void Update(const vector<vec3> &pPoints);
	bool WasTriggered() { return mTriggered;  }
	const vec3 GetCenter() { return mCollision.getCenter(); }
	const vec3 GetSize() { return mCollision.getSize(); }

private:
	bool			mArmed,
					mTriggered;

	AxisAlignedBox	mCollision;
};

class MechBass2App : public App
{
public:
	void setup() override;
	void update() override;
	void draw() override;
	void cleanup() override;

	CameraPersp		SceneView;
	CameraUi		SceneCtrl;

private:
	void setupCloud();
	void setupDS();
	void setupHitboxes();

	void updateCloud();
	void updateHitboxes();

	void drawCloud();
	void drawHitboxes();

	CinderDSRef		mDS;
	Channel16u		mChannelDepth;

	gl::BatchRef	mBatchCloud;
	gl::VboRef		mVboCloud;
	gl::GlslProgRef	mShaderCloud;
	
	gl::BatchRef	mBatchHands;
	gl::VboRef		mVboHands;
	gl::GlslProgRef	mShaderHands;

	gl::GlslProgRef	mShaderTriggers;

	vector<vec3>	mPointsAll,
					mPointsHands;

	vector<MBHitBox>	mHitboxes,
						mActive,
						mInactive;

};
#endif
