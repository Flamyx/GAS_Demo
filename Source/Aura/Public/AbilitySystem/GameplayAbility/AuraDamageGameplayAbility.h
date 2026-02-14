// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraDamageGameplayAbility();

	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
	
	UPROPERTY(EditDefaultsOnly)
	float DebuffChance = 20.f;
	UPROPERTY(EditDefaultsOnly)
	float DebuffDamage = 5.f;
	UPROPERTY(EditDefaultsOnly)
	float DebuffFrequency = 1.f;
	UPROPERTY(EditDefaultsOnly)
	float DebuffDuration = 4.f;
	
	
protected: 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ResidualDamageEffectClass;
};
