// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TheWeapon.generated.h"

class UStaticMeshComponent;
class UCapsuleComponent;
class UProjectileMovementComponent;
class USplineComponent;

UENUM()
enum class EWeaponState :uint8
{
	InHand,
	Flying,
	Returning
};

UCLASS()
class LEGENDSOFVANARA_API ATheWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATheWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WeaponMesh;

	// new pivot component
	//UPROPERTY(VisibleAnywhere)
	//USceneComponent* SpinPivot;

	//UPROPERTY(VisibleAnywhere)
	//USceneComponent* RootScene;

	//UPROPERTY(VisibleAnywhere)
	//UCapsuleComponent* CollisionCapsule;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere)
	USplineComponent* ReturnSpline;

	EWeaponState WeaponState = EWeaponState::InHand;

	FVector ThrowStartLocation;

	UPROPERTY(EditAnywhere)
	float MaxThrowDistance = 4000.f;

	float RecallAlpha = 0.f;

	float RecallSpeed = 1.f;

	//functions
	void ThrowWeapon(const FVector& Direction);

	UFUNCTION(BlueprintCallable)
	void RecallWeapon();

	void CatchWeapon();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_StartTrailVFX();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlayFlyingSound();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_StopFlyingSound();
	
	void GenerateReturnSpline();

	

	/*void OnHit(UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);*/
};
