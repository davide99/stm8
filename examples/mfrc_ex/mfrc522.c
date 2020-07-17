#include "mfrc522.h"
#include "../../stm8/gpio.h"
#include "../../stm8/spi.h"
#include "../../stm8/util.h" //sleep

inline uint8_t PCD_GetVersion(){
    return PCD_ReadRegister(VersionReg);
}

void PCD_WriteRegister(uint8_t reg, uint8_t value){
    spi_begin_transaction();
    PC_ODR &= ~SHIFTL8(MFRC522_CS_PIN); //low
    spi_transfer(reg);
    spi_transfer(value);
    PC_ODR |= SHIFTL8(MFRC522_CS_PIN); //high
    spi_end_transaction();
}

void PCD_WriteRegisterMany(uint8_t reg, uint8_t count, uint8_t *values){
    uint8_t i;

    spi_begin_transaction();
    PC_ODR &= ~SHIFTL8(MFRC522_CS_PIN); //low
    spi_transfer(reg);

    for(i=0; i<count; i++){
        spi_transfer(values[i]);
    }

    PC_ODR |= SHIFTL8(MFRC522_CS_PIN); //high
    spi_end_transaction();
}

uint8_t PCD_ReadRegister(uint8_t reg){
    uint8_t temp;

    spi_begin_transaction();
    PC_ODR &= ~SHIFTL8(MFRC522_CS_PIN); //low
    spi_transfer(reg | 0x80u);
    temp = spi_transfer(0);
    PC_ODR |= SHIFTL8(MFRC522_CS_PIN); //high
    spi_end_transaction();

    return temp;
}

void PCD_ReadRegisterMany(uint8_t reg, uint8_t count, uint8_t *values, uint8_t rxAlign){
    uint8_t index;

    if(!count){
        return;
    }

    reg |= 0x80u;               // MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
    index = 0;                  // Index in values array.
    spi_begin_transaction();
    PC_ODR &= ~SHIFTL8(MFRC522_CS_PIN); //low
    count--;                    // One read is performed outside of the loop
    spi_transfer(reg);          // Tell MFRC522 which address we want to read

    if(rxAlign){ // Only update bit positions rxAlign..7 in values[0]
        // Create bit mask for bit positions rxAlign..7
		uint8_t mask = (0xFFu << rxAlign) & 0xFFu;
		// Read value and tell that we want to read the same address again.
		uint8_t value = spi_transfer(reg);
		// Apply mask to both current value of values[0] and the new data in value.
		values[0] = (values[0] & ~mask) | (value & mask);
		index++;
    }

    while (index < count) {
		values[index] = spi_transfer(reg); // Read value and tell that we want to read the same address again.
		index++;
	}
	values[index] = spi_transfer(0);       // Read the final byte. Send 0 to stop reading.

    PC_ODR |= SHIFTL8(MFRC522_CS_PIN); //high
    spi_end_transaction();
}

void PCD_ClearRegisterBitMask(uint8_t reg, uint8_t mask){
    uint8_t tmp;

    tmp = PCD_ReadRegister(reg);
    PCD_WriteRegister(reg, tmp & (~mask));
}

void PCD_SetRegisterBitMask(uint8_t reg, uint8_t mask){
    uint8_t tmp;

    tmp = PCD_ReadRegister(reg);
    PCD_WriteRegister(reg, tmp | mask);
}

static inline void PCD_Reset(){
    uint8_t count;
    PCD_WriteRegister(CommandReg, PCD_SoftReset);
    
    count = 0;
    do {
        delay(50);
    } while((PCD_ReadRegister(CommandReg) & SHIFTL8(4)) && (++count) < 3);
}

static inline void PCD_AntennaOn(){
    uint8_t value;

    value = PCD_ReadRegister(TxControlReg);
    if((value & 0x03u) != 0x03u){
        PCD_WriteRegister(TxControlReg, value | 0x03u);
    }
}

