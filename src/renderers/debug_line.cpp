/*
 * Copyright (c) 2012-2015 Daniele Bartolini and individual contributors.
 * License: https://github.com/taylor001/crown/blob/master/LICENSE
 */

#include "debug_line.h"
#include "math_utils.h"
#include "color4.h"
#include "vector3.h"
#include "matrix4x4.h"
#include "error.h"
#include <string.h> // memcpy
#include <bgfx/bgfx.h>

namespace crown
{

namespace debug_line
{
#if CROWN_PLATFORM_LINUX || CROWN_PLATFORM_ANDROID
	static const uint8_t vs_h[325] =
	{
		0x56, 0x53, 0x48, 0x04, 0xa4, 0x8b, 0xef, 0x49, 0x01, 0x00, 0x0f, 0x75, 0x5f, 0x6d, 0x6f, 0x64, // VSH....I...u_mod
		0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x04, 0x01, 0x00, 0x00, 0x01, 0x00, // elViewProj......
		0x20, 0x01, 0x00, 0x00, 0x61, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65, 0x20, 0x68, 0x69, //  ...attribute hi
		0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x61, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, // ghp vec4 a_color
		0x30, 0x3b, 0x0a, 0x61, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65, 0x20, 0x68, 0x69, 0x67, // 0;.attribute hig
		0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x61, 0x5f, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, // hp vec3 a_positi
		0x6f, 0x6e, 0x3b, 0x0a, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x68, 0x69, 0x67, 0x68, // on;.varying high
		0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x3b, // p vec4 v_color0;
		0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x6d, // .uniform highp m
		0x61, 0x74, 0x34, 0x20, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, // at4 u_modelViewP
		0x72, 0x6f, 0x6a, 0x3b, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, // roj;.void main (
		0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, // ).{.  highp vec4
		0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, //  tmpvar_1;.  tmp
		0x76, 0x61, 0x72, 0x5f, 0x31, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x31, 0x2e, 0x30, 0x3b, 0x0a, 0x20, // var_1.w = 1.0;.
		0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x2e, 0x78, 0x79, 0x7a, 0x20, 0x3d, 0x20, //  tmpvar_1.xyz =
		0x61, 0x5f, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x3b, 0x0a, 0x20, 0x20, 0x67, 0x6c, // a_position;.  gl
		0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x28, 0x75, 0x5f, 0x6d, // _Position = (u_m
		0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x20, 0x2a, 0x20, 0x74, // odelViewProj * t
		0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x5f, 0x63, 0x6f, // mpvar_1);.  v_co
		0x6c, 0x6f, 0x72, 0x30, 0x20, 0x3d, 0x20, 0x61, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x3b, // lor0 = a_color0;
		0x0a, 0x7d, 0x0a, 0x0a, 0x00,                                                                   // .}...
	};

	static const uint8_t fs_h[89] =
	{
		0x46, 0x53, 0x48, 0x04, 0xa4, 0x8b, 0xef, 0x49, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x76, 0x61, // FSH....I..J...va
		0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, // rying highp vec4
		0x20, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x3b, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, //  v_color0;.void
		0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, 0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x46, // main ().{.  gl_F
		0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x76, 0x5f, 0x63, 0x6f, 0x6c, // ragColor = v_col
		0x6f, 0x72, 0x30, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00,                                           // or0;.}...
	};
#elif CROWN_PLATFORM_WINDOWS
	static const uint8_t vs_h[419] =
	{
		0x56, 0x53, 0x48, 0x03, 0xa4, 0x8b, 0xef, 0x49, 0x01, 0x00, 0x0f, 0x75, 0x5f, 0x6d, 0x6f, 0x64, // VSH....I...u_mod
		0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x09, 0x01, 0x00, 0x00, 0x04, 0x00, // elViewProj......
		0x80, 0x01, 0x00, 0x03, 0xfe, 0xff, 0xfe, 0xff, 0x23, 0x00, 0x43, 0x54, 0x41, 0x42, 0x1c, 0x00, // ........#.CTAB..
		0x00, 0x00, 0x57, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0xff, 0x01, 0x00, 0x00, 0x00, 0x1c, 0x00, // ..W.............
		0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x02, 0x00, // ......P...0.....
		0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x5f, // ......@.......u_
		0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x00, 0x03, 0x00, // modelViewProj...
		0x03, 0x00, 0x04, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x73, // ..............vs
		0x5f, 0x33, 0x5f, 0x30, 0x00, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66, 0x74, 0x20, 0x28, // _3_0.Microsoft (
		0x52, 0x29, 0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, // R) HLSL Shader C
		0x6f, 0x6d, 0x70, 0x69, 0x6c, 0x65, 0x72, 0x20, 0x39, 0x2e, 0x32, 0x39, 0x2e, 0x39, 0x35, 0x32, // ompiler 9.29.952
		0x2e, 0x33, 0x31, 0x31, 0x31, 0x00, 0x51, 0x00, 0x00, 0x05, 0x04, 0x00, 0x0f, 0xa0, 0x00, 0x00, // .3111.Q.........
		0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, // .?..............
		0x00, 0x02, 0x0a, 0x00, 0x00, 0x80, 0x00, 0x00, 0x0f, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, // ................
		0x00, 0x80, 0x01, 0x00, 0x0f, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, // ................
		0x0f, 0xe0, 0x1f, 0x00, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x80, 0x01, 0x00, 0x0f, 0xe0, 0x05, 0x00, // ................
		0x00, 0x03, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0xe4, 0xa0, 0x01, 0x00, 0x00, 0x90, 0x05, 0x00, // ................
		0x00, 0x03, 0x01, 0x00, 0x0f, 0x80, 0x01, 0x00, 0xe4, 0xa0, 0x01, 0x00, 0x55, 0x90, 0x02, 0x00, // ............U...
		0x00, 0x03, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0xe4, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x05, 0x00, // ................
		0x00, 0x03, 0x01, 0x00, 0x0f, 0x80, 0x02, 0x00, 0xe4, 0xa0, 0x01, 0x00, 0xaa, 0x90, 0x02, 0x00, // ................
		0x00, 0x03, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0xe4, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x01, 0x00, // ................
		0x00, 0x02, 0x01, 0x00, 0x0f, 0x80, 0x03, 0x00, 0xe4, 0xa0, 0x05, 0x00, 0x00, 0x03, 0x01, 0x00, // ................
		0x0f, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x04, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, // ................
		0x0f, 0x80, 0x00, 0x00, 0xe4, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, // ................
		0x0f, 0x80, 0x00, 0x00, 0xe4, 0x90, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, // ................
		0xe4, 0x80, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x0f, 0xe0, 0x01, 0x00, 0xe4, 0x80, 0xff, 0xff, // ................
		0x00, 0x00, 0x00,                                                                               // ...
	};

