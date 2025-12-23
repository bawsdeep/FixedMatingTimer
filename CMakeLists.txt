name: Build ASA Plugin

on:
  push:
    branches: [ main, master ]
  pull_request:
    branches: [ main, master ]
  workflow_dispatch:

jobs:
  build:
    runs-on: windows-latest
    
    steps:
    - name: Checkout plugin repository
      uses: actions/checkout@v4
      with:
        path: FixedMatingTimer
    
    - name: Checkout AsaApi (headers only)
      uses: actions/checkout@v4
      with:
        repository: ArkServerApi/AsaApi
        path: AsaApi
    
    - name: Add MSBuild to PATH
      uses: microsoft/setup-msbuild@v2
    
    - name: Setup CMake
      uses: lukka/get-cmake@latest
    
    - name: List AsaApi structure
      run: |
        echo "Checking AsaApi directory structure:"
        dir AsaApi
        dir AsaApi\AsaApi
        dir AsaApi\AsaApi\Core
        dir AsaApi\AsaApi\Core\Public
    
    - name: Configure Plugin
      shell: cmd
      run: |
        cd FixedMatingTimer
        mkdir build
        cd build
        cmake .. -G "Visual Studio 17 2022" -A x64 -DARK_API_DIR="%GITHUB_WORKSPACE%\AsaApi\AsaApi"
    
    - name: Build Plugin
      shell: cmd
      run: |
        cd FixedMatingTimer\build
        cmake --build . --config Release
    
    - name: List build output
      run: |
        echo "Build output:"
        dir FixedMatingTimer\build\Release
    
    - name: Upload artifacts
      uses: actions/upload-artifact@v4
      with:
        name: FixedMatingTimer-Plugin
        path: |
          FixedMatingTimer/build/Release/FixedMatingTimer.dll
          FixedMatingTimer/PluginInfo.json
          FixedMatingTimer/config.json
        if-no-files-found: error
    
    - name: Create Release (on tag)
      if: startsWith(github.ref, 'refs/tags/')
      uses: softprops/action-gh-release@v1
      with:
        files: |
          FixedMatingTimer/build/Release/FixedMatingTimer.dll
          FixedMatingTimer/PluginInfo.json
          FixedMatingTimer/config.json
        draft: false
        prerelease: false
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
