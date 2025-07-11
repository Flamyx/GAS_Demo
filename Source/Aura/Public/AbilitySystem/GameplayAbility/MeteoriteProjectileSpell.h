// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/AuraDamageGameplayAbility.h"
#include "Actor/MeteoriteProjectile.h"
#include "MeteoriteProjectileSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMeteoriteProjectileSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle,
							const FGameplayAbilityActorInfo* ActorInfo,
							const FGameplayAbilityActivationInfo ActivationInfo) override;


protected:
	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FVector& ProjectileTargetLocation);

	UFUNCTION(BlueprintCallable)
	void StartChargeTimeline();

	void ChargeTick();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bChargeEnded = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> RedCircleActor;

private:
	float ChargeTime = 0.f;

	FTimerHandle ChargeTimerHandle;

	UPROPERTY(EditAnywhere)
	FScalableFloat ChargedMultiplier;

	UPROPERTY(EditAnywhere)
	float SpawnHeight;

	UPROPERTY(EditAnywhere)
	FVector2D SpawnOffsetBounds;

	UPROPERTY(EditAnywhere)
	float MaxChargeTime = 3.f;

	UPROPERTY(EditAnywhere)
	float FChargeTick = .2;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMeteoriteProjectile> ProjectileClass;
};
