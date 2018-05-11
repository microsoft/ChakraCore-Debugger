@if "debug"=="" echo off
setlocal

set DEPS=%~dp0..\..\deps
set OUTDIR=%~dp0Generated

if exist "%OUTDIR%" rmdir /s /q "%OUTDIR%"
mkdir "%OUTDIR%"

python "%~dp0GenerateProtocol.py" --generator_script "%DEPS%\inspector_protocol\CodeGenerator.py" --jinja_dir "%DEPS%\jinja\\" --markupsafe_dir "%DEPS%\markupsafe\\" --output_base "%OUTDIR%\\" --config "%~dp0inspector_protocol_config.json"
