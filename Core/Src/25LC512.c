#include "25LC512.h"

void EEPROM_Init(EEPROM_HandleTypeDef *eeprom, SPI_HandleTypeDef *hspi, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    eeprom->hspi = hspi;
    eeprom->GPIOx = GPIOx;
    eeprom->GPIO_Pin = GPIO_Pin;
    EEPROM_CS(eeprom, EEPROM_CS_PIN_SET);
    EEPROM_ReleaseDeepPowerDownMode(eeprom);
    EEPROM_ReadStatusRegister(eeprom);
    EEPROM_WriteEnableOrDisable(eeprom, EEPROM_Enable);
}

void EEPROM_CS(EEPROM_HandleTypeDef *eeprom, uint8_t CS_Status) {
    if (CS_Status == EEPROM_CS_PIN_RESET) {
        HAL_GPIO_WritePin(eeprom->GPIOx, eeprom->GPIO_Pin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(eeprom->GPIOx, eeprom->GPIO_Pin, GPIO_PIN_SET);
    }
    HAL_Delay(5);
}

void EEPROM_WriteEnableOrDisable(EEPROM_HandleTypeDef *eeprom, uint8_t EnableOrDisable) {
    uint8_t command = (EnableOrDisable == EEPROM_Enable) ? MC25LCxxx_SPI_WREN : MC25LCxxx_SPI_WRDI;
    EEPROM_CS(eeprom, EEPROM_CS_PIN_RESET);
    HAL_SPI_Transmit(eeprom->hspi, &command, 1, HAL_MAX_DELAY);
    EEPROM_CS(eeprom, EEPROM_CS_PIN_SET);
}

uint8_t EEPROM_ReleaseDeepPowerDownMode(EEPROM_HandleTypeDef *eeprom) {
    uint8_t command = MC25LCxxx_SPI_RDID;
    uint8_t response;
    EEPROM_CS(eeprom, EEPROM_CS_PIN_RESET);
    HAL_SPI_Transmit(eeprom->hspi, &command, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(eeprom->hspi, &response, 1, HAL_MAX_DELAY);
    EEPROM_CS(eeprom, EEPROM_CS_PIN_SET);
    return response;
}

uint8_t EEPROM_ReadStatusRegister(EEPROM_HandleTypeDef *eeprom) {
    uint8_t command = MC25LCxxx_SPI_RDSR;
    uint8_t status;
    EEPROM_CS(eeprom, EEPROM_CS_PIN_RESET);
    HAL_SPI_Transmit(eeprom->hspi, &command, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(eeprom->hspi, &status, 1, HAL_MAX_DELAY);
    EEPROM_CS(eeprom, EEPROM_CS_PIN_SET);
    return status;
}

void EEPROM_Write(EEPROM_HandleTypeDef *eeprom, uint16_t Address, uint8_t *Data, uint16_t Size) {
    uint8_t command = MC25LCxxx_SPI_WRITE;
    uint8_t addressHigh = (Address >> 8) & 0xFF;
    uint8_t addressLow = Address & 0xFF;

    EEPROM_CS(eeprom, EEPROM_CS_PIN_RESET);
    HAL_SPI_Transmit(eeprom->hspi, &command, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(eeprom->hspi, &addressHigh, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(eeprom->hspi, &addressLow, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(eeprom->hspi, Data, Size, HAL_MAX_DELAY);
    EEPROM_CS(eeprom, EEPROM_CS_PIN_SET);
    HAL_Delay(5);
}

void EEPROM_Read(EEPROM_HandleTypeDef *eeprom, uint16_t Address, uint8_t *Data, uint16_t Size) {
    uint8_t command = MC25LCxxx_SPI_READ;
    uint8_t addressHigh = (Address >> 8) & 0xFF;
    uint8_t addressLow = Address & 0xFF;

    EEPROM_CS(eeprom, EEPROM_CS_PIN_RESET);
    HAL_SPI_Transmit(eeprom->hspi, &command, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(eeprom->hspi, &addressHigh, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(eeprom->hspi, &addressLow, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(eeprom->hspi, Data, Size, HAL_MAX_DELAY);
    EEPROM_CS(eeprom, EEPROM_CS_PIN_SET);
}
