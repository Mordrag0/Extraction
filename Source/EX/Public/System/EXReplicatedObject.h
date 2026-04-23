// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EXReplicatedObject.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXReplicatedObject : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintPure)
	AActor* GetOwningActor() const { return GetTypedOuter<AActor>(); }
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;
	ENetRole GetOwnerRole() const { return GetOwningActor()->GetLocalRole(); }
	void Destroy();

protected:
	virtual void OnDestroyed() {}

public:

	//Are we a non-auth but owned by the local client.
	UFUNCTION()
	FORCEINLINE bool IsOwnedByLocalRemote() const { return GetOwnerRole() == ROLE_AutonomousProxy; }

	//Simulated proxies should skip all validation code; they are just slaves playing effects.
	UFUNCTION()
	FORCEINLINE bool IsSimulatedProxy() const { return GetOwnerRole() == ROLE_SimulatedProxy; }

	UFUNCTION()
	bool IsLocallyOwned() const;

	UFUNCTION()
	FORCEINLINE bool IsAuthority() const { return GetOwnerRole() == ROLE_Authority; }

	UFUNCTION()
	FORCEINLINE	bool IsNonOwningAuthority() const;

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetReplicates() const { return bReplicates; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetReplicates)
	bool bReplicates = true;
};
