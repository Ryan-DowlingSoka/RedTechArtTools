import unreal
from typing import List

eus: unreal.EditorUtilitySubsystem = unreal.get_editor_subsystem(unreal.EditorUtilitySubsystem)
eal: unreal.EditorAssetLibrary = unreal.EditorAssetLibrary
sel: List[unreal.Object] = unreal.EditorUtilityLibrary.get_selected_assets()

for obj in sel:
    a = unreal.BlueprintEditorLibrary.get_blueprint_asset(obj)
    b = unreal.BlueprintEditorLibrary.generated_class(a)
    unreal.log([a,b])

def get_assets_by_path_of_class( path, class_name):
    arh = unreal.AssetRegistryHelpers
    ar:unreal.AssetRegistry = arh.get_asset_registry()
    assets:List[unreal.AssetData] = ar.get_assets_by_path(path, recursive=True)
    assets = [x for x in assets if x.asset_class == class_name]
    return assets

anim_sequences = get_assets_by_path_of_class("/Game/Path/To/A", "AnimSequence")

def create_anim_montage(name, folder, anim_sequence):
    at: unreal.AssetTools = unreal.AssetToolsHelpers.get_asset_tools()
    asset_factory:unreal.AnimMontageFactory = unreal.AnimMontageFactory(None)
    asset_factory.source_animation = your_anim_sequence
    at.create_asset(name, folder, unreal.AnimMontage, asset_factory)

def AddUserData(obj: unreal.SoundCue):
    user_data: List[unreal.AssetUserData] = obj.get_editor_property("asset_user_data")
    new_user_data = unreal.YOUR_CLASS_HERE(outer=obj)
    # Set any settings on your user data here.
    user_data.append(new_user_data)
    obj.set_editor_property("asset_user_data", user_data)