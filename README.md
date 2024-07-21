# Bluetooch-Controlled_DC-Motor-Speed
## Getting Started
### Prerequisites
:electron: Hardware
    <ul>
        <li>MSP430FR2355 Launchpad</li>
        <li>Liquid Crystal Display</li>
        <li>DC Motor</li>
        <li>Breadboard</li>
        <li>Jump wires</li>
        <li>Resistors</li>
    </ul>
:desktop_computer: Software
 <ul>
    <li>Code Composer IDE</li>
 </ul>

### Installation
1. clone the repo
   ```sh
   git clone https://github.com/DucAnh-Vu-Trinh/Bluetooth-Controlled_DC-Motor-Speed.git
   ```
2. build the programe into MSP430FR2355
## Usage
:ballot_box_with_check: MSP430FR2355 communicates with the mobile app through *Bluetooth* by **I2C** Protocol
:ballot_box_with_check: **TIMERB0** and **TIMERB1** combine to implement **PWM** to control the Motor's Speed