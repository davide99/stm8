#ifndef STM8_MFRC522_H
#define STM8_MFRC522_H

#include <stdint.h>

#ifndef MFRC522_CS_PIN
    #define MFRC522_CS_PIN          4 //Port C
#endif

#ifndef MFRC522_IRQ_PIN
    #define MFRC522_IRQ_PIN         4 //Port D
#endif

//Maximum UID size
#define UID_MAX_SIZE          10

//Described in 9.3.3.6 / table 98 of the datasheet at http://www.nxp.com/documents/data_sheet/MFRC522.pdf
#define PCD_RX_MAX_GAIN       0x07u << 4u

// The MIFARE Classic uses a 4 bit ACK/NAK. Any other value than 0xA is NAK.
#define MF_ACK                0xAu
// A Mifare Crypto1 key is 6 bytes.
#define MF_KEY_SIZE           6u

//MFRC522 registers. Described in charapter 9 of the datasheet
//When using SPI all addresses are shifted one bit left in the "SPI address byte" (section 8.1.2.3)
enum PCD_Register {
    // Page 0: Command and status
    CommandReg              = 0x01u << 1u,    // starts and stops command execution
    ComIEnReg               = 0x02u << 1u,    // enable and disable interrupt request control bits
    DivIEnReg               = 0x03u << 1u,    // enable and disable interrupt request control bits
    ComIrqReg               = 0x04u << 1u,    // interrupt request bits
    DivIrqReg               = 0x05u << 1u,    // interrupt request bits
    ErrorReg                = 0x06u << 1u,    // error bits showing the error status of the last command executed 
    Status1Reg              = 0x07u << 1u,    // communication status bits
    Status2Reg              = 0x08u << 1u,    // receiver and transmitter status bits
    FIFODataReg             = 0x09u << 1u,    // input and output of 64 byte FIFO buffer
    FIFOLevelReg            = 0x0Au << 1u,    // number of bytes stored in the FIFO buffer
    WaterLevelReg           = 0x0Bu << 1u,    // level for FIFO underflow and overflow warning
    ControlReg              = 0x0Cu << 1u,    // miscellaneous control registers
    BitFramingReg           = 0x0Du << 1u,    // adjustments for bit-oriented frames
    CollReg                 = 0x0Eu << 1u,    // bit position of the first bit-collision detected on the RF interface
    
    // Page 1: Command
    ModeReg                 = 0x11u << 1u,    // defines general modes for transmitting and receiving 
    TxModeReg               = 0x12u << 1u,    // defines transmission data rate and framing
    RxModeReg               = 0x13u << 1u,    // defines reception data rate and framing
    TxControlReg            = 0x14u << 1u,    // controls the logical behavior of the antenna driver pins TX1 and TX2
    TxASKReg                = 0x15u << 1u,    // controls the setting of the transmission modulation
    TxSelReg                = 0x16u << 1u,    // selects the internal sources for the antenna driver
    RxSelReg                = 0x17u << 1u,    // selects internal receiver settings
    RxThresholdReg          = 0x18u << 1u,    // selects thresholds for the bit decoder
    DemodReg                = 0x19u << 1u,    // defines demodulator settings
    MfTxReg                 = 0x1Cu << 1u,    // controls some MIFARE communication transmit parameters
    MfRxReg                 = 0x1Du << 1u,    // controls some MIFARE communication receive parameters
    SerialSpeedReg          = 0x1Fu << 1u,    // selects the speed of the serial UART interface
    
    // Page 2: Configuration
    CRCResultRegH           = 0x21u << 1u,    // shows the MSB and LSB values of the CRC calculation
    CRCResultRegL           = 0x22u << 1u,
    ModWidthReg             = 0x24u << 1u,    // controls the ModWidth setting?
    RFCfgReg                = 0x26u << 1u,    // configures the receiver gain
    GsNReg                  = 0x27u << 1u,    // selects the conductance of the antenna driver pins TX1 and TX2 for modulation 
    CWGsPReg                = 0x28u << 1u,    // defines the conductance of the p-driver output during periods of no modulation
    ModGsPReg               = 0x29u << 1u,    // defines the conductance of the p-driver output during periods of modulation
    TModeReg                = 0x2Au << 1u,    // defines settings for the internal timer
    TPrescalerReg           = 0x2Bu << 1u,    // the lower 8 bits of the TPrescaler value. The 4 high bits are in TModeReg.
    TReloadRegH             = 0x2Cu << 1u,    // defines the 16-bit timer reload value
    TReloadRegL             = 0x2Du << 1u,
    TCounterValueRegH       = 0x2Eu << 1u,    // shows the 16-bit timer value
    TCounterValueRegL       = 0x2Fu << 1u,
    
