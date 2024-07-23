// Fill out your copyright notice in the Description page of Project Settings.


#include "HSMExporter.h"
#include "EngineUtils.h"

// Sets default values
AHSMExporter::AHSMExporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	file_name = "example";
	Output_path = FPaths::ProjectUserDir();
}

// Called when the game starts or when spawned
void AHSMExporter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BEGIN PLAY"));
	//get current world
	UWorld* world = GetWorld();

	if (world)
	{
		UE_LOG(LogTemp, Warning, TEXT("HAY WORLD"));
		//scene
		if (bScene)
		{
			GenerateSceneJson(world, GetActorByType<AActor>(world));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Scene not choose to be generated"));
		}

		//cameras and metahumans
		if (bCameras && bMetahumans) {
			GenerateCamerasAndMetaHumanJson(world);
		}
		else {
			if (bCameras) { // only cameras
				GenerateCamerasJson(world, GetActorByType<ACameraActor>(world));
			}
			else { // only metahumans
				GenerateMetaHumanJson(world, GetActorByType<ACharacter>(world));
			}
		}
	}
}

// Called every frame
void AHSMExporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHSMExporter::GenerateSceneJson(const UWorld* world, TArray<AActor*> scenes)
{
	UE_LOG(LogTemp, Warning, TEXT("generate scene json"));
	for (AActor* scene : scenes) {
		UE_LOG(LogTemp, Warning, TEXT("Found Actor: %s and parent: %s"), *scene->GetName(), *scene->GetClass()->GetName());
	}
}

void AHSMExporter::GenerateCamerasJson(const UWorld* world, TArray<ACameraActor*> scenes)
{
	UE_LOG(LogTemp, Warning, TEXT("generate json cameras"));
}

void AHSMExporter::GenerateMetaHumanJson(const UWorld* world, TArray<ACharacter*> scenes)
{
	UE_LOG(LogTemp, Warning, TEXT("generate json metahumans"));
}

void AHSMExporter::GenerateCamerasAndMetaHumanJson(const UWorld* world)
{
	UE_LOG(LogTemp, Warning, TEXT("generate json cameas and metahumans"));
}
