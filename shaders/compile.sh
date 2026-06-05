if command -v glslc >/dev/null 2>&1; then
    GLSL_BIN="glslc"
elif [-n $VULKAN_SDK] && [-x "$VULKAN_SDK/bin/glslc"]; then
    GLSL_BIN = "$VULKAN_SDK/bin/glslc"
else
    GLSL_BIN = $(find ~/usr/local /opt -type f -name glslc -executable 2>/dev/null | head -n 1)
fi

if [ -z "$GLSL_BIN" ]; then
    echo "Error: glslc not found. Please install Vulkan SDK or add it to the PATH." >&2
    exit 1
fi

$GLSL_BIN shader.vert -o vert.spv
$GLSL_BIN shader.frag -o frag.spv
