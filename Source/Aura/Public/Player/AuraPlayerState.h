// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AuraPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32);
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AAuraPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	const FLevelUpInfo& GetLevelUpInfo(int32 Level);

	FORCEINLINE int32 GetXP() const { return XP; }
	int32 FindLevelForXP(int32 IncomingXP);

	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }

	void GrantXP(int32 IncomingXP);
	void AddToLevel(int32 NumLevelUps);
	void SetLevel(int32 IncomingLevel);
	void AddToAttributePoints(int32 IncomingAttributePoints);
	void AddToSpellPoints(int32 IncomingSpellPoints);

	FLevelUpInfo FindLevelInfo();

	FOnPlayerStatChanged XPDelegate;
	FOnPlayerStatChanged LevelDelegate;
	FOnPlayerStatChanged SpellPointsDelegate;
	FOnPlayerStatChanged AttributePointsDelegate;

protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfoAsset> LevelUpInfo;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	int32 XP = 0;
	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoints)
	int32 AttributePoints = 2;
	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoints)
	int32 SpellPoints = 2;
	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);
};
