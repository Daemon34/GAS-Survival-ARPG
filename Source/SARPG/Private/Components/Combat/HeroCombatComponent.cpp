// Clement Lecoeur All Rights Reserved


#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapons/SARPGHeroWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "SARPGGameplayTags.h"

#include "SARPGDebugHelper.h"

ASARPGHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
    return Cast<ASARPGHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

ASARPGHeroWeapon* UHeroCombatComponent::GetHeroCurrentEquippedWeapon() const
{
    return Cast<ASARPGHeroWeapon>(GetCharacterCurrentEquippedWeapon());
}

float UHeroCombatComponent::GetHeroCurrentEquippedWeaponDamageAtLevel(float InLevel) const
{
    return GetHeroCurrentEquippedWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
    if (OverlappedActors.Contains(HitActor))
    {
        return;
    }

    OverlappedActors.AddUnique(HitActor);

    FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
        GetOwningPawn(),
        SARPGGameplayTags::Shared_Event_MeleeHit,
        Data
    );

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
        GetOwningPawn(),
        SARPGGameplayTags::Player_Event_HitPause,
        FGameplayEventData()
    );
}

void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InterractedActor)
{
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
        GetOwningPawn(),
        SARPGGameplayTags::Player_Event_HitPause,
        FGameplayEventData()
    );
}
