# DirectX12-Examples
Introductory programs to begin learning DirectX 12.

These examples are based off of the highly detailed DirectX 12 Tutorials found [here](https://www.3dgep.com/learning-directx12-1/ "Learning DirectX 12").


Additional References for these projects include *Introduction to 3D Game Programming with DirectX 12*

## Notes for Adding new Projects

1. **Setup Visual Studio**

     Run the visual studio installer, and ensure that *Game Development with C++* is selected.
     
2. **Copy d3dcompiler_47.dll to Output Folder**
     
     This DLL is part of the Windows 10 SDK, a copy can be found in C:\Program Files (x86)\Windows Kits\10\Redist\D3D\x64\d3dcompiler_47.dll
     
     The output folder for projects is generally the *Debug* folder in the main directory.

3. **Configure Include Path**

     Project -> Properties -> VC++ Directories -> Include Directories
  
     Add the path to the DX12Lib folder and the project's current directory
  
4. **Add Additional Dependencies**

     Project -> Properties -> Linker -> Input -> Additional Dependencies
  
     Add D3d12.lib, DXGI.lib, D3Dcompiler.lib, shlwapi.lib, dxguid.lib
     
5. **Select 64-bit Compilation**
     
     Ensure that the target platform is x64 using the selector before building the project to avoid runtime issues with matrix math.
     
6. **Repeat Steps 3-5 for each new project**
