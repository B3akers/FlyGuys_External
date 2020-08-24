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
#pragma once
namespace offsets {
	namespace signatures {
		enum {
			game_object_manager = 0x17B70E8,
			global_game_state = 45900040, //Class$MPG.Utility.Singleton<GlobalGameStateClient>
		};
	}
}

namespace unity {
	enum {
		native_ptr = 0x10,
		compoment_owner = 0x30,
		components_size = 0x40,
		components_ptr = 0x30,
		mono_ptr = 0x28,
		transform_compoment = 0x8,
		object_tag = 0x54,
	};

	namespace camera {
		enum {
			enabled = 0x38, //Behaviour::GetEnabled
			projection_matrix = 0x9C, //Camera::GetProjectionMatrix
			view_matrix = 0x5C, //Camera::GetWorldToCameraMatrix
		};
	};
};