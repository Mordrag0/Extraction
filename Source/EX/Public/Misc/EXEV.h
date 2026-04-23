// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "System/EXInteract.h"
#include "EXEV.generated.h"

class AEXCharacter;
class UEXProgress;
class AEXBarricade;

UCLASS()
class EX_API AEXEV : public AEXInteract // #Interactable
{
	GENERATED_BODY()
	
public:	
	AEXEV();

protected:
	virtual void BeginPlay() override;

public:	

	//~ Begin AEXInteract Interface
	virtual bool CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const override;
	virtual bool StartInteract_Implementation(AEXCharacter* Player) override;
	virtual bool StopInteract_Implementation(AEXCharacter* Player) override;
	//~ End AEXInteract Interface

	virtual void Reset() override;

	virtual void StartProgress(AEXCharacter* Player) override;

	//~ Begin AActor Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	//~ End AActor Interface

	UFUNCTION(BlueprintCallable, Category = "EV")
	class USplineComponent* GetPath() const;
	UFUNCTION(BlueprintCallable, Category = "EV")
	FORCEINLINE class AEXEVPath* GetPathActor() const { return PathActor; }
	UFUNCTION(BlueprintCallable, Category = "EV")
	FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return Mesh; }
	UFUNCTION(BlueprintCallable, Category = "EV")
	FORCEINLINE UEXProgress* GetHealthComp() const { return RepairComp; }


	UFUNCTION(BlueprintCallable, Category = "EV")
	FORCEINLINE AEXBarricade* GetBarricade() const { return Barricades.IsValidIndex(CurrentBarricadeIdx) ? Barricades[CurrentBarricadeIdx] : nullptr; }
	UFUNCTION(BlueprintCallable, Category = "EV")
	FORCEINLINE bool IsRepaired() const { return bRepaired; }
	UFUNCTION(BlueprintCallable, Category = "EV")
	FORCEINLINE bool IsFinished() const { return bFinished; }

	UFUNCTION()
	void BarricadePassed() { CurrentBarricadeIdx++; }

	AEXCharacter* GetMovingPlayer() const { return MovingPlayer; }


	virtual void Passed() override;
	virtual void SetActive() override;

protected:
	UFUNCTION()
	void PlayerEnteredArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void PlayerLeftArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	AEXCharacter* MovingPlayer = nullptr;

	UFUNCTION()
	void OnRepaired(AEXPlayerController* Player);
	UFUNCTION()
	void OnDestroyed(AEXPlayerController* Player);

	virtual void OnCompleted(AEXPlayerController* Player) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV")
	float Speed = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV")
	float RepairSpeed = 20.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EV")
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "EV")
	float Health = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "EV")
	bool bRepaired = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "EV")
	TArray<AEXBarricade*> Barricades;

	int32 CurrentBarricadeIdx = 0;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "EV")
	TArray<float> SpawnChecks;

	int32 CurrentSpawnCheckIdx = 0;

	UPROPERTY(EditDefaultsOnly, Category = "EV")
	FColor HealthbarColor = FColor::Yellow;

	UPROPERTY(EditInstanceOnly, Category = "EV")
	class AEXEVPath* PathActor = nullptr;

	bool bFinished = false;

	UPROPERTY()
	class AEXLevelRules* LevelRules = nullptr;

	UPROPERTY()
	AEXCharacter* RepairingPlayer = nullptr;

private:

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Area = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgress* EVProgressComp = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgress* RepairComp = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgress* DestroyComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* RepairWidgetComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* DestroyWidgetComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgressWidget* RepairWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProgressWidget* DestroyWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXEVMovementComponent* Movement = nullptr;

};
