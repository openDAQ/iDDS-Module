# iDDS Wrapper

The `idds_wrapper` provides a wrapper around CycloneDDS ([CycloneDDS C++ Binding](https://github.com/eclipse-cyclonedds/cyclonedds-cxx)) for handling iDDS communications. Currently includes the creation of topics for node discovery, messaging between devices, and data streaming.

## AboutNode (Domain 0) - Node Advertisement Topic
This topic is used to advertise the presence and nature of an iDDS node.

### IDL Structure
```cpp
struct AboutNode {
    @key RealNodeID realNodeID;
    BuildStandard buildStandard;
    OperationalStatus operationalStatus;
    string<64> statusReason;
    LogicalNodeID logicalNodeID;
    DomainID domainID;
    string<40> ipAddress;
    Time time;
};
```

### Data Fields
| Data Field Name   | Example Field Entry |
|------------------|--------------------------------|
| realNodeID       | manufacturer = "openDAQ", productType = "testDevice", serialNumber = "XXXXX" |
| buildStandard    | hardwareVersion = "1.0", softwareVersion = "1.0" |
| operationalStatus | OpStatusOperating |
| OpStatusFaulted  | "no errors" |
| logicalNodeID    | "node1" |
| domainID         | 2 |
| ipAddress        | "127.0.0.1" |
| time            | seconds = 1740618440, nanoseconds = 17406184758031 |

> **Time** is represented as seconds and nanoseconds since the epoch.

---

## Message (Domain 0) - Communication Between iDDS Devices
This topic is used for sending commands and receiving replies between iDDS devices.

### IDL Structure
```cpp
struct Message {
   @key LogicalNodeID targetLogicalNodeID;
   LogicalNodeID sourceLogicalNodeID;
   string<MAXIMUM_MESSAGE_BODY_LENGTH> messageBody;
   ReferenceNumber myReferenceNumber;
   ReferenceNumber yourReferenceNumber;
   Time time;
   unsigned long fragmentNumber;
   boolean moreFragments;
};
```

### Data Fields
| Data Field Name      | Example Field Entry |
|---------------------|--------------------|
| targetLogicalNodeID | "node1" |
| sourceLogicalNodeID | "node2" |
| messageBody        | More information below |
| myReferenceNumber  | 123 |
| yourReferenceNumber | 321 |
| time              | seconds = 1740618440, nanoseconds = 17406184758031 |
| fragmentNumber     | 1 (position of fragment) |
| moreFragments      | false (used when data payload exceeds maximum and must be divided; not yet implemented) |

### Message Body Structure
#### Request Syntax
```xml
<methodCall>
    <methodName>General.StartOperating</methodName>
    <params />
</methodCall>
```

#### Response Syntax
```xml
<methodResponse>
    <code>0</code>
    <message />
    <params />
</methodResponse>
```

---

## ParameterDataSeries (Domain 2) - Data Streaming Topic
This topic is used for real-time data streaming of parameter values.

### IDL Structure
```cpp
struct ParameterDataSeries {
   @key ParameterID parameterID;
   ParameterValues samples;
   Timestamps times;
   unsigned long qualityFlags;
   unsigned long instanceValue;
};
```

### Data Fields
| Data Field Name | Example Field Entry |
|---------------|--------------------|
| parameterID   | 6445 (Unique channel ID) |
| samples       | 1.0 (Sample value; demo channels use double values) |
| times        | seconds = 1740618440, nanoseconds = 17406184758031 |
| qualityFlags  | 0 (Not yet implemented) |
| instanceValue | 1001 |

This module facilitates structured iDDS communications, enabling reliable data exchange between openDAQ-based systems.
