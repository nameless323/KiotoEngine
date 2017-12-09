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
blending:
    blendOp: Add
    srcBlend: SrcAlpha
    dstBlend: OneMinusSrcAlpha
colorMask: All