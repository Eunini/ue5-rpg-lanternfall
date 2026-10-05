#include "LanternPlayer.h"
#include "LanternUsable.h"
#include "JourneySubsystem.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Misc/CommandLine.h"
#include "PortfolioCapture.h"
#include "Misc/Parse.h"

ALanternPlayer::ALanternPlayer()
{
 PrimaryActorTick.bCanEverTick=true;
 Camera=CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
 Camera->SetupAttachment(GetRootComponent());Camera->SetRelativeLocation(FVector(0,0,65));Camera->bUsePawnControlRotation=true;
 bUseControllerRotationYaw=true;GetCharacterMovement()->MaxWalkSpeed=360;
}
void ALanternPlayer::BeginJourney()
{
 GetGameInstance()->GetSubsystem<UJourneySubsystem>()->Status=FText::FromString(TEXT("LANTERNFALL / One circuit. Two futures."));
}
void ALanternPlayer::SetupPlayerInputComponent(UInputComponent* Input)
{
 Super::SetupPlayerInputComponent(Input);
 Input->BindAxis(TEXT("Forward"),this,&ALanternPlayer::Forward);Input->BindAxis(TEXT("Right"),this,&ALanternPlayer::Right);
 Input->BindAxis(TEXT("Yaw"),this,&ALanternPlayer::Yaw);Input->BindAxis(TEXT("Pitch"),this,&ALanternPlayer::Pitch);
 Input->BindAction(TEXT("Use"),IE_Pressed,this,&ALanternPlayer::Use);
 Input->BindAction(TEXT("Choice1"),IE_Pressed,this,&ALanternPlayer::Choice1);
 Input->BindAction(TEXT("Choice2"),IE_Pressed,this,&ALanternPlayer::Choice2);
 Input->BindAction(TEXT("Choice3"),IE_Pressed,this,&ALanternPlayer::Choice3);
 Input->BindAction(TEXT("Save"),IE_Pressed,this,&ALanternPlayer::Save);
 Input->BindAction(TEXT("Load"),IE_Pressed,this,&ALanternPlayer::Load);
 Input->BindAction(TEXT("Journal"),IE_Pressed,this,&ALanternPlayer::Journal);
 Input->BindAction(TEXT("Jump"),IE_Pressed,this,&ACharacter::Jump);
 Input->BindAction(TEXT("Jump"),IE_Released,this,&ACharacter::StopJumping);
}
void ALanternPlayer::Forward(float V)
{
 if (Controller && !bJournal && !GetGameInstance()->GetSubsystem<UJourneySubsystem>()->IsDialogueOpen())
  AddMovementInput(FRotationMatrix(FRotator(0,Controller->GetControlRotation().Yaw,0)).GetUnitAxis(EAxis::X),V);
}
void ALanternPlayer::Right(float V)
{
 if (Controller && !bJournal && !GetGameInstance()->GetSubsystem<UJourneySubsystem>()->IsDialogueOpen())
  AddMovementInput(FRotationMatrix(FRotator(0,Controller->GetControlRotation().Yaw,0)).GetUnitAxis(EAxis::Y),V);
}
void ALanternPlayer::Yaw(float V){if (!bJournal && !GetGameInstance()->GetSubsystem<UJourneySubsystem>()->IsDialogueOpen()) AddControllerYawInput(V);}
void ALanternPlayer::Pitch(float V){if (!bJournal && !GetGameInstance()->GetSubsystem<UJourneySubsystem>()->IsDialogueOpen()) AddControllerPitchInput(V);}
ALanternUsable* ALanternPlayer::Focused() const
{
 FHitResult Hit;FCollisionQueryParams Query;Query.AddIgnoredActor(this);
 const FVector Start=Camera->GetComponentLocation();
 if (GetWorld()->LineTraceSingleByChannel(Hit,Start,Start+Camera->GetForwardVector()*300,ECC_Visibility,Query))
  return Cast<ALanternUsable>(Hit.GetActor());
 return nullptr;
}
void ALanternPlayer::Use(){if (auto* Target=Focused()) Target->Interact(this);}
void ALanternPlayer::Choice1(){GetGameInstance()->GetSubsystem<UJourneySubsystem>()->ChooseDialogue(0);}
void ALanternPlayer::Choice2(){GetGameInstance()->GetSubsystem<UJourneySubsystem>()->ChooseDialogue(1);}
void ALanternPlayer::Choice3(){GetGameInstance()->GetSubsystem<UJourneySubsystem>()->ChooseDialogue(2);}
void ALanternPlayer::Save(){auto* J=GetGameInstance()->GetSubsystem<UJourneySubsystem>();if(J->SaveJourney()) J->Status=FText::FromString(TEXT("Journey saved."));}
void ALanternPlayer::Load(){GetGameInstance()->GetSubsystem<UJourneySubsystem>()->LoadJourney();}
void ALanternPlayer::Journal(){bJournal=!bJournal;}

