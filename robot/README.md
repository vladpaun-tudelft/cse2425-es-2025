# Robot (CSE2425 ES 2025)

RP2040 robot codebase for line following, obstacle avoidance, and fixed-distance travel.

## Build + Flash

- Latest main code:
  - `make latest`
- Snapshots:
  - `make assignment1`
  - `make assignment2`
  - `make assignment3`
  - `make assignment4`
  - `make assignment6`

## Project Layout

- `src/` current working code
- `snapshots/` archived assignment versions (each has its own `src/` and `CMakeLists.txt`)

## Notes

- Uses Pico SDK (see `pico_sdk_import.cmake`).