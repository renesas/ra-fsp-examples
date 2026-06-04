**Navigation:** [🏠 can_fd_notes](can_fd_notes.md)

# Board-Specific Guides

## EK-RA4C1

### Additional Hardware
* For using on-board CAN Transceivers: 3 x Jumper wires.
* For using external CAN-FD Transceiver Board (e.g., OM13099): Optional.
    * 1 x OM13099 (CAN-FD Transceiver Board):
        * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
        * Manufacturer Product Number: OM13099.
    * 9 x Jumper wires.

### Specific Connections
* The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use the on-board debug functionality.

#### On-board CAN Transceivers

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 Pin |   | Board 2 Pin |
|-------------|---|-------------|
| J33:1 | <span style="font-size:1.6em;">↔</span> | J33:1 |
| J33:2 | <span style="font-size:1.6em;">↔</span> | J33:2 |
| J33:3 | <span style="font-size:1.6em;">↔</span> | J33:3 |

</div>

### Optional: External CAN-FD Transceiver Board
* The user must configure P402 as CAN CTX and P401 as CAN CRX instead of the default pins to run with External CAN-FD Transceiver Board.

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P402 (J2:48) | CAN0 TX (J3:4) | — |
| CRX P401 (J1:4) | CAN0 RX (J3:2) | — |
| VCC (J4:1) | 3V3 (J3:12) | — |
| 5V0 (J4:2) | 5V0 (J3:14) | — |
| GND (J4:25) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P402 (J2:48) |
| — | CAN1 RX (J1:2) | CRX P401 (J1:4) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## EK-RA4E2

### Additional Hardware
* For using on-board CAN Transceivers: 3 x Jumper wires.
* For using external CAN-FD Transceiver Board (e.g., OM13099): Optional.
    * 1 x OM13099 (CAN-FD Transceiver Board):
        * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
        * Manufacturer Product Number: OM13099.
    * 9 x Jumper wires.

### Specific Connections
#### On-board CAN Transceivers

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 Pin |   | Board 2 Pin |
|-------------|---|-------------|
| J32:1 | <span style="font-size:1.6em;">↔</span> | J32:1 |
| J32:2 | <span style="font-size:1.6em;">↔</span> | J32:2 |
| J32:3 | <span style="font-size:1.6em;">↔</span> | J32:3 |

</div>

#### Optional: External CAN-FD Transceiver Board
* The user must configure P103 as CAN CTX and P102 as CAN CRX instead of the default pins to run with External CAN-FD Transceiver Board.

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P103 (J2:36) | CAN0 TX (J3:4) | — |
| CRX P102 (J2:37) | CAN0 RX (J3:2) | — |
| VCC (J1:1) | 3V3 (J3:12) | — |
| 5V0 (J18:5) | 5V0 (J3:14) | — |
| GND (J1:14) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P103 (J2:36) |
| — | CAN1 RX (J1:2) | CRX P102 (J2:37) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## EK-RA4L1

### Additional Hardware
* For using on-board CAN Transceivers: 3 x Jumper wires.
* For using external CAN-FD Transceiver Board (e.g., OM13099): Optional.
    * 1 x OM13099 (CAN-FD Transceiver Board):
        * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
        * Manufacturer Product Number: OM13099.
    * 9 x Jumper wires.

### Specific Connections
#### On-board CAN Transceivers

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 Pin |   | Board 2 Pin |
|-------------|---|-------------|
| J33:1 | <span style="font-size:1.6em;">↔</span> | J33:1 |
| J33:2 | <span style="font-size:1.6em;">↔</span> | J33:2 |
| J33:3 | <span style="font-size:1.6em;">↔</span> | J33:3 |

</div>

