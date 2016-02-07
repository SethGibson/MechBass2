#include "cinder/Json.h"
#include "cinder/Rand.h"
#include "MechBass2App.h"

MBHitBox::MBHitBox()
{

}

MBHitBox::MBHitBox(vec3 pPos, float pSize, int pCooldown) :
mArmed(true), mTriggered(false)
{
	mCollision = AxisAlignedBox(pPos - (pSize*0.5f), pPos + (pSize*0.5f));
}

MBHitBox::~MBHitBox()
{

}

void MBHitBox::Update(const vector<vec3> &pPoints)
{
	mTriggered = false;
	int found = 0;
	for (auto p = begin(pPoints); p != end(pPoints); ++p)
	{
		if (mCollision.contains(*p))
		{
			found++;
			if (found >= 8)
			{
				mArmed = false;
				mTriggered = true;
				return;
			}
		}
	}
}

void MechBass2App::setup()
{
	getWindow()->setSize(1280, 720);
	setFrameRate(60);

	setupDS();
	setupCloud();
	setupHitboxes();

	vec2 fovs = mDS->getDepthFOVs();

	SceneView.setPerspective(fovs.y, getWindowAspectRatio(), 100.0f, 4000.0f);
	SceneView.lookAt(vec3(0, 0, 0), vec3(0,0,1000), vec3(0, -1, 0));
	SceneView.setPivotDistance(500.0f);
	SceneCtrl = CameraUi(&SceneView, getWindow());
}

void MechBass2App::update()
{
	updateCloud();
	updateHitboxes();
}

void MechBass2App::draw()
{
	gl::clear(Color(0, 0, 0));
	drawCloud();
	drawHitboxes();

}

void MechBass2App::setupDS()
{
	mDS = CinderDSAPI::create();
	mDS->init();
	mDS->initDepth(FrameSize::DEPTHSD, 60);
	mDS->start();

	mChannelDepth = Channel16u(mDS->getDepthWidth(), mDS->getDepthHeight());
}

void MechBass2App::setupCloud()
{
	mPointsAll.clear();
	ivec2 depthDims = mDS->getDepthSize();
	for (int dy = 0; dy < depthDims.y; ++dy)
	{
		for (int dx = 0; dx < depthDims.x; ++dx)
		{
			mPointsAll.push_back(vec3(0));
			mPointsHands.push_back(vec3(0));
		}
	}

	mVboCloud = gl::Vbo::create(GL_ARRAY_BUFFER, mPointsAll, GL_DYNAMIC_DRAW);
	gl::VboMeshRef vboMeshAll = gl::VboMesh::create(geom::Sphere().radius(6).subdivisions(16));
	geom::BufferLayout vboAttribsAll;
	vboAttribsAll.append(geom::CUSTOM_0, 3, sizeof(vec3), 0, 1);
	vboMeshAll->appendVbo(vboAttribsAll, mVboCloud);

	mShaderCloud = gl::GlslProg::create(loadAsset("shaders/pointcloud.vert"), loadAsset("shaders/pointcloud.frag"));
	mBatchCloud = gl::Batch::create(vboMeshAll, mShaderCloud, { { geom::CUSTOM_0, "iPosition" } });

	mVboHands = gl::Vbo::create(GL_ARRAY_BUFFER, mPointsHands, GL_DYNAMIC_DRAW);
	gl::VboMeshRef vboMeshHands = gl::VboMesh::create(geom::Sphere().radius(6).subdivisions(16));
	geom::BufferLayout vboAttribsHands;
	vboAttribsHands.append(geom::CUSTOM_0, 3, sizeof(vec3), 0, 1);
	vboMeshHands->appendVbo(vboAttribsHands, mVboHands);

	mShaderHands = gl::GlslProg::create(loadAsset("shaders/pointcloud.vert"), loadAsset("shaders/pointcloudhands.frag"));
	mBatchHands = gl::Batch::create(vboMeshHands, mShaderHands, { { geom::CUSTOM_0, "iPosition" } });

	mShaderTriggers = gl::GlslProg::create(loadAsset("shaders/triggervolume.vert"), loadAsset("shaders/triggervolume.frag"));
}

