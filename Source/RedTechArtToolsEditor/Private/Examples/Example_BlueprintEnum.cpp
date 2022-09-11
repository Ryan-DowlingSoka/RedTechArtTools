// MIT License
//
// Copyright (c) 2022 Ryan DowlingSoka
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Example_BlueprintEnum.h"

DEFINE_LOG_CATEGORY(LogBlueprintEnumExample);

#define LOCTEXT_NAMESPACE "BlueprintExample"

// Sets default values
AExample_BlueprintEnum::AExample_BlueprintEnum()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AExample_BlueprintEnum::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AExample_BlueprintEnum::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch(BPEnum.GetIndex())
	{
	case 0:
		UE_LOG(LogBlueprintEnumExample, Warning, TEXT("%s"), *BPEnum.GetName().ToString());
		break;
	case 1:
		UE_LOG(LogBlueprintEnumExample, Warning, TEXT("%d"), BPEnum.GetIndex());
		break;
	case 2:
		UE_LOG(LogBlueprintEnumExample, Warning, TEXT("%d"), BPEnum.GetValue());
		break;
	default:
		break;
	}
}

#undef LOCTEXT_NAMESPACE