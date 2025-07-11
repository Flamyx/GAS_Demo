// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayEffectTypes.h"
#include "Components/SphereComponent.h"
#include "MeteoriteProjectile.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AMeteoriteProjectile : public AActor
{
	GENERATED_BODY()

public:
	AMeteoriteProjectile();

	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"));
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"));
	FGameplayEffectSpecHandle PeriodicDamageEffectSpecHandle;

	void SetSphereRadius(float ChargeRatio);

protected:

	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	float BlastRadius;
	UPROPERTY(EditAnywhere)
	float MaxBlastRadius;

	TArray<AActor*> AffectedActors;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	float LifeSpan = 15.f;
	
	bool bHit = false;
};
