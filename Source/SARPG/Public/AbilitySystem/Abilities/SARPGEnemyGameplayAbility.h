// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SARPGGameplayAbility.h"
#include "SARPGEnemyGameplayAbility.generated.h"

class ASARPGEnemyCharacter;
class UEnemyCombatComponent;
/**
 * 
 */
UCLASS()
class SARPG_API USARPGEnemyGameplayAbility : public USARPGGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "SARPG|Ability")
	ASARPGEnemyCharacter* GetEnemyCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "SARPG|Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "SARPG|Ability")
	FGameplayEffectSpecHandle MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat);

private:
	TWeakObjectPtr<ASARPGEnemyCharacter> CachedSARPGEnemyCharacter;
};
