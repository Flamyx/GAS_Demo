// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCanEquipOrSpendPoints, bool, bCanEquip, bool, bCanSpend, const FString&, Description, const FString&, NextDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellDeselect, const FGameplayTag&, AbilityTag);

struct FSelectedAbility
{
	FGameplayTag AbilityTag;
	FGameplayTag StatusTag;
	FGameplayTag InputTag;
	FGameplayTag AbilityTypeTag;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Level")
	FOnPlayerStatChangedSignature OnSpellPointsAddedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FCanEquipOrSpendPoints OnCanEquipOrSpendPointsDelegate;
	
	UFUNCTION(BlueprintCallable)
	void UpgradeSpell();

	UFUNCTION(BlueprintCallable)
	void GlobeDeselect();
		
	UFUNCTION(BlueprintCallable)
	int32 GetPlayerLevel();
	
	UFUNCTION(BlueprintCallable)
	void OnSpellClicked(const FGameplayTag& AbilityTag);
	
	UFUNCTION(BlueprintCallable)
	void OnInputGlobeClicked(const FGameplayTag& InputTag);
	
	UFUNCTION(BlueprintCallable)
	void OnEquipButtonPressed();
	
	UPROPERTY(BlueprintAssignable)
	FOnSpellDeselect OnSpellDeselectedDelegate;

protected:
	UPROPERTY(BlueprintReadWrite)
	int32 AvailableSpellPoints;
	
	void SpellEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PrevSlot);

private:
	void OnSpellPointsAdded(int32 IncomingSpellPoints);
	
	FSelectedAbility SelectedAbility;
	
	FGameplayTag SelectedSlot;
	bool bWaitingForSpellInput = false;
};
