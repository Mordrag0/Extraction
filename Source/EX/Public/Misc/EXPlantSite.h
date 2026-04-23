// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "System/EXInteract.h"
#include "EXPlantSite.generated.h"

class AEXGenerator;

UCLASS()
class EX_API AEXPlantSite : public AEXInteract // #Interactable
{
	GENERATED_BODY()
	
public:	
	AEXPlantSite();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

//~ Begin IEXInteractable Interface
	virtual bool CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const override;
	virtual bool StartInteract_Implementation(AEXCharacter* Player) override;
	virtual bool StopInteract_Implementation(AEXCharacter* Player) override;
//~ End IEXInteractable Interface
//~ Begin AEXInteract Interface
	virtual void ChangeIconVisibility(bool bVisible) override;
	virtual void StartProgress(AEXCharacter* Player) override;
	virtual bool CanBeDefused() const override { return true; }
//~ End AEXInteract Interface
//~ Begin AActor Interface
	virtual void Reset() override;
protected:
	virtual void BeginPlay() override;
//~ End AActor Interface

public:
	bool CanBePlanted() const;

	void SetGenerator(AEXGenerator* InGenerator);

protected:
	UFUNCTION()
	virtual void OnPlanted(AEXCharacter* Player);
	UFUNCTION()
	void OnDefused(AEXPlayerController* Player);
	virtual void OnCompleted(AEXPlayerController* Player) override;

	UPROPERTY(EditDefaultsOnly, Category = "Plant")
	TSubclassOf<class AEXC4> C4Class = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Plant")
	float TimeToPlant = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Plant")
	float TimeToExplode = 25.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Plant")
	float TimeToDefuse = 5.0f;
	
	UPROPERTY(ReplicatedUsing = OnRep_PlantedC4)
	AEXC4* PlantedC4 = nullptr;

	UFUNCTION()
	void OnRep_PlantedC4();

	UPROPERTY(EditAnywhere, Category = "Plant")
	ETeam PlantingTeam = ETeam::Attack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Plant")
	class AEXGenerator* Generator = nullptr;

	UPROPERTY()
	TArray<AEXCharacter*> InteractingPlayers;

private:

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootScene = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Area = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgress* C4ProgressComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "C4", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* IconWidgetComp = nullptr;
	void ClearC4();
};
