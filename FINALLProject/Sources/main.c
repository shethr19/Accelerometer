#include <hidef.h>
#include "derivative.h"
#include "SCI.h" //allows us to use serial communication

char string[20];
unsigned short val;
int counter = 0;
float x;  //angle
int xRounded;
 
void setClk();
void delay1ms(int multiple);

void BCD (int bcdVal);
void BCDBar (int bcdVal);


void OutCRLF(void)
{
  SCI_OutChar(CR);
  SCI_OutChar(LF);
}

void main(void) 
{

    TSCR1 = 0x90;
    TSCR2 = 0x04;
    TIOS = 0xFC;
    
    PERT = 0x03;
    
    TCTL3 = 0x00;
    TCTL4 = 0x0A;
    TIE = 0x01;
    IRQCR = 0x00;
    EnableInterrupts;
    

  //configure ADC channel
    ATDCTL1 = 0x4F; //configure for 12 bit resolution
    ATDCTL3 = 0x88; //right justified
    ATDCTL4 = 0x02; //prescaler set to 1  6MHz/(2*(2+1)) = 1Mhz
    ATDCTL5 = 0x26; //continous conversion on channel 6
    setClk(); //set the E-Clock speed
    
    SCI_Init(9600); //initialize serial port
    
    
    DDRJ = 0xFF; 
    DDRP = 0xFF;
    DDRT = 0x01;
    
    //Configure PAD0-PAD7 Pins
    DDR1AD = 0x0F; //0x00; //driving the row with output pins using PT1AD
    PER1AD = 0x0F;
    //PERP = 0xFF;

    //test serial communication between ESDX and PC
    SCI_OutString("Connection established.");
    SCI_OutChar(CR);
       
    while(1)                    
    {      
       if(counter%3 == 2)  //mode 0 will output the degree in decimal and the LEDs in binary
  	    {
  	     
          val= ATDDR0; //stores digital value
  	      
  	        	      
  	      //output angle using appropriate linear equation
  	      if(val < 2211) 
  	      {
  	        x =  0.1437*val - 297.78;
  	        if (val < 2068){
  	          x = 0; 
  	        }
  	      } 
  	      else if (val >= 2212 && val <= 2476)
  	      {
  	        x = 0.213*val - 455.12;

  	      } 
  	      else if (val >= 2213 && val <3000)
  	      {
  	       x = 1.6071*val - 3901.3;
  	       if(x>90){
  	         x = 90;
  	       }
  	      }
  	      
  	      	      
  	      SCI_OutUDec(x);
  	      //SCI_OutUDec(val);
  	      BCD(x);
  	      delay1ms(750);
  	      SCI_OutChar(CR);
  	      //delay1ms(250);
  	      PTJ= 0x01;
  	    
  	   } 
  	   else if (counter%3 == 1) //mode 1 will show the LED as bars for degrees in 10s
  	    {
  	      
  	      val= ATDDR0; //stores digital value
  	      
  	      //output angle using appropriate linear equation
  	      if(val < 2211) 
  	      {
  	        x =  0.1437*val - 297.78;
  	        if (val < 2068){
  	          x = 0; 
  	        }
  	      } 
  	      else if (val >= 2212 && val <= 2476)
  	      {
  	        x = 0.213*val - 455.12;
  	      } 
  	      else if (val >= 2213 && val <3000)
  	      {
  	       x = 1.6071*val - 3901.3;
  	       if(x>90)
  	       {
  	         x = 90;
  	       }
  	       
  	      }
  	      
  	      
  	      xRounded = x/10;
  	      SCI_OutUDec(x);
  	      BCDBar(xRounded);
  	      delay1ms(750);
  	      SCI_OutChar(CR);
  	      //delay1ms(250);
  	      PTJ= 0x01;
  	    } 
  	    else
  	    {
  	       PTJ = 0x00;  
  	    }
  	    
      }
      
  }

interrupt VectorNumber_Vtimch0 void ISR_Vtimch0(void)
{
   unsigned int temp;
   counter = counter + 1;
   PTJ= 0x01;
   temp = TC0;  
}

//Bus Speed
void setClk(void)
{
  CPMUCLKS = 0x80; // choose PLL = 1
  CPMUOSC = 0x00; // source clock chosen
  CPMUPROT = 0x26; // protection of clock config is enable
  // now frequency_ref = 1MHz
  CPMUSYNR = 0x17; // VCOFRQ = 00, SYNDIN = 32 = F; 48MHz = VCOCLK = 2*1MHz*(1+SYNDIV);
  CPMUFLG = 0x00; // LOCK = 0
  // PLLCLK = 48MHz/4 = 112MHz and BUSCLK = PLLCLK/2 = 6MHz 
}

//Delay Function
void delay1ms(int multiple)
{
      int ix;
      TSCR1 = 0x90;   /* enable timer and fast timer flag clear */
      TSCR2 = 0x00;   /* disable timer interrupt, set prescaler to 1*/
      TIOS = 0x02;   /* enable OC0 */        //(not necessary)
      TC0 = TCNT + 8000;     
      for(ix = 0; ix < multiple; ix++) {
             while(!(TFLG1_C1F));
             TC1 += 8000;
      }
}

void BCD(int bcdVal)
{
    
    unsigned char ones;
    unsigned int tens;
    unsigned int hund;
    unsigned int temp;
    
    
    temp = bcdVal; 
    //hund = temp/100; 
    temp = temp%100;   
    tens = temp/10;    
    ones = temp%10;    
    
    
    PT1AD = ones;  //lower bits
    //PTP = 0b00000110;  //upper bits
    
    if(tens == 1) 
   {
      PTP = 0b00000010;
   } 
   else if(tens == 2)  
   {
      PTP = 0b00000100;
   }
   else if(tens == 3)  
   {
      PTP = 0b00000110;
   }
   else if(tens == 4)  
   {
      PTP = 0b00001000;
   }
   else if (tens == 5) 
   {
      PTP = 0b00001010;
   }
   else if(tens == 6)  
   {
      PTP = 0b00001100;
   }
   else if(tens == 7)  
   {
      PTP = 0b00001110;
   }
   else if(tens == 8)  
   {
      PTP = 0b00010000;
   }
   else if(tens == 9)  
   {
      PTP = 0b00010010;   
   }
   else 
   
   {
      PTP = 0b00000000;
   }
    
    
}

void BCDBar (int bcdVal) 
{
   
   if(bcdVal == 1) 
   {
      PT1AD = 0b00000001;
      PTP = 0b00000000;
   } 
   else if(bcdVal == 2)  
   {
      PT1AD = 0b00000011;
      PTP = 0b00000000;
   }
   else if(bcdVal == 3)  
   {
      PT1AD = 0b00000111;
      PTP = 0b00000000;
   }
   else if(bcdVal == 4)  
   {
      PT1AD = 0b00001111;
      PTP = 0b00000000;
   }
   else if (bcdVal == 5) 
   {
      PT1AD = 0b00001111;
      PTP = 0b00000010;
   }
   else if(bcdVal == 6)  
   {
      PT1AD = 0b00001111;
      PTP = 0b00000110;
   }
   else if(bcdVal == 7)  
   {
      PT1AD = 0b00001111;
      PTP = 0b00001110;
   }
   else if(bcdVal == 8)  
   {
      PT1AD = 0b00001111;
      PTP = 0b00011110;
   }
   else if(bcdVal == 9)  
   {
      PT1AD = 0b00001111;
      PTP = 0b00111110;
   }
   else 
   {
      PT1AD = 0x00;
      PTP = 0x00;
   }
    
}