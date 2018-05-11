# ChakraCore-Debugger

Debugging companion for the ChakraCore JavaScript engine

## Prerequisites

* Visual Studio 2017
* Python 2.7 (For generating the protocol parser only; must be in the PATH)

## Building

ChakraCore-Debugger depends on ChakraCore from [NuGet](https://www.nuget.org/packages/Microsoft.ChakraCore.vc140).

1. Clone the project (and submodules)
```console
> git clone --recurse-submodules https://github.com/kfarnung/ChakraCore-Debugger
```
2. Launch the solution file
```console
> cd .\ChakraCore-Debugger\
> .\ChakraCoreDebugger.sln
```
3. Right-click the "Debug.Sample" project and click "Set as StartUp Project"
4. Choose the desired build configuration and platform (e.g. Debug - x64)
5. Right-click on the Solution in "Solution Explorer" and click "Restore NuGet Packages"
6. Set the debugger arguments in the "Debug.Sample" project to pass the name of a script to run (`test.js` is dropped
   with the executable by default)
7. Hit `F5` to start debugging

## Contributing

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.microsoft.com.

When you submit a pull request, a CLA-bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., label, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
