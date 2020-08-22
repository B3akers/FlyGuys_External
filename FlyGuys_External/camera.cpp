/* This file is part of FlyGuys_External by b3akers, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) b3akers 2020
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "camera.hpp"
#include "memory.hpp"
#include "imgui/imgui.h"

void camera::set_base( uint64_t b ) {
	base = b;
}

uint64_t camera::get_base( ) {
	return base;
}

bool camera::is_active( ) {
	return base;
}

void camera::update( ) {
	if ( !is_active( ) )
		return;

	memory::read_memory( base, data, unity::camera::projection_matrix + sizeof( D3DMATRIX ) );
}

bool camera::world_to_screen( const vector& world, vector& screen_out ) {
	D3DXMATRIX m_world;
	d3d_helper::tmpD3DXMatrixIdentity( &m_world );

	auto& io = ImGui::GetIO( );

	auto w = io.DisplaySize.x;
	auto h = io.DisplaySize.y;

	D3DVIEWPORT9 v_viewport;
	v_viewport.X = 0;
	v_viewport.Y = 0;
	v_viewport.Width = w;
	v_viewport.Height = h;
	v_viewport.MinZ = 1.f;
	v_viewport.MaxZ = 0.f;

	auto screen_v = D3DXVECTOR3( );
	auto world_v = D3DXVECTOR3( );
	world_v.x = world.x;
	world_v.y = world.z;
	world_v.z = world.y;

	auto projection_matrix = reinterpret_cast<D3DMATRIX*>( data + unity::camera::projection_matrix );
	auto view_matrix = reinterpret_cast<D3DMATRIX*>( data + unity::camera::view_matrix );

	d3d_helper::tmpD3DXVec3Project( &screen_v, &world_v, &v_viewport, projection_matrix, view_matrix, &m_world );

	if ( screen_v.z < 0.0001f ) {
		screen_out.x = -1;
		screen_out.y = -1;
		return false;
	}

	screen_out.x = screen_v.x / w * w;
	screen_out.y = screen_v.y / h * h;

	return true;
}