#!/usr/bin/env python3

from __future__ import annotations

import struct
from pathlib import Path


PACKET_SIZE = 311


def build_packet(
    *,
    rpm: float,
    speed_ms: float,
    tire_fl_f: float,
    tire_fr_f: float,
    tire_rl_f: float,
    tire_rr_f: float,
    fuel: float,
    lap_number: int,
    race_position: int,
    gear: int,
) -> bytes:
    packet = bytearray(PACKET_SIZE)

    def write_f32(offset: int, value: float) -> None:
        packet[offset : offset + 4] = struct.pack("<f", value)

    def write_u16(offset: int, value: int) -> None:
        packet[offset : offset + 2] = struct.pack("<H", value)

    def write_u8(offset: int, value: int) -> None:
        packet[offset] = value & 0xFF

    write_f32(16, rpm)
    write_f32(244, speed_ms)
    write_f32(256, tire_fl_f)
    write_f32(260, tire_fr_f)
    write_f32(264, tire_rl_f)
    write_f32(268, tire_rr_f)
    write_f32(276, fuel)
    write_u16(300, lap_number)
    write_u8(302, race_position)
    write_u8(307, gear)
    return bytes(packet)


def main() -> None:
    fixtures_dir = Path("test/fixtures")
    fixtures_dir.mkdir(parents=True, exist_ok=True)

    fixtures = {
        "forza_nominal_dash.bin": build_packet(
            rpm=4321.0,
            speed_ms=25.0,
            tire_fl_f=122.0,
            tire_fr_f=140.0,
            tire_rl_f=158.0,
            tire_rr_f=176.0,
            fuel=0.25,
            lap_number=9,
            race_position=3,
            gear=4,
        ),
        "forza_reverse_dash.bin": build_packet(
            rpm=1250.0,
            speed_ms=2.5,
            tire_fl_f=95.0,
            tire_fr_f=96.0,
            tire_rl_f=94.0,
            tire_rr_f=95.0,
            fuel=1.2,
            lap_number=12,
            race_position=8,
            gear=0,
        ),
        "forza_highspeed_dash.bin": build_packet(
            rpm=8123.0,
            speed_ms=76.4,
            tire_fl_f=210.0,
            tire_fr_f=215.0,
            tire_rl_f=208.0,
            tire_rr_f=212.0,
            fuel=0.05,
            lap_number=27,
            race_position=1,
            gear=6,
        ),
    }

    for name, data in fixtures.items():
        (fixtures_dir / name).write_bytes(data)


if __name__ == "__main__":
    main()
