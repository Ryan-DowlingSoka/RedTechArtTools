# import unreal
# from typing import cast
# import os

# RED_TECH_ART_TOOLS_CONTENT_PATH = unreal.Name("/RedTechArtTools")
# RED_TECH_ART_TOOLS_EDITOR_UTILITY_WIDGET_PATH = "/RedTechArtTools/EditorWidgets"


# def on_init():
#     register_widgets(RED_TECH_ART_TOOLS_EDITOR_UTILITY_WIDGET_PATH)


# def register_widgets(path:str):
#     eus: unreal.EditorUtilitySubsystem = unreal.get_editor_subsystem(unreal.EditorUtilitySubsystem)

#     asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
#     paths_to_scan = cast(unreal.Array, [str(path)])
#     asset_registry.scan_paths_synchronous(paths_to_scan, force_rescan=True)
#     assets = asset_registry.get_assets_by_path(path, recursive=True)
#     if assets is None:
#         return

#     for asset in assets:
#         obj = asset.get_asset()
#         if isinstance(obj, unreal.EditorUtilityWidgetBlueprint):
#             print("widget " + str(obj))
#             blueprint_class = obj.generated_class()
#             if blueprint_class is not None:
#                 cdo = blueprint_class.get_default_object()
#                 if cdo is not None:
#                     try:
#                         if cdo.call_method("get_auto_register"):
#                             eus.register_tab_and_get_id(obj)
#                             print("registered " + str(obj))
#                     except:
#                         pass
#         elif isinstance(obj, unreal.Blueprint):
#             blueprint_class = obj.generated_class()
#             print("bp " + str(blueprint_class))
#             if isinstance(blueprint_class, unreal.EditorUtilityToolMenuEntry):
#                 cdo = blueprint_class.get_default_object()
#                 print("cdo " + str(cdo))
#                 if cdo:
#                     tool_menus = unreal.ToolMenus.get()
#                     tool_menus.add_menu_entry_object(cdo)
