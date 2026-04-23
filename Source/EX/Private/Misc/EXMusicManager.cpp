// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXMusicManager.h"
#include "Player/EXPersistentUser.h"
#include "Player/EXLocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "System/EXGameplayStatics.h"

AEXMusicManager::AEXMusicManager()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
}

void AEXMusicManager::RefreshVolume()
{
	if (AudioComp)
	{
		float VolumeMultiplier = UEXGameplayStatics::GetMusicVolume() * UEXGameplayStatics::GetMasterVolume();
		AudioComp->SetVolumeMultiplier(VolumeMultiplier);
	}
	else
	{
		PlayRandom();
	}
}

void AEXMusicManager::BeginPlay()
{
	Super::BeginPlay();

	if (IsNetMode(NM_DedicatedServer))
	{
		return;
	}
	LoadSongs();
}

void AEXMusicManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AEXMusicManager::PlayRandom()
{
	if (LoadedSongs.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No song loaded."));
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_PlaySong, this, &AEXMusicManager::PlayRandom, 1.0f, false);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("Playing a song."));

	if (SongIdx == -1) // First song, pick at random
	{
		SongIdx = FMath::RandRange(0, LoadedSongs.Num() - 1);
	}
	else // Pick a random song that isn't the one that was just played
	{
		int32 NewSongIdx = FMath::RandRange(0, LoadedSongs.Num() - 2);
		SongIdx = (NewSongIdx >= SongIdx) ? SongIdx = NewSongIdx + 1 : SongIdx = NewSongIdx;
	}

	float VolumeMultiplier = UEXGameplayStatics::GetMusicVolume() * UEXGameplayStatics::GetMasterVolume();
	if (VolumeMultiplier <= KINDA_SMALL_NUMBER)
	{
		UE_LOG(LogTemp, Log, TEXT("Songs muted."));
		return;
	}
	AudioComp = UGameplayStatics::SpawnSound2D(this, LoadedSongs[SongIdx], VolumeMultiplier);
	if (AudioComp)
	{
		AudioComp->OnAudioFinished.AddDynamic(this, &AEXMusicManager::SongFinished);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error playing song."));
	}
}

void AEXMusicManager::SongFinished()
{
	AudioComp = nullptr;
	PlayRandom();
}

void AEXMusicManager::LoadSongs()
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	for (const auto& SC : Songs)
	{
		LoadedSongs.Add(StreamableManager.LoadSynchronous(SC));
	}
}
