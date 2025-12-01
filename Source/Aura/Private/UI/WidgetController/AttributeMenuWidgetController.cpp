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
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);

	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	OnAttributePointsAddedDelegate.Broadcast(AuraPlayerState->GetAttributePoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);

	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair, AS](const FOnAttributeChangeData& Data) 
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->AttributePointsDelegate.AddUObject(this, &UAttributeMenuWidgetController::OnAttributePointsAdded);
}

void UAttributeMenuWidgetController::OnAttributePointsAdded(int32 IncomingAttributePoints)
{
	OnAttributePointsAddedDelegate.Broadcast(IncomingAttributePoints);
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->UpgradeAttribute(AttributeTag);
}

//void UAttributeMenuWidgetController::SubstractAttributePoint()
//{
//	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
//	AuraPlayerState->AddToAttributePoints(-1);
//}
//
//int32 UAttributeMenuWidgetController::GetAvailableAttributePoints()
//{
//	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
//	return AuraPlayerState->GetAttributePoints();
//}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::AddPointToAttribute(const FString& AttributeName) const
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	/*FName TagName = FName(*AttributeName);
	FGameplayTag AttributeTag = FGameplayTag::RequestGameplayTag(TagName);*/
	for (auto& Pair : AS->TagsToAttributes)
	{
		if (Pair.Key.ToString().Find(AttributeName) >= 0)
		{
			const FGameplayAttribute Attribute = Pair.Value();
			AbilitySystemComponent->ApplyModToAttribute(Attribute, EGameplayModOp::Additive, 1);
			IPlayerInterface::Execute_RecalculateSecondaryAttributes(AbilitySystemComponent->GetAvatarActor());
		}
	}
	
}
