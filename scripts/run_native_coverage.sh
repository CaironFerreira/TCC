#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
UNITY_DIR="$ROOT_DIR/.pio/libdeps/test_native/Unity/src"
BUILD_ROOT="$ROOT_DIR/coverage/build"
REPORT_ROOT="$ROOT_DIR/coverage/reports"
CXX="${CXX:-c++}"
CC="${CC:-cc}"
GCOVR="$ROOT_DIR/.venv/bin/gcovr"
LLVM_GCOV="xcrun llvm-cov gcov"

COMMON_CPP_FLAGS=(
  -std=c++17
  -O0
  --coverage
  -Isrc
  -Itest
  -Itest/support/stubs
  -I"$UNITY_DIR"
)
COMMON_C_FLAGS=(
  -O0
  --coverage
  -I"$UNITY_DIR"
)

SHARED_SOURCES=(
  src/adapters/display/DisplayService.cpp
  src/adapters/display/LayoutSupport.cpp
    src/adapters/input/GpioButtonInput.cpp
    src/adapters/network/udp/UdpReceiver.cpp
  src/adapters/network/web/PortalHtml.cpp
  src/adapters/network/wifi/WiFiConfigPortal.cpp
  src/adapters/telemetry/Forza7Decoder.cpp
  src/application/App.cpp
  src/application/instruments/FuelGauge.cpp
  src/application/instruments/RpmGauge.cpp
  src/application/instruments/SpeedGauge.cpp
  src/application/instruments/TireTempGauge.cpp
  src/application/services/InstrumentCluster.cpp
  src/application/services/TelemetryService.cpp
)

unit_suites=(
  test_display_service
  test_forza7_decoder
  test_fuel_gauge
  test_gpio_button_input
  test_instrument_cluster
  test_ui_status
  test_wifi_config_portal
  test_rpm_gauge
  test_speed_gauge
  test_telemetry_service
  test_tire_temp_gauge
  test_udp_receiver
)

integration_suites=(
  test_protocol_regression
  test_telemetry_robustness
  test_telemetry_pipeline
)

component_suites=(
  test_app
)

all_suites=(
  "${unit_suites[@]}"
  "${integration_suites[@]}"
  "${component_suites[@]}"
)

suite_path() {
  printf '%s/test_main.cpp' "test/$1"
}

select_suites() {
  case "$1" in
    unit)
      printf '%s\n' "${unit_suites[@]}"
      ;;
    integration)
      printf '%s\n' "${integration_suites[@]}"
      ;;
    component)
      printf '%s\n' "${component_suites[@]}"
      ;;
    all)
      printf '%s\n' "${all_suites[@]}"
      ;;
    *)
      echo "Unknown group: $1" >&2
      return 1
      ;;
  esac
}

build_group() {
  local group="$1"
  local build_dir="$BUILD_ROOT/$group"
  local report_dir="$REPORT_ROOT/$group"
  local -a suites=()

  while IFS= read -r suite; do
    suites+=("$suite")
  done < <(select_suites "$group")

  rm -rf "$build_dir" "$report_dir"
  mkdir -p "$build_dir/bin" "$report_dir"

  "$CC" "${COMMON_C_FLAGS[@]}" -c "$UNITY_DIR/unity.c" -o "$build_dir/unity.o"

  local -a object_files=("$build_dir/unity.o")
  local source rel obj_dir obj_path
  for source in "${SHARED_SOURCES[@]}"; do
    rel="${source#src/}"
    obj_path="$build_dir/src/${rel%.cpp}.o"
    obj_dir="$(dirname "$obj_path")"
    mkdir -p "$obj_dir"
    "$CXX" "${COMMON_CPP_FLAGS[@]}" -c "$ROOT_DIR/$source" -o "$obj_path"
    object_files+=("$obj_path")
  done

  local suite source_path test_obj binary
  for suite in "${suites[@]}"; do
    source_path="$ROOT_DIR/$(suite_path "$suite")"
    test_obj="$build_dir/tests/${suite}.o"
    binary="$build_dir/bin/${suite}"
    mkdir -p "$(dirname "$test_obj")"

    "$CXX" "${COMMON_CPP_FLAGS[@]}" -c "$source_path" -o "$test_obj"
    "$CXX" --coverage -o "$binary" "$test_obj" "${object_files[@]}"
    "$binary"
  done

  "$GCOVR" \
    "$build_dir" \
    --root "$ROOT_DIR" \
    --object-directory "$build_dir" \
    --filter "$ROOT_DIR/src/" \
    --exclude "$ROOT_DIR/test/" \
    --gcov-executable "$LLVM_GCOV" \
    --txt-summary \
    > "$report_dir/summary.txt"

  "$GCOVR" \
    "$build_dir" \
    --root "$ROOT_DIR" \
    --object-directory "$build_dir" \
    --filter "$ROOT_DIR/src/" \
    --exclude "$ROOT_DIR/test/" \
    --gcov-executable "$LLVM_GCOV" \
    --txt \
    > "$report_dir/files.txt"

  "$GCOVR" \
    "$build_dir" \
    --root "$ROOT_DIR" \
    --object-directory "$build_dir" \
    --filter "$ROOT_DIR/src/" \
    --exclude "$ROOT_DIR/test/" \
    --gcov-executable "$LLVM_GCOV" \
    --json-summary "$report_dir/summary.json" \
    --json-summary-pretty
}

if [ "$#" -eq 0 ]; then
  for group in unit integration component all; do
    build_group "$group"
  done
else
  for group in "$@"; do
    build_group "$group"
  done
fi
