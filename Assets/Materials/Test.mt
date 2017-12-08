shader: "Shaders\\Fallback"
renderLayer: Opaque
fill: Solid
cull: Back
winding: CCW
depthStencil:
    enableStencil: false
    ZTest: LEqual
    ZWrite: On
blending:
    blendOp: Add
    srcBlend: SrcAlpha
    dstBlend: OneMinusSrcAlpha
colorMask: all