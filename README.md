# ChakraCore-Debugger

ChakraCore-Debugger is a C++ companion library which enables the debugging of
[ChakraCore](https://github.com/Microsoft/ChakraCore/) using the
[DevTools Protocol](https://chromedevtools.github.io/devtools-protocol/v8/).

## Features

- Enables drop-in support for debugging ChakraCore in host applications
- Supports [Visual Studio Code](https://code.visualstudio.com/) and other compatible clients
- Exposes a C ABI for use by C99 consumers
- Flexibility to replace the transport service code to meet the needs of the host

## Status

This project is still in active development, but ready for integration. Future work is tracked in the
[issue tracker](https://github.com/Microsoft/ChakraCore-Debugger/milestones).

## Usage

### Prerequisites

* Windows
* Visual Studio 2017
* Python 2.7 (For generating the protocol parser only; must be in the PATH)

### Building

ChakraCore-Debugger depends on ChakraCore from [NuGet](https://www.nuget.org/packages/Microsoft.ChakraCore.vc140). All
other dependencies are header-only and included as submodules.

1. Clone the project (and submodules):
```console
> git clone --recurse-submodules https://github.com/Microsoft/ChakraCore-Debugger
```
2. Launch the solution file:
```console
> cd .\ChakraCore-Debugger\
> .\ChakraCore.Debugger.sln
```
3. Right-click the "ChakraCore.Debugger.Sample" project and click "Set as StartUp Project".
4. Choose the desired build configuration and platform (e.g. Debug - x64).
5. Right-click on the Solution in "Solution Explorer" and click "Restore NuGet Packages".
6. Set the debugger arguments in the "ChakraCore.Debugger.Sample" project to pass the startup parameters and the name of
   a script to run (e.g. `--inspect-brk --port 9229 test.js`).
7. Hit `F5` to start debugging.

### Connecting

Connect to the sample application using [Visual Studio Code](https://code.visualstudio.com/).

1. Navigate to a working folder and launch Visual Studio Code:
```console
> pushd <working folder>
> code .
```
2. Switch to the "Debug (Ctrl+Shift+D)" panel and click the "Configure or Fix 'launch.json'" gear button.
3. Select the "Node.js" environment.
4. Add a new configuration:
```json
{
    "type": "node",
    "request": "attach",
    "name": "Attach",
    "port": 9229,
    "protocol": "inspector"
}
```
5. Save the "launch.json" file and click the "Start Debugging" play button.
6. Execution should break on the first line of the script.

## Documentation

- [Design Spec](https://github.com/Microsoft/ChakraCore-Debugger/blob/master/doc/debug-companion.md)

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

## Reporting Security Issues

Security issues and bugs should be reported privately, via email, to the Microsoft Security
Response Center (MSRC) at [secure@microsoft.com](mailto:secure@microsoft.com). You should
receive a response within 24 hours. If for some reason you do not, please follow up via
email to ensure we received your original message. Further information, including the
[MSRC PGP](https://technet.microsoft.com/en-us/security/dn606155) key, can be found in
the [Security TechCenter](https://technet.microsoft.com/en-us/security/default).

## Contact Us

If you have any questions, please [open an issue](https://github.com/Microsoft/ChakraCore-Debugger/issues) and prepend
"[Question]" to the title.
