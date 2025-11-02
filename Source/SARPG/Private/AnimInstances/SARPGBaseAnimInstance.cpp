// Clement Lecoeur All Rights Reserved


#include "AnimInstances/SARPGBaseAnimInstance.h"
#include "SARPGFunctionLibrary.h"

bool USARPGBaseAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
{
    if (APawn* OwningPawn = TryGetPawnOwner()) {
        return USARPGFunctionLibrary::NativeDoesActorHaveTag(OwningPawn, TagToCheck);
    }

    return false;
}