void ALanternPlayer::Tick(float Delta)
{
 Super::Tick(Delta);
 if (IsLocallyControlled() && FParse::Param(FCommandLine::Get(),TEXT("PortfolioDemo")))
 {
  const auto* Capture=GetWorld()->GetSubsystem<ULanternCaptureSubsystem>();
  if (!FParse::Param(FCommandLine::Get(),TEXT("PortfolioCapture")) || (Capture && Capture->IsReady())) AdvanceDemo(Delta);
 }
}
void ALanternPlayer::AdvanceDemo(float Delta)
{
 DemoAge+=Delta;StepAge+=Delta;
 if (!Controller || DemoAge<2.f) return;
 auto* J=GetGameInstance()->GetSubsystem<UJourneySubsystem>();
 const bool Relay=FParse::Param(FCommandLine::Get(),TEXT("RelayRoute"));
 auto Next=[this](){++DemoStep;StepAge=0.f;};
 auto Find=[this](ELanternAction Action,FName Id=NAME_None)->ALanternUsable*
 {
  for (TActorIterator<ALanternUsable> It(GetWorld());It;++It)
   if (It->Action==Action && (Id.IsNone() || It->PickupId==Id)) return *It;
  return nullptr;
 };
 auto Approach=[this,Delta](ALanternUsable* Target)->bool
 {
  if (!Target) return false;
  const FVector Offset=Target->GetActorLocation()-Camera->GetComponentLocation();
  Controller->SetControlRotation(FMath::RInterpTo(Controller->GetControlRotation(),Offset.Rotation(),Delta,3.f));
  const FVector Direction=(Target->GetActorLocation()-GetActorLocation()).GetSafeNormal2D();
  if (FVector::DistSquared2D(Target->GetActorLocation(),GetActorLocation())>FMath::Square(180.f))
  {AddMovementInput(Direction);return false;}
  return true;
 };
 auto WalkTo=[this,Delta](FVector Goal)->bool
 {
  FVector Offset=Goal-GetActorLocation();Offset.Z=0;
  Controller->SetControlRotation(FMath::RInterpTo(Controller->GetControlRotation(),Offset.Rotation(),Delta,3.f));
  if (Offset.Size2D()<65.f) return true;
  AddMovementInput(Offset.GetSafeNormal2D());return false;
 };
 switch (DemoStep)
 {
  case 0:if (auto* Target=Find(ELanternAction::Nera);Approach(Target)){Target->Interact(this);Next();}break;
  case 1:if (StepAge>5.f){J->ChooseDialogue(Relay?1:0);Next();}break;
  case 2:if (auto* Target=Find(ELanternAction::Cell,TEXT("Cell_A"));Approach(Target)){Target->Interact(this);Next();}break;
  case 3:if (StepAge>1.5f)Next();break;
  case 4:if (WalkTo(FVector(-50,450,0)))Next();break;
  case 5:if (WalkTo(FVector(250,450,0)))Next();break;
  case 6:if (auto* Target=Find(ELanternAction::Cell,TEXT("Cell_B"));Approach(Target)){Target->Interact(this);bJournal=true;Next();}break;
  case 7:if (StepAge>3.f){bJournal=false;Save();Next();}break;
  case 8:AddMovementInput(FVector(0,1,0));if (StepAge>1.5f)Next();break;
  case 9:bDemoRestored=J->LoadJourney() && J->GetCells()==2;Next();break;
  case 10:if (StepAge>2.f)Next();break;
  case 11:if (auto* Target=Find(Relay?ELanternAction::Relay:ELanternAction::Clinic);Approach(Target)){Target->Interact(this);Next();}break;
  case 12:if (StepAge>3.f)Next();break;
  case 13:if (auto* Target=Find(ELanternAction::Nera);Approach(Target)){Target->Interact(this);Next();}break;
  case 14:if (StepAge>6.f){J->ChooseDialogue(0);Next();}break;
  case 15:if (StepAge>3.f){bJournal=true;Save();Next();}break;
  default:break;
 }
}
