**Navigation:** [🏠 gptp_notes](gptp_notes.md)

# MCK-RA8T2 – Board-Specific Guide

## Specific Connections

`GPTP1_PPS` is on pin **P714 (CN16-P3)** on every MCK-RA8T2 board.

## Hardware Configuration
None — every board runs in its default configuration and does not require any configuration
switch changes.

**Note:** This was not verified against the MCK-RA8T2 user manual; confirm the board's default
switch settings before publishing this guide.

## SEGGER RTT Control Block Address

Each project links its `_SEGGER_RTT` control block at a different RAM address (depends on that
project's own linker layout). If you attach J-Link RTT Viewer with a manual **Address** instead of
**Auto Detection**, use the value for the board you're viewing:

| Project | Role | `_SEGGER_RTT` Address |
|---|---|---|
| `tsn_controller_mck_ra8t2_ep` | Grand Master (GM) | `0x22005410` |
| `tsn_bridge_mck_ra8t2_ep` | Bridge | `0x22005410` |
| `tsn_slave_mck_ra8t2_ep` | Device (Slave) | `0x22005410` |

**This address is not fixed.** If application code changes and the project is rebuilt, it can
shift — re-check with `grep _SEGGER_RTT <project>.map` after any rebuild. When in doubt, prefer
J-Link RTT Viewer's **Auto Detection** over a hardcoded manual address.

**Navigation:** [🏠 gptp_notes](gptp_notes.md) | [Supported Boards →](gptp_notes.md#supported-boards)

<br> <br>
