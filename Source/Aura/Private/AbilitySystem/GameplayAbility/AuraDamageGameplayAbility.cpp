// Copyright Sparrow Inc.


#include "AbilitySystem/GameplayAbility/AuraDamageGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"

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
		Spec->SetSetByCallerMagnitude(DamageType, Damage.GetValueAtLevel(GetAbilityLevel()));

	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*Spec,
												UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FAuraDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FAuraDamageEffectParams DamageEffectParams;
	DamageEffectParams.WorldContextObject = GetAvatarActorFromActorInfo();
	DamageEffectParams.AbilityLevel = GetAbilityLevel();
	
	DamageEffectParams.DamageEffectClass = DamageEffectClass;
	
	DamageEffectParams.SourceASC = GetAbilitySystemComponentFromActorInfo();
	DamageEffectParams.TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	
	DamageEffectParams.Damage = Damage.GetValueAtLevel(GetAbilityLevel());
	DamageEffectParams.DamageType = DamageType;
	
	DamageEffectParams.DebuffChance = DebuffChance;
	DamageEffectParams.DebuffDamage = DebuffDamage;
	DamageEffectParams.DebuffFrequency = DebuffFrequency;
	DamageEffectParams.DebuffDuration = DebuffDuration;
	
	DamageEffectParams.DeathImpulseMagnitude = DeathImpulseMagnitude;
	
	return DamageEffectParams;
}
