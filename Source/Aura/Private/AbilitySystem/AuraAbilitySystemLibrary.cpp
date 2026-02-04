// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Overlay/AuraHud.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Game/AuraGameModeBase.h"
#include "Character/AuraEnemy.h"
#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"
#include "Engine/OverlapResult.h"
#include "AuraAbilityTypes.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetContoller(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAttributeSet* AS = PS->GetAttributeSet();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			UOverlayWidgetController* OWC = AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
			return OWC;
		}
	}

	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAttributeSet* AS = PS->GetAttributeSet();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			UAttributeMenuWidgetController* OWC = AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
			return OWC;
		}
	}

	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAttributeSet* AS = PS->GetAttributeSet();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			USpellMenuWidgetController* SWC = AuraHUD->GetSpellMenuWidgetController(WidgetControllerParams);
			return SWC;
		}
	}
	return nullptr;
}

bool UAuraAbilitySystemLibrary::UpdateOverlay(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AuraHUD->UpdateOverlay();
			return true;
		}
	}
	return false;
}

FGameplayTag UAuraAbilitySystemLibrary::FindInputTagFromAbilityInfo(const UObject* WorldContextObject, const FGameplayTag& AbilityTag)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return FGameplayTag();
	auto AbilityInfo = AuraGameMode->AbilityInfo;
	return AbilityInfo->GetAbilityInfo(AbilityTag).InputTag;
}

FGameplayTag UAuraAbilitySystemLibrary::FindStatusTagFromAbilityInfo(const UObject* WorldContextObject,
	const FGameplayTag& AbilityTag)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return FGameplayTag();
	auto AbilityInfo = AuraGameMode->AbilityInfo;
	
	return AbilityInfo->GetAbilityInfo(AbilityTag).StatusTag;

}

void UAuraAbilitySystemLibrary::InitializeEnemyAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	auto CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo CharacterInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	auto PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(ASC->GetAvatarActor());
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());
	
	auto SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(ASC->GetAvatarActor());
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	auto VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(ASC->GetAvatarActor());
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	auto CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;

	for (auto AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
		
	}

	const FCharacterClassDefaultInfo DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (auto AbilityClass : DefaultInfo.StartupAbilities)
	{
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

float UAuraAbilitySystemLibrary::GetXPReward(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	auto CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) 
		return 0;

	auto Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

	return XPReward;
}

UBehaviorTree* UAuraAbilitySystemLibrary::GetBehaviorTree(const UObject* WorldContextObject, ECharacterClass CharacterClass)
{
	auto CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	const FCharacterClassDefaultInfo DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	UBehaviorTree* CharacterClassBT = DefaultInfo.BehaviorTree;
	return CharacterClassBT;
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return nullptr;
	auto CharacterClassInfo = AuraGameMode->CharacterClassInfo;
	return CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return nullptr;
	auto AbilityInfo = AuraGameMode->AbilityInfo;
	return AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	const AActor* SourceActor = Cast<AActor>(WorldContextObject);
	AActor* MutableSourceActor = const_cast<AActor*>(SourceActor);

	// query scene to see what we hit
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, 
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), 
			FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			AActor* Actor = Overlap.GetActor();
			if (Actor->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Actor) && UAuraAbilitySystemLibrary::IsNotFriend(MutableSourceActor, Actor))
			{
				auto AuraCharacter = Cast<AAuraCharacterBase>(Actor);
				
				// Can simply use Actor, but with GetAvatar it's ok as well
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Actor));
			}
		}
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bFirstIsPlayer = FirstActor->ActorHasTag(FName("Player"));
	const bool bSecondIsPlayer = SecondActor->ActorHasTag(FName("Player"));
	const bool bFriendly = bFirstIsPlayer == bSecondIsPlayer;
	return !bFriendly;
}

FGameplayTag UAuraAbilitySystemLibrary::FindAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (auto Tag : AbilitySpec.Ability.Get()->GetAssetTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
				return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemLibrary::FindInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability) 
	{
		for (auto Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
				return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemLibrary::FindStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (auto StatusTag: AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Status"))))
			return StatusTag;
	}
	return FGameplayTag();
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}
