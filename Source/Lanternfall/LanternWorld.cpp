#include "LanternWorld.h"
#include "LanternUsable.h"
#include "PortfolioVisual.h"
#include "Engine/World.h"
ALanternWorld::ALanternWorld(){SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));}
void ALanternWorld::OnConstruction(const FTransform& Transform)
{
 Super::OnConstruction(Transform);
 for (AActor* Actor:Actors) if (IsValid(Actor)) Actor->Destroy();
 Actors.Empty();PortfolioVisual::Clear(this);
 using namespace PortfolioVisual;
 Environment(this);
 const FLinearColor Slate(.09f,.16f,.2f), Stone(.34f,.37f,.36f), Amber(.8f,.39f,.1f);
 Part(this,FVector(500,0,-50),FVector(7000,6000,100),Slate);
 Part(this,FVector(450,0,3),FVector(2400,500,8),Stone);
 for (int32 Side:{-1,1})
 {
  Part(this,FVector(1600,Side*950,260),FVector(650,1100,520),Stone);
  Part(this,FVector(1600,Side*950,550),FVector(740,1200,60),Slate);
  Part(this,FVector(1260,Side*950,275),FVector(30,500,290),Slate);
  Part(this,FVector(1250,Side*950,450),FVector(35,1000,22),Side>0?Amber:FLinearColor(.1f,.48f,.7f),false);
  Label(this,Side>0?TEXT("01 / FIELD CLINIC"):TEXT("02 / SIGNAL RELAY"),FVector(1210,Side*950,490),48);
 }
 Part(this,FVector(2500,-950,1050),FVector(90,90,2100),Stone);
 for (int32 z=300;z<1900;z+=350) Part(this,FVector(2500,-950,z),FVector(360,360,35),Slate);
 for (int32 i=0;i<9;++i)
  Part(this,FVector(-1500+i*700,2200,250),FVector(350,180,500),Stone);
 for (int32 i=0;i<7;++i)
  Part(this,FVector(i*600-1500,-2200,120),FVector(420,420,240),Slate);
 for (int32 i=0;i<5;++i)
 {
  Part(this,FVector(-1500, -1200+i*650,550),FVector(50,50,1100),Stone);
  Part(this,FVector(-1500,-1200+i*650,1000),FVector(120,120,70),Amber,false);
 }
 auto Spawn=[this](ELanternAction Action,FName Id,FVector Location,const TCHAR* Blueprint)
 {
  UClass* Class=LoadClass<ALanternUsable>(nullptr,Blueprint);if (!Class) Class=ALanternUsable::StaticClass();
  FActorSpawnParameters Params;Params.Owner=this;Params.bAllowDuringConstructionScript=true;
  ALanternUsable* Actor=GetWorld()->SpawnActor<ALanternUsable>(Class,Location,FRotator::ZeroRotator,Params);
  if (Actor){Actor->Configure(Action,Id);Actors.Add(Actor);}
 };
 Spawn(ELanternAction::Nera,NAME_None,FVector(100,0,90),TEXT("/Game/Lanternfall/Blueprints/BP_Nera.BP_Nera_C"));
 Spawn(ELanternAction::Clinic,NAME_None,FVector(1100,800,90),TEXT("/Game/Lanternfall/Blueprints/BP_Terminal.BP_Terminal_C"));
 Spawn(ELanternAction::Relay,NAME_None,FVector(1100,-800,90),TEXT("/Game/Lanternfall/Blueprints/BP_Terminal.BP_Terminal_C"));
 Spawn(ELanternAction::Cell,TEXT("Cell_A"),FVector(-250,350,65),TEXT("/Game/Lanternfall/Blueprints/BP_EnergyCell.BP_EnergyCell_C"));
 Spawn(ELanternAction::Cell,TEXT("Cell_B"),FVector(600,-300,65),TEXT("/Game/Lanternfall/Blueprints/BP_EnergyCell.BP_EnergyCell_C"));
 Spawn(ELanternAction::Cell,TEXT("Cell_C"),FVector(1100,500,65),TEXT("/Game/Lanternfall/Blueprints/BP_EnergyCell.BP_EnergyCell_C"));
 Label(this,TEXT("LANTERNFALL / RESERVE COURTYARD"),FVector(-1200,0,1050),85);
}


void ALanternWorld::BeginPlay()
{
 Super::BeginPlay();OnConstruction(GetActorTransform());
}
