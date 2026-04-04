// Copyright Sparrow Inc.


#include "AbilitySystem/GameplayAbility/AuraBuffAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraBuffAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	auto ASC = GetAbilitySystemComponentFromActorInfo();
	bool bNegate = false;
	if (ASC->HasMatchingGameplayTag(FAuraGameplayTags::Get().Effect_Status_Applied))
	{
		//CommitAbility(Handle, ActorInfo, ActivationInfo)
		ASC->SetLooseGameplayTagCount(FAuraGameplayTags::Get().Effect_Status_Applied, 0);
		bNegate = true;
		ModifyResistances(bNegate);
	}
	else
	{
		ModifyResistances(bNegate);
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	BuffStatusDelegate.Broadcast(!bNegate);
}

void UAuraBuffAbility::ModifyResistances(bool bNegate)
{	
	auto ASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle EffectSpecHandle;
	if (bNegate)
	{
		EffectSpecHandle = ASC->MakeOutgoingSpec(DebuffEffect, GetAbilityLevel(),EffectContext);
	}
	else
	{
		EffectSpecHandle = ASC->MakeOutgoingSpec(BuffEffect, GetAbilityLevel(),EffectContext);
		ASC->AddLooseGameplayTag(FAuraGameplayTags::Get().Effect_Status_Applied);
	}
	
	EffectSpecHandle.Data.Get()->AddDynamicAssetTag(FAuraGameplayTags::Get().Abilities_Passive_AuraBuff);
	for (TTuple<FGameplayTag, FScalableFloat> Pair: ResistancesToBuffs)
	{
		auto Val = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle,Pair.Key,Val);
	}
	
	ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}
