// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "System/EXInteract.h"
#include "EXC4.generated.h"

class AEXCharacter;
class UEXProgress;

UCLASS()
class EX_API AEXC4 : public AEXInteract // #Interactable
{
	GENERATED_BODY()
	
public:	
	AEXC4();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Explode(AEXPlayerController* Player);
	UFUNCTION()
	void UpdateTimer(float Seconds);

	void NetDestroy();

public:

	UFUNCTION(BlueprintCallable, Category = "C4")
	virtual void StartTimer(float Seconds);

	//~ Begin AEXInteract Interface
	virtual bool CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const override;
	virtual bool StartInteract_Implementation(AEXCharacter* Player) override;
	virtual bool StopInteract_Implementation(AEXCharacter* Player) override;
	//~ End AEXInteract Interface

	virtual void Reset() override;

	virtual void StartProgress(AEXCharacter* Player) override;

	FORCEINLINE UEXProgress* GetDefuseComp() const { return DefuseComp; }

	void SetTarget(AActor* InTarget);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetProgressComp(UEXProgress* InProgressComp) override;

protected:
	UPROPERTY(Replicated)
	ETeam PlantingTeam = ETeam::Invalid;
	UPROPERTY(ReplicatedUsing = OnRep_Exploded)
	bool bExploded;
	UFUNCTION()
	void OnRep_Exploded();

	UPROPERTY()
	AActor* Target = nullptr;

	UFUNCTION()
	void OnDefused(AEXPlayerController* Player);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C4")
	float DefuseRange = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "C4")
	FColor ProgressbarColor = FColor::Yellow;

	UPROPERTY(EditDefaultsOnly, Category = "C4")
	FColor DefusebarColor = FColor::Blue;

	UPROPERTY(EditDefaultsOnly, Category = "C4")
	float BaseDamage = 100;
	UPROPERTY(EditDefaultsOnly, Category = "C4")
	float DamageRadius = 500;
	UPROPERTY(EditDefaultsOnly, Category = "C4")
	TSubclassOf<UDamageType> DamageTypeClass;
	UPROPERTY(EditDefaultsOnly, Category = "C4")
	TSubclassOf<class AEXExplosionEffect> ExplosionClass = nullptr;
		
	UPROPERTY()
	UEXProgress* C4ProgressComp = nullptr;

private:

	UPROPERTY(VisibleDefaultsOnly, Category = "C4", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootScene = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "C4", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DamageSourceLocation = nullptr;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "C4", Meta = (AllowPrivateAccess = "true"))
	class UEXTimerWidget* UserWidget = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "C4", Meta = (AllowPrivateAccess = "true"))
	class UEXProgressWidget* ProgressWidget = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "C4", Meta = (AllowPrivateAccess = "true"))
	class UEXProgressWidget* DefuseWidget = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "C4", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshComp = nullptr;
	
	UPROPERTY(VisibleInstanceOnly, Category = "C4", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* TimerWidgetComp = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "C4", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* ProgressWidgetComp = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "C4", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* DefuseWidgetComp = nullptr;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "C4", Meta = (AllowPrivateAccess = "true"))
	class UEXProgress* DefuseComp = nullptr;

};
