# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

VCV Rack 2.x plugin called **SeqSwitch** (brand: CherryTree). It contains a single module — a sequential switch that routes an input signal to one of 8 outputs, advancing on clock pulses with optional reset.

## Build Commands

```bash
# Build the plugin (produces plugin.dylib)
make

# Clean build artifacts
make clean

# Build distributable .vcvplugin ZIP
make dist

# Install to VCV Rack plugins directory
make install
```

The build uses VCV Rack's `plugin.mk` framework. The Rack SDK path is set in `Makefile` via `RACK_DIR` (currently points to `../Rack-SDK` relative path pattern).

## Architecture

- **`src/plugin.cpp`** — Plugin entry point. The `init()` function registers all module models with Rack.
- **`src/plugin.hpp`** — Shared header declaring the global `pluginInstance` pointer. All module source files include this.
- **`src/SeqSwitch.cpp`** — The SeqSwitch module. Contains both the DSP logic (`struct SeqSwitch : Module`) and the panel/widget layout (`struct SeqSwitchWidget : ModuleWidget`).
- **`plugin.json`** — Plugin metadata and module registry (slug, name, tags, version). Must stay in sync with the C++ model registrations.
- **`res/SeqSwitch.svg`** — Panel faceplate SVG loaded at runtime.

## VCV Rack Module Pattern

Each module follows a standard pattern:
1. A `Module` subclass with enums for `ParamId`, `InputId`, `OutputId`, `LightId` and a `process()` override for per-sample DSP.
2. A `ModuleWidget` subclass that sets the panel SVG and places UI components (knobs, ports, lights) using `mm2px()` coordinates.
3. A `Model*` global created via `createModel<ModuleClass, WidgetClass>("Slug")` and registered in `plugin.cpp`'s `init()`.

When adding a new module: create the source file in `src/`, declare `extern Model*` in `plugin.cpp`, and add the corresponding entry to `plugin.json`'s `modules` array. All `.cpp` files in `src/` are automatically compiled (via wildcard in Makefile).

## Key Dependencies

- **VCV Rack SDK** (`rack.hpp`) — provides `Module`, `ModuleWidget`, `dsp::SchmittTrigger`, port/param/light helpers, and the `mm2px()` coordinate system.
- C++11 or later (set by Rack SDK flags).
