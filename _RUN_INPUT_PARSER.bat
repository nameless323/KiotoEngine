echo off
set ProjectDir=%1
echo Running shader parser with command line inDir: %ProjectDir%Assets\Shaders\sInp hlslOutDir: %ProjectDir%Assets\autogen cppOutDir: %ProjectDir%Sources\Internal\Render\Shaders\autogen templatesDir: %ProjectDir%ShaderInputsParser\ShaderInputsParser\Templates
%ProjectDir%ShaderInputsParser\ShaderInputsParser\bin\Debug\netcoreapp3.1\ShaderInputsParser.exe inDir: %ProjectDir%Assets\Shaders\sInp hlslOutDir: %ProjectDir%Assets\Shaders\autogen cppOutDir: %ProjectDir%Sources\Internal\Render\Shaders\autogen templatesDir: %ProjectDir%ShaderInputsParser\ShaderInputsParser\Templates shadersDir: %ProjectDir%Assets\Shaders forceRegenerate