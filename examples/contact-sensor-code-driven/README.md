# Code driven contact app

## Description

This is an example contact sensor app that is fully code driven.

Its goal is to show code size for a code driven application.

## Folder structure

- `clusters` contains non-migrated code-driven cluster implementations
- `device` contains the shared code among all platform implementations
- `<platform>` (like `linux`) contains the platform specific startup code
  of the application
