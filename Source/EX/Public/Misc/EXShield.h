// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EXPlacedAbility.h"
#include "EXShield.generated.h"

/**
 * 
 */
UCLASS()
class EX_API AEXShield : public AEXPlacedAbility
{
	GENERATED_BODY()
	
public:
	AEXShield();

private:

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh = nullptr;
};
