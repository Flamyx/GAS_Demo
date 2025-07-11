// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "TeleportToLocation.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTeleportToLocationSimpleDelegate);

/**
 * 
 */
UCLASS()
class AURA_API UTeleportToLocation : public UAbilityTask
{
	GENERATED_BODY()

public:
	//UAbilityTask_PlayMontageAndWait Check;
	UFUNCTION(BlueprintCallable,
		Category = "Ability|Tasks",
		meta = (DisplayName = "TeleportToLocation", HidePin = "OwningAbility",
			DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTeleportToLocation* CreateTeleportToLocation(UGameplayAbility* OwningAbility, AActor* Actor, const FVector& TargetLocation);

	UPROPERTY(BlueprintAssignable)
	FTeleportToLocationSimpleDelegate OnCompleted;

protected:
	UPROPERTY()
	AActor* Actor;

	UPROPERTY()
	FVector3d SpawnLocation;

private:
	virtual void Activate() override;

	void TeleportActor();
	
};
