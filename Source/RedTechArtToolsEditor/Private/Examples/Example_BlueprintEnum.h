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

#pragma once

#include "CoreMinimal.h"
#include "RedBPEnum.h"
#include "GameFramework/Actor.h"
#include "Example_BlueprintEnum.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBlueprintEnumExample, Display, All);

UCLASS()
class REDTECHARTTOOLSEDITOR_API AExample_BlueprintEnum : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AExample_BlueprintEnum();

	//Set Blueprint Enum with a defined class in the constructor.
	//Note the path can be used with the class specifier (class specifiers are stripped.)
	UPROPERTY(EditAnywhere)
	FRedBPEnum BPEnum = FRedBPEnum("UserDefinedEnum'/RedTechArtTools/Examples/E_ExampleEnumeration.E_ExampleEnumeration'");

	//Mutable BP Enum, where the user in the editor can change the blueprint enum class of the property.
	//Note that the path is used without the class specifier.
	UPROPERTY(EditAnywhere, meta=(UserCanSetEnum), AdvancedDisplay)
	FRedBPEnum BPEnumMutable = FRedBPEnum("/RedTechArtTools/Examples/E_ExampleEnumeration.E_ExampleEnumeration");

	//Mutable BP Enum, where the user in the editor can change the blueprint enum class of the property.
	UPROPERTY(EditAnywhere, meta=(UserCanSetEnum), AdvancedDisplay)
	FRedBPEnum BPEnumEmpty;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
