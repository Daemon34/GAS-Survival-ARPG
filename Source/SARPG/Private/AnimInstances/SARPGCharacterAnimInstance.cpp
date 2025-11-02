// Clement Lecoeur All Rights Reserved


#include "AnimInstances/SARPGCharacterAnimInstance.h"
#include "Characters/SARPGBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

void USARPGCharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<ASARPGBaseCharacter>(TryGetPawnOwner());

	if (OwningCharacter) {
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void USARPGCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent) {
		return;
	}

	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.0f;

	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
}
