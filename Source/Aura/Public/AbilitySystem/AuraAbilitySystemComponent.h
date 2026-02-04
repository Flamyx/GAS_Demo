// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Data/AbilityInfo.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGivenDelegate);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FonStatusChangedDelegate, const FGameplayTag& /* AbilityTag */, const FGameplayTag& /* StatusTag */, int32 /* AbilityLevel */);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpellEquippedDelegate);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;
	FAbilitiesGivenDelegate AbilitiesGiven;
	FonStatusChangedDelegate StatusChangedDelegate;
	FOnSpellEquippedDelegate OnSpellEquipped;
	bool bStartupAbilitiesGiven = false;
	
	bool CheckIsAbilityAdded(const FGameplayTag& InputTag, bool bClearIfExists);
	void AddAbility(TSubclassOf<UGameplayAbility> &AbilityClass, int32 Level, bool bClearIfExists = false);
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate);
	
	//void UpdateAbility(const FForEachAbility& Delegate, );

	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	void UpgradeSpell(const FGameplayTag& SpellTag);
	UFUNCTION(Server, Reliable)
	void ServerUpgradeSpell(const FGameplayTag& SpellTag);
	void EquipSpell(const FGameplayTag &InputTag, const FGameplayTag& SpellTag);
	UFUNCTION(Server, Reliable)
	void ServerEquipSpell(const FGameplayTag &InputTag, const FGameplayTag &AbilityTag);
	
	void UpdateAbilities(int32 Level);
	
	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 Level);
	
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

	bool GetDescriptions(const FGameplayTag& AbilityTag, FString& Description, FString& NextDescription);
	
protected:
	virtual void OnRep_ActivateAbilities();

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
	
	
};
