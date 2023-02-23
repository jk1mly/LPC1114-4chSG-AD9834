#include "mbed.h"

BufferedSerial uart0(P1_7, P1_6,115200);  //TX, RX
SPI spi(P0_9, P0_8, P0_6);    //mosi, miso, sclk
DigitalOut LE1(P1_4);   //dds1
DigitalOut LE2(P1_1);
DigitalOut LE3(P0_2);
DigitalOut LE4(P0_7);
DigitalOut CS1(P1_5);   //dac1
DigitalOut CS2(P1_2);
DigitalOut CS3(P1_9);
DigitalOut CS4(P0_3);
DigitalOut SYNC1(P0_4);
DigitalOut SYNC2(P0_5);

//cs control func.
void cs_hi(uint8_t num);    //dac
void cs_lo(uint8_t num);
void le_hi(uint8_t num);    //dds
void le_lo(uint8_t num);

//uart read buf
const uint8_t buf_size=15;
void buf_read(uint8_t num); //uart read func.
char read_buf[buf_size];    //uart read buf
void buf2val();             //buf to vals change func. return to 'freq' and 'pha' global var 
uint32_t freq;              //Hz
uint16_t pha,ampl;          //deg. loaded mV

//DDS control
#define res_inv 4           //res=67108864/2^28
uint8_t i;
void waveset(uint8_t ch, uint32_t freq, uint16_t pha, uint16_t ampl);    //waveset func.

//DAC control
#define dac_fs 2500     //DAC full scale Vout
#define dac_res 4096    //dac resolution 2^12
#define g 7             //driver amp gain

int main(){
    for(i=1;i<=4;++i) cs_hi(i); //CS init
    for(i=1;i<=4;++i) le_hi(i); //LE init
    spi.format(16,2);   //spi mode setting. 2byte transfer, mode 2
    SYNC1=0;
    SYNC2=0;
    while(true) {
        for(i=1;i<=4;++i){
            buf_read(buf_size);//uart buf read
            buf2val();
            waveset(i,freq,pha,ampl);
        }
        
        for(i=1;i<=4;++i) le_lo(i);
        spi.write(0x2000);      //accum. reset
        for(i=1;i<=4;++i) le_hi(i);

        SYNC1=!SYNC1;
        SYNC2=!SYNC2;
    }
}

//uart char number read func.
void buf_read(uint8_t num){
    char local_buf[1];
    uint8_t i;
    for (i=0;i<num;++i){
        uart0.read(local_buf,1);
        read_buf[i]=local_buf[0];
    }
}

//buf to val change func.
void buf2val(){
    uint8_t i,j;
    uint32_t pow10;
    freq=0;
    pha=0;
    ampl=0;
    for(i=0;i<8;++i){
        pow10=1;
        for(j=0;j<7-i;++j){
            pow10=10*pow10;
        }
        freq=freq+(read_buf[i]-48)*pow10;
    }
    for(i=0;i<3;++i){
        pow10=1;
        for(j=0;j<2-i;++j){
            pow10=10*pow10;
        }
        pha=pha+(read_buf[i+8]-48)*pow10;
    }
    for(i=0;i<4;++i){
        pow10=1;
        for(j=0;j<3-i;++j){
            pow10=10*pow10;
        }
        ampl=ampl+(read_buf[i+11]-48)*pow10;
    }
}

//cs control func.
void cs_hi(uint8_t num){
    if(num==1) CS1=1;
    else if(num==2) CS2=1;
    else if(num==3) CS3=1;
    else if(num==4) CS4=1;
}
void cs_lo(uint8_t num){
    if(num==1) CS1=0;
    else if(num==2) CS2=0;
    else if(num==3) CS3=0;
    else if(num==4) CS4=0;
}
void le_hi(uint8_t num){
    if(num==1) LE1=1;
    else if(num==2) LE2=1;
    else if(num==3) LE3=1;
    else if(num==4) LE4=1;
}
void le_lo(uint8_t num){
    if(num==1) LE1=0;
    else if(num==2) LE2=0;
    else if(num==3) LE3=0;
    else if(num==4) LE4=0;
}

//wave set func.
void waveset(uint8_t ch, uint32_t freq, uint16_t pha, uint16_t ampl){
    uint16_t buf;
    if(freq>30000000)freq=30000000;
    if(pha>360)pha=360;
    if(ampl>2100)ampl=2100;

    le_lo(ch);
    spi.write(0x2100);
    buf=((res_inv*freq)&0x3FFF)+0x4000;
    spi.write(buf);
    buf=((res_inv*freq)>>14)+0x4000;
    spi.write(buf);
    buf=(4096*pha/360)+0xC000;
    spi.write(buf);
    le_hi(ch);

    cs_lo(ch);
    buf=((1200-4*ampl/g)*dac_res/dac_fs)<<2;    //(1/res)*(1200/3)*(3-ampl*2/(200*g))
    spi.write(buf);
    cs_hi(ch);
}