// Clement Lecoeur All Rights Reserved


#include "AnimInstances/Hero/SARPGHeroAnimInstance.h"
#include "Characters/SARPGHeroCharacter.h"

void USARPGHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwningCharacter) {
		OwningHeroCharacter = Cast<ASARPGHeroCharacter>(OwningCharacter);
	}
}

void USARPGHeroAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (bHasAcceleration) {
		IdleElapsedTime = 0.0f;
		bShouldEnterRelaxState = false;
	}
	else {
		IdleElapsedTime += DeltaSeconds;
		bShouldEnterRelaxState = (IdleElapsedTime >= EnterRelaxStateThreshold);
	}
}
