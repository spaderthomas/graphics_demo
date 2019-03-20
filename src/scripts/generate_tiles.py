import os, sys
sys.path.append("../../")
from machine_conf import GLOBAL_PROJ_ROOT_DIR

old_cwd = os.getcwd()
os.chdir(GLOBAL_PROJ_ROOT_DIR)

WARNING = '''# THIS FILE WAS AUTOGENERATED BY generate_tiles.py
'''
print("/src/scripts/generate_tiles.py: Generating tile tdscript definitions")


tiles = [
    "grass", \
    "grass_flower1", \
    "grass_flower2", \
    "grass_flower3", \
    "sand", \
    "sand_cracked", \
    "sand_path_top", \
    "sand_path_bottom", \
    "sand_path_left", \
    "sand_path_right", \
    "sand_path_topleft_inner", \
    "sand_path_topright_inner", \
    "sand_path_bottomleft_inner", \
    "sand_path_bottomright_inner", \
    "sand_path_topleft_outer", \
    "sand_path_topright_outer", \
    "sand_path_bottomleft_outer", \
    "border_left_sand_grass", \
    "sand_path_bottomright_outer", \
    "border_bottom_sand_grass", \
    "border_left_sand_grass", \
    "border_top_sand_grass", \
    "border_right_sand_grass", \
    "border_bottom_right_sand_grass", \
    "border_bottom_left_sand_grass", \
    "border_top_left_sand_grass", \
    "border_top_right_sand_grass", \
    "border_bottom_right_grass", \
    "border_bottom_left_grass", \
    "border_top_left_grass", \
    "border_top_right_grass", \
    "wood_floor", \
    ]


with open("src/scripts/utils/tiles.tds", "w") as f:
    f.write(WARNING);
    for i, tile in enumerate(tiles):
        path = tile + '.png'
        lua_def = '''
entity.%s = {}
entity.%s.components = {}
entity.%s.components.Graphic_Component = {
    Animations = {
        %s = {
            "%s.png"
        }
    }
    default_animation = "%s"
    z = 0
}
entity.%s.components.Position_Component = {}

''' % (tile, tile, tile, tile, tile, tile, tile)
        
        f.write(lua_def)
        
os.chdir(old_cwd)
