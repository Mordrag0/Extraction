// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXAbility.h"
#include "EXSmoke.generated.h"

UCLASS()
class EX_API AEXSmoke : public AEXAbility
{
	GENERATED_BODY()
	
public:	
	AEXSmoke();

protected:
	virtual void BeginPlay() override;


};
