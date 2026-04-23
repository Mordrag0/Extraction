// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "System/EXInteract.h"
#include "EXDropOffLocation.generated.h"

class USceneComponent;
class UEXProgress;
class UEXProgressWidget;
class UWidgetComponent;
class AEXPlayerController;
class AEXCarryableObjective;

UCLASS()
class EX_API AEXDropOffLocation : public AEXInteract // #Interactable
{
	GENERATED_BODY()
	
public:	
	AEXDropOffLocation();

protected:
	virtual void BeginPlay() override;

public:
	//~ Begin AEXInteract Interface
	virtual bool CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const override;
	virtual bool StartInteract_Implementation(AEXCharacter* Player) override;
	virtual bool StopInteract_Implementation(AEXCharacter* Player) override;
	//~ End AEXInteract Interface

	virtual void StartProgress(AEXCharacter* Player) override;

	virtual void OnCompleted(AEXPlayerController* Player) override;

	void Reset() override;

	void SetActive() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Objective")
	TArray<AEXCarryableObjective*> Carryables;

	void ShowCarryable(int32 Idx);

private:

	class AEXGameModeBase* GMB = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgress* DeliveryProgressComp = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* DeliverWidgetComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgressWidget* DeliverWidget = nullptr;

};
