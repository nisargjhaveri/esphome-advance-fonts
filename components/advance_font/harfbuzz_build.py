Import('env')

def harfbuzz_build_middleware(env, node):
    """
    `node.name` - a name of File System Node
    `node.get_path()` - a relative path
    `node.get_abspath()` - an absolute path
    """

    if "/harfbuzz/" not in node.get_path():
        return node
    elif node.get_path().endswith("/harfbuzz/harfbuzz.cc"):
        return node
    else:
        return None

env.AddBuildMiddleware(harfbuzz_build_middleware)
