:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.

@if "%batchdebug%"=="" echo off
setlocal

set DEPS=%~dp0..\..\deps
set OUTDIR=%~dp0Generated

if exist "%OUTDIR%" rmdir /s /q "%OUTDIR%"
mkdir "%OUTDIR%"

echo.
echo Generating files...

python "%DEPS%\inspector_protocol\convert_protocol_to_json.py" "%~dp0js_protocol.pdl" "%~dp0js_protocol.json"
python "%~dp0GenerateProtocol.py" --generator_script "%DEPS%\inspector_protocol\CodeGenerator.py" --jinja_dir "%DEPS%\jinja\\" --markupsafe_dir "%DEPS%\markupsafe\\" --output_base "%OUTDIR%\\" --config "%~dp0inspector_protocol_config.json"

echo Generated files into %OUTDIR%
echo.
