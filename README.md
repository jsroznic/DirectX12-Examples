# DirectX12-Examples
Introductory programs to begin learning DirectX 12.

These examples are based off of the highly detailed DirectX 12 Tutorials found [here](https://www.3dgep.com/learning-directx12-1/ "Learning DirectX 12").


Additional References for these projects include *Introduction to 3D Game Programming with DirectX 12*

## Compiling Existing Projects

1. **Setup Visual Studio**

     Run the visual studio installer, and ensure that *Game Development with C++* is selected.
     
2. **Select 64-bit Compilation**
     
     Ensure that the target platform is x64 using the selector before building the project to avoid runtime issues with matrix math.

## Notes for Adding new Projects

1. **Configure Include Path**

     Project -> Properties -> VC++ Directories -> Include Directories
  
     Add the path to the DX12Lib folder and the project's current directory as follows: $(ProjectDir);$(SolutionDir)\DX12Lib;$(IncludePath)
  
2. **Add Additional Dependencies**

     Project -> Properties -> Linker -> Input -> Additional Dependencies
  
     Add D3d12.lib;DXGI.lib;D3Dcompiler.lib;shlwapi.lib;dxguid.lib;
     
3. **Disable Conformance Mode**

     Project -> Properties -> C/C++ -> Language -> Conformance Mode
  
     Set Conformance Mode to OFF due to issues with event.h in Windows 10.
     
4. **Select 64-bit Compilation**
     
     Ensure that the target platform is x64 using the selector before building the project to avoid runtime issues with matrix math.
     
5. **Repeat Steps 1-4 for each new project**
