// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();
	

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	
private:
	void DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
						 const FGameplayEffectSpec& Spec,
						 FAggregatorEvaluateParameters& EvaluationParameters,
						 const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& TagsToCaptureDefs) const;
	 
	void DetermineBaseDamage(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
							 const FGameplayEffectSpec& Spec,
							 FAggregatorEvaluateParameters& EvaluationParameters,
							 const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& TagsToCaptureDefs, 
							 float& Damage) const;
	bool DetermineIsBlocked(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                        FAggregatorEvaluateParameters& EvaluationParameters) const;
	void ApplyArmorToDamage(const FGameplayEffectCustomExecutionParameters& ExecutionParams, int32 SourceLevel,
	                        int32 TargetLevel, FAggregatorEvaluateParameters& EvaluationParameters,
	                        const UCharacterClassInfo* CharacterClassInfo, float& Damage) const;
	void DetermineCriticalHit(const FGameplayEffectCustomExecutionParameters& ExecutionParams, int32 TargetLevel,
	                          FAggregatorEvaluateParameters& EvaluationParameters,
	                          const UCharacterClassInfo* CharacterClassInfo, float& Damage,
	                          FGameplayEffectContextHandle EffectContextHandle) const;
};
