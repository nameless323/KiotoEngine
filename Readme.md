# Kioto engine.

Copyright (c) 2017 Aleksandr Vorontcov
Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).

This is an open source C++ game engine. You can find Codestyle.md file nearby.
Trello board: https://trello.com/b/ZlwreFRX/kioto-engine

You must add "Sources\Internal" and "KiotoEngine" to "c++\Additional Include Directories" before you build the engine first time.
Use x64 configuration, Windows 10 or higher. Visual Studio 2019 - v142 toolset. Windows SKD version not lower than 10.0.18362.0.0.

You must also create file AssetsConfig.yaml in project dir, near the .sln file (e.g. C:\KiotoEngine\AssetsConfig.yaml) and add to this file following line: enginePath: {path to engie}
### Possible content of the AssetsConfig.yaml
```yaml
enginePath: "C:\\KiotoEngine"
```
Also build step should be properly configured. For debug it should copy AssetsConfig.yaml, for Release all the assets.
### Example
```cpp
// Debug:

RD /S /Q $(TargetDir)Assets
(robocopy $(ProjectDir) $(TargetDir)Assets AssetsConfig.yaml) ^& IF %ERRORLEVEL% LEQ 1 exit 0

// Release:
RD /S /Q $(TargetDir)Assets
(robocopy $(ProjectDir)Assets $(TargetDir)Assets /S /E)^& IF %ERRORLEVEL% LEQ 1 exit 0
```

To be able to run the engine, you also need to build ShaderInputsParser and make sure that you have a proper build step that runs the parser to generate the shaders inputs cpp and hlsl files. 
### Example of the build step
```cpp
echo Running shader parser with command line inDir: $(ProjectDir)Assets\Shaders\sInp hlslOutDir: $(ProjectDir)Assets\autogen cppOutDir: $(ProjectDir)Sources\Internal\Render\Shaders\autogen templatesDir: $(ProjectDir)ShaderInputsParser\ShaderInputsParser\Templates
$(ProjectDir)ShaderInputsParser\ShaderInputsParser\bin\Debug\netcoreapp3.1\ShaderInputsParser.exe inDir: $(ProjectDir)Assets\Shaders\sInp hlslOutDir: $(ProjectDir)Assets\Shaders\autogen cppOutDir: $(ProjectDir)Sources\Internal\Render\Shaders\autogen templatesDir: $(ProjectDir)ShaderInputsParser\ShaderInputsParser\Templates shadersDir:  $(ProjectDir)Assets\Shaders
IF %ERRORLEVEL% NEQ 0 exit /b 1
```

