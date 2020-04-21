version: 0.01
passes:
    renderPass: 
        name: "Forward"
        pipelineConfig: "PipelineConfigs\\Default.pcfg"
        shader: "Shaders\\Fallback.hlsl"
        textures:
            Diffuse: "Textures\\rick_and_morty.dds"
            Mask: "Textures\\brick.dds"
    renderPass: 
        name: "Wireframe"
        pipelineConfig: "PipelineConfigs\\Wireframe.pcfg"
        shader: "Shaders\\Wireframe.hlsl"