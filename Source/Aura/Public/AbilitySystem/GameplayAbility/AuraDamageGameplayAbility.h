// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/AuraGameplayAbility.h"
#include "AuraAbilityTypes.h"
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
	
	FAuraDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FGameplayTag DamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat Damage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffChance = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDamage = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffFrequency = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDuration = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DeathImpulseMagnitude = 1.2f;
	
protected: 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ResidualDamageEffectClass;
};
