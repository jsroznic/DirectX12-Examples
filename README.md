# DirectX12-Examples
Introductory programs to begin learning DirectX 12.

These examples are based off of the highly detailed DirectX 12 Tutorials found [here](https://www.3dgep.com/learning-directx12-1/ "Learning DirectX 12").


Additional References for these projects include *Introduction to 3D Game Programming with DirectX 12*

## Notes for Adding new Projects

1. **Configure Include Path**

     Project -> Properties -> VC++ Directories -> Include Directories
  
     Add the path to the DX12Lib Folder
  
2. **Add Additional Dependencies**

     Project -> Properties -> Linker -> Input -> Additional Dependencies
  
     Add D3d12.lib and DXGI.lib
