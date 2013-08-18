SECTION .text


global MSG_ReadBits
MSG_ReadBits:
    jmp 0x8130730

global GetMinBitCount
GetMinBitCount:
    jmp 0x8130500

global MSG_WriteDeltaClient
MSG_WriteDeltaClient:
    jmp 0x813f49c

global MSG_WriteDeltaField
MSG_WriteDeltaField:
    jmp 0x813e22a

global MSG_WriteDeltaPlayerstate
MSG_WriteDeltaPlayerstate:
    jmp 0x813f5a6

global MSG_WriteEntityIndex
MSG_WriteEntityIndex:
    jmp 0x813de54

global MSG_WriteBit1
MSG_WriteBit1:
    jmp 0x81306dc

global MSG_WriteBits
MSG_WriteBits:
    jmp 0x813061c

global MSG_ReadDeltaUsercmdKey
MSG_ReadDeltaUsercmdKey:
    jmp 0x81392ae

global MSG_SetDefaultUserCmd
MSG_SetDefaultUserCmd:
    jmp 0x8130ad0

global MSG_WriteBitsCompress
MSG_WriteBitsCompress:
    jmp 0x8130f3c

global MSG_WriteReliableCommandToBuffer
MSG_WriteReliableCommandToBuffer:
    jmp 0x813e162