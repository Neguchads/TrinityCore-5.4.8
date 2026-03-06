@echo off
setlocal

cd /d "%~dp0"

set IMAGE_NAME=trinitycore548
set IMAGE_TAR=%IMAGE_NAME%.tar

if "%GITHUB_TOKEN%"=="" if exist ".env" (
  for /f "usebackq tokens=1,* delims==" %%A in (".env") do (
    if /I "%%~A"=="GITHUB_TOKEN" set "GITHUB_TOKEN=%%~B"
  )
)
if not "%GITHUB_TOKEN%"=="" set "GITHUB_TOKEN=%GITHUB_TOKEN:"=%"

if "%GITHUB_TOKEN%"=="" (
  echo GITHUB_TOKEN is not set.
  echo Set it in .env as GITHUB_TOKEN=... or export it in your shell, then rerun this script.
  exit /b 1
)

docker build --secret id=github_token,env=GITHUB_TOKEN -t %IMAGE_NAME% -f Dockerfile . || exit /b 1

docker save %IMAGE_NAME% -o %IMAGE_TAR% || exit /b 1

echo Image %IMAGE_NAME% has been built and saved to %IMAGE_TAR%.