### Optional: External CAN-FD Transceiver Board
* The user must configure P402 as CAN CTX and P401 as CAN CRX instead of the default pins to run with External CAN-FD Transceiver Board.

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P402 (J2:48) | CAN0 TX (J3:4) | — |
| CRX P401 (J4:5) | CAN0 RX (J3:2) | — |
| VCC (J1:1) | 3V3 (J3:12) | — |
| 5V0 (J1:2) | 5V0 (J3:14) | — |
| GND (J1:26) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P402 (J2:48) |
| — | CAN1 RX (J1:2) | CRX P401 (J4:5) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## EK-RA6E2

### Additional Hardware
* For using on-board CAN Transceivers: 3 x Jumper wires.
* For using external CAN-FD Transceiver Board (e.g., OM13099): Optional.
    * 1 x OM13099 (CAN-FD Transceiver Board):
        * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
        * Manufacturer Product Number: OM13099.
    * 9 x Jumper wires.

### Specific Connections
#### On-board CAN Transceivers

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 Pin |   | Board 2 Pin |
|-------------|---|-------------|
| J32:1 | <span style="font-size:1.6em;">↔</span> | J32:1 |
| J32:2 | <span style="font-size:1.6em;">↔</span> | J32:2 |
| J32:3 | <span style="font-size:1.6em;">↔</span> | J32:3 |

</div>

#### Optional: External CAN-FD Transceiver Board
* The user must configure P103 as CAN CTX and P102 as CAN CRX instead of the default pins to run with External CAN-FD Transceiver Board.

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P103 (J2:36) | CAN0 TX (J3:4) | — |
| CRX P102 (J2:37) | CAN0 RX (J3:2) | — |
| VCC (J1:1) | 3V3 (J3:12) | — |
| 5V0 (J18:5) | 5V0 (J3:14) | — |
| GND (J1:14) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P103 (J2:36) |
| — | CAN1 RX (J1:2) | CRX P102 (J2:37) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## EK-RA6M5

### Additional Hardware
* For using on-board CAN Transceivers: 3 x Jumper wires.
* For using external CAN-FD Transceiver Board (e.g., OM13099): Optional.
    * 1 x OM13099 (CAN-FD Transceiver Board):
        * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
        * Manufacturer Product Number: OM13099.
    * 9 x Jumper wires.

### Specific Connections
#### On-board CAN Transceivers

* The user must short E33, E34, and E35 to use on-board CAN transceiver.

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 Pin |   | Board 2 Pin |
|-------------|---|-------------|
| J33:1 | <span style="font-size:1.6em;">↔</span> | J33:1 |
| J33:2 | <span style="font-size:1.6em;">↔</span> | J33:2 |
| J33:3 | <span style="font-size:1.6em;">↔</span> | J33:3 |

</div>

#### Optional: External CAN-FD Transceiver Board
* The user must either cut E33, E34, and E35 to prevent conflicts with the on-board CAN transceiver or configure alternative CAN CTX and CRX pins instead of cutting E33, E34, and E35.

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P609 (J3:12) | CAN0 TX (J3:4) | — |
| CRX P610 (J3:13) | CAN0 RX (J3:2) | — |
| VCC (J18:4) | 3V3 (J3:12) | — |
| 5V0 (J18:5) | 5V0 (J3:14) | — |
| GND (J18:6) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P609 (J3:12) |
| — | CAN1 RX (J1:2) | CRX P610 (J3:13) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## EK-RA8D1

### Additional Hardware
* 1 x OM13099 (CAN-FD Transceiver Board):
    * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
    * Manufacturer Product Number: OM13099.
* 9 x Jumper wires.

### Specific Connections
* The user must set the configuration switches SW1: All OFF. Do not enable SW1-4 and SW1-5 together.

#### External CAN-FD Transceiver Board:

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P401 (J51:7) | CAN0 TX (J3:4) | — |
| CRX P402 (J52:15) | CAN0 RX (J3:2) | — |
| VCC (J18:4) | 3V3 (J3:12) | — |
| 5V0 (J18:5) | 5V0 (J3:14) | — |
| GND (J18:6) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P401 (J51:7) |
| — | CAN1 RX (J1:2) | CRX P402 (J52:15) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## EK-RA8D2

