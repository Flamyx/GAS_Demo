// Copyright Sparrow Inc.


#include "AbilitySystem/GameplayAbility/AuraBuffAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraBuffAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	ModifyResistances();
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraBuffAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(BuffHandle);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAuraBuffAbility::ModifyResistances(bool bNegate)
{	
	auto EffectSpecHandle = MakeOutgoingGameplayEffectSpec(BuffEffect, GetAbilityLevel());
	auto EffectSpec = EffectSpecHandle.Data.Get();
	for (TTuple<FGameplayTag, FScalableFloat> Pair: ResistancesToBuffs)
	{
		EffectSpec->SetSetByCallerMagnitude(Pair.Key, Pair.Value.GetValueAtLevel(GetAbilityLevel()));
	}
	
	BuffHandle = GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*EffectSpec);
}
