// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_ResidualPeriodicDamage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMMC_ResidualPeriodicDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_ResidualPeriodicDamage();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition FireResistanceDef;
	FGameplayEffectAttributeCaptureDefinition LightningResistanceDef;
	
};
