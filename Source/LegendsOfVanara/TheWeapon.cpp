// Fill out your copyright notice in the Description page of Project Settings.


#include "TheWeapon.h"
#include "LegendsOfVanaraCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SplineComponent.h"

// Sets default values
ATheWeapon::ATheWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = WeaponMesh;

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile");
	ProjectileMovement->UpdatedComponent = WeaponMesh;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bAutoActivate = false;
	ProjectileMovement->bRotationFollowsVelocity = true;

	ReturnSpline = CreateDefaultSubobject<USplineComponent>("Spline");
}

// Called when the game starts or when spawned
void ATheWeapon::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATheWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Flying state
	if (WeaponState == EWeaponState::Flying)
	{
		float Distance = FVector::Dist(GetActorLocation(), ThrowStartLocation);

		AddActorLocalRotation(FRotator(4000.f * DeltaTime, 0.f, 0.f));

		if (Distance > MaxThrowDistance)
		{
			RecallWeapon();
		}
	}

	//Returning state
	if (WeaponState == EWeaponState::Returning)
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
		if (OwnerCharacter)
		{
			FVector HandLocation = OwnerCharacter->GetMesh()->GetSocketLocation(TEXT("Gada_Socket"));

			//updating the spline end point in tick 
			ReturnSpline->SetLocationAtSplinePoint(
				2,
				HandLocation,
				ESplineCoordinateSpace::World
			);

			ReturnSpline->UpdateSpline();
		}

		RecallAlpha += DeltaTime * RecallSpeed;

		float SplineLength = ReturnSpline->GetSplineLength();
		float Distance = RecallAlpha * SplineLength;

		FVector NewLocation = ReturnSpline->GetLocationAtDistanceAlongSpline(
			Distance,
			ESplineCoordinateSpace::World);

		SetActorLocation(NewLocation);

		// spin weapon
		AddActorLocalRotation(FRotator(4000.f * DeltaTime, 0.f,  0.f));

		if (RecallAlpha >= 1.f)
		{
			CatchWeapon();
		}
	}

}

void ATheWeapon::ThrowWeapon(const FVector& Direction)
{
	if (WeaponState != EWeaponState::InHand)
		return;

	WeaponState = EWeaponState::Flying;

	BP_StartTrailVFX();
	BP_PlayFlyingSound();

	ThrowStartLocation = GetActorLocation();

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bSweepCollision = true;
	ProjectileMovement->Velocity = Direction.GetSafeNormal() * ProjectileMovement->InitialSpeed;
	ProjectileMovement->Activate();
}

void ATheWeapon::RecallWeapon()
{
	if (WeaponState == EWeaponState::Returning)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("TheWeapon::RecallWeapon()"));

	WeaponState = EWeaponState::Returning;

	BP_StartTrailVFX();

	ProjectileMovement->Deactivate();

	GenerateReturnSpline();

	RecallAlpha = 0.f;
}

void ATheWeapon::CatchWeapon()
{
	WeaponState = EWeaponState::InHand;

	BP_StopFlyingSound();

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement->StopMovementImmediately();

	ALegendsOfVanaraCharacter* OwnerCharacter = Cast<ALegendsOfVanaraCharacter>(GetOwner());

	OwnerCharacter->BP_PlayCatchAnimation();
	OwnerCharacter->isAttacking = false;

	if (OwnerCharacter)
	{
		AttachToComponent(
			OwnerCharacter->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("Gada_Socket")
		);
	}
}

void ATheWeapon::GenerateReturnSpline()
{
	ReturnSpline->ClearSplinePoints();

	FVector Start = GetActorLocation();

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
		return;

	//get hand socket location
	FVector End = OwnerCharacter->GetMesh()->GetSocketLocation(TEXT("Gada_Socket"));

	FVector Mid = (Start + End) * 0.5f;
	Mid.Z += 300.f;
	Mid.X += -300.f;

	ReturnSpline->AddSplinePoint(Start, ESplineCoordinateSpace::World);
	ReturnSpline->AddSplinePoint(Mid, ESplineCoordinateSpace::World);
	ReturnSpline->AddSplinePoint(End, ESplineCoordinateSpace::World);

	ReturnSpline->UpdateSpline();
}