    // Page 3: Test Registers
    TestSel1Reg             = 0x31u << 1u,    // general test signal configuration
    TestSel2Reg             = 0x32u << 1u,    // general test signal configuration
    TestPinEnReg            = 0x33u << 1u,    // enables pin output driver on pins D1 to D7
    TestPinValueReg         = 0x34u << 1u,    // defines the values for D1 to D7 when it is used as an I/O bus
    TestBusReg              = 0x35u << 1u,    // shows the status of the internal test bus
    AutoTestReg             = 0x36u << 1u,    // controls the digital self-test
    VersionReg              = 0x37u << 1u,    // shows the software version
    AnalogTestReg           = 0x38u << 1u,    // controls the pins AUX1 and AUX2
    TestDAC1Reg             = 0x39u << 1u,    // defines the test value for TestDAC1
    TestDAC2Reg             = 0x3Au << 1u,    // defines the test value for TestDAC2
    TestADCReg              = 0x3Bu << 1u     // shows the value of ADC I and Q channels
};

// MFRC522 commands. Described in chapter 10 of the datasheet.
enum PCD_Command {
    PCD_Idle                = 0x00u,          // no action, cancels current command execution
    PCD_Mem                 = 0x01u,          // stores 25 bytes into the internal buffer
    PCD_GenerateRandomID    = 0x02u,          // generates a 10-byte random ID number
    PCD_CalcCRC             = 0x03u,          // activates the CRC coprocessor or performs a self-test
    PCD_Transmit            = 0x04u,          // transmits data from the FIFO buffer
    PCD_NoCmdChange         = 0x07u,          // no command change, can be used to modify the CommandReg register bits without affecting the command, for example, the PowerDown bit
    PCD_Receive             = 0x08u,          // activates the receiver circuits
    PCD_Transceive          = 0x0Cu,          // transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission
    PCD_MFAuthent           = 0x0Eu,          // performs the MIFARE standard authentication as a reader
    PCD_SoftReset           = 0x0Fu           // resets the MFRC522
};
    
// Commands sent to the PICC.
enum PICC_Command {
    // The commands used by the PCD to manage communication with several PICCs (ISO 14443-3, Type A, section 6.4)
    PICC_CMD_REQA           = 0x26u,        // REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
    PICC_CMD_WUPA           = 0x52u,        // Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
    PICC_CMD_CT             = 0x88u,        // Cascade Tag. Not really a command, but used during anti collision.
    PICC_CMD_SEL_CL1        = 0x93u,        // Anti collision/Select, Cascade Level 1
    PICC_CMD_SEL_CL2        = 0x95u,        // Anti collision/Select, Cascade Level 2
    PICC_CMD_SEL_CL3        = 0x97u,        // Anti collision/Select, Cascade Level 3
    PICC_CMD_HLTA           = 0x50u,        // HaLT command, Type A. Instructs an ACTIVE PICC to go to state HALT.
    PICC_CMD_RATS           = 0xE0u,        // Request command for Answer To Reset.
    // The commands used for MIFARE Classic (from http://www.mouser.com/ds/2/302/MF1S503x-89574.pdf, Section 9)
    // Use PCD_MFAuthent to authenticate access to a sector, then use these commands to read/write/modify the blocks on the sector.
    // The read/write commands can also be used for MIFARE Ultralight.
    PICC_CMD_MF_AUTH_KEY_A  = 0x60u,        // Perform authentication with Key A
    PICC_CMD_MF_AUTH_KEY_B  = 0x61u,        // Perform authentication with Key B
    PICC_CMD_MF_READ        = 0x30u,        // Reads one 16 byte block from the authenticated sector of the PICC. Also used for MIFARE Ultralight.
    PICC_CMD_MF_WRITE       = 0xA0u,        // Writes one 16 byte block to the authenticated sector of the PICC. Called "COMPATIBILITY WRITE" for MIFARE Ultralight.
    PICC_CMD_MF_DECREMENT   = 0xC0u,        // Decrements the contents of a block and stores the result in the internal data register.
    PICC_CMD_MF_INCREMENT   = 0xC1u,        // Increments the contents of a block and stores the result in the internal data register.
    PICC_CMD_MF_RESTORE     = 0xC2u,        // Reads the contents of a block into the internal data register.
    PICC_CMD_MF_TRANSFER    = 0xB0u,        // Writes the contents of the internal data register to a block.
    // The commands used for MIFARE Ultralight (from http://www.nxp.com/documents/data_sheet/MF0ICU1.pdf, Section 8.6)
    // The PICC_CMD_MF_READ and PICC_CMD_MF_WRITE can also be used for MIFARE Ultralight.
    PICC_CMD_UL_WRITE       = 0xA2u         // Writes one 4 byte page to the PICC.
};
    
