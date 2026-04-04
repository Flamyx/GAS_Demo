// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AuraGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
};
