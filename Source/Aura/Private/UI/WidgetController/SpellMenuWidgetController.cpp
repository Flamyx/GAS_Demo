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
	
	GetAuraASC()->SpellEquippedDelegate.AddUObject(this, &USpellMenuWidgetController::SpellEquipped);
	
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
	SelectedAbility.AbilityTypeTag = UAuraAbilitySystemLibrary::FindAbilityTypeTagFromSpec(*AbilitySpec);
}

void USpellMenuWidgetController::OnInputGlobeClicked(const FGameplayTag& InputTag)
{
	if (!bWaitingForSpellInput) return;
	SelectedAbility.InputTag = InputTag;
	
	GetAuraASC()->ServerEquipSpell(SelectedAbility.InputTag, SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::OnEquipButtonPressed()
{
	//Here should be broadcast of delegate to play animation on either offensive spell tree or passive
	bWaitingForSpellInput = true;
	
	if (SelectedAbility.StatusTag.MatchesTagExact(FAuraGameplayTags::Get().Status_Equipped))
	{
		SelectedSlot = UAuraAbilitySystemLibrary::FindInputTagFromSpec(*GetAuraASC()->GetSpecFromAbilityTag(SelectedAbility.AbilityTag));
	}
}

void USpellMenuWidgetController::SpellEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PrevSlot)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
	FAuraAbilityInfo PrevSlotInfo;
	PrevSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	PrevSlotInfo.InputTag = PrevSlot;
	PrevSlotInfo.StatusTag = GameplayTags.Status_Unlocked;
	AbilityCfgDelegate.Broadcast(PrevSlotInfo);
	
	FAuraAbilityInfo SlotInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAuraASC())->GetAbilityInfo(AbilityTag);
	SlotInfo.InputTag = Slot;
	SlotInfo.StatusTag = Status;
	AbilityCfgDelegate.Broadcast(SlotInfo);
	
	bWaitingForSpellInput = false;
	//Here should bne broadcaszt of delegate to stop animatrions
	OnSpellDeselectedDelegate.Broadcast(AbilityTag);
}

void USpellMenuWidgetController::OnSpellPointsAdded(int32 IncomingSpellPoints)
{
	OnSpellPointsAddedDelegate.Broadcast(IncomingSpellPoints);
	AvailableSpellPoints = GetAuraPS()->GetSpellPoints();
	
}
