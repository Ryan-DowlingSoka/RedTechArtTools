import unreal
from typing import List, cast

eul = unreal.EditorUtilityLibrary
assets = cast(List[unreal.Object], eul.get_selected_assets())
for asset in assets:
    if isinstance(asset, unreal.StaticMesh):
        platforms = {"Windows": 0, "Switch": 1}
        asset.set_minimum_lod_for_platforms(platforms)
