# Time-Aware Shaper (TAS) Example Project

This example project demonstrates the IEEE 802.1Qbv Time-Aware Shaper (TAS) on the Ethernet
switch (ESWM) of a single Renesas RA MCU. TAS lets the switch hardware open and close each
egress queue's transmission gate on a repeating, time-based cycle, so time-critical traffic
classes get guaranteed transmission windows and cannot be delayed by best-effort traffic.

## Documentation

| Document | Description | 🔗 Link |
|----------|-------------|--------|
| Time-Aware Shaper Notes | Overview, runtime selection, configuration, and verification | [View on GitHub](https://github.com/renesas/ra-fsp-examples/tree/master/example_projects/_docs/layer3_ethernet_switch/ethernet_switch_tsn/tas/tas_notes.md) |
| Board-Specific Guide | Board-specific details | [View on GitHub](https://github.com/renesas/ra-fsp-examples/tree/master/example_projects/_docs/layer3_ethernet_switch/ethernet_switch_tsn/tas/tas_board_specific_notes.md) |

**Navigation:** [TAS notes](tas_notes.md) | [Board-specific guide](tas_board_specific_notes.md)
