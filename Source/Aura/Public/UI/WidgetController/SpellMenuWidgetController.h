// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCanEquipOrSpendPointsDelegate, bool, bCanEquip, bool, bCanSpend, const FString&, Description, const FString&, NextDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpellButtonClickedDelegate);


struct FSelectedAbility
{
	FGameplayTag AbilityTag;
	FGameplayTag StatusTag;
	FGameplayTag InputTag;
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
	FCanEquipOrSpendPointsDelegate OnCanEquipOrSpendPointsDelegate;
	
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
	
	
	FOnSpellButtonClickedDelegate OnSpellButtonClickedDelegate;

protected:
	UPROPERTY(BlueprintReadWrite)
	int32 AvailableSpellPoints;

private:
	void OnSpellPointsAdded(int32 IncomingSpellPoints);
	
	FSelectedAbility SelectedAbility;
	
};
