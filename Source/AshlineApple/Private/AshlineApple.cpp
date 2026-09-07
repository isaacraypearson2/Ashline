#include "AshlineApple.h"

IMPLEMENT_MODULE(FAshlineAppleModule, AshlineApple);

DEFINE_LOG_CATEGORY(LogAshlineApple);

void FAshlineAppleModule::StartupModule()
{
	UE_LOG(LogAshlineApple, Log, TEXT("AshlineApple started (MetalFX / GameController hooks)."));
}

void FAshlineAppleModule::ShutdownModule()
{
}
