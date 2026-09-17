# Arm(R) Ethos(TM)-U Monitor

Ethos-U monitor is a library that provides an example of how to extract the
values of the PMU registers on the Arm Ethos-U55 NPU during an ongoing
inference, using an arbitrary timer source to trigger interrupts at a set
interval. It implements two backends,
[EventRecorder](https://www.keil.com/pack/doc/compiler/EventRecorder/html/index.html)
and printf. An example application showing how to use Ethos-U monitor is
included.

## Usage
When the Ethos-U monitor constructor is called, the user specifies which backend
to use, as well as the
[```EventIDs```](https://www.keil.com/pack/doc/compiler/EventRecorder/html/group__EventRecorder__Data.html#ga44fa52e2007e535753fd4ba59b84d55d)
for the EventRecorder entries. In the second step, ``configure`` is used to
configure which PMU registers that is to be recorded. A list of available PMU
registers can be found in [pmu_ethosu.h](https://gitlab.arm.com/artificial-intelligence/ethos-u/ethos-u-core-driver/-/blob/main/include/pmu_ethosu.h?ref_type=heads#L58)
in the core-driver repository. ```monitorSample``` is then used to record the
values of the registers on the selected backend. Lastly, ```release```
disables the PMU.

### Functions
| Function name | Description |
| ----------- | ----------- |
|```EthosUMonitor(eventRecordIds, backend)``` | ```eventRecordIds``` lists the [```EventIDs```](https://www.keil.com/pack/doc/compiler/EventRecorder/html/group__EventRecorder__Data.html#ga44fa52e2007e535753fd4ba59b84d55d) that will be associated with the list of PMU registers configured in ```configure``` when using ```EVENT_RECORDER``` as backend.<br /> ```backend``` describes which backend to use, and can be either ```PRINTF``` or ```EVENT_RECORDER``` |
|```configure(driver, eventIds)``` | Configures the PMU to monitor the PMU registers listed in ```eventIds```. The maximum number of PMU registers to monitor is 4.|
| ```release(driver)``` | Disables the PMU. |
|```monitorSample(driver)``` | Samples the PMU registers configured in ```configure``` and logs them using the selected backend. |

### Example
An example on how to use Ethos-U monitor can be found in the
[baremetal](https://gitlab.arm.com/artificial-intelligence/ethos-u/ethos-u-core-platform/-/blob/main/applications/baremetal/main.cpp?ref_type=heads)
application in the [ethosu-core-platform](https://gitlab.arm.com/artificial-intelligence/ethos-u/ethos-u-core-platform) repository.

The functions ```ethosu_inference_begin``` and ```ethosu_inference_end``` are
defined as weak symbols in
[ethosu_driver.c](https://gitlab.arm.com/artificial-intelligence/ethos-u/ethos-u-core-driver/-/blob/main/src/ethosu_driver.c?ref_type=heads#L238).

 They are called by the Ethos-U driver before and after the Ethos-U custom
 operator is executed. In the
 [example application](https://gitlab.arm.com/artificial-intelligence/ethos-u/ethos-u-core-platform/-/blob/main/applications/baremetal/main.cpp?ref_type=heads)
 a timer source is configured to periodically trigger an interrupt, on which the
 the configured PMU registers are sampled one by one and recorded using
 [EventRecorder](https://www.keil.com/pack/doc/compiler/EventRecorder/html/index.html)
 as backend. Note that the events in this example are logged from an interrupt
 context which will impact the execution time.

 Below is a sequence diagram showing a simplified version of how the different
 components interact.

![ethosu monitor](docs/ethosu_monitor_example_usage.svg "ethosu monitor sequence diagram").

### Output format
#### EventRecorder
For each of the configured PMU registers, an event is recorded using the
[EventRecord2](https://www.keil.com/pack/doc/compiler/EventRecorder/html/group__EventRecorder__Data.html#gab91eb760432ad0a10652a2c922db9566)
function. Each event consists of two int32-values and an id.

In the example application, the following PMU registers are recorded:
| EventID | Value 1 | Value 2 |
| ------- | ------- | ------- |
| Id1 | ETHOSU_PMU_CYCLE | Register value |
| Id2 | ETHOSU_PMU_NPU_ACTIVE | Register value |

```Id1``` and ```Id2``` are defined by the ```eventRecordIds``` parameter in the
constructor of Ethos-U monitor. ```Value 1``` is the PMU register number, as
represented by the [ethosu_pmu_event_type enum](https://gitlab.arm.com/artificial-intelligence/ethos-u/ethos-u-core-driver/-/blob/main/include/pmu_ethosu.h?ref_type=heads#L58).
```Value 2``` is the value of the PMU register when read by the
```monitorSample``` function.

#### printf
When using
```printf``` the register values are printed on the format
```ethosu_pmu_cntr<counter_no> : <register_value>```, where ```counter_no``` is
the index (0-3) of the configured PMU register, and ```register_value``` is the
value of the PMU register.
