@echo off
setlocal

cd /d "%~dp0"

set IMAGE_NAME=trinitycore548:local
set IMAGE_TAR=trinitycore548.tar
set REPO_ROOT=%~dp0..\..

if not exist "%REPO_ROOT%\CMakeLists.txt" (
  echo Could not find TrinityCore repo root at %REPO_ROOT%.
  echo Expected to find CMakeLists.txt two levels above this script.
  exit /b 1
)

docker build -t "%IMAGE_NAME%" -f Dockerfile "%REPO_ROOT%" || exit /b 1

docker save "%IMAGE_NAME%" -o "%IMAGE_TAR%" || exit /b 1

echo Image %IMAGE_NAME% has been built and saved to %IMAGE_TAR%.
