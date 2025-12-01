// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public: 
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(float IncomingXP);

	UFUNCTION(BlueprintNativeEvent)
	void AddToLevel(int32 NumLevelUps);

	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 inSpellPoints);

	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 inAttributePoints);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP();

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(int32 inLevel);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(int32 inLevel);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints();

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints();

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(float inXP);

	UFUNCTION(BlueprintNativeEvent)
	void RecalculateSecondaryAttributes();
};
