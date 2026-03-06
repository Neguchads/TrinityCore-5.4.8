@echo off
setlocal

cd /d "%~dp0"

set IMAGE_NAME=trinitycore548
set IMAGE_TAR=%IMAGE_NAME%.tar

docker image inspect %IMAGE_NAME% >nul 2>&1
if errorlevel 1 (
  if not exist "%IMAGE_TAR%" (
    echo Image %IMAGE_NAME% was not found and %IMAGE_TAR% is missing.
    echo Run create-image.bat on a build machine and copy this folder, including %IMAGE_TAR%.
    exit /b 1
  )
  echo Loading image %IMAGE_NAME% from %IMAGE_TAR%...
  docker load -i "%IMAGE_TAR%"
  if errorlevel 1 exit /b %errorlevel%
)

docker compose up -d --no-build
