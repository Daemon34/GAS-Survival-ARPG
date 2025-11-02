// Clement Lecoeur All Rights Reserved


#include "AbilitySystem/Abilities/SARPGEnemyGameplayAbility.h"
#include "Characters/SARPGEnemyCharacter.h"
#include "AbilitySystem/SARPGAbilitySystemComponent.h"
#include "SARPGGameplayTags.h"

ASARPGEnemyCharacter* USARPGEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
    if(!CachedSARPGEnemyCharacter.IsValid())
    {
        CachedSARPGEnemyCharacter = Cast<ASARPGEnemyCharacter>(CurrentActorInfo->AvatarActor);
	}
    return CachedSARPGEnemyCharacter.IsValid() ? CachedSARPGEnemyCharacter.Get() : nullptr;
}

UEnemyCombatComponent* USARPGEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	return GetEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();
}

FGameplayEffectSpecHandle USARPGEnemyGameplayAbility::MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat)
{
	check(EffectClass);

    FGameplayEffectContextHandle ContextHandle = GetSARPGAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
    ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

    FGameplayEffectSpecHandle EffectSpecHandle = GetSARPGAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
        EffectClass,
        GetAbilityLevel(),
        ContextHandle
    );

    EffectSpecHandle.Data->SetSetByCallerMagnitude(SARPGGameplayTags::Shared_SetByCaller_BaseDamage, InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel()));

    return EffectSpecHandle;
}
