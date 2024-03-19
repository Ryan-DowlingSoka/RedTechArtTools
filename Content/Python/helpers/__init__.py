# Copyright (c) 2022 Ryan DowlingSoka - MIT License

"""Helper module for misc utilities."""

from typing import TypeVar, Optional, cast
import unreal

T = TypeVar("T", bound=unreal.Object)


def unreal_cast(unreal_type: type[T], object: unreal.Object) -> Optional[T]:
    """An unreal static_cast style cast. Returns None if the unreal.Object is not an instance of the given class."""
    if isinstance(object, unreal_type):
        return object
    else:
        return None
