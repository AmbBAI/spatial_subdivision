#include "aabb_render.h"
using namespace bgfx;

bgfx::VertexDecl AabbVertex::ms_decl;

const AabbVertex AabbRender::aabbVertices[8] = {
	{ -0.5f, -0.5f, -0.5f },
	{ -0.5f, -0.5f, 0.5f },
	{ -0.5f, 0.5f, -0.5f },
	{ -0.5f, 0.5f, 0.5f },
	{ 0.5f, -0.5f, -0.5f },
	{ 0.5f, -0.5f, 0.5f },
	{ 0.5f, 0.5f, -0.5f },
	{ 0.5f, 0.5f, 0.5f }
};;

const uint16_t AabbRender::aabbIndices[24] = {
	0, 1, /**/ 0, 2, /**/ 1, 3, /**/ 2, 3,
	4, 5, /**/ 4, 6, /**/ 5, 7, /**/ 6, 7,
	0, 4, /**/ 1, 5, /**/ 2, 6, /**/ 3, 7
};

void AabbRender::init()
{
	AabbVertex::init();

	const bgfx::Memory* mem = nullptr;
	mem = bgfx::makeRef(aabbVertices, sizeof(aabbVertices));
	vbh = bgfx::createVertexBuffer(mem, AabbVertex::ms_decl);
	mem = bgfx::makeRef(aabbIndices, sizeof(aabbIndices));
	ibh = bgfx::createIndexBuffer(mem);

	program = loadProgram("vs_line", "fs_line");
}

void AabbRender::close()
{
	bgfx::destroyVertexBuffer(vbh);
	bgfx::destroyIndexBuffer(ibh);
	bgfx::destroyProgram(program);
}

void AabbRender::prepareRender(int _count)
{
	BX_CHECK(idb == nullptr, "idb is not empty!");

	idb = bgfx::allocInstanceDataBuffer(_count, instanceStride);
	instanceCount = 0;
}

bool AabbRender::addInstance(const Aabb& _aabb, uint32_t _abgr)
{
	BX_CHECK(idb != nullptr, "idb is null!");
	if (instanceCount >= idb->num) return false;

	uint8_t* data = idb->data + instanceCount * instanceStride;

	float* mtx = (float*)data;
	float len[3] = {
		_aabb.m_max[0] - _aabb.m_min[0],
		_aabb.m_max[1] - _aabb.m_min[1],
		_aabb.m_max[2] - _aabb.m_min[2] };
	bx::mtxScale(mtx, len[0], len[1], len[2]);
	mtx[12] = len[0] / 2.f + _aabb.m_min[0];
	mtx[13] = len[1] / 2.f + _aabb.m_min[1];
	mtx[14] = len[2] / 2.f + _aabb.m_min[2];

	float* color = (float*)&data[64];
	color[0] = (_abgr & 0xff) / 255.f;
	color[1] = ((_abgr >> 8) & 0xff) / 255.f;
	color[2] = ((_abgr >> 16) & 0xff) / 255.f;
	color[3] = ((_abgr >> 24) & 0xff) / 255.f;

	instanceCount += 1;
	return true;
}

void AabbRender::submit(int _idx, const float* _mtx)
{
	bgfx::setTransform(_mtx);
	bgfx::setProgram(program);
	bgfx::setVertexBuffer(vbh);
	bgfx::setIndexBuffer(ibh);
	bgfx::setInstanceDataBuffer(idb);
	bgfx::setState(BGFX_STATE_PT_LINES
		| BGFX_STATE_RGB_WRITE
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_MSAA);
	bgfx::submit(_idx);

	idb = nullptr;
}


