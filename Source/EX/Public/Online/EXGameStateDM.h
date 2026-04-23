// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXGameStateBase.h"
#include "EXGameStateDM.generated.h"

class AEXGameModeDM;

/**
 * 
 */
UCLASS()
class EX_API AEXGameStateDM : public AEXGameStateBase
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	FGameResultDM SerializeGameScore() const;

	virtual void SendGameResult() const override;


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual TArray<AEXPlayerState*> GetSpectatorMembers() const override;

	void AddSpectator(AEXPlayerState* PS);
	void RemoveSpectator(AEXPlayerState* PS);

protected:
	virtual void CreateCurrentState(const FName& NewState, int32 NewStateDuration) override;

	UFUNCTION()
	void OnRep_CurrentMatchState();

	UPROPERTY(ReplicatedUsing = OnRep_CurrentMatchState)
	FEXMatchStateDM CurrentMatchStateDM;

	UPROPERTY(Replicated)
	TArray<AEXPlayerState*> Spectators;


private:
	AEXGameModeDM* GM = nullptr;
};
