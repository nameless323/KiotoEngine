version: 0.01
passes:
    renderPass: 
        name: "Forward"
        pipelineConfig: "PipelineConfigs\\Default.pcfg"
        shader: "Shaders\\sInp\\UnlitMovingTex.sinp"
        textures:
            Diffuse: "Textures\\rick_and_morty_2.dds"
            Mask: "Textures\\pattern3.dds"
    renderPass: 
        name: "Wireframe"
        pipelineConfig: "PipelineConfigs\\Wireframe.pcfg"
        shader: "Shaders\\sInp\\Wireframe.sinp"