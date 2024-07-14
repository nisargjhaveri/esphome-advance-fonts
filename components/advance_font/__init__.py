import os

import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.core import CORE, HexInt
from esphome.const import (
    __version__,
    CONF_FILE,
    CONF_ID,
    CONF_RAW_DATA_ID,
    CONF_SIZE,
)
from esphome.helpers import copy_file_if_changed

DOMAIN = "font"
DEPENDENCIES = ["display"]
MULTI_CONF = True

harfbuzz_ns = cg.esphome_ns.namespace("advance_font")

Font = harfbuzz_ns.class_("Font")

CONFIG_SCHEMA = cv.All({
    cv.Required(CONF_ID): cv.declare_id(Font),
    cv.Required(CONF_FILE): cv.file_,
    cv.Optional(CONF_SIZE, default=20): cv.int_range(min=1),
    cv.GenerateID(CONF_RAW_DATA_ID): cv.declare_id(cg.uint8),
});

async def to_code(config):
    # Copy supporting files for harfbuzz build
    copy_files()

    # Add harfbuzz as a library and setup build flags
    cg.add_library("harfbuzz", None, "https://github.com/harfbuzz/harfbuzz.git#9.0.0")
    cg.add_build_flag("-DHB_TINY")
    cg.add_build_flag("-DHB_CONFIG_OVERRIDE_H=\"\\\"$PROJECT_DIR/harfbuzz/config-override.h\\\"\"")
    cg.add_platformio_option("extra_scripts", ["pre:harfbuzz_build.py"])

    # Read the font file and create a static array
    filename = CORE.relative_config_path(config[CONF_FILE])
    with open(filename, mode='rb') as file:
        fileContent = file.read()

    file_content = [HexInt(x) for x in fileContent]
    font_data = cg.static_const_array(config[CONF_RAW_DATA_ID], file_content)

    # Create the font object
    cg.new_Pvariable(
        config[CONF_ID],
        font_data,
        len(file_content),
        config[CONF_SIZE],
    )

def copy_files() -> bool:
    dir = os.path.dirname(__file__)

    copy_file_if_changed(
        os.path.join(dir, "harfbuzz_build.py"),
        CORE.relative_build_path("harfbuzz_build.py"),
    )

    copy_file_if_changed(
        os.path.join(dir, "harfbuzz", "config-override.h"),
        CORE.relative_build_path("harfbuzz/config-override.h"),
    )
