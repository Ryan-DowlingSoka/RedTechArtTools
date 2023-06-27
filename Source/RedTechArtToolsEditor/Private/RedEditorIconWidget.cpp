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

#include "RedEditorIconWidget.h"

#include "Brushes/SlateImageBrush.h"
#include "Components/Image.h"
#include "HAL/FileManager.h"
#include "Launch/Resources/Version.h"

void URedEditorIconWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 1
	const FSlateBrush& BrushRef = GetBrush();
#else
	const FSlateBrush& BrushRef = Brush;
#endif

	if (!IconPath.Path.IsEmpty() && IFileManager::Get().FileExists(*FPaths::ConvertRelativePathToFull(*IconPath.Path)))
	{
		if (const FString Ext = FPaths::GetExtension(IconPath.Path); Ext == "svg")
		{
			IconBrush.Reset(new FSlateVectorImageBrush(IconPath.Path, IconSize, BrushRef.TintColor, BrushRef.Tiling));
		}
		else if (Ext == "png")
		{
			IconBrush.Reset(new FSlateDynamicImageBrush(FName(*IconPath.Path), IconSize,
			                                            BrushRef.TintColor.GetSpecifiedColor(), BrushRef.Tiling));
		}
		if (IconBrush != nullptr)
		{
			SetBrush(*IconBrush.Get());
		}
	}
}
