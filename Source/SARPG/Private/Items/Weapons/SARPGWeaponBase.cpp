// Clement Lecoeur All Rights Reserved


#include "Items/Weapons/SARPGWeaponBase.h"
#include "Components/BoxComponent.h"
#include "SARPGFunctionLibrary.h"

#include "SARPGDebugHelper.h"

ASARPGWeaponBase::ASARPGWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	WeaponCollisionBox->SetBoxExtent(FVector(20.0f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxEndOverlap);
}

void ASARPGWeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instigator as the owning pawn for the weapon : %s"), *GetName());

	if (APawn* HitPawn = Cast<APawn>(OtherActor)) {
		if (USARPGFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn)) {
			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
		}
	}
}

void ASARPGWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instigator as the owning pawn for the weapon : %s"), *GetName());

	if (APawn* HitPawn = Cast<APawn>(OtherActor)) {
		if (USARPGFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn)) {
			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
		}
	}
}
