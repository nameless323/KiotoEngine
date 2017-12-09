version: 0.01
shader: "Shaders\\Fallback"
renderLayer: Opaque
fill: Solid
cull: Back
windingCCW: true
depthStencil:
    enableStencil: false
    ZTest: LEqual
    ZWrite: true
    writeMask: 0xF
    readMask: 0xF
    stencilFront:
        fail: Zero
        ZFfail: Zero
        pass: Zero
        func: Never
    stencilBack:
        fail: Zero
        ZFfail: Zero
        pass: Zero
        func: Never
blending:
    blendOp: Add
    srcBlend: SrcAlpha
    dstBlend: OneMinusSrcAlpha
colorMask: All