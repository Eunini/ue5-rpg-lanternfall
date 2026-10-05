#include "LanternUsable.h"
#include "JourneySubsystem.h"
#include "PortfolioVisual.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

ALanternUsable::ALanternUsable()
{
 Body=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));SetRootComponent(Body);
 Body->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));
 Body->SetCollisionProfileName(TEXT("BlockAll"));
 PrimaryActorTick.bCanEverTick=true;PrimaryActorTick.TickInterval=.2f;
}
void ALanternUsable::Configure(ELanternAction Value,FName Id){Action=Value;PickupId=Id;OnConstruction(GetActorTransform());}
void ALanternUsable::OnConstruction(const FTransform& Transform)
{
 Super::OnConstruction(Transform);
 PortfolioVisual::Clear(this);
 PowerLight=nullptr;PowerIndicator=nullptr;
 const bool Cell=Action==ELanternAction::Cell;
 const bool Nera=Action==ELanternAction::Nera;
 Body->SetRelativeScale3D(Cell?FVector(.4f,.4f,.3f):(Nera?FVector(.55f,.65f,.95f):FVector(.65f,.65f,1.8f)));
 const FLinearColor Blue(.06f,.5f,.7f),Amber(.9f,.48f,.13f),Dark(.035f,.07f,.09f),Skin(.64f,.35f,.21f);
 PortfolioVisual::Color(Body,Nera?Dark:(Action==ELanternAction::Relay?Blue:Amber));
 const FVector Scale=Body->GetRelativeScale3D();
 auto Detail=[this,Scale](FVector Location,FVector Size,FLinearColor Tint,const TCHAR* Shape=TEXT("Cube"))
 {return PortfolioVisual::Part(this,Location/Scale,Size/Scale,Tint,false,Shape);};
 if (Nera)
 {
  Detail(FVector(0,0,70),FVector(42),Skin,TEXT("Sphere"));
  Detail(FVector(0,0,85),FVector(46,46,22),Dark,TEXT("Sphere"));
  for (int32 Side:{-1,1})
  {
   Detail(FVector(0,Side*17,-70),FVector(20,24,50),Dark);
   Detail(FVector(-3,Side*42,-4),FVector(21,20,80),Skin);
   Detail(FVector(-22,Side*9,73),FVector(3,5,5),Dark);
  }
  Detail(FVector(-28,0,5),FVector(4,59,11),Amber);
  Detail(FVector(-30,-15,28),FVector(5,17,13),Blue);
 }
 else if (Cell)
 {
  Detail(FVector(0,0,20),FVector(47,47,10),Dark);
  Detail(FVector(0,0,-20),FVector(47,47,10),Dark);
  Detail(FVector(-21,0,0),FVector(4,22,20),Blue);
 }
 else
 {
  Detail(FVector(-34,0,30),FVector(5,50,38),Dark);
  auto* Screen=Detail(FVector(-38,0,30),FVector(3,34,14),Action==ELanternAction::Relay?Blue:Amber);
  PowerIndicator=Cast<UMaterialInstanceDynamic>(Screen->GetMaterial(0));
  PowerLight=NewObject<UPointLightComponent>(this);
  AddInstanceComponent(PowerLight);PowerLight->SetupAttachment(Body);
  PowerLight->SetRelativeLocation(FVector(-70,0,60)/Scale);
  PowerLight->SetLightColor(Action==ELanternAction::Relay?Blue:Amber);
  PowerLight->SetIntensity(0.f);PowerLight->SetAttenuationRadius(600.f);
  PowerLight->SetCastShadows(false);PowerLight->RegisterComponent();
  Detail(FVector(-36,0,-20),FVector(5,44,10),Dark);
 }
 PortfolioVisual::Label(this,GetPrompt().ToString(),FVector(0,0,Cell?65:(Nera?110:130))/Scale,Cell?23:30);
}
FText ALanternUsable::GetPrompt() const
{
 switch (Action)
 {
  case ELanternAction::Cell:return FText::FromString(TEXT("ENERGY CELL / E"));
  case ELanternAction::Clinic:return FText::FromString(TEXT("CLINIC CIRCUIT / E"));
  case ELanternAction::Relay:return FText::FromString(TEXT("RELAY CIRCUIT / E"));
  default:return FText::FromString(TEXT("NERA / E TO TALK"));
 }
}
void ALanternUsable::Interact_Implementation(APawn* Interactor){PerformAction(Interactor);}
void ALanternUsable::PerformAction(APawn* Interactor)
{
 if (!Interactor || !Interactor->IsLocallyControlled() || FVector::DistSquared(GetActorLocation(),Interactor->GetActorLocation())>FMath::Square(350.f)) return;
 auto* Journey=GetGameInstance()->GetSubsystem<UJourneySubsystem>();
 if (Action==ELanternAction::Nera) Journey->OpenDialogue();
 else if (Action==ELanternAction::Cell) Journey->CollectCell(PickupId);
 else Journey->DeliverPower(Action==ELanternAction::Clinic?1:2);
}
void ALanternUsable::Tick(float Delta)
{
 Super::Tick(Delta);
 if (const auto* Journey=GetGameInstance()->GetSubsystem<UJourneySubsystem>())
 {
  if (Action==ELanternAction::Cell)
  {
   const bool Claimed=Journey->IsClaimed(PickupId);
   SetActorHiddenInGame(Claimed);SetActorEnableCollision(!Claimed);
  }
  else if (Action==ELanternAction::Clinic || Action==ELanternAction::Relay)
  {
   const int32 Destination=Action==ELanternAction::Clinic?1:2;
   const bool Active=Journey->IsPowered() && Journey->GetRoute()==Destination;
   if (PowerLight) PowerLight->SetIntensity(Active?1700.f:0.f);
   if (PowerIndicator) PowerIndicator->SetVectorParameterValue(TEXT("Tint"),
      Active?FLinearColor(.12f,.8f,.38f):FLinearColor(.04f,.12f,.16f));
  }
 }
}


void ALanternUsable::BeginPlay(){Super::BeginPlay();OnConstruction(GetActorTransform());}