inline void PCD_Init(){
    PC_DDR |= SHIFTL8(MFRC522_CS_PIN); //output
    PC_CR1 |= SHIFTL8(MFRC522_CS_PIN); //pushpull
    PC_ODR |= SHIFTL8(MFRC522_CS_PIN); //high

    //Soft reset
    PCD_Reset();

    //Reset baud rates
    PCD_WriteRegister(TxModeReg, 0);
    PCD_WriteRegister(RxModeReg, 0);

    //Reset ModWidthReg
    PCD_WriteRegister(ModWidthReg, 0x26u);

    /*
     * When communicating with a PICC we need a timeout if something goes wrong.
     * f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
     * TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
     */
    //TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
    PCD_WriteRegister(TModeReg, 0x80u);
    //TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25μs.
    PCD_WriteRegister(TPrescalerReg, 0xA9u);
    //Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
    PCD_WriteRegister(TReloadRegH, 0x03u);
    PCD_WriteRegister(TReloadRegL, 0xE8u);
    //Default 0x00. Force a 100% ASK modulation independent of the ModGsPReg register setting
    PCD_WriteRegister(TxASKReg, 0x40u);
    //Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
    PCD_WriteRegister(ModeReg, 0x3Du);

    PCD_AntennaOn();

    //Set maximum gain
    PCD_WriteRegister(RFCfgReg, PCD_RX_MAX_GAIN);
}

uint8_t PCD_TransceiveData(uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign, int8_t checkCRC){
    return PCD_CommunicateWithPICC(PCD_Transceive, 0x30u, sendData, sendLen, backData, backLen, validBits, rxAlign, checkCRC);
    //                                               ^-- RxIRq and IdleIRq
}

