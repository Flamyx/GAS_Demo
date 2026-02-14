// Copyright Sparrow Inc.


#include "AbilitySystem/GameplayAbility/AuraDamageGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

UAuraDamageGameplayAbility::UAuraDamageGameplayAbility()
{
	bReplicateInputDirectly = true;
}

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1);
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (Spec)
	{
		for (TTuple<FGameplayTag, FScalableFloat> Pair : DamageTypes)
		{
			Spec->SetSetByCallerMagnitude(Pair.Key, Pair.Value.GetValueAtLevel(GetAbilityLevel()));
		}
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*Spec,
												UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}
