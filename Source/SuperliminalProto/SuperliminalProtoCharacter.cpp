// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperliminalProtoCharacter.h"
#include "SuperliminalProtoProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Widget/Crosshair.h"
#include "GameActor/Movable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// ASuperliminalProtoCharacter

ASuperliminalProtoCharacter::ASuperliminalProtoCharacter()
{	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void ASuperliminalProtoCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	//Add Input Mapping Context
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			Subsystem->AddMappingContext(SuperliminalMappingContext, 0);
			Crosshair = Cast<UCrosshair>(CreateWidget(GetWorld(), CrosshairWidget));
			if (Crosshair)
				Crosshair->AddToViewport();
		}
	}
}
void ASuperliminalProtoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(Crosshair))
		TraceValidTarget();

	if (Gate.IsOpen())
	{
		float ScaleModified;
		FVector Location = CustomBoxTrace(StartingBounds, StartingDistance, ScaleModified);
		HeldObject->SetWorldLocation(Location);
		HeldObject->SetWorldScale3D(ScaleModified * StartingScale);
	}
}
//////////////////////////////////////////////////////////////////////////// Input

void ASuperliminalProtoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASuperliminalProtoCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASuperliminalProtoCharacter::Look);

		// Interacted Object
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ASuperliminalProtoCharacter::Interact);

		// Rotate Object
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ASuperliminalProtoCharacter::Rotate);

		EnhancedInputComponent->BindAction(HeldShift, ETriggerEvent::Triggered, this, &ASuperliminalProtoCharacter::HeldShiftObject);

		EnhancedInputComponent->BindAction(HeldCtrl, ETriggerEvent::Triggered, this, &ASuperliminalProtoCharacter::HeldCtrlObject);
	}
}


void ASuperliminalProtoCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ASuperliminalProtoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASuperliminalProtoCharacter::TraceValidTarget()
{
	FHitResult OutHit;
	bool IsHit = LineTraceFromCamera(OutHit);
	if (IsHit)
	{
		AMovable* ActorMovable = Cast<AMovable>(OutHit.GetActor());
		if (ActorMovable)
		{
			if (!ActorMovable->GetLocked())
			{
				if (!Crosshair->IsHover)
				{
					Crosshair->Hover();
				}
			}
		}
		else
		{
			if (Crosshair->IsHover)
			{
				Crosshair->UnHover();
			}
		}
	}
}

bool ASuperliminalProtoCharacter::LineTraceFromCamera(FHitResult& OutHit)
{
	FVector CameraLoc = FirstPersonCameraComponent->GetComponentLocation();
	FVector CameraForwardDir = FirstPersonCameraComponent->GetForwardVector();

	FVector EndTrace = CameraLoc + CameraForwardDir * MaxRange;

	TArray<AActor*> ActorToIgnore;

	return UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		CameraLoc,
		EndTrace,
		TraceTypeQuery1,
		false,
		ActorToIgnore,
		EDrawDebugTrace::None,
		OutHit,
		true
	);
}

FVector ASuperliminalProtoCharacter::CustomBoxTrace(FVector StartBounds,float startingDistance, float& ScaleModifier)
{
	FVector CurrentBounds = StartBounds;
	FVector Location;
	float CurrenDistance;
	for (int32 i = 10; i < UKismetMathLibrary::FTrunc(MaxRange / 5); i++)
	{
		CurrenDistance = i * 5;
		CurrentBounds = StartBounds * CurrenDistance / startingDistance;
		Location = FirstPersonCameraComponent->GetComponentLocation() + FirstPersonCameraComponent->GetForwardVector() * CurrenDistance;
		TArray<AActor*> ActorToIgnore;
		FHitResult OutHit;
		bool IsHit = UKismetSystemLibrary::BoxTraceSingle(
			GetWorld(),
			Location,
			Location,
			CurrentBounds * 1.65,
			FRotator::ZeroRotator,
			TraceTypeQuery1,
			true,
			ActorToIgnore,
			EDrawDebugTrace::None,
			OutHit,
			true
		);
		if (IsHit)
		{
			ScaleModifier = CurrenDistance / startingDistance;
			return Location;
		}
	}
	
	ScaleModifier = CurrenDistance / startingDistance;
	return Location;
}