### Additional Hardware
* 1 x OM13099 (CAN-FD Transceiver Board):
    * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
    * Manufacturer Product Number: OM13099.
* 9 x Jumper wires.

### Specific Connections
#### External CAN-FD Transceiver Board:

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P312 (J17:4) | CAN0 TX (J3:4) | — |
| CRX P311 (J17:3) | CAN0 RX (J3:2) | — |
| VCC (J1:7) | 3V3 (J3:12) | — |
| 5V0 (J1:8) | 5V0 (J3:14) | — |
| GND (J1:40) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P312 (J17:4) |
| — | CAN1 RX (J1:2) | CRX P311 (J17:3) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## EK-RA8E2

### Additional Hardware
* For using on-board CAN Transceivers: 3 x Jumper wires.
* For using external CAN-FD Transceiver Board (e.g., OM13099): Optional.
    * 1 x OM13099 (CAN-FD Transceiver Board):
        * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
        * Manufacturer Product Number: OM13099.
    * 9 x Jumper wires.

### Specific Connections
#### On-board CAN Transceivers

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 Pin |   | Board 2 Pin |
|-------------|---|-------------|
| J7:1 | <span style="font-size:1.6em;">↔</span> | J7:1 |
| J7:2 | <span style="font-size:1.6em;">↔</span> | J7:2 |
| J7:3 | <span style="font-size:1.6em;">↔</span> | J7:3 |

</div>

### Optional: External CAN-FD Transceiver Board
* The user must configure P401 as CAN CTX and P402 as CAN CRX instead of the default pins to run with External CAN-FD Transceiver Board.

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P401 (J2:3) | CAN0 TX (J3:4) | — |
| CRX P402 (J2:4) | CAN0 RX (J3:2) | — |
| VCC (J4:1) | 3V3 (J3:12) | — |
| 5V0 (J4:2) | 5V0 (J3:14) | — |
| GND (J4:19) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P401 (J2:3) |
| — | CAN1 RX (J1:2) | CRX P402 (J2:4) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## EK-RA8M1

### Additional Hardware
* For using on-board CAN Transceivers: 3 x Jumper wires.
* For using external CAN-FD Transceiver Board (e.g., OM13099): Optional.
    * 1 x OM13099 (CAN-FD Transceiver Board):
        * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
        * Manufacturer Product Number: OM13099.
    * 9 x Jumper wires.

### Specific Connections
#### On-board CAN Transceivers
* The user must short E48, E49, and E50 to use on-board CAN transceiver

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 Pin |   | Board 2 Pin |
|-------------|---|-------------|
| J62:1 | <span style="font-size:1.6em;">↔</span> | J62:1 |
| J62:2 | <span style="font-size:1.6em;">↔</span> | J62:2 |
| J62:3 | <span style="font-size:1.6em;">↔</span> | J62:3 |

</div>

#### Optional: External CAN-FD Transceiver Board
* The user must either cut E48, E49, and E50 to prevent conflicts with the on-board CAN transceiver or configure alternative CAN CTX and CRX pins instead of cutting E48, E49, and E50.

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P312 (J51:12) | CAN0 TX (J3:4) | — |
| CRX P311 (J51:8) | CAN0 RX (J3:2) | — |
| VCC (J18:4) | 3V3 (J3:12) | — |
| 5V0 (J18:5) | 5V0 (J3:14) | — |
| GND (J18:6) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P312 (J51:12) |
| — | CAN1 RX (J1:2) | CRX P311 (J51:8) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## EK-RA8M2

### Additional Hardware
* For using on-board CAN Transceivers: 3 x Jumper wires.
* For using external CAN-FD Transceiver Board (e.g., OM13099): Optional.
    * 1 x OM13099 (CAN-FD Transceiver Board):
        * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
        * Manufacturer Product Number: OM13099.
    * 9 x Jumper wires.

