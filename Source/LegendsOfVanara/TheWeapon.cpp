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

	// Capsule as ROOT again
	//CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>("CollisionCapsule");

	/*RootComponent = CollisionCapsule;

	CollisionCapsule->InitCapsuleSize(20.f, 60.f);
	CollisionCapsule->SetRelativeRotation(FRotator(0.f, 0.f, 90.f));
	CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionCapsule->SetCollisionProfileName(TEXT("BlockAll"));
	CollisionCapsule->SetGenerateOverlapEvents(false);
	CollisionCapsule->SetNotifyRigidBodyCollision(true);*/

	//SpinPivot = CreateDefaultSubobject<USceneComponent>("SpinPivot");
	//RootComponent = SpinPivot;


	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	//WeaponMesh->SetupAttachment(SpinPivot);
	RootComponent = WeaponMesh;

	//WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f)); // adjust axis
	//WeaponMesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile");
	ProjectileMovement->UpdatedComponent = WeaponMesh;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bAutoActivate = false;
	ProjectileMovement->bRotationFollowsVelocity = true;

	ReturnSpline = CreateDefaultSubobject<USplineComponent>("Spline");

	//WeaponMesh->OnComponentHit.AddDynamic(this, &ATheWeapon::OnHit);

}

// Called when the game starts or when spawned
void ATheWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	//CollisionCapsule->OnComponentHit.AddDynamic(this, &ATheWeapon::OnHit);

}

// Called every frame
void ATheWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (WeaponState == EWeaponState::Flying)
	{
		float Distance = FVector::Dist(GetActorLocation(), ThrowStartLocation);

		AddActorLocalRotation(FRotator(4000.f * DeltaTime, 0.f, 0.f));

		if (Distance > MaxThrowDistance)
		{
			RecallWeapon();
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red,
		FString::Printf(TEXT("Weapon Location: %s"), *GetActorLocation().ToString()));

	/*DrawDebugCapsule(
		GetWorld(),
		GetActorLocation(),  // use actor location, not component
		40.f, 10.f,
		FQuat::Identity,
		FColor::Red, false, -1.f, 0, 3.f
	);*/

	if (WeaponState != EWeaponState::InHand)
	{
		//AddActorLocalRotation(FRotator(0, 0,2000 * DeltaTime));
		//AddActorWorldRotation(FRotator(0, 0,1000 * DeltaTime));
	}

	if (WeaponState == EWeaponState::Returning)
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
		if (OwnerCharacter)
		{
			FVector HandLocation = OwnerCharacter->GetMesh()->GetSocketLocation(TEXT("Gada_Socket"));

			// Update the END point of the spline
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
		//SpinPivot->AddLocalRotation(FRotator(1500.f * DeltaTime, 0.f, 0.f ));

		// -------- spin around center (offset trick) --------
		/*FVector CenterOffset = FVector(0.f, 0.f, 100.f); // adjust based on mesh

		WeaponMesh->AddLocalOffset(CenterOffset);
		WeaponMesh->AddLocalRotation(FRotator(2000.f * DeltaTime, 0.f, 0.f));
		WeaponMesh->AddLocalOffset(-CenterOffset);*///////////
		// ---------------------------------------------------

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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("TheWeapon::ThrowWeapon()"));

	WeaponState = EWeaponState::Flying;

	BP_StartTrailVFX();
	BP_PlayFlyingSound();

	// store start location
	ThrowStartLocation = GetActorLocation();

	// enable collision on capsule (this handles hits now)
	//CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//CollisionCapsule->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	//CollisionCapsule->IgnoreActorWhenMoving(GetOwner(), true);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	//ProjectileMovement->UpdatedComponent = CollisionCapsule;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bSweepCollision = true;
	//ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->Velocity = Direction.GetSafeNormal() * ProjectileMovement->InitialSpeed;
	//ProjectileMovement->Velocity = Direction * 1000.f;
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

	// get hand socket location instead of actor origin
	FVector End = OwnerCharacter->GetMesh()->GetSocketLocation(TEXT("Gada_Socket"));

	FVector Mid = (Start + End) * 0.5f;
	Mid.Z += 300.f;
	Mid.X += -300.f;

	ReturnSpline->AddSplinePoint(Start, ESplineCoordinateSpace::World);
	ReturnSpline->AddSplinePoint(Mid, ESplineCoordinateSpace::World);
	ReturnSpline->AddSplinePoint(End, ESplineCoordinateSpace::World);

	ReturnSpline->UpdateSpline();
}

/*void ATheWeapon::OnHit(UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (WeaponState != EWeaponState::Flying)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("TheWeapon::OnHit()"));

	if (OtherActor && OtherActor != GetOwner())
	{
		OtherActor->Destroy();
	}

	RecallWeapon();
}*/