void ASuperliminalProtoCharacter::Interact()
{
	if (IsValid(HeldObject))
	{
		DropObject();
	}
	else
	{
		PickupObject();
	}
}

void ASuperliminalProtoCharacter::Rotate(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>() * 5.f;
	if (IsValid(HeldObject))
	{
		FRotator HeldRotate = HeldObject->GetComponentRotation();
		FRotator HeldRotateBaseRoll = UKismetMathLibrary::MakeRotator(HeldRotate.Roll + InputValue, HeldRotate.Pitch, HeldRotate.Yaw);
		FRotator HeldRotateBasePitch = UKismetMathLibrary::MakeRotator(HeldRotate.Roll, HeldRotate.Pitch + InputValue, HeldRotate.Yaw);
		FRotator HeldRotateBaseYaw = UKismetMathLibrary::MakeRotator(HeldRotate.Roll, HeldRotate.Pitch, HeldRotate.Yaw + InputValue);
		FRotator SelectedRotateYawPitch = UKismetMathLibrary::SelectRotator(HeldRotateBasePitch, HeldRotateBaseYaw, bHeldShift);
		FRotator SelectedRotate = UKismetMathLibrary::SelectRotator(HeldRotateBaseRoll, SelectedRotateYawPitch, bHeldCtrl);
		HeldObject->SetWorldRotation(SelectedRotate);
	}
}

void ASuperliminalProtoCharacter::PickupObject()
{
	FHitResult OutHit;
	bool IsHit = LineTraceFromCamera(OutHit);
	if (IsHit)
	{
		AMovable* movable = Cast<AMovable>(OutHit.GetActor());
		if (movable)
		{
			if (movable->SelfPickup(FirstPersonCameraComponent))
			{
				FVector origin;
				float radius;
				HeldObject = movable->StaticMeshCom;
				LerpLocation(HeldObject->GetRelativeLocation());
				UKismetSystemLibrary::GetComponentBounds(HeldObject, origin, StartingBounds, radius);
				StartingScale = HeldObject->GetComponentScale();
				StartingDistance = UKismetMathLibrary::Vector_Distance(FVector::Zero(), HeldObject->GetRelativeLocation());
				Gate.Open();
			}
		}
	}
}

void ASuperliminalProtoCharacter::DropObject()
{
	Cast<AMovable>(HeldObject->GetOwner())->SelfDrop();
	HeldObject = nullptr;
	Gate.Close();
}

void ASuperliminalProtoCharacter::HeldShiftObject()
{
	bHeldShift = !bHeldShift;
}

void ASuperliminalProtoCharacter::HeldCtrlObject()
{
	bHeldCtrl = !bHeldCtrl;
}

void ASuperliminalProtoCharacter::LerpLocation(FVector startLocation)
{
	LerpValue = 0;
	OnMove = true;
	StartLocation = startLocation;
	float Distance = UKismetMathLibrary::Vector_Distance(FVector::Zero(), StartLocation);
	EndLocation = UKismetMathLibrary::VLerp(StartLocation, Distance * FVector(1.f, 0.f, 0.f), 1);
	HeldObject->SetRelativeLocation(EndLocation);
}

void ASuperliminalProtoCharacter::Lerp(float DeltaTime)
{
	if (OnMove)
	{
		if (LerpValue <= 1)
		{
			LerpValue += 1 * DeltaTime;
			float Distance = UKismetMathLibrary::Vector_Distance(FVector::Zero(), StartLocation);
			EndLocation = UKismetMathLibrary::VLerp(StartLocation, Distance * FVector(1.f, 0.f, 0.f), 0);
			HeldObject->SetRelativeLocation(EndLocation, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
		}
		else
		{
			OnMove = false;
		}
	}
}
