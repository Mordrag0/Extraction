// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EXDummy.generated.h"

class UEXDummyAnimInstance;

UENUM(BlueprintType)
enum class EDummyType : uint8
{
	Damage,
	Ability,
};

UCLASS()
class EX_API AEXDummy : public AActor
{
	GENERATED_BODY()
	
public:	
	AEXDummy();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Dummy")
	void PlayHitEffects();
	UFUNCTION(BlueprintNativeEvent, Category = "Dummy")
	void PlayDeathEffects();

	UFUNCTION()
	void Die();

	virtual void Reset() override;

protected:
	virtual void BeginPlay() override;


	UFUNCTION()
	void AbilityHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Scored();

	UPROPERTY(EditDefaultsOnly, Category = "Dummy")
	EDummyType Type;

	UPROPERTY(EditAnywhere, Category = "Dummy")
	float MaxHealth = 100.f;

	UPROPERTY(Transient)
	float Health = 0.f;

	UPROPERTY(EditAnywhere, Category = "Dummy")
	int32 Score = 5.f;

	bool bDown = false;

	UPROPERTY()
	UEXDummyAnimInstance* AnimInst = nullptr;

private:

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Scene = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CollisionComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh = nullptr;

};
