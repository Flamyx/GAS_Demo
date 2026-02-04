// Copyright Sparrow Inc.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	auto SpellPointsPS = GetAuraPS()->GetSpellPoints();
	OnSpellPointsAddedDelegate.Broadcast(SpellPointsPS);
	AvailableSpellPoints = SpellPointsPS;
	BroadcastAbilityInfo();
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->SpellPointsDelegate.AddLambda(
		[this](int32 SpellPoints) {
			OnSpellPointsAddedDelegate.Broadcast(SpellPoints);
			AvailableSpellPoints = GetAuraPS()->GetSpellPoints();
		}
	);
	
	GetAuraASC()->StatusChangedDelegate.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 Level)
		{
			auto AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAuraASC());
			auto AbilityCfg = AbilityInfo->GetAbilityInfo(AbilityTag);
			auto AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
			if (AbilitySpec)
			{
				AbilityCfg.StatusTag = StatusTag;
				AbilityCfg.InputTag = UAuraAbilitySystemLibrary::FindInputTagFromSpec(*AbilitySpec);
			}
			AbilityCfgDelegate.Broadcast(AbilityCfg);
			
			bool bCanEquip = AbilityCfg.StatusTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Status.Unlocked")) || AbilityCfg.StatusTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Status.Equipped"));
			bool bCanSpendPoints = AvailableSpellPoints > 0 && AbilityTag.IsValid() && !AbilityCfg.StatusTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Status.Locked"));
			FString Description;
			FString NextDestription;
			bool bSuccess = GetAuraASC()->GetDescriptions(AbilityTag, Description, NextDestription);
			OnCanEquipOrSpendPointsDelegate.Broadcast(bCanEquip, bCanSpendPoints, Description, NextDestription);
		}
	);
	
	GetAuraASC()->OnSpellEquipped.AddDynamic(this, &USpellMenuWidgetController::BroadcastEquippedSpell);
}

void USpellMenuWidgetController::UpgradeSpell()
{
	GetAuraASC()->UpgradeSpell(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::GlobeDeselect()
{
	SelectedAbility.AbilityTag = FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.StatusTag = FAuraGameplayTags::Get().Status_Locked;
	SelectedAbility.InputTag = FGameplayTag();
	OnCanEquipOrSpendPointsDelegate.Broadcast(false, false, FString(), FString());
}

int32 USpellMenuWidgetController::GetPlayerLevel()
{
	return GetAuraPS()->GetPlayerLevel();
}

void USpellMenuWidgetController::OnSpellClicked(const FGameplayTag& AbilityTag)
{
	auto AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	FGameplayTag StatusTag;
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None) || AbilitySpec == nullptr)
	{
		StatusTag = FAuraGameplayTags::Get().Status_Locked;
	}
	else
	{
		StatusTag = UAuraAbilitySystemLibrary::FindStatusTagFromSpec(*AbilitySpec);
	}
	
	bool bCanEquip = StatusTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Status.Unlocked")) || StatusTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Status.Equipped"));
	bool bCanSpendPoints = AvailableSpellPoints > 0 && AbilityTag.IsValid() && !StatusTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Status.Locked"));
	FString Description;
	FString NextDestription;
	bool bSuccess = GetAuraASC()->GetDescriptions(AbilityTag, Description, NextDestription);
	OnCanEquipOrSpendPointsDelegate.Broadcast(bCanEquip, bCanSpendPoints, Description, NextDestription);
	
	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.StatusTag = StatusTag;
}

void USpellMenuWidgetController::OnInputGlobeClicked(const FGameplayTag& InputTag)
{
	SelectedAbility.InputTag = InputTag;
	GetAuraASC()->EquipSpell(SelectedAbility.InputTag, SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::OnSpellPointsAdded(int32 IncomingSpellPoints)
{
	OnSpellPointsAddedDelegate.Broadcast(IncomingSpellPoints);
	AvailableSpellPoints = GetAuraPS()->GetSpellPoints();
	
}
