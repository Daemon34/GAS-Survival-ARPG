// Clement Lecoeur All Rights Reserved


#include "AnimInstances/Hero/SARPGHeroLinkedAnimLayer.h"
#include "AnimInstances/Hero/SARPGHeroAnimInstance.h"

USARPGHeroAnimInstance* USARPGHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
	return Cast<USARPGHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
