SECTION .text

global PbServerInitialize
PbServerInitialize:
    jmp 0x810ecde

global PbServerProcessEvents
PbServerProcessEvents:
    jmp 0x810ef08

global PbAuthClient
PbAuthClient:
    jmp 0x810e47a

global PbPassConnectString
PbPassConnectString:
    jmp 0x810e47a

global PbSvAddEvent
PbSvAddEvent:
    jmp 0x810ea20

global PbCapatureConsoleOutput
PbCapatureConsoleOutput:
    jmp 0x810e66a

global PbServerForceProcess
PbServerForceProcess:
    jmp 0x810ee36