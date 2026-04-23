#pragma once

#include "Net/UnrealNetwork.h"

namespace PushReplicationParams
{
	extern EX_API const FDoRepLifetimeParams Default;
	extern EX_API const FDoRepLifetimeParams InitialOnly;
	extern EX_API const FDoRepLifetimeParams OwnerOnly;
	extern EX_API const FDoRepLifetimeParams SkipOwner;
	extern EX_API const FDoRepLifetimeParams SimulatedOnly;
	extern EX_API const FDoRepLifetimeParams AutonomousOnly;
	extern EX_API const FDoRepLifetimeParams SimulatedOrPhysics;
	extern EX_API const FDoRepLifetimeParams InitialOrOwner;
	extern EX_API const FDoRepLifetimeParams Custom;
	extern EX_API const FDoRepLifetimeParams ReplayOrOwner;
	extern EX_API const FDoRepLifetimeParams ReplayOnly;
	extern EX_API const FDoRepLifetimeParams SimulatedOnlyNoReplay;
	extern EX_API const FDoRepLifetimeParams SimulatedOrPhysicsNoReplay;
	extern EX_API const FDoRepLifetimeParams SkipReplay;
	extern EX_API const FDoRepLifetimeParams Never;
}