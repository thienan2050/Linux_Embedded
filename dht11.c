uint16_t ReadDHT(uint8_t *integralRH,uint8_t *decimalRH,uint8_t *integralT,uint8_t *decimalT)
{
    uint32_t tick,m,n = 0;
    uint8_t rawData[40],checksum;
    /*Set PE4 as output.*/
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);
    /*MCU send out start signal at least 18ms.*/
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4,0x00);
    /*Delay 18ms ...*/
    delay_us(18000);
    /*MCU pulls up 20-40us.*/
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4,GPIO_PIN_4);
    /*Delay 40us.*/
    delay_us(40);
    /*Set PE4 as input to read data.*/
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_4);
    
    /*get the current tick.*/
    tick = ui32g_usec;
    /*Wait DHT11 responses low voltage.*/
    while(readPin() == 0x00)
        {
            /*if wait over 80us, return 0.*/
            if((ui32g_usec - tick) > 80)
                return 0;
        }
        /*get the current tick.*/
        tick = ui32g_usec;
        /*Wait DHT11 responses high voltage.*/
        while(readPin() == GPIO_PIN_4)
        {
            /*if wait over 80us, return 0.*/
            if((ui32g_usec - tick) > 80)
                return 0;
        }
        /*Start receiving data.*/
        while(n < 40)
            {
                tick = ui32g_usec;
                /*Wait 50us, low voltage from DHT- The indicator of next bit.*/
                while(readPin() == 0x00)
                {
                    if((ui32g_usec - tick) > 50)
                        return 0;
                }
                /*Delay 29us:
                 *  + Data "0" will last for 26-28us.
                 *  + Data "1" will last for 70us.
                 */
                delay_us(29);
                if(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_4) == 0)
                    /*After 29us, low level means received data is "0".*/
                    rawData[n] = 0;
                else
                    /*After 29us, is is still high level means received data is "1".*/
                {
                    rawData[n] = 1;
                    tick = ui32g_usec;
                    /*We must wait (70 - 29).*/
                    while(readPin() == GPIO_PIN_4)
                    {
                        if((ui32g_usec - tick) > 35)
                            continue;
                    }
                }
                n++;
            }
        uint8_t RH = 0,T = 0,subRH = 0,subT = 0;
            for(m = 0;m < 8;m++)
            {
                /*The integral RH of DHT11.*/
                RH |= rawData[m] << (7-m);
                /*The decimal RH of DHT11.*/
                subRH |= rawData[m + 8] << (7-m);
                /*The integral temperature of DHT11.*/
                T |= rawData[m + 16] << (7-m);
                /*The decimal temperature of DHT11.*/
                subT |= rawData[m + 24] << (7 - m);
                /*Check sum data.*/
                checksum |= rawData[m + 32] << (7 - m);
            }
            if(checksum != (RH + T + subRH + subT))
                return 0;
            else
            {
                *integralT = T;
                *integralRH = RH;
                *decimalT = subT;
                *decimalRH = subRH;
            }
            return 1;
}
