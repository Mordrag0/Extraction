// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EXReloadDoneNotify.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXReloadDoneNotify : public UAnimNotify
{
	GENERATED_BODY()

public:

	// Begin UAnimNotify interface
	virtual FString GetNotifyName_Implementation() const override;
#if WITH_EDITOR
	virtual void ValidateAssociatedAssets() override;
#endif


private:
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	// End UAnimNotify interface

};
