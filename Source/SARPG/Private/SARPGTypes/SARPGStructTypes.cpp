// Clement Lecoeur All Rights Reserved


#include "SARPGTypes/SARPGStructTypes.h"
#include "AbilitySystem/Abilities/SARPGHeroGameplayAbility.h"

bool FSARPGHeroAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}