void MechBass2App::updateCloud()
{
	mDS->update();

	mChannelDepth = mDS->getDepthFrame();
	auto iter = mChannelDepth.getIter();

	mPointsAll.clear();
	mPointsHands.clear();

	while (iter.line())
	{
		while (iter.pixel())
		{
			auto z = iter.v();
			if (z>100&&z < 2000)
			{
				float x = iter.x();
				float y = iter.y();

				vec3 world = mDS->getDepthSpacePoint(vec3(x, y, z));
				if (z > 800)
				{
					if ((int)x % 4 == 0 && (int)y % 4 == 0)
						mPointsAll.push_back(world);
				}
				else
				{
					if ((int)x % 4 == 0 && (int)y %4==0)
						mPointsHands.push_back(world);
				}
			}
		}
	}

	mVboCloud->bufferData(mPointsAll.size()*sizeof(vec3), mPointsAll.data(), GL_DYNAMIC_DRAW);
	mVboHands->bufferData(mPointsHands.size()*sizeof(vec3), mPointsHands.data(), GL_DYNAMIC_DRAW);
}

void MechBass2App::drawCloud()
{
	gl::pushMatrices();
	gl::setMatrices(SceneView);
	gl::scale(-1, 1, 1);
	gl::enableDepthRead();
	mBatchCloud->getGlslProg()->uniform("u_LightPos", vec3(200, -500, 0));
	mBatchCloud->getGlslProg()->uniform("u_EyePos", SceneView.getEyePoint());
	mBatchCloud->drawInstanced(mPointsAll.size());
	mBatchHands->getGlslProg()->uniform("u_LightPos", vec3(200, -500, 0));
	mBatchCloud->getGlslProg()->uniform("u_EyePos", SceneView.getEyePoint());
	mBatchHands->drawInstanced(mPointsHands.size());
	gl::disableDepthRead();
	gl::popMatrices();
}

void MechBass2App::setupHitboxes()
{
	JsonTree hitboxCfg(loadAsset("config/hitbox_config.json"));
	JsonTree hitboxes(hitboxCfg.getChild("hitboxes"));

	for (JsonTree::ConstIter i = hitboxes.begin(); i != hitboxes.end(); ++i)
	{
		float x = i->getChild("x").getValue<float>();
		float y = i->getChild("y").getValue<float>();
		float z = i->getChild("z").getValue<float>();
		float u = i->getChild("size").getValue<float>();
		int v = i->getChild("cooldown").getValue<int>();

		mHitboxes.push_back(MBHitBox(vec3(x,y,z), u, v));
	}
}

void MechBass2App::updateHitboxes()
{
	mActive.clear();
	mInactive.clear();
	for (auto b = begin(mHitboxes); b != end(mHitboxes); ++b)
	{
		b->Update(mPointsHands);
		if (b->WasTriggered())
			mActive.push_back(*b);
		else
			mInactive.push_back(*b);
	}
}

void MechBass2App::drawHitboxes()
{
	vector<vec3> active;
	vector<vec3> inactive;

	gl::pushMatrices();
	gl::setMatrices(SceneView);
	gl::scale(-1, 1, 1);
	gl::enableDepthRead();
	//gl::disableDepthWrite();

	gl::cullFace(GL_BACK);
	mShaderTriggers->bind();
	mShaderTriggers->uniform("u_LightPos", vec3(200, -500, 0));
	mShaderTriggers->uniform("u_EyePos", SceneView.getEyePoint());
	mShaderTriggers->uniform("u_RimColor", vec3(0, 1.0, 0.5));
	mShaderTriggers->uniform("u_SpecColor", vec3(0.809, 0.093, 0.574));
	mShaderTriggers->uniform("u_SpecPower", 4.0f);
	mShaderTriggers->uniform("u_RimPower", 1.5f);

	for (auto v : mActive)
		gl::drawSphere(v.GetCenter(), v.GetSize().x*0.75f, 32);

	mShaderTriggers->uniform("u_RimColor", vec3(0.5, 0.5, 0.5));
	mShaderTriggers->uniform("u_SpecColor", vec3(0));
	mShaderTriggers->uniform("u_RimPower", 1.0f);

	for (auto w : mInactive)
		gl::drawSphere(w.GetCenter(), w.GetSize().x*0.6f, 32);
	
	gl::disableDepthRead();
	//gl::enableDepthWrite();
	gl::popMatrices();
}

void MechBass2App::cleanup()
{
	mDS->stop();
}

CINDER_APP(MechBass2App, RendererGl)
