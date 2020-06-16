version: 0.01
passes:
    renderPass: 
        name: "Forward"
        pipelineConfig: "PipelineConfigs\\Default.pcfg"
        shader: "Shaders\\sInp\\Fallback.sinp"
        textures:
            Diffuse: "Textures\\rick_and_morty.dds"
            Mask: "Textures\\pattern2.dds"
    renderPass: 
        name: "Wireframe"
        pipelineConfig: "PipelineConfigs\\Wireframe.pcfg"
        shader: "Shaders\\sInp\\Wireframe.sinp"