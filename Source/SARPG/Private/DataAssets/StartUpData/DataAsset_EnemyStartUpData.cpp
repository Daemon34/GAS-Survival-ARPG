// Clement Lecoeur All Rights Reserved


#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"
#include "AbilitySystem/SARPGAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/SARPGEnemyGameplayAbility.h"

void UDataAsset_EnemyStartUpData::GiveToAbilitySystemComponent(USARPGAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

	if (!EnemyCombatAbilities.IsEmpty()) {
		for (const TSubclassOf<USARPGEnemyGameplayAbility>& AbilityClass : EnemyCombatAbilities) {
			if (!AbilityClass) continue;

			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;
			InASCToGive->GiveAbility(AbilitySpec);
		}
	}
}
