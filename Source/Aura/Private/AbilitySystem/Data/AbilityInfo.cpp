// Copyright Sparrow Inc.


#include "AbilitySystem/Data/AbilityInfo.h"

FAuraAbilityInfo UAbilityInfo::GetAbilityInfo(const FGameplayTag& GameplayTag)
{
	for (const FAuraAbilityInfo& AbilityInfo : AbilityInfos)
	{
		if (AbilityInfo.AbilityTag.MatchesTagExact(GameplayTag))
		{
			return AbilityInfo;
		}
	}

	return FAuraAbilityInfo();
}
