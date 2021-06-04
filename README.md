This is a Simplified Weather Simulator, based on the LES model.
It uses OpenCL 1.2, which may be difficult to use, so it is wrapped in a container class: `OCL`.

Visual Studio generated layout.
x64/ contains the Debug, and Release binaries, and kernels.
The current executables have been built with the `EvalKernel.cl` in mind, so it needs to be in the same directory as the executable.
`Test.cpp` is the file that uses the `OCL` wrapper.
`OCL.cpp` is the file that contains the implementation for the encapsulated `OCL` class.


## Build instructions

Possible to clone a copy of the latest repository on Github:
```
git clone https://github.com/OKaemii/OpenCL-ArrayChunking.git
```

### Requirements

* Tested on Windows 10
* Requires Visual Studio to build
* Program: Visual Studio 2019
* Libray: OpenCL 1.2
* Library: OpenMP

### Build steps

OCL_SDK_Light is the directory for where OpenCL 1.2 is installed.

The project waas build with the command:
```
/permissive- /ifcOutput "x64\Release\" /GS /GL /W3 /Gy /Zc:wchar_t /I"D:\SDK\OCL_SDK_Light\include" /Zi /Gm- /O2 /sdl /Fd"x64\Release\vc142.pdb" /Zc:inline /fp:precise /D "NDEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /Gd /Oi /MD /openmp /FC /Fa"x64\Release\" /EHsc /nologo /Fo"x64\Release\" /Fp"x64\Release\TestOpenCL.pch" /diagnostics:column 
```

### Test steps

Press the green button in Visual Studio.

# User manual 

Possible to clone a copy of the latest repository on Github:

```
git clone https://github.com/OKaemii/OpenCL-ArrayChunking.git
```

This is a C++ implementation.

Launch the executable in the release folder via the command line.

It is required that you have the necessary kernels in the same directory as the executable.

A scan of your devices will occur.

Select the devices to use to perform simulation testing.


# OCL Class

The `OCL` class can be imported using:

```#include OCL.h
```

The constructor will display a list of devices.

The `init` method allows the selection of such devices, and initialises the domain space.

The `run` method allows the execution of a single forward step into the simulation.

