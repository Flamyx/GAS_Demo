// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SpellInfo.generated.h"


USTRUCT(BlueprintType)
struct FAuraSpellInfo {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SpellTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Description = FString();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCurveTable> SpellUpgradeRequirement;

};

/**
 * 
 */
UCLASS()
class AURA_API USpellInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell Info")
	TArray<FAuraSpellInfo> SpellInfos;

	FAuraSpellInfo GetSpellInfo(const FGameplayTag& GameplayTag);
	
};
