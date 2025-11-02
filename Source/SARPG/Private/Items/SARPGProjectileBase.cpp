// Clement Lecoeur All Rights Reserved


#include "Items/SARPGProjectileBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SARPGFunctionLibrary.h"
#include "SARPGGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "SARPGDebugHelper.h"

ASARPGProjectileBase::ASARPGProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);

	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 700.0f;
	ProjectileMovementComponent->MaxSpeed = 900.0f;
	ProjectileMovementComponent->Velocity = FVector(1.0f, 0.0f, 0.0f);
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	InitialLifeSpan = 4.0f;
}

void ASARPGProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap) {
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

void ASARPGProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);

	APawn* HitPawn = Cast<APawn>(OtherActor);
	if (!HitPawn || !USARPGFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn)) {
		Destroy();
		return;
	}

	bool bIsValidBlock = false;
	const bool bIsPlayerBlocking = USARPGFunctionLibrary::NativeDoesActorHaveTag(HitPawn, SARPGGameplayTags::Player_Status_Blocking);

	if(bIsPlayerBlocking) {
		bIsValidBlock = USARPGFunctionLibrary::IsValidBlock(this, HitPawn);
	}

	FGameplayEventData Data;
	Data.Instigator = this;
	Data.Target = HitPawn;

	if(bIsValidBlock){
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitPawn,
			SARPGGameplayTags::Player_Event_SuccessfulBlock,
			Data
		);
	}
	else {
		HandleApplyProjectileDamage(HitPawn, Data);
	}

	Destroy();
}

void ASARPGProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedActors.Contains(OtherActor)) {
		return;
	}

	OverlappedActors.AddUnique(OtherActor);

	if (APawn* HitPawn = Cast<APawn>(OtherActor)) {
		if (USARPGFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn)) {
			FGameplayEventData Data;
			Data.Instigator = GetInstigator();
			Data.Target = HitPawn;

			HandleApplyProjectileDamage(HitPawn, Data);
		}
	}
}

void ASARPGProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	checkf(ProjectileDamageEffectSpecHandle.IsValid(), TEXT("Forgot to assign a valid spec handle to the projectile: %s"), *GetActorNameOrLabel());

	const bool bWasApplied = USARPGFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);

	if (bWasApplied) {
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			SARPGGameplayTags::Shared_Event_HitReact,
			InPayload
		);
	}
}

