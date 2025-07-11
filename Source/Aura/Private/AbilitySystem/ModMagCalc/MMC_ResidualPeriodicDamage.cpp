// Copyright Sparrow Inc.


#include "AbilitySystem/ModMagCalc/MMC_ResidualPeriodicDamage.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"


UMMC_ResidualPeriodicDamage::UMMC_ResidualPeriodicDamage()
{
	FireResistanceDef.AttributeToCapture = UAuraAttributeSet::GetFireResistanceAttribute();
	FireResistanceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	FireResistanceDef.bSnapshot = false;

	LightningResistanceDef.AttributeToCapture = UAuraAttributeSet::GetLightningResistanceAttribute();
	LightningResistanceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	LightningResistanceDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(FireResistanceDef);
	RelevantAttributesToCapture.Add(LightningResistanceDef);
}

float UMMC_ResidualPeriodicDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float FireResistance = 0;
	GetCapturedAttributeMagnitude(FireResistanceDef, Spec, EvaluationParameters, FireResistance);
	float LightningResistance = 0;
	GetCapturedAttributeMagnitude(LightningResistanceDef, Spec, EvaluationParameters, LightningResistance);

	const auto Avatar = Spec.GetEffectContext().GetEffectCauser();
	UAbilitySystemComponent* InstigatorASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Avatar);
	float InstigatorIntelligence = InstigatorASC->GetNumericAttribute(UAuraAttributeSet::GetIntelligenceAttribute());

	float FireDamageMagnitude = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage_Residual_Fire);
	float LightningDamageMagnitude = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage_Lightning);

	float FireDamage = FireDamageMagnitude * FMath::LogX(10.f, InstigatorIntelligence) * (1 - FireResistance / 100.f);
	float LightningDamage = LightningDamageMagnitude * FMath::LogX(10.f, InstigatorIntelligence) * (1 - LightningResistance / 100.f);

	/*FString DebugMessage = FString::Printf(
		TEXT("Fire damage %.2f Lightning damage %.2f"),
		FireDamage, LightningDamage
	);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, DebugMessage);
	*/
	return FireDamage + LightningDamage;
}
