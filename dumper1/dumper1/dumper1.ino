
#include <Wire.h>

#define ADDRESS 0x28

void dump()
{
 Wire.beginTransmission(ADDRESS);  
 Wire.write(0);
 Wire.endTransmission();
 
 for ( byte upper=0; upper<=1; upper++ )
 {
   for ( int i=0; i<32; i++)
   {
     Wire.beginTransmission(ADDRESS+upper);
     Wire.write(i*8);
     Wire.endTransmission();
     delay(100);

     Wire.beginTransmission(ADDRESS+upper);
     
     Wire.requestFrom(ADDRESS+upper, 8);
 
     //Wait till we get all the bytes
     while ( int x = Wire.available()<8 ) {Serial.print("nombre d'octets a lire : ") ; Serial.println(x);delay(10);};
     
     printInt(upper); Serial.print("-");
     printInt(i*8); Serial.print(":     ");
 
     for ( int j=0; j<8; j++ )
     {
       printInt(Wire.read()); Serial.print("     ");
     }
       
     Serial.println();  
     Wire.endTransmission();
   }
 }
}
void printInt(int i)
{
 Serial.print("0x");
 
 if ( i<16 )
   Serial.print("0");
   
 Serial.print(i, HEX);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dump();

}

void loop() {
  // put your main code here, to run repeatedly:

}
