// 명령어			옵션			원본 파일이 있는 위치			사본 파일을 저장할 위치

xcopy			/y/s		.\Engine\Public\*.*			.\EngineSDK\Inc\	

xcopy			/y			.\Engine\Bin\Engine.dll		.\Client\Bin\
xcopy			/y			.\Engine\Bin\Engine.dll		.\Tool\Bin\
xcopy			/y			.\Engine\Bin\ShaderFiles\*.* .\Client\Bin\ShaderFiles\
xcopy			/y			.\Engine\Bin\ShaderFiles\*.* .\Tool\Bin\ShaderFiles\
xcopy			/y			.\Engine\Bin\Engine.lib		.\EngineSDK\Lib\