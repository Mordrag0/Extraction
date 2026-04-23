// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "EXTypes.h"
#include "EX.h"
#include "EXTeam.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamChangedSignature, AEXTeam*, Team);

class AEXSpawnArea;
class AEXPlayerController;

/**
 * 
 */

UCLASS(notplaceable)
class EX_API AEXTeam : public AInfo
{
	GENERATED_BODY()
	
public:
	AEXTeam();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Team")
	void SetSpawnArea(AEXSpawnArea* SpawnArea);

	FORCEINLINE AEXSpawnArea* GetCurrentSpawnArea() const { return ActiveSpawnArea; }

	virtual void AddToTeam(AEXPlayerState* PS);
	virtual void RemoveFromTeam(AEXPlayerState* PS);

	UFUNCTION(BlueprintPure, Category = "Team")
	FORCEINLINE ETeam GetType() const { return Type; }

	UFUNCTION(BlueprintCallable, Category = "Team")
	void SetType(ETeam InType);

	UFUNCTION()
	int32 NumMembers() const { return Members.Num(); }
	UFUNCTION()
	TArray<AEXPlayerState*> GetMembers() const { return Members; }

	AActor* GetPlayerStart(AEXPlayerController* Player);

	UFUNCTION(BlueprintCallable, Category = "Team")
	FORCEINLINE bool IsAttacking() const { return Type == ETeam::Attack; }
	UFUNCTION(BlueprintCallable, Category = "Team")
	FORCEINLINE bool IsDefending() const { return Type == ETeam::Defense; }
	UFUNCTION(BlueprintCallable, Category = "Team")
	FORCEINLINE bool IsSpectating() const { return Type == ETeam::Spectator; }

	FString GetTeamName() const;

	FLinearColor GetColor() const { return Color; }

	FORCEINLINE bool IsFull() const { return NumMembers() == MaxMembers; }

	void SetStartedOnAtt(bool bInStartedOnAtt) { bStartedOnAttack = bInStartedOnAtt; }
	bool GetStartedOnAtt() const { return bStartedOnAttack; }

	FOnTeamChangedSignature OnChanged;

	bool IsSame(AEXTeam* Other) const { return GetType() == Other->GetType(); }
	bool IsValid() const { return Type != ETeam::Invalid; }
protected:

	FLinearColor Color = FLinearColor::Green;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Type)
	ETeam Type = ETeam::Invalid;

	UFUNCTION()
	void OnRep_Type();


	UPROPERTY()
	TArray<AEXPlayerState*> Members;

	UPROPERTY()
	AEXSpawnArea* ActiveSpawnArea = nullptr;

	FTimerHandle TimerHandle_EndBackSpawn;

	bool bStartedOnAttack = false;

	int32 MaxMembers = 5;
};
