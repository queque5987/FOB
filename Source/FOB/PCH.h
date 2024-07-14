#pragma once
#define COLLISION_CHANNEL_MONSTER ECollisionChannel::ECC_EngineTraceChannel1
#define COLLISION_CHANNEL_PLAYER ECollisionChannel::ECC_EngineTraceChannel2

#define PLAYER_DASHING		int32(1) << 0
#define PLAYER_AIMING		int32(1) << 1
#define PLAYER_CROUCH		int32(1) << 2