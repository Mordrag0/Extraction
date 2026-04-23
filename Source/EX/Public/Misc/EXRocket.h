// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXAbility.h"
#include "EXRocket.generated.h"

class UEXHomingMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExploded, AEXRocket*, Rocket);

UCLASS()
class EX_API AEXRocket : public AEXAbility
{
	GENERATED_BODY()
	
public:	
	AEXRocket();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	FORCEINLINE UEXHomingMovementComponent* GetMovementComp() const { return Movement; }

	FOnExploded OnExploded;

	void SetTarget(const FVector& Target);

protected:
	virtual void BeginPlay() override;

	void Explode(bool bComponentHit, const FHitResult Hit);

	UPROPERTY(EditDefaultsOnly, Category = "Rocket")
	float Damage = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Rocket")
	float DamageRadius = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Rocket")
	TSubclassOf<UDamageType> DamageType = nullptr;

	UFUNCTION(NetMulticast, Reliable)// #EXTODORELIABLE
	void Multicast_Reliable_PlayEffects(bool bComponentHit, const FHitResult Hit);

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bExploded = false;


	UPROPERTY(EditDefaultsOnly, Category = "Rocket")
	TSubclassOf<class AEXExplosionEffect> ExplosionClass = nullptr;

private:

	UPROPERTY(VisibleAnywhere, Category = "Rocket", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Rocket", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* ExplosionPoint = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Rocket", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Rocket", Meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* Collision = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Rocket", Meta = (AllowPrivateAccess = "true"))
	class UEXHomingMovementComponent* Movement = nullptr;


};
