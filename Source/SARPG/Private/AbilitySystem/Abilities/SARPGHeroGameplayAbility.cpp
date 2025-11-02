// Clement Lecoeur All Rights Reserved


#include "AbilitySystem/Abilities/SARPGHeroGameplayAbility.h"
#include "Characters/SARPGHeroCharacter.h"
#include "Controllers/SARPGHeroController.h"
#include "AbilitySystem/SARPGAbilitySystemComponent.h"
#include "SARPGGameplayTags.h"

ASARPGHeroCharacter* USARPGHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
    if (!CachedSARPGHeroCharacter.IsValid()) {
        CachedSARPGHeroCharacter = Cast<ASARPGHeroCharacter>(CurrentActorInfo->AvatarActor);
    }
    return CachedSARPGHeroCharacter.IsValid() ? CachedSARPGHeroCharacter.Get() : nullptr;
}

ASARPGHeroController* USARPGHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
    if (!CachedSARPGHeroController.IsValid()) {
        CachedSARPGHeroController = Cast<ASARPGHeroController>(CurrentActorInfo->PlayerController);
    }
    return CachedSARPGHeroController.IsValid() ? CachedSARPGHeroController.Get() : nullptr;
}

UHeroCombatComponent* USARPGHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
    return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

UHeroUIComponent* USARPGHeroGameplayAbility::GetHeroUIComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroUIComponent();
}

FGameplayEffectSpecHandle USARPGHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount)
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

    EffectSpecHandle.Data->SetSetByCallerMagnitude(SARPGGameplayTags::Shared_SetByCaller_BaseDamage, InWeaponBaseDamage);

    if (InCurrentAttackTypeTag.IsValid()) {
        EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, InUsedComboCount);
    }

    return EffectSpecHandle;
}

bool USARPGHeroGameplayAbility::GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainingCooldownTime)
{
	check(InCooldownTag.IsValid());

	FGameplayEffectQuery gameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTag.GetSingleTagContainer());
    TArray<TPair<float, float>> TimeRemainingAndDuration = GetAbilitySystemComponentFromActorInfo()->GetActiveEffectsTimeRemainingAndDuration(gameplayEffectQuery);

    if (!TimeRemainingAndDuration.IsEmpty()) {
		TotalCooldownTime = TimeRemainingAndDuration[0].Value;
        RemainingCooldownTime = TimeRemainingAndDuration[0].Key;
    }

    return RemainingCooldownTime > 0.0f;
}
