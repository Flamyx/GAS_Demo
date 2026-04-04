// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/AuraGameplayAbility.h"
#include "AuraBuffAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAuraBuffStatus, bool, bIsBuff);

class UNiagaraComponent;

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
	
protected:
	UFUNCTION(BlueprintCallable)
	void ModifyResistances(bool bNegate = false);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> BuffEffect;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DebuffEffect;
	
	UPROPERTY(BlueprintAssignable)
	FAuraBuffStatus BuffStatusDelegate;

	
};
