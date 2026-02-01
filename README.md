# TrinityCore (5.4.8.18414)

[![linux-build](https://github.com/brian8544/TrinityCore-5.4.8/actions/workflows/linux-build.yml/badge.svg?branch=master)](https://github.com/brian8544/TrinityCore-5.4.8/actions/workflows/linux-build.yml?query=branch%3Amaster)
[![win-x64-build](https://github.com/brian8544/TrinityCore-5.4.8/actions/workflows/win-x64-build.yml/badge.svg?branch=master)](https://github.com/brian8544/TrinityCore-5.4.8/actions/workflows/win-x64-build.yml?query=branch%3Amaster)

## Introduction

TrinityCore is an open-source game server framework for Mists of Pandaria (Build 18414). It aims to recreate the gameplay experience of World of Warcraft in a stable, blizzlike, and customizable way.

The original code is based on MaNGOS, CMaNGOS, TrinityCore, SunwellCore, SkyFire, and Pandaria, and has since then had extensive development to improve stability, in-game mechanics, and modularity. It is written in C++ and provides a solid foundation for creating private servers that mimic the mechanics and behavior of the official WoW servers.

## Modules

We support modules. You can add, remove, or update modules to extend core functionality or customize gameplay without modifying the core source.

## Requirements

- Platform: Linux, Windows, or macOS
- CPU: AMD64 or AArch64
- MySQL: 5.7 or 8.0-8.1
- CMake: 3.27.2 or newer
- OpenSSL: 1.1.1 or 3.0-3.1.1 (3.2.0 not supported with MySQL 8.0.33+)
- Boost: 1.85.0 (MSVC 14.3 x64) or 1.81+ on Linux
- Windows SDK: 10.0.22621
- Visual Studio: 2019 Community 16.4+ (Desktop, non-preview)
- GCC 13+ or Clang 12+

## Contributing

If you want to contribute, please read `.github/CONTRIBUTING.md` and open a pull request.

## Authors & Contributors

Read the file `.github/THANKS.md`.

## License

The source code is released under GPL-2.0. See `.github/COPYING.md`.