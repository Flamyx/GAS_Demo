// Copyright Sparrow Inc.


#include "AbilitySystem/ModMagCalc/ModMagCalc_XP.h"
#include "AbilitySystem/AuraAttributeSet.h"


UModMagCalc_XP::UModMagCalc_XP()
{
	XPDef.AttributeToCapture = UAuraAttributeSet::GetIncomingXPAttribute();
	XPDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	XPDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(XPDef);
}

float UModMagCalc_XP::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//Spec.GetEffectContext().GetEffectCauser();
	return 0.f;
}
