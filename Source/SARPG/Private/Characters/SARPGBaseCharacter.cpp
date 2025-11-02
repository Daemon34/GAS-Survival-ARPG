// Clement Lecoeur All Rights Reserved


#include "Characters/SARPGBaseCharacter.h"
#include "AbilitySystem/SARPGAbilitySystemComponent.h"
#include "AbilitySystem/SARPGAttributeSet.h"
#include "MotionWarpingComponent.h"

// Sets default values
ASARPGBaseCharacter::ASARPGBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;

	SARPGAbilitySystemComponent = CreateDefaultSubobject<USARPGAbilitySystemComponent>(TEXT("SARPGAbilitySystemComponent"));
	SARPGAttributeSet = CreateDefaultSubobject<USARPGAttributeSet>(TEXT("SARPGAttributeSet"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

UAbilitySystemComponent* ASARPGBaseCharacter::GetAbilitySystemComponent() const
{
	return GetSARPGAbilitySystemComponent();
}

UPawnCombatComponent* ASARPGBaseCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

UPawnUIComponent* ASARPGBaseCharacter::GetPawnUIComponent() const
{
	return nullptr;
}

void ASARPGBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (SARPGAbilitySystemComponent) {
		SARPGAbilitySystemComponent->InitAbilityActorInfo(this, this);
		ensureMsgf(CharacterStartUpData.IsNull(), TEXT("Forgot to assign start up data to %s"), *GetName());
	}
}

