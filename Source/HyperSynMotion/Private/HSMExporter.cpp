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

	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());

	for (AActor* scene : scenes) {
		// Crear el objeto JSON para la escena
		TSharedPtr<FJsonObject> SceneObject = MakeShareable(new FJsonObject());

		// Nombre del actor (opcional, puedes cambiarlo por otro nombre relevante)
		/*SceneObject->SetStringField(TEXT("name"), scene->GetName());*/
		// Extraer el nombre del blueprint
		FString BlueprintPath;
		if (UBlueprintGeneratedClass* BlueprintClass = Cast<UBlueprintGeneratedClass>(scene->GetClass()))
		{
			if (UBlueprint* Blueprint = Cast<UBlueprint>(BlueprintClass->ClassGeneratedBy))
			{
				BlueprintPath = Blueprint->GetPathName();
			}
		}
		SceneObject->SetStringField(TEXT("name"), "//Script/Engine.Blueprint'" + BlueprintPath + "'" );


		// Posición
		UE_LOG(LogTemp, Warning, TEXT("Actor location: %s"), *scene->GetActorLocation().ToString());

		FVector ActorLocation = scene->GetActorLocation();
		TSharedPtr<FJsonObject> PositionObject = MakeShareable(new FJsonObject());
		PositionObject->SetNumberField(TEXT("x"), ActorLocation.X);
		PositionObject->SetNumberField(TEXT("y"), ActorLocation.Y);
		PositionObject->SetNumberField(TEXT("z"), ActorLocation.Z);
		SceneObject->SetObjectField(TEXT("position"), PositionObject);



		// Rotación
		UE_LOG(LogTemp, Warning, TEXT("Actor rotation: %s"), *scene->GetActorRotation().ToString());

		FRotator ActorRotation = scene->GetActorRotation();
		TSharedPtr<FJsonObject> RotationObject = MakeShareable(new FJsonObject());
		RotationObject->SetNumberField(TEXT("x"), ActorRotation.Pitch);
		RotationObject->SetNumberField(TEXT("y"), ActorRotation.Yaw);
		RotationObject->SetNumberField(TEXT("z"), ActorRotation.Roll);
		SceneObject->SetObjectField(TEXT("rotation"), RotationObject);


		// Escala
		UE_LOG(LogTemp, Warning, TEXT("Actor scale: %s"), *scene->GetActorScale3D().ToString());

		FVector ActorScale = scene->GetActorScale3D();
		TSharedPtr<FJsonObject> ScaleObject = MakeShareable(new FJsonObject());
		ScaleObject->SetNumberField(TEXT("x"), ActorScale.X);
		ScaleObject->SetNumberField(TEXT("y"), ActorScale.Y);
		ScaleObject->SetNumberField(TEXT("z"), ActorScale.Z);
		SceneObject->SetObjectField(TEXT("scale"), ScaleObject);

		// Función auxiliar para procesar propiedades de tipo FVector3f
		auto ProcessVectorProperty = [&](const FString& PropertyName, const FString& FieldName) {
			FProperty* Property = scene->GetClass()->FindPropertyByName(*PropertyName);
			if (Property) {
				void* PropertyAddress = Property->ContainerPtrToValuePtr<void>(scene);
				FVector3f PropertyValue = *reinterpret_cast<FVector3f*>(PropertyAddress);
				TSharedPtr<FJsonObject> PropertyObject = MakeShareable(new FJsonObject());
				PropertyObject->SetNumberField(TEXT("x"), PropertyValue.X);
				PropertyObject->SetNumberField(TEXT("y"), PropertyValue.Y);
				PropertyObject->SetNumberField(TEXT("z"), PropertyValue.Z);
				SceneObject->SetObjectField(*FieldName, PropertyObject);
			}
		};
		// CropMin
		FProperty* CropMin = scene->GetClass()->FindPropertyByName("CropMin");
		void* CropMinAddress = CropMin->ContainerPtrToValuePtr<void>(scene);
		FVector3f CropMinValue = *reinterpret_cast<FVector3f*>(CropMinAddress);
		FString CropMinString = FString::Printf(TEXT("(%f, %f, %f)"), CropMinValue.X, CropMinValue.Y, CropMinValue.Z);
		UE_LOG(LogTemp, Warning, TEXT("Actor cropmin: %s"),*CropMinString);

		ProcessVectorProperty(TEXT("CropMin"), TEXT("crop_bounding_box_min"));

		// CropMax
		FProperty* CropMax = scene->GetClass()->FindPropertyByName("CropMax");
		void* CropMaxAddress = CropMax->ContainerPtrToValuePtr<void>(scene);
		FVector3f CropMaxValue = *reinterpret_cast<FVector3f*>(CropMaxAddress);
		FString CropMaxString = FString::Printf(TEXT("(%f, %f, %f)"), CropMaxValue.X, CropMaxValue.Y, CropMaxValue.Z);
		UE_LOG(LogTemp, Warning, TEXT("Actor cropmax: %s"), *CropMaxString);

		ProcessVectorProperty(TEXT("CropMax"), TEXT("crop_bounding_box_max"));

		// CropCenter
		FProperty* CropCenter = scene->GetClass()->FindPropertyByName("CropCenter");
		void* CropCenterAddress = CropCenter->ContainerPtrToValuePtr<void>(scene);
		FVector3f CropCenterValue = *reinterpret_cast<FVector3f*>(CropCenterAddress);
		FString CropCenterString = FString::Printf(TEXT("(%f, %f, %f)"), CropCenterValue.X, CropCenterValue.Y, CropCenterValue.Z);
		UE_LOG(LogTemp, Warning, TEXT("Actor cropcenter: %s"), *CropCenterString);

		ProcessVectorProperty(TEXT("CropCenter"), TEXT("crop_bounding_center"));

		// CropRotation
		FProperty* CropRotation = scene->GetClass()->FindPropertyByName("CropRotation");
		void* CropRotationAddress = CropRotation->ContainerPtrToValuePtr<void>(scene);
		FVector3f CropRotationValue = *reinterpret_cast<FVector3f*>(CropRotationAddress);
		FString CropRotationString = FString::Printf(TEXT("(%f, %f, %f)"), CropRotationValue.X, CropRotationValue.Y, CropRotationValue.Z);
		UE_LOG(LogTemp, Warning, TEXT("Actor croprotation: %s"), *CropRotationString);

		ProcessVectorProperty(TEXT("CropRotation"), TEXT("crop_rotation"));

		// Agregar la escena al objeto raíz
		RootObject->SetObjectField(TEXT("scene"), SceneObject);
	}

	// Convertir el objeto JSON a una cadena and save
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
	FString json_file_path = Output_path + "/" + file_name + ".json";
	FFileHelper::SaveStringToFile(OutputString, *json_file_path, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_None);
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