	static const uint8_t fs_h[137] =
	{
		0x46, 0x53, 0x48, 0x03, 0xa4, 0x8b, 0xef, 0x49, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x03, 0xff, 0xff, // FSH....I..|.....
		0xfe, 0xff, 0x16, 0x00, 0x43, 0x54, 0x41, 0x42, 0x1c, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, // ....CTAB....#...
		0x00, 0x03, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, // ................
		0x1c, 0x00, 0x00, 0x00, 0x70, 0x73, 0x5f, 0x33, 0x5f, 0x30, 0x00, 0x4d, 0x69, 0x63, 0x72, 0x6f, // ....ps_3_0.Micro
		0x73, 0x6f, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, 0x68, // soft (R) HLSL Sh
		0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x69, 0x6c, 0x65, 0x72, 0x20, 0x39, 0x2e, // ader Compiler 9.
		0x32, 0x39, 0x2e, 0x39, 0x35, 0x32, 0x2e, 0x33, 0x31, 0x31, 0x31, 0x00, 0x1f, 0x00, 0x00, 0x02, // 29.952.3111.....
		0x0a, 0x00, 0x00, 0x80, 0x00, 0x00, 0x0f, 0x90, 0x01, 0x00, 0x00, 0x02, 0x00, 0x08, 0x0f, 0x80, // ................
		0x00, 0x00, 0xe4, 0x90, 0xff, 0xff, 0x00, 0x00, 0x00,                                           // .........
	};
#endif

	static bgfx::VertexDecl s_decl;
	static bgfx::ProgramHandle s_prog;

	void init()
	{
		s_decl
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
			.end();

		bgfx::ShaderHandle vs = bgfx::createShader(bgfx::makeRef(vs_h, sizeof(vs_h)));
		CE_ASSERT(bgfx::isValid(vs), "Failed to create vertex shader");
		bgfx::ShaderHandle fs = bgfx::createShader(bgfx::makeRef(fs_h, sizeof(fs_h)));
		CE_ASSERT(bgfx::isValid(fs), "Failed to create fragment shader");
		s_prog = bgfx::createProgram(vs, fs, true);
		CE_ASSERT(bgfx::isValid(s_prog), "Failed to create program");
	}

