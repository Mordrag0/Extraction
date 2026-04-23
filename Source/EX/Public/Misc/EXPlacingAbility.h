// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EXPlacingAbility.generated.h"

class AEXCharacter;
class UMaterialInstance;
class UMaterialInstanceDynamic;

UCLASS()
class EX_API AEXPlacingAbility : public AActor
{
	GENERATED_BODY()
	
public:	
	AEXPlacingAbility();

	void SetCharacter(AEXCharacter* InPlayer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	FORCEINLINE bool IsValidPlacement() const { return bCurrentlyValidPlacement; }

protected:

	void SetVaildPlacement(bool bValid);

	UFUNCTION()
	void Adjust();
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float Dist = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UMaterialInstance* MaterialInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FLinearColor ValidColor = FLinearColor::Blue;
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FLinearColor InvalidColor = FLinearColor::Red;
	
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial = nullptr;
	UPROPERTY()
	AEXCharacter* Player = nullptr;

	UPROPERTY()
	bool bCurrentlyValidPlacement = true;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float MinNormalZ = 0.7f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Ability", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, Category = "Ability", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;
};
