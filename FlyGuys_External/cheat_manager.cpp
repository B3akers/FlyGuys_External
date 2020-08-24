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
#include "cheat_manager.hpp"
#include "process_search.hpp"
#include "memory.hpp"

#include "unity_sdk.hpp"
#include "offsets.hpp"
#include "fnv_hash.hpp"

#include "overlay.hpp"
#include "draw_manager.h"
#include "menu.hpp"

#include <iostream>
#include <chrono>
#include <thread>

//todo make seperate classes for features

using namespace std::chrono_literals;

uint64_t cheat_manager::game_assembly;
uint64_t cheat_manager::unity_player;
camera cheat_manager::main_camera;

std::vector<std::pair<vector, uint64_t>> real_door_list;
std::vector<vector> tip_toe_list;

void cheat_manager::start( ) {
	std::printf( "Looking for Fall Guys process...\n" );

	auto list = process_search::find_processes( "FallGuys_client.exe" );

	while ( list.empty( ) ) {
		std::this_thread::sleep_for( 5s );
		list = process_search::find_processes( "FallGuys_client.exe" );
	}

	memory::attach( list.front( ) );

	game_assembly = memory::get_module( "GameAssembly.dll" );
	unity_player = memory::get_module( "UnityPlayer.dll" );

	unity_sdk::read_memory = memory::read_memory;
	unity_sdk::write_memory = memory::write_memory;

	std::printf( "Process found. Loop started!.\n" );

	overlay::game_window = FindWindowA( nullptr, "FallGuys_client" );
	overlay::init_window_create_device( );
	overlay::start_render_loop( );
}

const float default_max_speed = 9.500000f;

const float default_anyCollisionStunForce = 28.000000f;
const float default_dynamicCollisionStunForce = 10.000000f;
const float default_stumbleBackAngle = 80.0f;
const float default_fallOverAngle = 30.0f;
const float default_collisionPlayerToPlayerUnpinMultiplier = 0.200000003f;
const float default_minStunTime = 0.1000000015f;
const float default_maxGroundedStunTime = 5.0f;
const float default_SmallImpactMinForceThreshold = 8.0f;
const float default_MediumImpactMinForceThreshold = 10.0f;
const float default_LargeImpactMinForceThreshold = 2000.0f;
const float default_CollisionThreshold = 14.000000f;
const float default_carryMaxSpeed = 8.000000f;
const float default_carryPickupDuration = 0.100000f;

const float default_playerGrabDetectRadius = 6.0f;
const float default_playerGrabCheckDistance = 2.0f;
const float default_playerGrabberMaxForce = 0.6000000238f;
const float default_playerGrabBreakTime = 1.200000048f;
const float default_armLength = 1.0f;
const float default_playerGrabCheckPredictionBase = 0.1000000015f;
const float default_playerGrabImmediateVelocityReduction = 0.5f;
const float default_playerGrabberDragDirectionContribution = 0.50f;
const float default_grabCooldown = 0.5f;
const float default_playerGrabRegrabDelay = 2.0f;
const float default_playerGrabBreakTimeJumpInfluence = 0.01999999955f;
const float default_forceReleaseRegrabCooldown = 1.0f;
const float default_breakGrabAngle = 75.0f;
const float default_playerGrabbeeMaxForce = 1.0f;
const float default_playerGrabBreakSeparationForce = 7.0f;
const float default_playerGrabbeeInvulnerabilityWindow = 1.5f;
const float default_objectGrabAdditionalForceScale = 10.0f;
const float default_objectGrabAdditionalPushForceScale = 3.0f;
const float default_carryAlwaysLoseTussleWhenGrabbed = 1.0f;
const float default_playerGrabberVelocityComponent = 0.1000000015f;
const float default_playerGrabbeeVelocityComponent = 0.200000003f;

