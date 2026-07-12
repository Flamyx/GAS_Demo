// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/AuraDamageGameplayAbility.h"
#include "ElectrocuteCourse.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UElectrocuteCourse : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();
	
	UFUNCTION(BlueprintCallable)
	void StoreHitResult(const FHitResult& HitResult);
	
protected:
	UPROPERTY(BlueprintReadWrite)
	FVector MouseHitLocation;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> MouseHitActor;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ACharacter> OwnerCharacter;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> OwnerPlayerController;
	
};
