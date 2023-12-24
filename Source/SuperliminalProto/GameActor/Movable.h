// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Movable.generated.h"

UCLASS()
class SUPERLIMINALPROTO_API AMovable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* StaticMeshCom;

protected:
	bool Locked;
	USceneComponent* SceneRoot;

	bool IsHeld;

public:
	bool SelfPickup(USceneComponent* ParentComp);
	void SelfDrop();
	bool GetLocked();
	void SetLocked(bool isLocked);
};
