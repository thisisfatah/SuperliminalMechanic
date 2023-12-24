// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Gate/Gate.h"
#include "SuperliminalProtoCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ASuperliminalProtoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* SuperliminalMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	
public:
	ASuperliminalProtoCharacter();

protected:
	virtual void BeginPlay();

public:
		
	virtual void Tick(float DeltaTime) override;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* InteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* RotateAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* HeldShift;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* HeldCtrl;

protected:
	/** Widget*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
		TSubclassOf<class UCrosshair> CrosshairWidget;
	class UCrosshair* Crosshair;

	UStaticMeshComponent* HeldObject;

	/**Trace */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Trace)
		float MaxRange = 3000.f;

	bool OnMove;
	float LerpValue;
	float StartingDistance;
	FVector StartLocation;
	FVector EndLocation;
	FVector StartingBounds;
	FVector StartingScale;

	bool bHeldShift;
	bool bHeldCtrl;

	UPROPERTY(VisibleAnywhere)
		FGate Gate = FGate(true);

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void TraceValidTarget();

	bool LineTraceFromCamera(FHitResult& OutHit);
	FVector CustomBoxTrace(FVector StartBounds, float StartingDistance, float& ScaleModifier);

	void Interact();

	void Rotate(const FInputActionValue& Value);

	void PickupObject();
	void DropObject();

	void HeldShiftObject();
	void HeldCtrlObject();

	void LerpLocation(FVector StartLocation);

	void Lerp(float DeltaTime);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }


};

