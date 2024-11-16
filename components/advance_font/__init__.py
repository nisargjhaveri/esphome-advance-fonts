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

DOMAIN = "font"
DEPENDENCIES = ["display"]
MULTI_CONF = True

advance_font_ns = cg.esphome_ns.namespace("advance_font")

Font = advance_font_ns.class_("Font")

CONFIG_SCHEMA = cv.All({
    cv.Required(CONF_ID): cv.declare_id(Font),
    cv.Required(CONF_FILE): cv.file_,
    cv.Optional(CONF_SIZE, default=20): cv.int_range(min=1),
    cv.GenerateID(CONF_RAW_DATA_ID): cv.declare_id(cg.uint8),
});

async def to_code(config):
    cg.add_library("tiny-text-renderer", None, "https://github.com/nisargjhaveri/tiny-text-renderer#v0.0.4")

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