/**
 * Use the CRC coprocessor in the MFRC522 to calculate a CRC_A.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
uint8_t PCD_CalculateCRC(
    uint8_t *data,      //< In: Pointer to the data to transfer to the FIFO for CRC calculation.
    uint8_t length,     //< In: The number of bytes to transfer.
    uint8_t *result) {  //< Out: Pointer to result buffer. Result is written to result[0..1], low byte first.
        
    PCD_WriteRegister(CommandReg, PCD_Idle);          // Stop any active command.
    PCD_WriteRegister(DivIrqReg, 0x04u);              // Clear the CRCIRq interrupt request bit
    PCD_WriteRegister(FIFOLevelReg, 0x80u);           // FlushBuffer = 1, FIFO initialization
    PCD_WriteRegisterMany(FIFODataReg, length, data); // Write data to the FIFO
    PCD_WriteRegister(CommandReg, PCD_CalcCRC);       // Start the calculation
    
    // Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73μs.
    // TODO check/modify for other architectures than Arduino Uno 16bit

    for (uint16_t i = 5000; i > 0; i--) {
        // DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq reserved CRCIRq reserved reserved
        uint8_t n = PCD_ReadRegister(DivIrqReg);
        if (n & 0x04u) {                             // CRCIRq bit set - calculation done
            PCD_WriteRegister(CommandReg, PCD_Idle); // Stop calculating CRC for new content in the FIFO.
            // Transfer the result from the registers to the result buffer
            result[0] = PCD_ReadRegister(CRCResultRegL);
            result[1] = PCD_ReadRegister(CRCResultRegH);
            return STATUS_OK;
        }
    }
    // 89ms passed and nothing happend. Communication with the MFRC522 might be down.
    return STATUS_TIMEOUT;
}

/**
 * Transfers data to the MFRC522 FIFO, executes a command, waits for completion and transfers data back from the FIFO.
 * CRC validation can only be done if backData and backLen are specified.
 *
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
uint8_t PCD_CommunicateWithPICC(
    uint8_t command,    //< The command to execute. One of the PCD_Command enums.
    uint8_t waitIRq,    //< The bits in the ComIrqReg register that signals successful completion of the command.
    uint8_t *sendData,  //< Pointer to the data to transfer to the FIFO.
    uint8_t sendLen,    //< Number of bytes to transfer to the FIFO.
    uint8_t *backData,  //< NULL or pointer to buffer if data should be read back after executing the command.
    uint8_t *backLen,   //< In: Max number of bytes to write to *backData. Out: The number of uint8_ts returned.
    uint8_t *validBits, //< In/Out: The number of valid bits in the last byte. 0 for 8 valid bits.
    uint8_t rxAlign,    //< In: Defines the bit position in backData[0] for the first bit received.
    int8_t checkCRC){   //< In: True => The last two bytes of the response is assumed to be a CRC_A that must be validated.

    //Prepare values for BitFramingReg
    uint8_t txLastBits = validBits ? *validBits : 0;
    uint8_t bitFraming = (rxAlign << 4) + txLastBits;

    PCD_WriteRegister(CommandReg, PCD_Idle);                //Stop any active command.
    PCD_WriteRegister(ComIrqReg, 0x7Fu);                    //Clear all seven interrupt request bits
    PCD_WriteRegister(FIFOLevelReg, 0x80u);                 //FlushBuffer = 1, FIFO initialization
    PCD_WriteRegisterMany(FIFODataReg, sendLen, sendData);  //Write sendData to the FIFO
    PCD_WriteRegister(BitFramingReg, bitFraming);           //Bit adjustments
    PCD_WriteRegister(CommandReg, command);                 //Execute the command
    if (command == PCD_Transceive) {
        PCD_SetRegisterBitMask(BitFramingReg, 0x80u);       //StartSend=1, transmission of data starts
    }
    
    // Wait for the command to complete.
    // In PCD_Init() we set the TAuto flag in TModeReg. This means the timer automatically starts when the PCD stops transmitting.
    // Each iteration of the do-while-loop takes 17.86μs.
    // TODO check/modify for other architectures than Arduino Uno 16bit
    uint16_t i;
    for (i = 20000; i > 0; i--) {
        uint8_t n = PCD_ReadRegister(ComIrqReg);   // ComIrqReg[7..0] bits are: Set1 TxIRq RxIRq IdleIRq HiAlertIRq LoAlertIRq ErrIRq TimerIRq
        if (n & waitIRq) {                  // One of the interrupts that signal success has been set.
            break;
        }
        if (n & 0x01) {                     // Timer interrupt - nothing received in 25ms
            return STATUS_TIMEOUT;
        }
    }
    // 35.7ms and nothing happend. Communication with the MFRC522 might be down.
    if (i == 0) {
        return STATUS_TIMEOUT;
    }
    
    // Stop now if any errors except collisions were detected.
    // ErrorReg[7..0] bits are: WrErr TempErr reserved BufferOvfl CollErr CRCErr ParityErr ProtocolErr
    uint8_t errorRegValue = PCD_ReadRegister(ErrorReg);
    if (errorRegValue & 0x13) {  // BufferOvfl ParityErr ProtocolErr
        return STATUS_ERROR;
    }
  
    uint8_t _validBits = 0;
    
    // If the caller wants data back, get it from the MFRC522.
    if (backData && backLen) {
        uint8_t n = PCD_ReadRegister(FIFOLevelReg); // Number of bytes in the FIFO

        if (n > *backLen) {
            return STATUS_NO_ROOM;
        }

        *backLen = n; // Number of bytes returned
        PCD_ReadRegisterMany(FIFODataReg, n, backData, rxAlign); // Get received data from FIFO
        // RxLastBits[2:0] indicates the number of valid bits in the last received uint8_t. If this value is 000b, the whole uint8_t is valid.
        _validBits = PCD_ReadRegister(ControlReg) & 0x07u;
        if (validBits) {
            *validBits = _validBits;
        }
    }
    
    // Tell about collisions
    if (errorRegValue & 0x08u) {     // CollErr
        return STATUS_COLLISION;
    }
    
    // Perform CRC_A validation if requested.
    if (backData && backLen && checkCRC) {
        // In this case a MIFARE Classic NAK is not OK.
        if (*backLen == 1 && _validBits == 4) {
            return STATUS_MIFARE_NACK;
        }
        // We need at least the CRC_A value and all 8 bits of the last uint8_t must be received.
        if (*backLen < 2 || _validBits != 0) {
            return STATUS_CRC_WRONG;
        }
        // Verify CRC_A - do our own calculation and store the control in controlBuffer.
        uint8_t controlBuffer[2];
        uint8_t status = PCD_CalculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]);
        if (status != STATUS_OK) {
            return status;
        }
        if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1])) {
            return STATUS_CRC_WRONG;
        }
    }
    return STATUS_OK;

}

uint8_t PICC_HaltA() {
    uint8_t result;
    uint8_t buffer[4];
    
    // Build command buffer
    buffer[0] = PICC_CMD_HLTA;
    buffer[1] = 0;
    // Calculate CRC_A
    result = PCD_CalculateCRC(buffer, 2, &buffer[2]);
    if (result != STATUS_OK) {
        return result;
    }
    
    // Send the command.
    // The standard says:
    //		If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
    //		HLTA command, this response shall be interpreted as 'not acknowledge'.
    // We interpret that this way: Only STATUS_TIMEOUT is a success.
    result = PCD_TransceiveData(buffer, sizeof(buffer), 0, 0, 0, 0, 0);
    if (result == STATUS_TIMEOUT) {
        return STATUS_OK;
    }
    if (result == STATUS_OK) { // That is ironically NOT ok in this case ;-)
        return STATUS_ERROR;
    }
    return result;
}

/**
 * Transmits SELECT/ANTICOLLISION commands to select a single PICC.
 * Before calling this function the PICCs must be placed in the READY(*) state by calling PICC_RequestA() or PICC_WakeupA().
 * On success:
 * 		- The chosen PICC is in state ACTIVE(*) and all other PICCs have returned to state IDLE/HALT. (Figure 7 of the ISO/IEC 14443-3 draft.)
 * 		- The UID size and value of the chosen PICC is returned in *uid along with the SAK.
 * 
 * A PICC UID consists of 4, 7 or 10 bytes.
 * Only 4 bytes can be specified in a SELECT command, so for the longer UIDs two or three iterations are used:
 * 		UID size	Number of UID bytes		Cascade levels		Example of PICC
 * 		========	===================		==============		===============
 * 		single				 4						1				MIFARE Classic
 * 		double				 7						2				MIFARE Ultralight
 * 		triple				10						3				Not currently in use?
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
uint8_t PICC_Select(Uid *uid) { //< Pointer to Uid struct
    int8_t uidComplete, selectDone;
    uint8_t cascadeLevel = 1;
    uint8_t result;
    uint8_t count;
    uint8_t checkBit;
    uint8_t index;
    uint8_t uidIndex;                   // The first index in uid->uidByte[] that is used in the current Cascade Level.
    int8_t currentLevelKnownBits;       // The number of known UID bits in the current Cascade Level.
    uint8_t buffer[9];                  // The SELECT/ANTICOLLISION commands uses a 7 byte standard frame + 2 bytes CRC_A
    uint8_t bufferUsed;                 // The number of bytes used in the buffer, ie the number of bytes to transfer to the FIFO.
    uint8_t rxAlign;                    // Used in BitFramingReg. Defines the bit position for the first bit received.
    uint8_t txLastBits;                 // Used in BitFramingReg. The number of valid bits in the last transmitted byte.
    uint8_t *responseBuffer;
    uint8_t responseLength;

    // Description of buffer structure:
    //		Byte 0: SEL 				Indicates the Cascade Level: PICC_CMD_SEL_CL1, PICC_CMD_SEL_CL2 or PICC_CMD_SEL_CL3
    //		Byte 1: NVB					Number of Valid Bits (in complete command, not just the UID): High nibble: complete bytes, Low nibble: Extra bits.
    //		Byte 2: UID-data or CT		See explanation below. CT means Cascade Tag.
    //		Byte 3: UID-data
    //		Byte 4: UID-data
    //		Byte 5: UID-data
    //		Byte 6: BCC					Block Check Character - XOR of bytes 2-5
    //		Byte 7: CRC_A
    //		Byte 8: CRC_A
    // The BCC and CRC_A are only transmitted if we know all the UID bits of the current Cascade Level.
    //
    // Description of bytes 2-5: (Section 6.5.4 of the ISO/IEC 14443-3 draft: UID contents and cascade levels)
    //		UID size	Cascade level	Byte2	Byte3	Byte4	Byte5
    //		========	=============	=====	=====	=====	=====
    //		 4 bytes		1			uid0	uid1	uid2	uid3
    //		 7 bytes		1			CT		uid0	uid1	uid2
    //						2			uid3	uid4	uid5	uid6
    //		10 bytes		1			CT		uid0	uid1	uid2
    //						2			CT		uid3	uid4	uid5
    //						3			uid6	uid7	uid8	uid9

    // Prepare MFRC522
    PCD_ClearRegisterBitMask(CollReg, 0x80u);        // ValuesAfterColl=1 => Bits received after collision are cleared.

    // Repeat Cascade Level loop until we have a complete UID.
    uidComplete = 0;
    do {
        // Set the Cascade Level in the SEL byte, find out if we need to use the Cascade Tag in byte 2.
        switch (cascadeLevel) {
            case 1:
                buffer[0] = PICC_CMD_SEL_CL1;
                uidIndex = 0;
                break;

            case 2:
                buffer[0] = PICC_CMD_SEL_CL2;
                uidIndex = 3;
                break;

            case 3:
                buffer[0] = PICC_CMD_SEL_CL3;
                uidIndex = 6;
                break;

            default:
                return STATUS_INTERNAL_ERROR;
        }

        // How many UID bits are known in this Cascade Level?
        currentLevelKnownBits = 0;

        // Copy the known bits from uid->uidByte[] to buffer[]
        uint8_t bytesToCopy = 0;

        // Repeat anti collision loop until we can transmit all UID bits + BCC and receive a SAK - max 32 iterations.
        selectDone = 0;
        do {
            // Find out how many bits and bytes to send and receive.
            if (currentLevelKnownBits >= 32) { // All UID bits in this Cascade Level are known. This is a SELECT.
                buffer[1] = 0x70u; // NVB - Number of Valid Bits: Seven whole bytes
                // Calculate BCC - Block Check Character
                buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
                // Calculate CRC_A
                result = PCD_CalculateCRC(buffer, 7, &buffer[7]);
                if (result != STATUS_OK) {
                    return result;
                }
                txLastBits = 0; // 0 => All 8 bits are valid.
                bufferUsed = 9;
                // Store response in the last 3 bytes of buffer (BCC and CRC_A - not needed after tx)
                responseBuffer = &buffer[6];
                responseLength = 3;
            } else { // This is an ANTICOLLISION.
                txLastBits = currentLevelKnownBits % 8;
                count = currentLevelKnownBits / 8;    // Number of whole bytes in the UID part.
                index = 2 + count;                    // Number of whole bytes: SEL + NVB + UIDs
                buffer[1] = (index << 4u) + txLastBits;    // NVB - Number of Valid Bits
                bufferUsed = index + (txLastBits ? 1 : 0);
                // Store response in the unused part of buffer
                responseBuffer = &buffer[index];
                responseLength = sizeof(buffer) - index;
            }

            // Set bit adjustments
            rxAlign = txLastBits;   // Having a separate variable is overkill. But it makes the next line easier to read.
            // RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]
            PCD_WriteRegister(BitFramingReg, (rxAlign << 4u) + txLastBits);

            // Transmit the buffer and receive the response.
            result = PCD_TransceiveData(buffer, bufferUsed, responseBuffer, &responseLength, &txLastBits, rxAlign, 0);
            if (result == STATUS_COLLISION) { // More than one PICC in the field => collision.
                // CollReg[7..0] bits are: ValuesAfterColl reserved CollPosNotValid CollPos[4:0]
                uint8_t valueOfCollReg = PCD_ReadRegister(CollReg);
                if (valueOfCollReg & 0x20u) { // CollPosNotValid
                    return STATUS_COLLISION; // Without a valid collision position we cannot continue
                }
                uint8_t collisionPos = valueOfCollReg & 0x1Fu; // Values 0-31, 0 means bit 32.
                if (collisionPos == 0) {
                    collisionPos = 32;
                }
                if (collisionPos <= currentLevelKnownBits) { // No progress - should not happen
                    return STATUS_INTERNAL_ERROR;
                }
                // Choose the PICC with the bit set.
                currentLevelKnownBits = collisionPos;
                count = currentLevelKnownBits % 8; // The bit to modify
                checkBit = (currentLevelKnownBits - 1) % 8;
                index = 1 + (currentLevelKnownBits / 8) + (count ? 1 : 0); // First byte is index 0.
                buffer[index] |= (1u << checkBit);
            } else if (result != STATUS_OK) {
                return result;
            } else { // STATUS_OK
                if (currentLevelKnownBits >= 32) { // This was a SELECT.
                    selectDone = 1; // No more anticollision
                    // We continue below outside the while.
                } else { // This was an ANTICOLLISION.
                    // We now have all 32 bits of the UID in this Cascade Level
                    currentLevelKnownBits = 32;
                    // Run loop again to do the SELECT.
                }
            }
        } while (!selectDone);

        // We do not check the CBB - it was constructed by us above.

        // Copy the found UID bytes from buffer[] to uid->uidByte[]
        index = (buffer[2] == PICC_CMD_CT) ? 3 : 2; // source index in buffer[]
        bytesToCopy = (buffer[2] == PICC_CMD_CT) ? 3 : 4;
        for (count = 0; count < bytesToCopy; count++) {
            uid->uidByte[uidIndex + count] = buffer[index++];
        }

        // Check response SAK (Select Acknowledge)
        if (responseLength != 3 || txLastBits != 0) { // SAK must be exactly 24 bits (1 byte + CRC_A).
            return STATUS_ERROR;
        }

        // Verify CRC_A - do our own calculation and store the control in buffer[2..3] - those bytes are not needed anymore.
        result = PCD_CalculateCRC(responseBuffer, 1, &buffer[2]);
        if (result != STATUS_OK) {
            return result;
        }
        if ((buffer[2] != responseBuffer[1]) || (buffer[3] != responseBuffer[2])) {
            return STATUS_CRC_WRONG;
        }
        if (responseBuffer[0] & 0x04u) { // Cascade bit set - UID not complete yes
            cascadeLevel++;
        } else {
            uidComplete = 1;
            uid->sak = responseBuffer[0];
        }
    }while(!uidComplete); // End of while (!uidComplete)

    // Set correct uid->size
    uid->size = 3 * cascadeLevel + 1;

    return STATUS_OK;
}

int8_t PICC_ReadCardSerial(Uid* uid){
	return (PICC_Select(uid) == STATUS_OK);
}

inline void PCD_InitInterrupt(){
    PD_DDR &= ~SHIFTL8(MFRC522_IRQ_PIN);    //input
    PD_CR1 |=  SHIFTL8(MFRC522_IRQ_PIN);    //pullup
    PD_CR2  =  SHIFTL8(MFRC522_IRQ_PIN);    //enable external interrupt

    EXTI_CR1 |= 0b10000000; //<=+
    EXTI_CR1 &= 0b10111111; //<== interrupt on falling edge only
    EXTI_CR2 &= 0b11111011; //<== interrupt sensitivity on falling edge

    PCD_WriteRegister(ComIEnReg, 0xA0u); //enable interrupt

    __asm__("rim");
}

inline void PCD_ClearInterrupt(){
    PCD_WriteRegister(ComIrqReg, 0x7Fu);
}

inline void PCD_InterruptReactivateReception(){
    PCD_WriteRegister(FIFODataReg, PICC_CMD_REQA);
    PCD_WriteRegister(CommandReg, PCD_Transceive);
    PCD_WriteRegister(BitFramingReg, 0x87u);
}