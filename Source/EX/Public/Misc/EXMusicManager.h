// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "EXMusicManager.generated.h"

class USoundCue;
class UAudioComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class EX_API AEXMusicManager : public AInfo
{
	GENERATED_BODY()
	
public:
	AEXMusicManager();
	void RefreshVolume();
protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Music")
	void PlayRandom();

	UFUNCTION()
	void SongFinished();

	void LoadSongs();

	UPROPERTY(EditDefaultsOnly, Category = "Music")
	TArray<TSoftObjectPtr<USoundCue>> Songs;
	UPROPERTY()
	TArray<USoundCue*>LoadedSongs;

	UPROPERTY(BlueprintReadOnly, Category = "Music")
	UAudioComponent* AudioComp = nullptr;

	int32 SongIdx = -1;

	FTimerHandle TimerHandle_PlaySong;

private:

	UPROPERTY(BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root = nullptr;
};
