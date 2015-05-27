/*
* Copyright 2011-2015 Branimir Karadzic. All rights reserved.
* License: http://www.opensource.org/licenses/BSD-2-Clause
*/

#include "common.h"
#include "bgfx_utils.h"
#include <vector>
using namespace std;

struct PosColorVertex
{
	float m_x;
	float m_y;
	float m_z;
	uint32_t m_abgr;

	static void init()
	{
		ms_decl
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.end();
	};

	static bgfx::VertexDecl ms_decl;
};

bgfx::VertexDecl PosColorVertex::ms_decl;

int _main_(int /*_argc*/, char** /*_argv*/)
{
	uint32_t width = 1280;
	uint32_t height = 720;
	uint32_t debug = BGFX_DEBUG_TEXT;
	uint32_t reset = BGFX_RESET_VSYNC;

	bgfx::init(bgfx::RendererType::OpenGL);
	bgfx::reset(width, height, reset);

	// Enable debug text.
	bgfx::setDebug(debug);

	// Set view 0 clear state.
	bgfx::setViewClear(0
		, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
		, 0x303030ff
		, 1.0f
		, 0
		);

	Mesh* mesh = meshLoad("meshes/bunny.bin");
	bgfx::ProgramHandle program = loadProgram("vs_default", "fs_default");

	Aabb aabb = mesh->m_groups[0].m_aabb;
	for (auto g : mesh->m_groups)
	{
		aabb.m_min[0] = bx::fmin(g.m_aabb.m_min[0], aabb.m_min[0]);
		aabb.m_min[1] = bx::fmin(g.m_aabb.m_min[1], aabb.m_min[1]);
		aabb.m_min[2] = bx::fmin(g.m_aabb.m_min[2], aabb.m_min[2]);
		aabb.m_max[0] = bx::fmin(g.m_aabb.m_max[0], aabb.m_max[0]);
		aabb.m_max[1] = bx::fmin(g.m_aabb.m_max[1], aabb.m_max[1]);
		aabb.m_max[2] = bx::fmin(g.m_aabb.m_max[2], aabb.m_max[2]);
	}
	 
	PosColorVertex meshVertices[8] = {
		{ aabb.m_min[0], aabb.m_min[1], aabb.m_min[2], 0xffffff },
		{ aabb.m_min[0], aabb.m_min[1], aabb.m_max[2], 0xffffff },
		{ aabb.m_min[0], aabb.m_max[1], aabb.m_min[2], 0xffffff },
		{ aabb.m_min[0], aabb.m_max[1], aabb.m_max[2], 0xffffff },
		{ aabb.m_max[0], aabb.m_min[1], aabb.m_min[2], 0xffffff },
		{ aabb.m_max[0], aabb.m_min[1], aabb.m_max[2], 0xffffff },
		{ aabb.m_max[0], aabb.m_max[1], aabb.m_min[2], 0xffffff },
		{ aabb.m_max[0], aabb.m_max[1], aabb.m_max[2], 0xffffff }
	};

	uint16_t meshIndices[24] = {
		0, 1, /**/ 0, 2, /**/ 1, 3, /**/ 2, 3,
		4, 5, /**/ 4, 6, /**/ 5, 7, /**/ 6, 7,
		0, 4, /**/ 1, 5, /**/ 2, 6, /**/ 3, 7
	};

	// Create vertex stream declaration.
	PosColorVertex::init();

	// Create static vertex buffer.
	bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
		// Static data can be passed with bgfx::makeRef
		bgfx::makeRef(meshVertices, sizeof(meshVertices))
		, PosColorVertex::ms_decl
		);

	// Create static index buffer.
	bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
		// Static data can be passed with bgfx::makeRef
		bgfx::makeRef(meshIndices, sizeof(meshIndices))
		);

	bgfx::ProgramHandle lineProgram = loadProgram("vs_line", "fs_line");

	while (!entry::processEvents(width, height, debug, reset))
	{
		// Set view 0 default viewport.
		bgfx::setViewRect(0, 0, 0, width, height);

		// This dummy draw call is here to make sure that view 0 is cleared
		// if no other draw calls are submitted to view 0.
		bgfx::submit(0);

		int64_t now = bx::getHPCounter();
		static int64_t last = now;
		const int64_t frameTime = now - last;
		last = now;
		const double freq = double(bx::getHPFrequency());
		const double toMs = 1000.0 / freq;

		// Use debug font to print information about this example.
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 1, 0x4f, "hello world");
		bgfx::dbgTextPrintf(0, 2, 0x0f, "Frame: % 7.3f[ms]", double(frameTime)*toMs);

		float at[3] = { 0.0f, 1.0f, 0.0f };
		float eye[3] = { 0.0f, 1.0f, -2.5f };

		// Set view and projection matrix for view 0.
		const bgfx::HMD* hmd = bgfx::getHMD();
		if (NULL != hmd)
		{
			float view[16];
			bx::mtxQuatTranslationHMD(view, hmd->eye[0].rotation, eye);

			float proj[16];
			bx::mtxProj(proj, hmd->eye[0].fov, 0.1f, 100.0f);

			bgfx::setViewTransform(0, view, proj);

			// Set view 0 default viewport.
			//
			// Use HMD's width/height since HMD's internal frame buffer size
			// might be much larger than window size.
			bgfx::setViewRect(0, 0, 0, hmd->width, hmd->height);
		}
		else
		{
			float view[16];
			bx::mtxLookAt(view, eye, at);

			float proj[16];
			bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 100.0f);
			bgfx::setViewTransform(0, view, proj);

			// Set view 0 default viewport.
			bgfx::setViewRect(0, 0, 0, width, height);
		}

		float mtx[16];
		bx::mtxRotateY(mtx, 90.f);
		meshSubmit(mesh, 0, program, mtx);

		bgfx::setTransform(mtx);
		bgfx::setVertexBuffer(vbh);
		bgfx::setIndexBuffer(ibh);
		bgfx::setProgram(lineProgram);
		bgfx::setState(BGFX_STATE_PT_LINES
			| BGFX_STATE_RGB_WRITE
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_MSAA);
		bgfx::submit(0);

		// Advance to next frame. Rendering thread will be kicked to 
		// process submitted rendering primitives.
		bgfx::frame();
	}

	meshUnload(mesh);
	bgfx::destroyProgram(program);

	// Shutdown bgfx.
	bgfx::shutdown();

	return 0;
}