### Specific Connections
* The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
#### On-board CAN Transceivers

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 Pin |   | Board 2 Pin |
|-------------|---|-------------|
| J33:1 | <span style="font-size:1.6em;">↔</span> | J33:1 |
| J33:2 | <span style="font-size:1.6em;">↔</span> | J33:2 |
| J33:3 | <span style="font-size:1.6em;">↔</span> | J33:3 |

</div>

#### Optional: External CAN-FD Transceiver Board
* The user must configure P415 as CAN CTX and P414 as CAN CRX instead of the default pins to run with External CAN-FD Transceiver Board.

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P415 (J4:20) | CAN0 TX (J3:4) | — |
| CRX P414 (J2:12) | CAN0 RX (J3:2) | — |
| VCC (J1:1) | 3V3 (J3:12) | — |
| 5V0 (J1:2) | 5V0 (J3:14) | — |
| GND (J1:39) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P415 (J4:20) |
| — | CAN1 RX (J1:2) | CRX P414 (J2:12) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## EK-RA8P1

### Additional Hardware
* 1 x OM13099 (CAN-FD Transceiver Board):
    * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
    * Manufacturer Product Number: OM13099.
* 9 x Jumper wires.

### Specific Connections
#### External CAN-FD Transceiver Board:

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P312 (J17:4) | CAN0 TX (J3:4) | — |
| CRX P311 (J17:3) | CAN0 RX (J3:2) | — |
| VCC (J1:7) | 3V3 (J3:12) | — |
| 5V0 (J1:8) | 5V0 (J3:14) | — |
| GND (J1:40) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P312 (J17:4) |
| — | CAN1 RX (J1:2) | CRX P311 (J17:3) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## EK-RA8T2

### Additional Hardware
* For using on-board CAN Transceivers: 3 x Jumper wires.
* For using external CAN-FD Transceiver Board (e.g., OM13099): Optional.
    * 1 x OM13099 (CAN-FD Transceiver Board):
        * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
        * Manufacturer Product Number: OM13099.
    * 9 x Jumper wires.

### Specific Connections
* The user must set the configuration switches (SW6 and SW4) as below to use the on-board debug functionality and CANFD channel 1.

	| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
	|-------|-------|-------|-------|-------|-------|-------|-------|-------|--------|
	| OFF   | OFF   | OFF   | OFF   | ON    | OFF   | ON    | ON    | ON    | ON     |


    | SW4-1 | SW4-2 | SW4-3 | SW4-4 | SW4-5 | SW4-6 | SW4-7 | SW4-8 |
	|-------|-------|-------|-------|-------|-------|-------|-------|
	|  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |

#### On-board CAN Transceivers

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 Pin |   | Board 2 Pin |
|-------------|---|-------------|
| J38:1 | <span style="font-size:1.6em;">↔</span> | J38:1 |
| J38:2 | <span style="font-size:1.6em;">↔</span> | J38:2 |
| J38:3 | <span style="font-size:1.6em;">↔</span> | J38:3 |

</div>

#### Optional: External CAN-FD Transceiver Board
* The user must either cut E18 and E20 to prevent conflicts with the on-board CAN transceiver or configure alternative CAN CTX and CRX pins instead of cutting E18 and E20.

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P415 (J2:29) | CAN0 TX (J3:4) | — |
| CRX P414 (J2:35) | CAN0 RX (J3:2) | — |
| VCC (J2:1) | 3V3 (J3:12) | — |
| 5V0 (J3:2) | 5V0 (J3:14) | — |
| GND (J2:39) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P415 (J2:29) |
| — | CAN1 RX (J1:2) | CRX P414 (J2:35) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## FPB-RA8E1

### Additional Hardware
* 1 x OM13099 (CAN-FD Transceiver Board):
    * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
    * Manufacturer Product Number: OM13099.
* 9 x Jumper wires.

