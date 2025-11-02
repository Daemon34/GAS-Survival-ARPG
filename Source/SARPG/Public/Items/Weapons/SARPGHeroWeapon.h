// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/SARPGWeaponBase.h"
#include "SARPGTypes/SARPGStructTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "SARPGHeroWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SARPG_API ASARPGHeroWeapon : public ASARPGWeaponBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	FSARPGHeroWeaponData HeroWeaponData;

	UFUNCTION(BlueprintCallable)
	void AssignGrantedAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles);

	UFUNCTION(BlueprintPure)
	TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;

private:
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
};
