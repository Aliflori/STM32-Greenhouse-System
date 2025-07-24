#ifndef __25LC512_H
#define __25LC512_H

#include "stm32f1xx_hal.h"
#include "spi.h"
#include "gpio.h"

// EEPROM Definitions
#define MC25LCxxx_SPI_READ      0x03
#define MC25LCxxx_SPI_WRITE     0x02
#define MC25LCxxx_SPI_WREN      0x06
#define MC25LCxxx_SPI_WRDI      0x04
#define MC25LCxxx_SPI_RDSR      0x05
#define MC25LCxxx_SPI_WRSR      0x01
#define MC25LCxxx_SPI_PE        0x42
#define MC25LCxxx_SPI_SE        0xD8
#define MC25LCxxx_SPI_CE        0xC7
#define MC25LCxxx_SPI_RDID      0xAB
#define MC25LCxxx_SPI_DPD       0xB9

#define MC25LCxxx_STATUS_WPEN   0x80
#define MC25LCxxx_STATUS_BP1    0x08
#define MC25LCxxx_STATUS_BP0    0x04
#define MC25LCxxx_STATUS_WEL    0x02
#define MC25LCxxx_STATUS_WIP    0x01

#define EEPROM_Enable           1
#define EEPROM_Disable          0
#define EEPROM_CS_PIN_SET       1
#define EEPROM_CS_PIN_RESET     0

typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} EEPROM_HandleTypeDef;

void EEPROM_Init(EEPROM_HandleTypeDef *eeprom, SPI_HandleTypeDef *hspi, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void EEPROM_CS(EEPROM_HandleTypeDef *eeprom, uint8_t CS_Status);
void EEPROM_WriteEnableOrDisable(EEPROM_HandleTypeDef *eeprom, uint8_t EnableOrDisable);
uint8_t EEPROM_ReleaseDeepPowerDownMode(EEPROM_HandleTypeDef *eeprom);
uint8_t EEPROM_ReadStatusRegister(EEPROM_HandleTypeDef *eeprom);
void EEPROM_Write(EEPROM_HandleTypeDef *eeprom, uint16_t Address, uint8_t *Data, uint16_t Size);
void EEPROM_Read(EEPROM_HandleTypeDef *eeprom, uint16_t Address, uint8_t *Data, uint16_t Size);

#endif /* __25LC512_H */
