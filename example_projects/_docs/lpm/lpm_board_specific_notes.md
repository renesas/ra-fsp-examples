**Navigation:** [🏠 lpm_notes](lpm_notes.md)

# Board-Specific Guides

## EK‑RA2A1

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P301 (J2:2)       | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P302 (J2:4)       | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J2:12)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
	* Refer to the [EK-RA2A1](https://www.renesas.com/en/design-resources/boards-kits/ek-ra2a1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA2A2

### Additional Hardware
* None.

### Specific Connections
* None.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current: Not applicable.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA2E1

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P110 (J2:28)      | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P109 (J2:27)      | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J2:40)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA2E1](https://www.renesas.com/en/design-resources/boards-kits/ek-ra2e1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA2E2

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P401 (J1:3)       | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P400 (J1:2)       | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J1:13)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA2E2](https://www.renesas.com/en/design-resources/boards-kits/ek-ra2e2) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA2L1

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P301 (J1:30)      | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P302 (J1:29)      | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J1:40)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA2L1](https://www.renesas.com/en/design-resources/boards-kits/ek-ra2l1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA2L2

### Additional Hardware
* None.

### Specific Connections
* None.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Measure the voltage drops across R17 and calculate MCU's current (Icc); replace R17 with a larger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R17 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA2L2](https://www.renesas.com/en/design-resources/boards-kits/ek-ra2l2) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA4C1

### Additional Hardware
* None.

### Specific Connections
* The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use the on-board debug functionality.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA4C1](https://www.renesas.com/en/design-resources/boards-kits/ek-ra4c1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA4E2

### Additional Hardware
* None.

### Specific Connections
* None.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA4E2](https://www.renesas.com/en/design-resources/boards-kits/ek-ra4e2) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA4L1

### Additional Hardware
* None.

### Specific Connections
* None.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Remove R2.
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA4L1](https://www.renesas.com/en/design-resources/boards-kits/ek-ra4l1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA4M1

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P410 (J2:2)       | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P411 (J2:4)       | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J2:12)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
	* Refer to the [EK-RA4M1](https://www.renesas.com/en/design-resources/boards-kits/ek-ra4m1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA4M2

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P601 (J3:17)      | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P602 (J3:16)      | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J3:27)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Remove R2.
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA4M2](https://www.renesas.com/en/design-resources/boards-kits/ek-ra4m2) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA4M3

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P202 (J4:14)      | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P203 (J4:13)      | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J4:39)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Remove R2.
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA4M3](https://www.renesas.com/en/design-resources/boards-kits/ek-ra4m3) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA4W1

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P206 (CN7:7)      | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P205 (CN7:8)      | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (CN7:19)     | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of SS20.
	* To measure MCU's current, using a Digital Multi-meter connecting between pin1 and pin3 of E6 and measure current between pin1 and pin2 of J7; This is the recommended method. You can also measure and calculate the Icc by capturing the voltage drops across a resistor from 5 ohm to 20 ohm connected between pin1 and pin3 of E6.
	* Refer to the [EK-RA4W1](https://www.renesas.com/en/design-resources/boards-kits/ek-ra4w1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA6E2

### Additional Hardware
* None.

### Specific Connections
* None.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA6E2](https://www.renesas.com/en/design-resources/boards-kits/ek-ra6e2) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA6M1

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px; margin-bottom:8px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P100 (J2:2)       | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P101 (J2:4)       | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J2:12)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

<div style="margin-left:2em;">
Connect IRQ8 P415 (J2:5) to IRQ0-DS P206 (J4:12) because the user button S1 is used for IRQ8 P415 which cannot cancel Deep Software Standby mode.
</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
	* To get more accurate MCU's current (Icc), the user should cut traces between E10, E50 and E52 to isolate the Icc from the Analog power supply current AIcc.
	* Refer to the [EK-RA6M1](https://www.renesas.com/en/design-resources/boards-kits/ek-ra6m1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA6M2

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px; margin-bottom:8px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P410 (J2:2)       | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P411 (J2:20)      | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J2:12)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

<div style="margin-left:2em;">
Connect IRQ0 P105 (J1:37) to IRQ0-DS P206 (J1:3) because the user button S1 is used for IRQ0 P105 which cannot cancel Deep Software Standby mode.
</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
	* Refer to the [EK-RA6M2](https://www.renesas.com/en/design-resources/boards-kits/ek-ra6m2) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA6M3

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px; margin-bottom:8px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P410 (J3:35)      | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P411 (J3:36)      | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J3:39)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Remove R2.
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA6M3](https://www.renesas.com/en/design-resources/boards-kits/ek-ra6m3) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA6M3G

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px; margin-bottom:8px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P410 (J3:35)      | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P411 (J3:36)      | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J3:39)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Remove R2.
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA6M3G](https://www.renesas.com/en/design-resources/boards-kits/ek-ra6m3g) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA6M4

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px; margin-bottom:8px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P100 (J3:38)      | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P101 (J3:37)      | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J3:39)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Remove R2.
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA6M4](https://www.renesas.com/en/design-resources/boards-kits/ek-ra6m4) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA6M5

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px; margin-bottom:8px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P100 (J3:39)      | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P101 (J3:38)      | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J3:40)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Remove R2.
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA6M5](https://www.renesas.com/en/design-resources/boards-kits/ek-ra6m5) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA8D1

### Additional Hardware
* None.

### Specific Connections
* None.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Remove R2.
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA8D1](https://www.renesas.com/en/design-resources/boards-kits/ek-ra8d1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA8E2

### Additional Hardware
* None.

### Specific Connections
* None.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Remove R2.
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA8E2](https://www.renesas.com/en/design-resources/boards-kits/ek-ra8e2) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA8M1

### Additional Hardware
* None.

### Specific Connections
* None.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Remove R2.
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA8M1](https://www.renesas.com/en/design-resources/boards-kits/ek-ra8m1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## EK‑RA8P1

### Additional Hardware
* None.

### Specific Connections
* None.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
    * Remove R2.
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [EK-RA8P1](https://www.renesas.com/en/design-resources/boards-kits/ek-ra8p1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## FPB‑RA0E1

### Additional Hardware
* None.

### Specific Connections
* To use J-Link OB VCOM:
    * Short the E4 trace.
    * Replace R10 by 150ohm resistor.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current: Not applicable.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## FPB‑RA0E2

### Additional Hardware
* None.

### Specific Connections
* To use J-Link OB VCOM for Serial Terminal (UART), the user must close E46, E47.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current: Not applicable.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## FPB‑RA0L1

### Additional Hardware
* None.

### Specific Connections
* None.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current: Not applicable.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## FPB‑RA2E3

### Additional Hardware
* None.

### Specific Connections
* Connect P200 (J3:21) to IRQ0 P206 (J3:18) because the user button S1 (P200) does not have any interrupts to cancel/end LPM mode selected.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [FPB-RA2E3](https://www.renesas.com/en/design-resources/boards-kits/fpb-ra2e3) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## FPB‑RA2T1

### Additional Hardware
* None.

### Specific Connections
* The user must close E8, E15 and cut E10 to use P213 for LED1.
* The user must close E7 to use P914 for LED2.
* Connect P200 (J3:21)  <------->  IRQ0 P206 (J3:18) because user button S1 (P200) does not have interrupt to cancel/end lpm mode selected.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [FPB-RA2T1](https://www.renesas.com/en/design-resources/boards-kits/fpb-ra2t1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## FPB‑RA4E1

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px; margin-bottom:8px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P100 (J4:25)      | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P101 (J4:24)      | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J3:45)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins (CN1) on the kit connectors.
	* Refer to the [FPB-RA4E1](https://www.renesas.com/en/design-resources/boards-kits/fpb-ra4e1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## FPB‑RA6E1

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px; margin-bottom:8px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P100 (J4:25)      | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P101 (J4:24)      | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (J3:45)      | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins (CN1) on the kit connectors.
	* Refer to the [FPB-RA6E1](https://www.renesas.com/en/design-resources/boards-kits/fpb-ra6e1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## FPB‑RA8E1

### Additional Hardware
* None.

### Specific Connections
* None.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current:
	* Measure the voltage drops across R2 and calculate MCU's current (Icc); replace R2 by a bigger resistor if needed.
	* To measure MCU's current (Icc) directly, remove R2 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.
	* Refer to the [FPB-RA8E1](https://www.renesas.com/en/design-resources/boards-kits/fpb-ra8e1) schematic (Design Package) for more details.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## MCK‑RA4T1

### Additional Hardware
* 1 x Pmod BTN (e.g., https://digilent.com/reference/pmod/pmodbtn/start).

### Specific Connections
* Connect the Pmod BTN (J1:1-6) to the RA board via the PMOD1 Port (CN4:7-12).
* Use the BTN1 of Pmod BTN as the user push-button S1.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current: Not applicable.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## MCK‑RA6T2

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px; margin-bottom:8px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD PB04 (CN4:29)     | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD PB05 (CN4:27)     | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (CN4:25)     | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

<div style="margin-left:2em;">
Connect the RA board to the inverter board with INV2 Port to use SW2 on the inverter board as the user push-button S1.
</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current: Not applicable.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## MCK‑RA6T3

### Additional Hardware
* 1 x Pmod BTN (e.g., https://digilent.com/reference/pmod/pmodbtn/start).

### Specific Connections
* Connect the Pmod BTN (J1:1-6) to the RA board via the PMOD1 Port (CN4:7-12).
* Use the BTN1 of Pmod BTN as the user push-button S1.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current: Not applicable.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## MCK‑RA8T1

### Additional Hardware
* 1 x Pmod BTN (e.g., https://digilent.com/reference/pmod/pmodbtn/start).

### Specific Connections
* Connect the Pmod BTN (J1) to the PMOD Port (CN6) on the RA board. Please carefully align pin 1 on the Pmod BTN (J1) with pin 1 on the PMOD Port (CN6).
* Use the BTN1 of Pmod BTN as the user push-button S1.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current: Not applicable.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## MCK‑RA8T2

### Additional Hardware
* 1 x Pmod BTN (e.g., https://digilent.com/reference/pmod/pmodbtn/start).

### Specific Connections
* Connect the Pmod BTN (J1) to the Pmod 2A (CN5) on the RA board. Please carefully align pin 1 on the Pmod BTN (J1) with pin 1 on the Pmod 2A (CN5).
* Use the BTN1 of Pmod BTN as the user push-button S1.

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current: Not applicable.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>

## RSSK‑RA6T1

### Additional Hardware
* 1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
* 1 x Micro USB cable used to connect the Pmod USBUART to the RA board.
* 1 x Pmod BTN (e.g., https://digilent.com/reference/pmod/pmodbtn/start).

### Specific Connections

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px; margin-bottom:8px;">

| # | Board Pin             |       | Pmod USBUART Pin      |
|---|-----------------------|-------|-----------------------|
| 1 | RXD P206 (CN10:3)     | <span style="font-size:1.6em;">↔</span> | TXD (J2:3)            |
| 2 | TXD P205 (CN10:2)     | <span style="font-size:1.6em;">↔</span> | RXD (J2:2)            |
| 3 | GND      (CN10:4)     | <span style="font-size:1.6em;">↔</span> | GND (J2:5)            |

</div>

<div style="margin-left:2em; margin-bottom:8px;">
Use the BTN1 of the Pmod BTN as the user push-button S1.
</div>

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px; margin-bottom:8px;">

| # | Board Pin             |       | Pmod BTN Pin          |
|---|-----------------------|-------|-----------------------|
| 1 | IRQ06-DS P000 (CNB:5) | <span style="font-size:1.6em;">↔</span> | BTN1 (J1:2)           |
| 2 | VCC (CNB:18)          | <span style="font-size:1.6em;">↔</span> | VCC (J1:6)            |
| 3 | GND (CNB:20)          | <span style="font-size:1.6em;">↔</span> | GND (J1:5)            |

</div>

### Special Notes
1. Hardware Modifications Required for Measuring MCU Current: Not applicable.

**Navigation:** [🏠 lpm_notes](lpm_notes.md) | [Supported Boards →](lpm_notes.md#supported-boards)

<br>
