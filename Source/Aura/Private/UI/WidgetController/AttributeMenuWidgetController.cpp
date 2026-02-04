// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"
#include "Interaction/PlayerInterface.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	check(AttributeInfo);

	for (auto& Pair : GetAuraAS()->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	OnAttributePointsAddedDelegate.Broadcast(GetAuraPS()->GetAttributePoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	check(AttributeInfo);

	for (auto& Pair : GetAuraAS()->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	GetAuraPS()->AttributePointsDelegate.AddUObject(this, &UAttributeMenuWidgetController::OnAttributePointsAdded);
}

void UAttributeMenuWidgetController::OnAttributePointsAdded(int32 IncomingAttributePoints)
{
	OnAttributePointsAddedDelegate.Broadcast(IncomingAttributePoints);
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetAuraASC()->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}

// void UAttributeMenuWidgetController::AddPointToAttribute(const FString& AttributeName) const
// {
// 	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
// 	for (auto& Pair : AS->TagsToAttributes)
// 	{
// 		if (Pair.Key.ToString().Find(AttributeName) >= 0)
// 		{
// 			const FGameplayAttribute Attribute = Pair.Value();
// 			AbilitySystemComponent->ApplyModToAttribute(Attribute, EGameplayModOp::Additive, 1);
// 			IPlayerInterface::Execute_RecalculateSecondaryAttributes(AbilitySystemComponent->GetAvatarActor());
// 		}
// 	}
// 	
// }
