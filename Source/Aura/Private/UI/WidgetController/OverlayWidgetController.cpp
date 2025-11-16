// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) {
				OnHealthChanged.Broadcast(Data.NewValue);}
		);
	

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) {
				OnMaxHealthChanged.Broadcast(Data.NewValue);}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) {
				OnManaChanged.Broadcast(Data.NewValue);}
		);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) {
				OnMaxManaChanged.Broadcast(Data.NewValue);}
		);

	/* Example of a Callback way instead of Lambda
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxManaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxManaChanged);*/

	if (auto AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (!AuraASC->bStartupAbilitiesGiven)
			AuraASC->AbilitiesGiven.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		else
			OnInitializeStartupAbilities(AuraASC);

		AuraASC->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						FUIWidgetRow* Row = GetDataTableByRow<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
					/*const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
					GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, Msg);*/
				}
			}
		);
	}

	AuraPlayerState->XPDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	AuraPlayerState->LevelDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}
	);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* Aura_ASC)
{
	if (!Aura_ASC->bStartupAbilitiesGiven)
		return;
	
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{ 
		FGameplayTag AbilityTag = UAuraAbilitySystemLibrary::FindAbilityTagFromSpec(AbilitySpec);
		FAuraAbilityInfo AbilityCfg = AbilityInfo->GetAbilityInfo(AbilityTag);
		AbilityCfg.InputTag = UAuraAbilitySystemLibrary::FindInputTagFromSpec(AbilitySpec);
		AbilityCfgDelegate.Broadcast(AbilityCfg);
	});

	Aura_ASC->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnXPChanged(int32 newXP)
{
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	int32 PlayerLevel = AuraPlayerState->FindLevelForXP(newXP);

	auto CurrentLevelInfo = AuraPlayerState->GetLevelUpInfo(PlayerLevel);
	auto NextLevelInfo = AuraPlayerState->GetLevelUpInfo(PlayerLevel + 1);
	if (!CurrentLevelInfo.LevelUpRequirement || !NextLevelInfo.LevelUpRequirement)
		return;

	float PercentageXP = static_cast<float>(newXP - CurrentLevelInfo.LevelUpRequirement) / static_cast<float>(NextLevelInfo.LevelUpRequirement - CurrentLevelInfo.LevelUpRequirement);
	OnXPChangedDelegate.Broadcast(PercentageXP);
}


void UOverlayWidgetController::OnLvlUp(const FLevelUpInfo& LvlUpInfo)
{
	OnSpellPointsAddedDelegate.Broadcast(LvlUpInfo.SpellPointReward);
	OnAttributePointsAddedDelegate.Broadcast(LvlUpInfo.AttributePointReward);
}
