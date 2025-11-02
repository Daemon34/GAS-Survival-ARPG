// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SARPGGameplayAbility.h"
#include "SARPGHeroGameplayAbility.generated.h"

class ASARPGHeroCharacter;
class ASARPGHeroController;
/**
 * 
 */
UCLASS()
class SARPG_API USARPGHeroGameplayAbility : public USARPGGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "SARPG|Ability")
	ASARPGHeroCharacter* GetHeroCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "SARPG|Ability")
	ASARPGHeroController* GetHeroControllerFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "SARPG|Ability")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "SARPG|Ability")
	UHeroUIComponent* GetHeroUIComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "SARPG|Ability")
	FGameplayEffectSpecHandle MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount);

	UFUNCTION(BlueprintCallable, Category = "SARPG|Ability")
	bool GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainingCooldownTime);
private:
	TWeakObjectPtr<ASARPGHeroCharacter> CachedSARPGHeroCharacter;
	TWeakObjectPtr<ASARPGHeroController> CachedSARPGHeroController;
};
