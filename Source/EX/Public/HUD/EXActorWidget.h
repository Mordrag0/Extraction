// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXActorWidget.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXActorWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadOnly, Category = "ActorWidget")
	AActor* OwningActor;

public:

	UFUNCTION(BlueprintCallable, Category = "ActorWidget")
	void SetOwningActor(AActor* NewOwner);

	
	
};
