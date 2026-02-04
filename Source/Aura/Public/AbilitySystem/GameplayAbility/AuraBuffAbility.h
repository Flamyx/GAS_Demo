// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/AuraGameplayAbility.h"
#include "AuraBuffAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBuffAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FScalableFloat> ResistancesToBuffs;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
protected:
	UFUNCTION(BlueprintCallable)
	void ModifyResistances(bool bNegate = false);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> BuffEffect;
private:
	FActiveGameplayEffectHandle BuffHandle;
};
