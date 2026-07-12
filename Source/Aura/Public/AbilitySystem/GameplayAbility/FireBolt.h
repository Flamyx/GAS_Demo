// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/AuraProjectileSpell.h"
#include "FireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& ProjectileTargetLocation,  AActor* HomingTarget, bool bOverridePitch, float PitchOverride);
	
	UPROPERTY(EditAnywhere)
	int ProjectileLimit = 1;
	
	UPROPERTY(EditAnywhere)
	float Spread = 45.f;
	
	UPROPERTY(EditAnywhere)
	float HomingAccelerationMagnitude = 2400.f;
	
	UPROPERTY(EditAnywhere)
	bool bLaunchHomingProjectiles = true;
	
	UFUNCTION(BlueprintCallable)
	inline TArray< AAuraProjectile* > GetSpawnedProjectiles() { return ProjectileActors; }
	
	UFUNCTION(BlueprintCallable)
	void HomeToTarget(AAuraProjectile* Projectile, AActor* HomingTarget, bool bOverridePitch, float PitchOverride);
	
private:
	UPROPERTY(EditDefaultsOnly)
	int OffsetLocationValue = 0.f;
	
	// UPROPERTY(EditAnywhere)
	// AActor* HomingMarker;
	
	UPROPERTY(EditAnywhere)
	TArray< AAuraProjectile* > ProjectileActors;
};
