// Fill out your copyright notice in the Description page of Project Settings.


#include "Movable.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AMovable::AMovable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Root"));
	SetRootComponent(SceneRoot);

	StaticMeshCom = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMeshCom->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void AMovable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMovable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AMovable::SelfPickup(USceneComponent* ParentComp)
{
	if (GetLocked())
	{
		return false;
	}
	else
	{
		StaticMeshCom->SetSimulatePhysics(false); 
		StaticMeshCom->AttachToComponent(ParentComp, FAttachmentTransformRules::KeepWorldTransform);
		StaticMeshCom->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		IsHeld = true;

		return true;
	}
}

void AMovable::SelfDrop()
{
	StaticMeshCom->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	StaticMeshCom->SetSimulatePhysics(true);
	StaticMeshCom->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	IsHeld = false;
}

bool AMovable::GetLocked()
{
	return Locked;
}

void AMovable::SetLocked(bool isLocked)
{
	Locked = isLocked;
}

