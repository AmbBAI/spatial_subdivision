#ifndef _AABB_RENDER_H_
#define _AABB_RENDER_H_

#include "common.h"
#include "bgfx_utils.h"

struct AabbVertex
{
	float m_x;
	float m_y;
	float m_z;

	static void init()
	{
		ms_decl
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.end();
	};

	static bgfx::VertexDecl ms_decl;
};

class AabbRender
{
public:
	void init();
	void close();

	void prepareRender(int _count);
	bool addInstance(const Aabb& _aabb, uint32_t _abgr);
	void submit(int _idx, const float* _mtx);

private:
	static const AabbVertex aabbVertices[8];
	static const uint16_t aabbIndices[24];

	bgfx::VertexBufferHandle vbh;
	bgfx::IndexBufferHandle ibh;
	bgfx::ProgramHandle program;

	const uint16_t instanceStride = 80;
	const bgfx::InstanceDataBuffer* idb = nullptr;
	uint32_t instanceCount = 0;
};

#endif // !_AABB_RENDER_H_