	void shutdown()
	{
		bgfx::destroyProgram(s_prog);
	}

} // namespace debug_line

DebugLine::DebugLine(bool depth_test)
	: _depth_test(depth_test)
	, _num(0)
{
}

void DebugLine::add_line(const Vector3& start, const Vector3& end, const Color4& color)
{
	if (_num >= CROWN_MAX_DEBUG_LINES)
		 return;

	_lines[_num].p0 = start;
	_lines[_num].c0 = to_abgr(color);
	_lines[_num].p1 = end;
	_lines[_num].c1 = to_abgr(color);

	++_num;
}

void DebugLine::add_axes(const Matrix4x4& m, float length)
{
	const Vector3 pos = translation(m);
	add_line(pos, pos + x(m)*length, COLOR4_RED);
	add_line(pos, pos + y(m)*length, COLOR4_GREEN);
	add_line(pos, pos + z(m)*length, COLOR4_BLUE);
}

void DebugLine::add_circle(const Vector3& center, float radius, const Vector3& normal, const Color4& color, uint32_t segments)
{
	const Vector3 dir = normal;
	const Vector3 arr[] =
	{
		{ dir.z, dir.z, -dir.x -dir.y },
		{ -dir.y -dir.z, dir.x, dir.x }
	};
	const int idx = ((dir.z != 0.0f) && (-dir.x != dir.y));
	Vector3 right = arr[idx];
	normalize(right);

	const float incr = 360.0f / (float)(segments >= 3 ? segments : 3);
	float deg0 = 0.0f;
	for (uint32_t ss = 0; ss < segments; ++ss, deg0 += incr)
	{
		const float rad0 = to_rad(deg0);
		const float rad1 = to_rad(deg0 + incr);

		const Vector3 from0 = right*cos(-rad0) + cross(dir, right)*sin(-rad0) + dir*dot(dir, right)*(1.0f-cos(-rad0));
		const Vector3 from1 = right*cos(-rad1) + cross(dir, right)*sin(-rad1) + dir*dot(dir, right)*(1.0f-cos(-rad1));

		add_line(center + radius*from0, center + radius*from1, color);
	}
}

void DebugLine::add_cone(const Vector3& from, const Vector3& to, float radius, const Color4& color, uint32_t segments)
{
	Vector3 dir = to - from;
	normalize(dir);
	const Vector3 arr[] =
	{
		{ dir.z, dir.z, -dir.x -dir.y },
		{ -dir.y -dir.z, dir.x, dir.x }
	};
	const int idx = ((dir.z != 0.0f) && (-dir.x != dir.y));
	Vector3 right = arr[idx];
	normalize(right);

	const float incr = 360.0f / (float)(segments >= 3 ? segments : 3);
	float deg0 = 0.0f;
	for (uint32_t ss = 0; ss < segments; ++ss, deg0 += incr)
	{
		const float rad0 = to_rad(deg0);
		const float rad1 = to_rad(deg0 + incr);

		const Vector3 from0 = right*cos(-rad0) + cross(dir, right)*sin(-rad0) + dir*dot(dir, right)*(1.0f-cos(-rad0));
		const Vector3 from1 = right*cos(-rad1) + cross(dir, right)*sin(-rad1) + dir*dot(dir, right)*(1.0f-cos(-rad1));

		add_line(from + radius*from0, to, color);
		add_line(from + radius*from0, from + radius*from1, color);
	}
}

void DebugLine::add_sphere(const Vector3& center, const float radius, const Color4& color, uint32_t segments)
{
	add_circle(center, radius, VECTOR3_XAXIS, color, segments);
	add_circle(center, radius, VECTOR3_YAXIS, color, segments);
	add_circle(center, radius, VECTOR3_ZAXIS, color, segments);
}

void DebugLine::add_obb(const Matrix4x4& tm, const Vector3& half_extents, const Color4& color)
{
	const Vector3 o = vector3(tm.t.x, tm.t.y, tm.t.z);
	const Vector3 x = vector3(tm.x.x, tm.x.y, tm.x.z) * half_extents.x;
	const Vector3 y = vector3(tm.y.x, tm.y.y, tm.y.z) * half_extents.y;
	const Vector3 z = vector3(tm.z.x, tm.z.y, tm.z.z) * half_extents.z;

	// Back face
	add_line(o - x - y - z, o + x - y - z, color);
	add_line(o + x - y - z, o + x + y - z, color);
	add_line(o + x + y - z, o - x + y - z, color);
	add_line(o - x + y - z, o - x - y - z, color);

	add_line(o - x - y + z, o + x - y + z, color);
	add_line(o + x - y + z, o + x + y + z, color);
	add_line(o + x + y + z, o - x + y + z, color);
	add_line(o - x + y + z, o - x - y + z, color);

	add_line(o - x - y - z, o - x - y + z, color);
	add_line(o + x - y - z, o + x - y + z, color);
	add_line(o + x + y - z, o + x + y + z, color);
	add_line(o - x + y - z, o - x + y + z, color);
}

void DebugLine::reset()
{
	_num = 0;
}

void DebugLine::submit()
{
	if (!_num)
		return;

	if (!checkAvailTransientVertexBuffer(_num * 2, debug_line::s_decl))
		return;

	bgfx::TransientVertexBuffer tvb;
	bgfx::allocTransientVertexBuffer(&tvb, _num * 2, debug_line::s_decl);
	memcpy(tvb.data, _lines, sizeof(Line) * _num);

	bgfx::setState(BGFX_STATE_PT_LINES
		| BGFX_STATE_RGB_WRITE
		| (_depth_test ? (BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_DEPTH_WRITE) : 0)
		| BGFX_STATE_CULL_CW);
	bgfx::setVertexBuffer(&tvb, 0, _num * 2);
	bgfx::submit(0, debug_line::s_prog);
}

} // namespace crown
