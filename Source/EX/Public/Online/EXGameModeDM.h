// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online/EXGameModeBase.h"
#include "EXGameModeDM.generated.h"

class APlayerStart;

/**
 * 
 */
UCLASS()
class EX_API AEXGameModeDM : public AEXGameModeBase
{
	GENERATED_BODY()
	
public:

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void InitGameState() override;

	//~ Begin AGameModeBase Interface
	virtual int32 GetNumPlayers() override;
	virtual int32 GetNumSpectators() override;
	//~ End AGameModeBase Interface

	virtual void AddToSpectators(AEXPlayerController* PC);
	virtual void SwitchTeam(AEXPlayerController* PC);

	virtual void CharacterDied(AEXPlayerController* PC) override;

	void ChangeState(FEXMatchState* InMatchState) override;

protected:
	virtual void BeginPlay() override;

	TArray<AEXPlayerController*> Spectators;

private:
	class AEXGameStateDM* GSB = nullptr;

};
