#include "stdafx.h"

// Voler des octets
SInstructions StealBytes(LPVOID pTarget)
{
    SInstructions instructions      = {};
    instructions.m_uNumBytes        = 8;
    instructions.m_uNumInstructions = 2;

    // .text:00007FF697001790 89 54 24 10               mov     [rsp-8+arg_8], edx
    instructions.m_instructions[0] = {
        4,
        { 0x89, 0x54, 0x24, 0x10 }
    };

    // .text:00007FF697001794 89 4C 24 08               mov     [rsp-8+arg_0], ecx
    instructions.m_instructions[1] = {
        4,
        { 0x89, 0x4C, 0x24, 0x08 }
    };

    // replace instructions in target func wtih NOPs
    memset(pTarget, NOP, instructions.m_uNumBytes);

    return instructions;
}

uint32_t BuildTrampoline(LPVOID pTarget, LPVOID pDstMemForTrampoline)
{
    SInstructions stolenInstrs      = StealBytes(pTarget);

    uint8_t* stolenByteMemory       = (uint8_t*)pDstMemForTrampoline;
    uint8_t* jumpBackMemory         = stolenByteMemory + stolenInstrs.m_uNumBytes;  // m_uNumBytes: 4 + 4 = 8
    uint8_t* absoluteTableMemory    = jumpBackMemory + 13;                          // 13 is the size of a 64 bit mov/jmp instruction pair

    for (uint32_t i = 0; i < stolenInstrs.m_uNumInstructions; ++i)                  // stolenInstrs.m_uNumInstructions: x + y = 2
    {
        SInstruction& inst = stolenInstrs.m_instructions[i];

        // write stolen instruction (rewritten or otherwise) to trmapoline mem
        memcpy(stolenByteMemory, inst.bytes, inst.size);    // copy x and y
        stolenByteMemory += inst.size;                      // increment + 4 byte
    }

    // write jump back to hooked func
    // Saut vers le corps de la fonction accrochée
    WriteAbsoluteJump64(jumpBackMemory, (uint8_t*)pTarget + 5);

    return uint32_t(absoluteTableMemory - (uint8_t*)pDstMemForTrampoline);
}
