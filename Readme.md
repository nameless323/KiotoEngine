# Kioto engine.

This is an open source C++ game engine. You can find Codestyle.md file nearby.
Trello board: https://trello.com/b/ZlwreFRX/kioto-engine

You must add "Sources\Internal" and "KiotoEngine" to "c++\Additional Include Directories" before you build the engine first time.
Use x64 configuration, Windows 10 or higher. Visual Studio 2014 - v141 toolset. Windows SKD version not lower than 10.0.14393.0.

You must also create file AssetsConfig.yaml in project dir, near the .sln file (e.g. C:\KiotoEngine\AssetsConfig.yaml) and add to this file following line: enginePath: {path to engie}
```yaml
enginePath: "C:\\KiotoEngine"
```

