// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EXBarricade.generated.h"

class AEXGenerator;

UCLASS()
class EX_API AEXBarricade : public AActor
{
	GENERATED_BODY()
	
public:	
	AEXBarricade();

	FORCEINLINE float GetBlockingDistance() const { return BlockingDistance; }
	FORCEINLINE bool IsBarricadeActive() const { return bActive; }
	void BarricadePassed();

	AEXGenerator* GetGenerator() const { return Generator; }

	void Reset();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, Category = "Barricade")
	float BlockingDistance = 0.f;

	UFUNCTION()
	void OnRep_Active();

	UPROPERTY(ReplicatedUsing = OnRep_Active)
	bool bActive = false;

	UPROPERTY(EditInstanceOnly, Category = "Barricade")
	AEXGenerator* Generator = nullptr;

	UFUNCTION()
	void OnGenRepaired(AEXPlayerController* Player);

	UFUNCTION()
	void OnGenDestroyed(AEXPlayerController* Player);

	UFUNCTION()
	void OnActiveChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "Barricade")
	void OnGoUp();
	UFUNCTION(BlueprintImplementableEvent, Category = "Barricade")
	void OnGoDown();
private:

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root = nullptr;

public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
