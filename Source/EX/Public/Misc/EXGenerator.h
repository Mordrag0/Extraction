// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "System/EXInteract.h"
#include "EXGenerator.generated.h"

class AEXPlayerController;
class AEXC4;
class AEXPlantSite;

UCLASS()
class EX_API AEXGenerator : public AEXInteract // #Interactable
{
	GENERATED_BODY()
	
public:	
	AEXGenerator();

public:
//~ Begin AEXInteract Interface
	virtual bool CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const override;
	virtual bool StartInteract_Implementation(AEXCharacter* Player) override;
	virtual bool StopInteract_Implementation(AEXCharacter* Player) override;
//~ End AEXInteract Interface
//~ Begin AEXInteract Interface
	virtual void StartProgress(AEXCharacter* Player) override;
//~ End AEXInteract Interface
//~ Begin AActor Interface
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Reset() override;

protected:
	virtual void BeginPlay() override;
	//~ End AActor Interface
public:
	virtual void Passed() override;
	virtual void SetActive() override;

	void SetC4(AEXC4* InC4) { C4 = InC4; }

	void Kill(AEXPlayerController* Player);

	ETeam GetRepairingTeam() const { return RepairingTeam; }
protected:

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Generator")
	bool bRepaired = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	ETeam RepairingTeam = ETeam::Attack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	bool bRepairedByDefault = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Generator")
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "Generator")
	float Health = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Generator")
	FColor HealthbarColor = FColor::Yellow;

protected:

	UFUNCTION()
	void OnRepaired(AEXPlayerController* Player);
	UFUNCTION()
	void OnDestroyed(AEXPlayerController* Player);

	UPROPERTY()
	AEXC4* C4 = nullptr;
	UPROPERTY()
	AEXPlantSite* PlantSite = nullptr;

	UFUNCTION(BlueprintImplementableEvent, Category = "Generator")
	void OnGeneratorRepaired();
	UFUNCTION(BlueprintImplementableEvent, Category = "Generator")
	void OnGeneratorDestroyed();

public:
	UFUNCTION(BlueprintCallable, Category = "Components")
	FORCEINLINE class UEXProgress* GetRepairComp() const { return RepairComp; }
	UFUNCTION(BlueprintCallable, Category = "Components")
	FORCEINLINE class UEXProgress* GetDestroyComp() const { return DestroyComp; }
	UFUNCTION(BlueprintCallable, Category = "Components")
	FORCEINLINE bool IsRepaired() const { return bRepaired; }
	UFUNCTION(BlueprintCallable, Category = "Components")
	FORCEINLINE bool IsRepairedByDefault() const { return bRepairedByDefault; }
private:
	
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgress* RepairComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgress* DestroyComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgressWidget* RepairWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgressWidget* DestroyWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* RepairWidgetComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* DestroyWidgetComp = nullptr;

	UPROPERTY(EditAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* PlantSiteChild = nullptr;
};
