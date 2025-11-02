// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SARPGTypes/SARPGEnumTypes.h"
#include "SARPGGameplayAbility.generated.h"

class UPawnCombatComponent;
class USARPGAbilitySystemComponent;

UENUM(BlueprintType)
enum class ESARPGAbilityActivationPolicy : uint8
{
	Ontriggered,
	OnGiven
};
/**
 * 
 */
UCLASS()
class SARPG_API USARPGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	//~ Begin UGameplayAbility Interface.
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface

	UPROPERTY(EditDefaultsOnly, Category = "SARPGAbility")
	ESARPGAbilityActivationPolicy AbilityActivationPolicy = ESARPGAbilityActivationPolicy::Ontriggered;

	UFUNCTION(BlueprintPure, Category = "SARPG|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "SARPG|Ability")
	USARPGAbilitySystemComponent* GetSARPGAbilitySystemComponentFromActorInfo() const;

	FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "SARPG|Ability", meta = (DisplayName = "Apply Gameplay Effect Spec Handle To Target Actor", ExpandEnumAsExecs = "OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, ESARPGSuccessType& OutSuccessType);

	UFUNCTION(BlueprintCallable, Category = "SARPG|Ability")
	void ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults);

};
