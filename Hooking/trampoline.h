#pragma once

// Informations sur l'instruction désassemblé
struct SInstruction
{
    /**
     * Taille de cette instruction.
     */
    uint16_t size;

    /**
     * Octets machine de cette instruction.
     *
     * @note Le nombre d'octets étant indiqué par @size ci-dessus
     */
    uint8_t bytes[16];
};

struct SInstructions
{
    SInstruction    m_instructions[2];
    uint32_t        m_uNumInstructions;
    uint32_t        m_uNumBytes;
};

SInstructions StealBytes(LPVOID pTarget);

uint32_t BuildTrampoline(LPVOID pTarget, LPVOID dstMemForTrampoline);