### Specific Connections
#### External CAN-FD Transceiver Board:

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P312 (J2:31) | CAN0 TX (J3:4) | — |
| CRX P311 (J2:32) | CAN0 RX (J3:2) | — |
| VCC (J18:4) | 3V3 (J3:12) | — |
| 5V0 (J18:5) | 5V0 (J3:14) | — |
| GND (J18:6) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P312 (J2:31) |
| — | CAN1 RX (J1:2) | CRX P311 (J2:32) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## MCK-RA4T1

### Additional Hardware
* 1 x OM13099 (CAN-FD Transceiver Board):
    * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
    * Manufacturer Product Number: OM13099.
* 9 x Jumper wires.

### Specific Connections
#### External CAN-FD Transceiver Board:

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P401 (CN6:2) | CAN0 TX (J3:4) | — |
| CRX P402 (CN6:3) | CAN0 RX (J3:2) | — |
| VCC (CN2:24) | 3V3 (J3:12) | — |
| 5V0 (CN3:33) | 5V0 (J3:14) | — |
| GND (CN3:31) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P401 (CN6:2) |
| — | CAN1 RX (J1:2) | CRX P402 (CN6:3) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## MCK-RA6T3

### Additional Hardware
* 1 x OM13099 (CAN-FD Transceiver Board):
    * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
    * Manufacturer Product Number: OM13099.
* 9 x Jumper wires.

### Specific Connections
#### External CAN-FD Transceiver Board:

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P401 (CN6:2) | CAN0 TX (J3:4) | — |
| CRX P402 (CN6:3) | CAN0 RX (J3:2) | — |
| VCC (CN2:24) | 3V3 (J3:12) | — |
| 5V0 (CN3:33) | 5V0 (J3:14) | — |
| GND (CN3:31) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P401 (CN6:2) |
| — | CAN1 RX (J1:2) | CRX P402 (CN6:3) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## MCK-RA8T1

### Additional Hardware
* 1 x OM13099 (CAN-FD Transceiver Board):
    * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
    * Manufacturer Product Number: OM13099.
* 9 x Jumper wires.

### Specific Connections
#### External CAN-FD Transceiver Board:

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX P415 (CN5:2) | CAN0 TX (J3:4) | — |
| CRX P414 (CN5:3) | CAN0 RX (J3:2) | — |
| VCC (CN3:24) | 3V3 (J3:12) | — |
| 5V0 (CN4:34) | 5V0 (J3:14) | — |
| GND (CN3:32) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX P415 (CN5:2) |
| — | CAN1 RX (J1:2) | CRX P414 (CN5:3) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br>

## MCK-RA8T2

### Additional Hardware
* 1 x OM13099 (CAN-FD Transceiver Board):
    * Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
    * Manufacturer Product Number: OM13099.
* 9 x Jumper wires.

### Specific Connections
#### External CAN-FD Transceiver Board:

<div style="margin-left:2em; border:1px solid #ccc; border-radius:4px;">

| Board 1 | OM13099 | Board 2 |
|--------|---------|---------|
| CTX0 P203 (CN3:29) | CAN0 TX (J3:4) | — |
| CRX0 P202 (CN3:27) | CAN0 RX (J3:2) | — |
| VCC (CN3:23) | 3V3 (J3:12) | — |
| 5V0 (CN4:33) | 5V0 (J3:14) | — |
| GND (CN4:31) | GND (J3:16) | — |
| — | CAN1 TX (J1:4) | CTX0 P203 (CN3:29) |
| — | CAN1 RX (J1:2) | CRX0 P202 (CN3:27) |
| — | P3:2 <span style="font-size:1.6em;">↔</span> P3:13 | — |
| — | P3:7 <span style="font-size:1.6em;">↔</span> P3:18 | — |

</div>

### Special Notes
* None.

**Navigation:** [🏠 can_fd_notes](can_fd_notes.md) | [Supported Boards →](can_fd_notes.md#supported-boards)

<br> <br>