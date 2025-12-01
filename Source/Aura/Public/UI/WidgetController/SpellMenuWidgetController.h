// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Level")
	FOnPlayerStatChangedSignature OnSpellPointsAddedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FAbilityWidgetCfg SpellCfgDelegate;

private:
	void OnSpellPointsAdded(int32 IncomingSpellPoints);
	
};
