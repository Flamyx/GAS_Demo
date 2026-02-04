// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"
#include "Player/AuraPlayerController.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}

AAuraPlayerController* UAuraWidgetController::GetAuraPC()
{
	if (AuraPC == nullptr)
	{
		AuraPC = Cast<AAuraPlayerController>(PlayerController);
	}

	return AuraPC;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS()
{
	if (AuraPS == nullptr)
	{
		AuraPS = Cast<AAuraPlayerState>(PlayerState);
	}

	return AuraPS;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraASC()
{
	if (AuraASC == nullptr)
	{
		AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	
	return AuraASC;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAS()
{
	if (AuraAS == nullptr)
	{
		AuraAS = Cast<UAuraAttributeSet>(AttributeSet);
	}

	return AuraAS;
}

void UAuraWidgetController:: BroadcastAbilityInfo()
{
	if (!GetAuraASC()->bStartupAbilitiesGiven)
		return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
		{
			FGameplayTag AbilityTag = UAuraAbilitySystemLibrary::FindAbilityTagFromSpec(AbilitySpec);
			auto AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAuraPS());
			if (!AbilityInfo)
			{
				GEngine->AddOnScreenDebugMessage(-1, 150, FColor::Red, "No Ability Info Found");
			}
			else
			{
				FAuraAbilityInfo AbilityCfg = AbilityInfo->GetAbilityInfo(AbilityTag);
				AbilityCfg.InputTag = UAuraAbilitySystemLibrary::FindInputTagFromSpec(AbilitySpec);
				AbilityCfg.StatusTag = UAuraAbilitySystemLibrary::FindStatusTagFromSpec(AbilitySpec);
				AbilityCfgDelegate.Broadcast(AbilityCfg);
			}
		});

	AuraASC->ForEachAbility(BroadcastDelegate);
}
