# Copyright (C) Electronic Arts
# Author: Ryan DowlingSoka
# Date: 2022/12/05

import unreal
import os
from typing import cast, Optional, TypeVar, List, Tuple

PROJECT_PATH_ROOT = "/Game/"
RAW_PATH_ROOT = f"{unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())}Raw/"

SUPPORTED_CLASSES = (
    unreal.Texture2D,
    unreal.StaticMesh,
    unreal.SkeletalMesh,
)

extensions = {
    unreal.Name("StaticMesh"): [
        ".fbx",
        ".obj",
    ],
    unreal.Name("Texture2D"): [
        ".tga",
        ".png",
        ".psd",
        ".jpg",
        ".hdr",
        ".r16",
        ".exr",
    ],
}


def find_naive_raw_path(asset: unreal.AssetData, raw_path: str = RAW_PATH_ROOT) -> Optional[str]:
    asset_import_data: Optional[unreal.AssetImportData] = None
    current_import_path: Optional[str] = None
    current_import_basename: Optional[str] = None
    package_path: str = f"{asset.package_name}"
    new_path = package_path.replace(PROJECT_PATH_ROOT, RAW_PATH_ROOT, 1)
    unreal.log(f"\t Package path: {package_path}")
    unreal.log(f"\t Expected path: {new_path}")

    # Early out before asset load for invalid types.
    if asset.get_class() not in (c.static_class() for c in SUPPORTED_CLASSES):
        unreal.log(f"\t Invalid class: {asset.get_class()}")
        return None

    # Use current imported object as basename first.
    u_object = cast(unreal.Object, asset.get_asset())
    if any(isinstance(u_object, c) for c in SUPPORTED_CLASSES):
        asset_import_data = cast(unreal.AssetImportData, u_object.get_editor_property("asset_import_data"))

    if asset_import_data is not None:
        filenames = cast(List[str], asset_import_data.extract_filenames())
        if filenames:
            current_import_path = filenames[0]

    if current_import_path is not None:
        unreal.log(f"\t Current path: {current_import_path}")
        current_import_basename = os.path.basename(current_import_path)
        unreal.log(f"\t Current basename: {current_import_basename}")
        imported_path = os.path.dirname(new_path) + "/" + current_import_basename
        if os.path.isfile(imported_path):
            return imported_path

    # Look for matching extensions to the basename next.
    for ext in extensions[asset.asset_class]:
        ext_path = new_path + ext
        if os.path.isfile(ext_path):
            return ext_path

    return None


def update_asset_import_data(asset: unreal.AssetData, new_import_path: str) -> Tuple[bool, str]:
    u_object = cast(unreal.Object, asset.get_asset())
    if any(isinstance(u_object, c) for c in SUPPORTED_CLASSES):
        asset_import_data = cast(unreal.AssetImportData, u_object.get_editor_property("asset_import_data"))
        if asset_import_data is not None:
            if os.path.isfile(new_import_path):
                if asset_import_data.get_first_filename() == new_import_path:
                    return (False, "already matches")
                asset_import_data.scripted_add_filename(new_import_path, 0, "")
                return (True, f"path updated")
            return (False, f"path doesn't exist")
    return (False, f"asset is not texture, static mesh, or skeletal mesh")


def update_import_paths(assets: List[unreal.AssetData], raw_path: str = RAW_PATH_ROOT, force_save: bool = True):
    total_assets = len(assets)
    updated_assets = 0
    already_matching_assets = 0
    failed_assets = 0
    counter = 0

    assets_to_save: List[unreal.Object] = []

    with unreal.ScopedSlowTask(total_assets, "Updating Source Asset Locations") as slow_task:
        slow_task = cast(unreal.ScopedSlowTask, slow_task)
        slow_task.make_dialog(True)
        for asset in assets:
            if slow_task.should_cancel():
                break
            slow_task.enter_progress_frame(1, str(asset.asset_name))

            unreal.log(f"{counter:04d} / {total_assets:04d} :: {asset.package_path} ")
            new_path = find_naive_raw_path(asset, raw_path)
            unreal.log(f"\t {new_path}")
            if new_path is not None:
                result, message = update_asset_import_data(asset, new_path)
                if result:
                    updated_assets += 1
                    if force_save:
                        assets_to_save.append(cast(unreal.Object, asset.get_asset()))
                else:
                    if message == "already matches":
                        already_matching_assets += 1
                    else:
                        failed_assets += 1
                unreal.log(f"\t{message} : ({new_path})")
            else:
                failed_assets += 1

    with unreal.ScopedSlowTask(2, f"Saving {updated_assets} assets.") as slow_task:
        slow_task = cast(unreal.ScopedSlowTask, slow_task)
        unreal.log(f"Saving {updated_assets:04d} assets.")
        slow_task.enter_progress_frame(1)
        unreal.EditorAssetLibrary.save_loaded_assets(assets_to_save)

    unreal.log(f"Asset raw paths updated:               | {updated_assets:04d} / {total_assets:04d}")
    unreal.log(f"Asset raw paths already up to date:    | {already_matching_assets:04d} / {total_assets:04d}")
    unreal.log(f"Asset raw paths failed to update:      | {failed_assets:04d} / {total_assets:04d}")


def update_selected_asset_import_paths(raw_path: str = RAW_PATH_ROOT, force_save: bool = True):
    assets = cast(List[unreal.AssetData], unreal.EditorUtilityLibrary.get_selected_asset_data())
    update_import_paths(assets, raw_path, force_save)
