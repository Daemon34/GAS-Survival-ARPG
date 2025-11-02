// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SARPGTypes/SARPGStructTypes.h"
#include "SARPGAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class SARPG_API USARPGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	UFUNCTION(BlueprintCallable, Category = "SARPG|Ability", meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbilities(const TArray<FSARPGHeroAbilitySet>& InDefaultWeaponAbilities, const TArray<FSARPGHeroSpecialAbilitySet>& InDefaultSpecialWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);

	UFUNCTION(BlueprintCallable, Category = "SARPG|Ability")
	void RemoveGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category = "SARPG|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
};
