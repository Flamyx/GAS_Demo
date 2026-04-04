#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAuraDamageEffectParams
{
	GENERATED_BODY()
	
	FAuraDamageEffectParams(){};
	UPROPERTY()
	TObjectPtr<class UObject> WorldContextObject = nullptr;
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass = nullptr;
	UPROPERTY()
	TSubclassOf<UGameplayEffect> ResidualDamageEffectClass = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;
	
	FGameplayTag DamageType = FGameplayTag();
	
	float Damage = 0.f;
	float AbilityLevel = 0.f;
	float DebuffChance = 0.f;
	float DebuffFrequency = 0.f;
	float DebuffDamage = 0.f;
	float DebuffDuration = 0.f;
	
	float DeathImpulseMagnitude = 0.f;
	FVector DeathImpulse = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool IsSuccessfulDebuff() const { return bIsSuccessfulDebuff; }
	float GetDebuffDamage() const { return DebuffDamage; }
	float GetDebuffDuration() const { return DebuffDuration; }
	float GetDebuffFrequency() const { return DebuffFrequency; }
	FGameplayTag GetDamageType() const { return *DamageType; }
	FVector GetDeathImpulse() const { return DeathImpulse; }

	void SetIsCriticalHit(bool Value) { bIsCriticalHit = Value;  }
	void SetIsBlockedHit(bool Value) { bIsBlockedHit = Value;  }
	void SetIsSuccessfulDebuff(bool Value) { bIsSuccessfulDebuff = Value;  }
	void SetDebuffDuration(float Value) { DebuffDuration = Value; }
	void SetDebuffFrequency(float Value) { DebuffFrequency = Value; }
	void SetDebuffDamage(float Value) { DebuffDamage = Value; }
	void SetDamageType(TSharedPtr<FGameplayTag> Value) { DamageType = Value; }
	void SetDeathImpulse(const FVector& Value) { DeathImpulse = Value; }

	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const
	{
		return StaticStruct();
	}

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

protected:

	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;
	
	UPROPERTY()
	bool bIsSuccessfulDebuff = false;
	
	UPROPERTY()
	float DebuffDuration = 0.f;
	
	UPROPERTY()
	float DebuffFrequency = 0.f;
	
	UPROPERTY()
	float DebuffDamage = 0.f;
	
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;
	
	TSharedPtr<FGameplayTag> DamageType;
};

template<>
struct TStructOpsTypeTraits< FAuraGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FAuraGameplayEffectContext >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};