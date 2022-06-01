# Copyright (c) 2022 Ryan DowlingSoka - MIT License - See LICENSE file for more.
# WIP : Not ready for use.

import shutil
import unreal

class ContentOrganizerUtilities(object):

    @staticmethod
    def convert_content_path_to_disk_path(content_path:str) -> str:
        success, path = unreal.RedTechArtToolsBlueprintLibrary.convert_package_path_to_local_path(content_path)
        if success:
            return path
        else:
            return ""

    @staticmethod
    def move_directories_outside_unreal(source_path: str, target_path: str) -> bool:
        local_source_path = ContentOrganizerUtilities.convert_content_path_to_disk_path(source_path)
        local_target_path = ContentOrganizerUtilities.convert_content_path_to_disk_path(target_path)

        if local_source_path and local_target_path:
            shutil.copytree(local_source_path, local_target_path, dirs_exist_ok=True)


    @staticmethod
    def write_redirects_to_ini(redirects:str) -> bool:
        default_engine_ini = f"{unreal.Paths.project_config_dir()}/DefaultEngine.ini"
        return True