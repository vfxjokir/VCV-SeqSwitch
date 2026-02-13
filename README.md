# SeqSwitch

A [VCV Rack 2](https://vcvrack.com/) plugin by **CherryTree** (Josh Kirschenbaum).

## Overview

SeqSwitch is a sequential switch module that routes an input signal to one of 8 outputs. It advances through steps on incoming clock pulses and supports an optional reset input to jump back to step 1.

## Features

- **Signal input** routed to the active output
- **Clock input** advances to the next step
- **Reset input** returns to step 1
- **Steps knob** sets the sequence length (1-8)
- **8 outputs** with LED indicators showing the active step

## Building

Requires the [VCV Rack SDK](https://vcvrack.com/manual/PluginDevelopmentTutorial).

```bash
# Build
make

# Install to VCV Rack plugins directory
make install
```

The `RACK_DIR` variable in the Makefile should point to your local Rack SDK path.

## License

Proprietary