// PICC types we can detect. Remember to update PICC_GetTypeName() if you add more.
// last value set to 0xff, then compiler uses less ram, it seems some optimisations are triggered
enum PICC_Type {
    PICC_TYPE_UNKNOWN,
    PICC_TYPE_ISO_14443_4,          // PICC compliant with ISO/IEC 14443-4 
    PICC_TYPE_ISO_18092,            // PICC compliant with ISO/IEC 18092 (NFC)
    PICC_TYPE_MIFARE_MINI,          // MIFARE Classic protocol, 320 bytes
    PICC_TYPE_MIFARE_1K,            // MIFARE Classic protocol, 1KB
    PICC_TYPE_MIFARE_4K,            // MIFARE Classic protocol, 4KB
    PICC_TYPE_MIFARE_UL,            // MIFARE Ultralight or Ultralight C
    PICC_TYPE_MIFARE_PLUS,          // MIFARE Plus
    PICC_TYPE_MIFARE_DESFIRE,       // MIFARE DESFire
    PICC_TYPE_TNP3XXX,              // Only mentioned in NXP AN 10833 MIFARE Type Identification Procedure
    PICC_TYPE_NOT_COMPLETE = 0xff   // SAK indicates UID is not complete.
};

// Return codes from the functions in this library.
// Last value set to 0xff, then compiler uses less ram, it seems some optimisations are triggered
enum StatusCode {
    STATUS_OK,                      // Success
    STATUS_ERROR,                   // Error in communication
    STATUS_COLLISION,               // Collission detected
    STATUS_TIMEOUT,                 // Timeout in communication.
    STATUS_NO_ROOM,                 // A buffer is not big enough.
    STATUS_INTERNAL_ERROR,          // Internal error in the code. Should not happen ;-)
    STATUS_INVALID,                 // Invalid argument.
    STATUS_CRC_WRONG,               // The CRC_A does not match
    STATUS_MIFARE_NACK = 0xff       // A MIFARE PICC responded with NAK.
};


// A struct used for passing the UID of a PICC.
typedef struct {
    uint8_t size;			// Number of bytes in the UID. 4, 7 or 10.
    uint8_t	uidByte[10];
    uint8_t sak;			// The SAK (Select acknowledge) byte returned from the PICC after successful selection.
} Uid;

// Codes for writing to registers.
void PCD_WriteRegister(uint8_t reg, uint8_t value);
void PCD_WriteRegisterMany(uint8_t reg, uint8_t count, uint8_t *values);

// Codes for reading from registers.
uint8_t PCD_ReadRegister(uint8_t reg);
void PCD_ReadRegisterMany(uint8_t reg, uint8_t count, uint8_t *values, uint8_t rxAlign);

// Codes for setting / clearing particular bits.
void PCD_ClearRegisterBitMask(uint8_t reg, uint8_t mask);
void PCD_SetRegisterBitMask(uint8_t reg, uint8_t mask);

void    PCD_Init();
void    PCD_InitInterrupt();
void    PCD_ClearInterrupt();
void    PCD_InterruptReactivateReception();
uint8_t PCD_GetVersion();
uint8_t PICC_Select(Uid *uid, uint8_t);
uint8_t PICC_ReadCardSerial(Uid* uid);
uint8_t PICC_HaltA();
uint8_t PICC_RequestA(uint8_t *bufferATQA, uint8_t *bufferSize);
uint8_t PICC_WakeupA(uint8_t *bufferATQA, uint8_t *bufferSize);
uint8_t PICC_REQA_or_WUPA(uint8_t command, uint8_t *bufferATQA, uint8_t *bufferSize);
uint8_t PCD_CalculateCRC(uint8_t *data, uint8_t length, uint8_t* result);
uint8_t PCD_TransceiveData(uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign, int8_t checkCRC);
uint8_t PCD_CommunicateWithPICC(uint8_t command, uint8_t waitIRq, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign, int8_t checkCRC);

#endif