#include "AD9834AmplitudeControl.h"

// MASK for AD5640-1
#define MODE_NORMAL_MASK   0x00
#define MODE_PWD_1K_MASK   0x01
#define MODE_PWD_100K_MASK 0x02
#define MODE_PWD_NC_MASK   0x03

// Acquisition preprocessor's directives
#define ACQ_DELAY 3

const uint8_t DDS_FSYNC_PIN     = 21;
const uint8_t DAC_FSYNC_PIN     = 17;

// Constants for AD9834
const uint8_t RES_AD9834        = 28;
const uint32_t TWOPOWRES_AD9834 = pow(2, RES_AD9834);
const uint32_t MCLK             = 27000000; // 27 MHz
const double DDS_FULLSCALE      = 0.624;
const double DDS_MIDSCALE       = DDS_FULLSCALE / 2;

// Constants for AD5640-1
const double VREFOUT            = 1.25;
const uint8_t RES_AD5640        = 14;
const uint32_t TWOPOWRES_AD5640 = pow(2, RES_AD5640);
const double DAC_FULLSCALE      = 1.2;
const double DAC_MIDSCALE       = DAC_FULLSCALE / 2;

uint8_t registers[NUM_REGISTERS];

SPISettings adcSettings(125000, MSBFIRST, SPI_MODE1);
SPISettings dacSettings(125000, MSBFIRST, SPI_MODE2);
SPISettings ddsSettings(125000, MSBFIRST, SPI_MODE2);


void SPI_INIT(void)
{
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(SCK_PIN, OUTPUT);
  pinMode(DDS_FSYNC_PIN, OUTPUT);
  pinMode(DAC_FSYNC_PIN, OUTPUT);
  pinMode(ADC_EN_PIN, OUTPUT);
  pinMode(START_PIN, OUTPUT);
  pinMode(DRDY_PIN, INPUT);

  digitalWrite(START_PIN, LOW);
  digitalWrite(ADC_EN_PIN, HIGH);
  digitalWrite(DDS_FSYNC_PIN, HIGH);
  digitalWrite(DAC_FSYNC_PIN, HIGH);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);
}

void setDDS(const double A, const double F, const double P)
{
  // FREQUENCY & PHASE CONFIGURATION
  uint32_t frequencyReg = (F * TWOPOWRES_AD9834) / MCLK;
  uint16_t freqReg_lsb = 0x4000 | (frequencyReg & 0x3FFF);
  uint16_t freqReg_msb = 0x4000 | (frequencyReg >> 14);

  uint16_t phaseReg = (P * 4096) / (2 * PI);
  phaseReg = 0xC0 | phaseReg;

  SPI.beginTransaction(SPISettings(125000, MSBFIRST, SPI_MODE2));
  digitalWrite(DDS_FSYNC_PIN, LOW);
  
  // RESET CODE TRANSFER
  SPI.transfer(0x21);
  SPI.transfer(0x00);

  // FREQUENCY LSB TRANSFER
  SPI.transfer(freqReg_lsb >> 8);
  SPI.transfer(freqReg_lsb & 0xFF);

  // FREQUENCY MSB TRANSFER
  SPI.transfer(freqReg_msb >> 8);
  SPI.transfer(freqReg_msb & 0xFF);

  // PHASE REGISTER TRANSFER
  SPI.transfer(phaseReg >> 8);
  SPI.transfer(phaseReg & 0xFF);

  // NO RESET CODE TRANSFER
  SPI.transfer(0x20);
  SPI.transfer(0x00);
  
  digitalWrite(DDS_FSYNC_PIN, HIGH);
  SPI.endTransaction();

  delay(10);

  // AMPLITUDE CONFIGURATION
  double reversedDacAmp = (A * DAC_MIDSCALE) / DDS_MIDSCALE;
  setAmplitude(DAC_FULLSCALE - reversedDacAmp);
}

void setAmplitude(const double A)
{
  uint16_t dacValue = (A * TWOPOWRES_AD5640) / (2 * VREFOUT);
  dacValue =  dacValue | (MODE_NORMAL_MASK << RES_AD5640);

  SPI.beginTransaction(SPISettings(125000, MSBFIRST, SPI_MODE2));
  digitalWrite(DAC_FSYNC_PIN, LOW);
  
  SPI.transfer(dacValue >> 8); // MSB First !
  SPI.transfer(dacValue & 0xFF);
  
  digitalWrite(DAC_FSYNC_PIN, HIGH);
  SPI.endTransaction();
}

