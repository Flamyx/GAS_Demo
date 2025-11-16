// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/AuraGameplayAbility.h"
#include "NiagaraFunctionLibrary.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* SpawnCircle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Spread = 90.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinSpawnDistance = 150;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxSpawnDistance = 400;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MinionSpawnNumber = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MinionSpawnLimit = 5;
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<APawn>> MinionClasses;
	
	UFUNCTION(BlueprintPure)
	static int32 GetCurrentMinionNumber();
	

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSummonLocations();
	UFUNCTION(BlueprintPure)
	TSubclassOf<APawn> GetRandomMinionClass();

private:
	static int CurrentMinionNumber;
};
