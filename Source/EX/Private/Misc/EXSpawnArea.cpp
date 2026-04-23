// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/EXSpawnArea.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

AEXSpawnArea::AEXSpawnArea()
{
	bNetLoadOnClient = false;
}

APlayerStart* AEXSpawnArea::GetPlayerStart(int32 Idx)
{
	if (PlayerStarts.IsValidIndex(Idx))
	{
		return PlayerStarts[Idx];
	}
	else
	{
		return nullptr;
	}
}

void AEXSpawnArea::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	for (AActor* Actor : AttachedActors)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(Actor);
		if (PlayerStart)
		{
			PlayerStarts.Add(PlayerStart);
		}
	}
}