void cheat_manager::main_loop( ) {
	static bool update_objects = true;

	auto global_state_ptr = unity_sdk::read_memory_wrapper<uint64_t>( game_assembly + offsets::signatures::global_game_state );
	if ( !global_state_ptr )
		return;

	auto global = unity_sdk::read_memory_wrapper<MPG_Utility_Singleton_GlobalGameStateClient__c>( global_state_ptr );
	auto instance = global.static_fields( ).s_Instance( );
	if ( !instance.klass_ptr )
		return;

	auto game_state_machine = instance.fields._gameStateMachine( );
	if ( !game_state_machine.klass_ptr )
		return;

	auto current_state = game_state_machine.fields._currentState( );
	if ( !current_state.klass_ptr )
		return;

	const constexpr auto StateGameInProgress = fnv::hash_constexpr( "StateGameInProgress" );
	const constexpr auto round_door_dash = fnv::hash_constexpr( "round_door_dash" );
	const constexpr auto round_tip_toe = fnv::hash_constexpr( "round_tip_toe" );

	if ( fnv::hash_runtime( current_state.klass( )._1.name( ).c_str( ) ) == StateGameInProgress ) {
		auto state_game_in_progress = unity_sdk::read_memory_wrapper<FGClient_StateGameInProgress_o>( game_state_machine.fields._currentState_ptr );

		auto client_game_manager = state_game_in_progress.fields._clientGameManager( );
		if ( !client_game_manager.klass_ptr )
			return;

		auto player_list = client_game_manager.fields._players( );
		if ( !player_list.klass_ptr )
			return;

		auto game_state_view = instance.fields._GameStateView_k__BackingField( );
		if ( !game_state_view.klass_ptr )
			return;

		auto current_game_level = game_state_view.fields.CurrentGameLevelName( );
		if ( !current_game_level.klass_ptr )
			return;

		auto current_game_level_native = unity_sdk::read_mono_string( current_game_level, game_state_view.fields.CurrentGameLevelName_ptr );
		auto game_level = fnv::whash_runtime( current_game_level_native.c_str( ) );

		//we don't need performance here, we can just spam rpm 
		if ( update_objects ) {
			update_objects = false;

			real_door_list.clear( );
			tip_toe_list.clear( );

			main_camera.set_base( 0 );
			auto gameobjectmanager = unity_sdk::read_memory_wrapper<uint64_t>( unity_player + offsets::signatures::game_object_manager );
			for ( auto i = unity_sdk::read_memory_wrapper<uint64_t>( gameobjectmanager + 0x8 ); i != gameobjectmanager; i = unity_sdk::read_memory_wrapper<uint64_t>( i + 0x8 ) ) {
				if ( !i )
					break;

				auto game_object_ptr = unity_sdk::read_memory_wrapper<uint64_t>( i + 0x10 );

				if ( !game_object_ptr )
					continue;

				if ( unity_sdk::read_memory_wrapper<int16_t>( game_object_ptr + unity::object_tag ) != 5 )
					continue;

				auto component_size = unity_sdk::read_memory_wrapper<int32_t>( game_object_ptr + unity::components_size );
				auto components_ptr = unity_sdk::read_memory_wrapper<uintptr_t>( game_object_ptr + unity::components_ptr );

				for ( auto compoment_index = 0; compoment_index < component_size; compoment_index++ ) {
					auto current_compoment = unity_sdk::read_memory_wrapper<uintptr_t>( components_ptr + ( compoment_index * 0x10 ) + 0x8 );
					if ( !current_compoment )
						continue;

					auto compoment_mono = unity_sdk::read_memory_wrapper<Il2CppObject>( unity_sdk::read_memory_wrapper<uint64_t>( current_compoment + unity::mono_ptr ) );
					if ( !compoment_mono.klass_ptr )
						continue;

					const constexpr auto Camera = fnv::hash_constexpr( "Camera" );
					if ( fnv::hash_runtime( compoment_mono.klass( )._1.name( ).c_str( ) ) != Camera )
						continue;

					if ( unity_sdk::read_memory_wrapper<bool>( current_compoment + unity::camera::enabled ) ) {
						main_camera.set_base( current_compoment );
						break;
					}
				}

				if ( cheat_manager::main_camera.is_active( ) )
					break;
			}

			if ( game_level == round_door_dash ||
				game_level == round_tip_toe ) {
				for ( auto i = unity_sdk::read_memory_wrapper<uint64_t>( gameobjectmanager + 0x18 ); i != ( gameobjectmanager + 0x10 ); i = unity_sdk::read_memory_wrapper<uint64_t>( i + 0x8 ) ) {
					if ( !i )
						break;

					auto game_object_ptr = unity_sdk::read_memory_wrapper<uint64_t>( i + 0x10 );

					if ( !game_object_ptr )
						continue;

					auto component_size = unity_sdk::read_memory_wrapper<int32_t>( game_object_ptr + unity::components_size );
					auto components_ptr = unity_sdk::read_memory_wrapper<uintptr_t>( game_object_ptr + unity::components_ptr );

					if ( component_size > 1 && components_ptr ) {

						uint64_t real_door = 0;
						uint64_t tiptoe_platform = 0;

						for ( auto compoment_index = 0; compoment_index < component_size; compoment_index++ ) {
							auto current_compoment = unity_sdk::read_memory_wrapper<uintptr_t>( components_ptr + ( compoment_index * 0x10 ) + 0x8 );
							if ( !current_compoment )
								continue;

							auto compoment_mono_ptr = unity_sdk::read_memory_wrapper<uint64_t>( current_compoment + unity::mono_ptr );

							auto compoment_mono = unity_sdk::read_memory_wrapper<Il2CppObject>( compoment_mono_ptr );
							if ( !compoment_mono.klass_ptr )
								continue;

							auto class_name = fnv::hash_runtime( compoment_mono.klass( )._1.name( ).c_str( ) );

							switch ( class_name ) {
								case fnv::hash_constexpr( "RealDoorController" ):
									real_door = compoment_mono_ptr;
									break;
								case fnv::hash_constexpr( "TipToe_Platform" ):
									tiptoe_platform = compoment_mono_ptr;
									break;
							}
						}

						if ( real_door ) {
							auto transform = unity_sdk::read_memory_wrapper<uint64_t>( unity_sdk::read_memory_wrapper<uint64_t>( unity_sdk::read_memory_wrapper<uint64_t>( unity_sdk::read_memory_wrapper<uint64_t>( real_door + unity::native_ptr ) + unity::compoment_owner ) + unity::components_ptr ) + unity::transform_compoment );
							real_door_list.push_back( { unity_sdk::get_position( transform ), real_door } );
						} else if ( tiptoe_platform ) {
							auto tiptoe_platform_mono = unity_sdk::read_memory_wrapper<TipToe_Platform_o>( tiptoe_platform );
							if ( !tiptoe_platform_mono.fields._isFakePlatform )
								tip_toe_list.push_back( unity_sdk::get_position( unity_sdk::read_memory_wrapper<uint64_t>( unity_sdk::read_memory_wrapper<uint64_t>( unity_sdk::read_memory_wrapper<uint64_t>( unity_sdk::read_memory_wrapper<uint64_t>( real_door + unity::native_ptr ) + unity::compoment_owner ) + unity::components_ptr ) + unity::transform_compoment ) ) );
						}
					}
				}
			}
		}

		if (! cheat_manager::main_camera.is_active( ) )
			return;

		cheat_manager::main_camera.update( );

		for ( auto it = real_door_list.begin( ); it != real_door_list.end( ); it++ ) {
			auto door = unity_sdk::read_memory_wrapper<RealDoorController_o>( it->second );

			if ( door.fields._hasBroken ) {
				real_door_list.erase( it-- );
			} else {
				vector screen;
				if ( cheat_manager::main_camera.world_to_screen( it->first, screen ) )
					draw_manager::add_filled_rect_on_screen( screen, screen + vector( 20, 20 ), ImColor( 0.f, 0.f, 1.f ) );
			}
		}

		for ( auto& it : tip_toe_list ) {
			vector screen;
			if ( cheat_manager::main_camera.world_to_screen( it, screen ) )
				draw_manager::add_filled_rect_on_screen( screen, screen + vector( 15, 15 ), ImColor( 1.f, 0.f, 0.f ) );
		}

		auto character_data_monitor = client_game_manager.fields._characterDataMonitor( );
		character_data_monitor.fields._timeToRunNextCharacterControllerDataCheck = FLT_MAX;
		save_class( character_data_monitor, client_game_manager.fields._characterDataMonitor_ptr );

		auto player_list_items = System_Collections_Generic_Dictionary_Entry_TKey__TValue__array::m_Items( player_list.fields.entries_ptr, player_list.fields.count );
		for ( auto i = 0; i < player_list.fields.count; i++ ) {
			auto character_ptr = player_list_items[ i ].fields.value_ptr;
			auto character = unity_sdk::read_memory_wrapper<FallGuysCharacterController_o>( character_ptr );
			if ( !character.klass_ptr )
				continue;

			if ( character.fields._IsLocalPlayer_k__BackingField ) {
				auto character_data = character.fields._data( );

				if ( settings::movement::fly_enabled ) {
					character.fields._ApplyGravity_k__BackingField = false;
					character_data.fields.carryMaxSpeed = settings::movement::fly_speed;
					auto camera_transform = unity_sdk::read_memory_wrapper<uint64_t> ( unity_sdk::read_memory_wrapper<uint64_t>( unity_sdk::read_memory_wrapper<uint64_t> ( main_camera.get_base( ) + unity::compoment_owner ) + unity::components_ptr ) + unity::transform_compoment );
					auto direction = unity_sdk::get_forward( camera_transform );

					auto movement = unity_sdk::read_memory_wrapper<uint64_t> ( unity_sdk::read_memory_wrapper<uint64_t> ( character.fields._rigidbody_ptr  + unity::native_ptr ) + 0x60 );
					auto veloctiy = unity_sdk::read_memory_wrapper<vector> ( movement + 0x15C );

					if ( GetAsyncKeyState( 0x57 ) )
						veloctiy = direction * settings::movement::fly_speed;
					else
						veloctiy = vector( 0, 0, 0 );

					if ( GetAsyncKeyState( VK_SPACE ) )
						veloctiy.z = settings::movement::fly_speed;
					else if ( GetAsyncKeyState( VK_SHIFT ) )
						veloctiy.z = -settings::movement::fly_speed;
					else
						veloctiy.z = 0.f;

					unity_sdk::write_memory( movement + 0x15C, &veloctiy, sizeof( vector ) );
					save_class( character, character_ptr );
				} else if ( cheat_helper::disable_fly ) {
					character_data.fields.carryMaxSpeed = default_carryMaxSpeed;
					character.fields._ApplyGravity_k__BackingField = true;
					cheat_helper::disable_fly = false;

					save_class( character, character_ptr );
				}

				if ( settings::cheat::super_grab_enabled ) {
					character_data.fields.playerGrabDetectRadius = FLT_MAX;
					character_data.fields.playerGrabCheckDistance = FLT_MAX;
					character_data.fields.playerGrabberMaxForce = FLT_MAX;
					character_data.fields.playerGrabBreakTime = FLT_MAX;
					character_data.fields.armLength = FLT_MAX;
					character_data.fields.playerGrabCheckPredictionBase = FLT_MAX;
					character_data.fields.playerGrabMaxHeightDifference = FLT_MAX;
					character_data.fields.objectGrabAdditionalForceScale = FLT_MAX;
					character_data.fields.objectGrabAdditionalPushForceScale = FLT_MAX;
					character_data.fields.playerGrabbeeMaxForce = 0;
					character_data.fields.playerGrabImmediateVelocityReduction = 1;
					character_data.fields.playerGrabberDragDirectionContribution = 1;
					character_data.fields.grabCooldown = 0;
					character_data.fields.playerGrabRegrabDelay = 0;
					character_data.fields.playerGrabBreakTimeJumpInfluence = 0;
					character_data.fields.forceReleaseRegrabCooldown = 0;
					character_data.fields.breakGrabAngle = 360;
					character_data.fields.playerGrabBreakSeparationForce = 0.f;
					character_data.fields.playerGrabbeeInvulnerabilityWindow = 0.f;
					character_data.fields.carryPickupDuration = 0.f;
					character_data.fields.carryAlwaysLoseTussleWhenGrabbed = 0.f;
					character_data.fields.playerGrabberVelocityComponent = 1 - settings::cheat::grabber_velocity;
					character_data.fields.playerGrabbeeVelocityComponent = settings::cheat::grabber_velocity;
				} else {
					character_data.fields.playerGrabDetectRadius = default_playerGrabDetectRadius;
					character_data.fields.playerGrabCheckDistance = default_playerGrabCheckDistance;
					character_data.fields.playerGrabberMaxForce = default_playerGrabberMaxForce;
					character_data.fields.playerGrabBreakTime = default_playerGrabBreakTime;
					character_data.fields.armLength = default_armLength;
					character_data.fields.playerGrabCheckPredictionBase = default_playerGrabCheckPredictionBase;
					character_data.fields.playerGrabImmediateVelocityReduction = default_playerGrabImmediateVelocityReduction;
					character_data.fields.playerGrabberDragDirectionContribution = default_playerGrabberDragDirectionContribution;
					character_data.fields.grabCooldown = default_grabCooldown;
					character_data.fields.playerGrabRegrabDelay = default_playerGrabRegrabDelay;
					character_data.fields.playerGrabBreakTimeJumpInfluence = default_playerGrabBreakTimeJumpInfluence;
					character_data.fields.forceReleaseRegrabCooldown = default_forceReleaseRegrabCooldown;
					character_data.fields.breakGrabAngle = default_breakGrabAngle;
					character_data.fields.playerGrabbeeMaxForce = default_playerGrabbeeMaxForce;
					character_data.fields.playerGrabBreakSeparationForce = default_playerGrabBreakSeparationForce;
					character_data.fields.playerGrabbeeInvulnerabilityWindow = default_playerGrabbeeInvulnerabilityWindow;
					character_data.fields.objectGrabAdditionalForceScale = default_objectGrabAdditionalForceScale;
					character_data.fields.objectGrabAdditionalPushForceScale = default_objectGrabAdditionalPushForceScale;
					character_data.fields.carryPickupDuration = default_carryPickupDuration;
					character_data.fields.carryAlwaysLoseTussleWhenGrabbed = default_carryAlwaysLoseTussleWhenGrabbed;
					character_data.fields.playerGrabberVelocityComponent = default_playerGrabberVelocityComponent;
					character_data.fields.playerGrabbeeVelocityComponent = default_playerGrabbeeVelocityComponent;
				}

				if ( settings::movement::disable_stun_collision ) {
					character_data.fields.stumbleBackAngle = 0.f;
					character_data.fields.fallOverAngle = 0.f;
					character_data.fields.collisionPlayerToPlayerUnpinMultiplier = 0.f;
					character_data.fields.minStunTime = 0.f;
					character_data.fields.maxGroundedStunTime = 0.f;
					character_data.fields.SmallImpactMinForceThreshold = 0.f;
					character_data.fields.MediumImpactMinForceThreshold = 0.f;
					character_data.fields.LargeImpactMinForceThreshold = 0.f;
					character_data.fields.anyCollisionStunForce = FLT_MAX;
					character_data.fields.dynamicCollisionStunForce = FLT_MAX;
				} else {
					character_data.fields.stumbleBackAngle = default_stumbleBackAngle;
					character_data.fields.fallOverAngle = default_fallOverAngle;
					character_data.fields.collisionPlayerToPlayerUnpinMultiplier = default_collisionPlayerToPlayerUnpinMultiplier;
					character_data.fields.minStunTime = default_minStunTime;
					character_data.fields.maxGroundedStunTime = default_maxGroundedStunTime;
					character_data.fields.SmallImpactMinForceThreshold = default_SmallImpactMinForceThreshold;
					character_data.fields.MediumImpactMinForceThreshold = default_MediumImpactMinForceThreshold;
					character_data.fields.LargeImpactMinForceThreshold = default_LargeImpactMinForceThreshold;
					character_data.fields.anyCollisionStunForce = default_anyCollisionStunForce;
					character_data.fields.dynamicCollisionStunForce = default_dynamicCollisionStunForce;
				}

				auto character_ragdoll = character.fields._ragdollController( );
				character_ragdoll.fields.CollisionThreshold = settings::movement::disable_object_collisions ? FLT_MAX : default_CollisionThreshold;
				if ( settings::movement::disable_object_collisions )
					character_ragdoll.fields.CollisionUpperBodyTransfer = 0.f;

				if ( settings::movement::speed_enabled ) {
					character_data.fields.carryMaxSpeed = settings::movement::speed_boost;
					character_data.fields.normalMaxSpeed = settings::movement::speed_boost;

				} else if ( cheat_helper::disable_speed ) {
					character_data.fields.carryMaxSpeed = default_carryMaxSpeed;
					character_data.fields.normalMaxSpeed = default_max_speed;
					cheat_helper::disable_speed = false;
				}

				save_class( character_ragdoll, character.fields._ragdollController_ptr );
				save_class( character_data, character.fields._data_ptr );
			}
		}

	} else
		update_objects = true;
}