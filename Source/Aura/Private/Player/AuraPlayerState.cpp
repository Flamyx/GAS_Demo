// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

const FLevelUpInfo& AAuraPlayerState::GetLevelUpInfo(int32 InLevel)
{
	if (InLevel < LevelUpInfo->LevelUpInfos.Num())
		return LevelUpInfo->LevelUpInfos[InLevel];
	return LevelUpInfo->LevelUpInfos[0];
}

int32 AAuraPlayerState::FindLevelForXP(int32 IncomingXP)
{
	int32 ReturnLevel = -1;
	for (int i = 1; i < LevelUpInfo->LevelUpInfos.Num(); ++i)
	{
		auto CurrentLevelUpInfo = LevelUpInfo->LevelUpInfos[i];
		if (IncomingXP >= CurrentLevelUpInfo.LevelUpRequirement)
			ReturnLevel = i;
	}

	return ReturnLevel;
}

void AAuraPlayerState::GrantXP(int32 IncomingXP)
{
	XP += IncomingXP;
	XPDelegate.Broadcast(XP);
}

void AAuraPlayerState::AddToLevel(int32 NumLevelUps)
{
	SetLevel(NumLevelUps + Level);
}

void AAuraPlayerState::SetLevel(int32 IncomingLevel)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString(TEXT("In Set Level")));
	Level = IncomingLevel;
	LevelDelegate.Broadcast(Level);
}

void AAuraPlayerState::AddToAttributePoints(int32 IncomingAttributePoints)
{
	AttributePoints += IncomingAttributePoints;
	AttributePointsDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddToSpellPoints(int32 IncomingSpellPoints)
{
	SpellPoints += IncomingSpellPoints;
	SpellPointsDelegate.Broadcast(SpellPoints);
}

FLevelUpInfo AAuraPlayerState::FindLevelInfo()
{
	return LevelUpInfo->LevelUpInfos[FMath::Min(LevelUpInfo->LevelUpInfos.Num(), Level)];
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	LevelDelegate.Broadcast(Level);
}

void AAuraPlayerState::OnRep_XP(int32 OldXP)
{
	XPDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	AttributePointsDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	SpellPointsDelegate.Broadcast(SpellPoints);
}