void ADC_INIT(void)
{
  adcSendCommand(RESET_OPCODE_MASK);
  delay(100);

  adcSendCommand(SFOCAL_OPCODE_MASK);
  adcSendCommand(SYOCAL_OPCODE_MASK);
  adcSendCommand(SYGCAL_OPCODE_MASK);
  delay(100);

  adcSendCommand(WAKE_OPCODE_MASK);
  
  adcRegWrite(INPMUX_ADDR_MASK, ADS_P_AIN0 + ADS_N_AINCOM);
  adcRegWrite(DATARATE_ADDR_MASK, ADS_DR_4000);
  adcRegWrite(REF_ADDR_MASK, ADS_REFINT_ON_ALWAYS + ADS_REFSEL_INT);
  adcRegMap();
}

void adcSendCommand(uint8_t CMD)
{
  SPI.beginTransaction(SPISettings(ADC_SPI_SPEED, MSBFIRST, ADC_SPI_MODE));
  digitalWrite(ADC_EN_PIN, LOW);
  
  SPI.transfer(CMD);
  
  digitalWrite(ADC_EN_PIN, HIGH);
  SPI.endTransaction();
}

void adcRegWrite(const unsigned int REGNUM, const unsigned char D)
{
  uint8_t ulDataTx[3];
  ulDataTx[0] = REGWR_OPCODE_MASK + (REGNUM & 0x1f);
  ulDataTx[1] = 0x00;
  ulDataTx[2] = D;

  SPI.beginTransaction(SPISettings(ADC_SPI_SPEED, MSBFIRST, ADC_SPI_MODE));
  digitalWrite(ADC_EN_PIN, LOW);
  
  SPI.transfer(ulDataTx[0]);
  SPI.transfer(ulDataTx[1]);
  SPI.transfer(ulDataTx[2]);
  
  digitalWrite(ADC_EN_PIN, HIGH);
  SPI.endTransaction();
}

void adcRegMap(void)
{
  uint8_t regList[NUM_REGISTERS];
  adcReadRegs(0, NUM_REGISTERS, regList);
  Serial.println("Register Contents");
  Serial.println("---------------------");

  for(int index=0; index < NUM_REGISTERS ; index++)
  {
    Serial.print("Register 0x");
    Serial.print(index);
    Serial.print(" = 0x");
    Serial.println(regList[index]);
  }
}

void adcReadRegs(unsigned int regnum, unsigned int count, uint8_t *data)
{
  uint8_t ulDataTx[2];
  ulDataTx[0] = REGRD_OPCODE_MASK + (regnum & 0x1f);
  ulDataTx[1] = count - 1;

  SPI.beginTransaction(SPISettings(ADC_SPI_SPEED, MSBFIRST, ADC_SPI_MODE));
  digitalWrite(ADC_EN_PIN, LOW);
  
  SPI.transfer(ulDataTx[0]);
  SPI.transfer(ulDataTx[1]);

  for(int i = 0; i < count; i++)
  {
    data[i] = SPI.transfer(0);
    if(regnum + i < NUM_REGISTERS)
    {
      registers[regnum + i] = data[i];
    }
  }
  
  digitalWrite(ADC_EN_PIN, HIGH);
  SPI.endTransaction();
}

long adcDataRead(uint8_t *dStatus, uint8_t *dData, uint8_t *dCRC)
{
  uint8_t xcrc;
  uint8_t xstatus;
  long iData;

  
  SPI.beginTransaction(SPISettings(ADC_SPI_SPEED, MSBFIRST, ADC_SPI_MODE));
  digitalWrite(ADC_EN_PIN, LOW);
  
  if((registers[SYS_ADDR_MASK] & 0x01) == DATA_MODE_STATUS)
  {
    xstatus = SPI.transfer(0x00);
    dStatus[0] = (uint8_t)xstatus;
  }

  // get the conversion data (3 bytes)
  uint8_t data[3];
  data[0] = SPI.transfer(0x00);
  data[1] = SPI.transfer(0x00);
  data[2] = SPI.transfer(0x00);

  iData = data[0];
  iData = (iData<<8) + data[1];
  iData = (iData<<8) + data[2];
  if((registers[SYS_ADDR_MASK] & 0x02) == DATA_MODE_CRC)
  {
    xcrc = SPI.transfer(0x00);
    dCRC[0] = (uint8_t)xcrc;
  }
  
  digitalWrite(ADC_EN_PIN, HIGH);
  SPI.endTransaction();
  
  return iData ;
}

char *convertDoubleToString(double value)
{
  String str = String(value);
  int stringLength = str.length();
  char outString[stringLength + 1];
  
  strcpy(outString, str.c_str());

  return outString;
}

char *concatenateValues(double voltage,double current)
{
  String voltageStr = String(voltage);
  String currentStr = String(current);

  String concatenateStr = voltageStr + currentStr;

  int stringLength = concatenateStr.length();
  char outString[stringLength + 1];
  
  strcpy(outString, concatenateStr.c_str());

  return outString;
}