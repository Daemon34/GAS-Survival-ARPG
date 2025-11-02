// Clement Lecoeur All Rights Reserved


#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/SARPGAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/SARPGGameplayAbility.h"

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(USARPGAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	check(InASCToGive);

	GrantAbilities(ActivateOnGivenAbilities, InASCToGive, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);

	if (!StartUpGameplayEffects.IsEmpty()) {
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects) {
			if (!EffectClass) continue; 

			UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();
			InASCToGive->ApplyGameplayEffectToSelf(EffectCDO, ApplyLevel, InASCToGive->MakeEffectContext());
		}
	}
}

void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<USARPGGameplayAbility>>& InAbilitiesToGive, USARPGAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty()) {
		return;
	}

	for (const TSubclassOf<USARPGGameplayAbility>& Ability : InAbilitiesToGive) {
		if (!Ability) continue;

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		InASCToGive->GiveAbility(AbilitySpec);
	}
}
