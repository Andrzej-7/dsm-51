#include <8051.h>

    __xdata unsigned char * key_addr = (__xdata unsigned char *) 0xFF22;//klaw matr
    __xdata unsigned char * LCDWC = (__xdata unsigned char *) 0xFF80; // write cmd
    __xdata unsigned char * LCDWD = (__xdata unsigned char *) 0xFF81; // write data
    __xdata unsigned char * LCDRC = (__xdata unsigned char *) 0xFF82; // read cmd
    __xdata unsigned char * wyborSegmentu = (__xdata unsigned char *) 0xFF30;
    __xdata unsigned char * wyborZnaku = (__xdata unsigned char *) 0xFF38;
    __bit __at (0x96) zasilanieWyswietlacza;
    __bit __at (0xB5) odczytZKlawiatury;
    __bit __at (0x97) TEST;
    __bit __at (0x95) BUZZ;


    
     __xdata unsigned char * CS55B = (__xdata unsigned char *) 0xFF29; //wyjsc b
     __xdata unsigned char * CS55D = (__xdata unsigned char *) 0xFF2B; // zarzadza portem b

    __bit t0_flag;
    __bit __at (0x97) DLED; // bit 7 portu P1 sterujacy diode LED

    unsigned int TH0_LOW; //pojemnosc starszego bajtu stanu niskiego
    unsigned int TL0_LOW; //pojemnosc mlodszego bajtu stanu niskiego
    unsigned int TH0_HIGH; //pojemnosc starszego bajtu stanu wysokiego
    unsigned int TL0_HIGH; //pojemnosc starszego bajtu stanu niskiego

    

    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char index = 0;
    unsigned char indexFor2 = 0;
    unsigned char licznikFORf = 0;
    unsigned char myNAvalue = 0;
    unsigned char bufer;
    unsigned char PWM_buf = 30;
    unsigned char diod_pointer;
    unsigned char counterT;
        
                                                                    //wskaznik na segm z diodami
    __code unsigned char segmenty[4] = {0b000001,0b000010,0b000100,0b1000000}; // wskazniki na segmenty wysw 7-gm.

    //                                          0                 1                    2               3                  4
    __code unsigned char MENU[][20] = {">-1 MAIN SCREEN ", " -2 SETTINGS    "," -1 MAIN SCREEN ",">-2 SETTINGS    ", "AndriiZiatkevych",
    //      5                6                  7                    8                    9
     "PWM VALUE    ", " -2.1 PWM       ", " -2.2 LED       ", ">-2.1 PWM       ", ">-2.2 LED       ",
    //    10                   11               12                      13                 14                  15
     " -2.3 OTHER     ", " -2.4 LOAD DEF  ", ">-2.3 OTHER     ", ">-2.4 LOAD DEF  ", "                ", ">-BUZZ  OFF     ",
    //      16                17                18                   19                   20                  21
     ">-BUZZ  ON      ",">- STATE     OFF",">- STATE     ON " , " - VALUE     " ,">- VALUE     "," - STATE     OFF",
    //     22                 23                24                 25                  26                   27
     " - F1 OFF       ", " - F2 OFF       "," - F3 OFF       "," - F4 OFF       ", " - OK OFF       "," - ER OFF       ",
    //      28               29                  30                31                  32                   33
     ">- F1 OFF       ", ">- F2 OFF       ",">- F3 OFF       ",">- F4 OFF       ", ">- OK OFF       ",">- ER OFF       ", " - STATE     ON  ", "- VALUE  "};


//                                           0                    1                 2                   3                    4                  5
    __code unsigned char Fmenu[][20] = { ">- F1        OFF", ">- F2        OFF",">- F3        OFF",">- F4        OFF", ">- OK        OFF",">- ER        OFF",
//         6                 7                   8                  9                  10                 11
    " - F1        OFF", " - F2        OFF", " - F3        OFF"," - F4        OFF", " - OK        OFF"," - ER        OFF"};
                                             


    void delay(){
    unsigned char i = 255;
    while(i > 0){
        i--;
    }
}

void lcd_wait_whl_busy(){// Funkcja oczekiwania na zwolnienie wyświetlacza LCD do wykonania polecenia
    while(*LCDRC & 0b10000000);
}

void lc_send_command(unsigned char command){
    *LCDWC = command; // Write the command to the LCD data register
}


// Function to initialize the LCD
void lcd_init(){
    lcd_wait_whl_busy();
    lc_send_command(0b00111000);// Set the LCD for 8-bit mode

    lcd_wait_whl_busy();
    lc_send_command(0b00001111);// Turn on display
    
    lcd_wait_whl_busy();
    lc_send_command(0b00000110); //Wymusisz odpowiednie zachowanie wyświetlacza w przypadku wpisania danej do DD_RAM:
//- zapisanie danej do DD_RAM spowoduje każdorazowe zwiększenie adresu DD_RAM o 1
//- zapisanie danej do DD_RAM nie będzie wiązało się z przemieszczaniem pola wyświetlania
    
    lcd_wait_whl_busy();
    lc_send_command(0b00000001);//Wyczyścisz wyświetlacz.
    
}


   unsigned char VALUE = 30;





void zmianaLCD(unsigned char choise){

        //"-1 MAIN SCREEN ","-2 SETTINGS<"
        if(choise == 1){
             index = 2;
             indexFor2 = 3;
        }
        //"-1 MAIN SCREEN<", "-2 SETTINGS "
        else if(choise == 2){
            index = 0;
            indexFor2 = 1;
        }
        //"AndriiZiatkevych", "PWM VALUE: 030<"
        else if (choise == 3){
            index = 4;
            indexFor2 = 5;
        }
        // "-2.1PWM<", "-2.2LED"
        else if (choise == 4){
            index = 8;
            indexFor2 = 7;
        }
        //"-2.1 PWM", "-2.2LED<"
        else if (choise == 5){
            index = 6;
            indexFor2 = 9;
        }
        //"-2.3 OTHER<", "-2.4LOAD DEF"
        else if (choise == 6){
            index = 12;
            indexFor2 = 11;
        }
        //"-2.3 OTHER", "-2.4LOAD DEF<"
        else if (choise == 7){
            index = 10;
            indexFor2 = 13;
        }
        //">-BUZZ  OFF     "
        else if (choise == 8){
            index = 15;
            indexFor2 = 14;
        }
        //">-BUZZ  ON      "
        else if (choise == 9){
            index = 16;
            indexFor2 = 14;
        }
        //">- STATE OFF", "- VALUE 030"
        else if (choise == 10){
            index = 17;
            indexFor2 = 19;
        }
        //"- STATE OFF", ">- VALUE 030"
        else if (choise == 11){
            index = 21;
            
            indexFor2 = 20;
        }
        // >STATE ON
        else if (choise == 18){
           index = 18;
           indexFor2 = 19;
        }

        else if (choise == 19){
            index = 34;
            indexFor2 = 20;
        }
        
        


    lc_send_command(0b00000001);
    delay();
    for(i = 0; i < 16; i++){
         *LCDWD = MENU[index][i];
    }


    // code for writing to the second line, regardless of the value of "choise"
    delay();
    *LCDWC = 0b11000000;

    for(i = 0; i < 16; i++){
        if ((choise == 3 || choise == 10 || choise == 11 || choise == 18 || choise == 19) && i==13){
            *LCDWD = (VALUE/100)%10+'0';
            delay();
            *LCDWD = (VALUE/10)%10+'0';
            delay();
            *LCDWD = VALUE%10+'0';
            break;
        }
        *LCDWD = MENU[indexFor2][i];
    }



    

    }
    
// //                                           0                    1                 2                   3                    4                  5
//     __code unsigned char Fmenu[][20] = { ">- F1        OFF", ">- F2        OFF",">- F3        OFF",">- F4        OFF", ">- OK        OFF",">- ER        OFF",
// //         6                 7                   8                  9                  10                 11
//     " - F1        OFF", " - F2        OFF", " - F3        OFF"," - F4        OFF", " - OK        OFF"," - ER        OFF"};



    __code unsigned char ON[][4] = {"ON "};


//zmienny dlia Led i icz bufory
__bit F1_1 = 0;
__bit F1_2 = 0;
__bit F2_1 = 0;
__bit F2_2 = 0;
__bit F3_1 = 0;
__bit F3_2 = 0;
__bit F4_1 = 0;
__bit F4_2 = 0;
__bit ER_1 = 0;
__bit ER_2 = 0;
__bit OK_1 = 0;
__bit OK_2 = 0;
 
    

void Fon(unsigned char choise){



        //">- F1 "," - F2 "
        if (choise == 12){
            index = 0;
            indexFor2 = 7;
        }
        //"- F1 ",">- F2 "
        else if (choise == 13){
            index = 6;
            indexFor2 = 1;
        }
        //">- F3 "," - F4 "
        else if (choise == 14){
           index = 2;
           indexFor2 = 9;
        }
        //"- F3 ",">- F4 "
        else if (choise == 15){
            index = 8;
            indexFor2 = 3;
        }
        // ">- OK ", " - ER "
        else if (choise == 16){
            index = 4;
            indexFor2 = 11;
        }
        // "- OK ", ">- ER "
        else if (choise == 17){
            index = 10;
            indexFor2 = 5;
        }


    lc_send_command(0b00000001);
    delay();

    for(i = 0; i < 16; i++){

       //sprawdzamu przy wyszwietleniu cz wlaczony led jezeli tal, to stawiamy ON
        if (choise >= 12 && i > 12){
            if((choise == 12 || choise == 13) && F1_1){
            for (j = 0; j < 4; j++){
            delay();
            *LCDWD = ON[0][j];
            }
            break;
            }

        
            if((choise == 14 || choise == 15) && F3_1){
            for (j = 0; j < 4; j++){
            delay();
            *LCDWD = ON[0][j];
            }
            break;
            }

            if((choise == 16 || choise == 17) && OK_1){
            for (j = 0; j < 4; j++){
            delay();
            *LCDWD = ON[0][j];
            }
            break;
            }


        

        }



        *LCDWD = Fmenu[index][i];
        }
    
    
    


    delay();
    *LCDWC = 0b11000000;

    for(i = 0; i < 16; i++){
        //sprawdzamu przy wyszwietleniu cz wlaczony led jezeli tal, to stawiamy ON
        if (choise >= 12 && i > 12){

            if((choise == 12 || choise == 13) && F2_1){
            for (j = 0; j < 4; j++){
            delay();
            *LCDWD = ON[0][j];
            }
            break;
            }

            if((choise == 14 || choise == 15) && F4_1){
            for (j = 0; j < 4; j++){
            delay();
            *LCDWD = ON[0][j];
            }
            break;
            }

            if((choise == 16 || choise == 17) && ER_1){
            for (j = 0; j < 4; j++){
            delay();
            *LCDWD = ON[0][j];
            }
            break;
            }


        }



    *LCDWD = Fmenu[indexFor2][i];
    }

}



void PWM(){
    

unsigned char units = VALUE % 10;
unsigned char tens = (VALUE / 10) % 10;
unsigned char hundreds = (VALUE / 100) % 10;

zasilanieWyswietlacza = 1;
*wyborSegmentu = segmenty[2];

switch (hundreds) {
    case 0: *wyborZnaku = 0b0111111; break; // 0
    case 1: *wyborZnaku = 0b0000110; break; // 1
    case 2: *wyborZnaku = 0b1011011; break; // 2
    case 3: *wyborZnaku = 0b1001111; break; // 3
    case 4: *wyborZnaku = 0b1100110; break; // 4
    case 5: *wyborZnaku = 0b1101101; break; // 5
    case 6: *wyborZnaku = 0b1111101; break; // 6
    case 7: *wyborZnaku = 0b0000111; break; // 7
    case 8: *wyborZnaku = 0b1111111; break; // 8
    case 9: *wyborZnaku = 0b1101111; break; // 9
    
}

zasilanieWyswietlacza = 0;
delay();
zasilanieWyswietlacza = 1;
*wyborSegmentu = segmenty[1];

switch (tens) {
    case 0: *wyborZnaku = 0b0111111; break; // 0
    case 1: *wyborZnaku = 0b0000110; break; // 1
    case 2: *wyborZnaku = 0b1011011; break; // 2
    case 3: *wyborZnaku = 0b1001111; break; // 3
    case 4: *wyborZnaku = 0b1100110; break; // 4
    case 5: *wyborZnaku = 0b1101101; break; // 5
    case 6: *wyborZnaku = 0b1111101; break; // 6
    case 7: *wyborZnaku = 0b0000111; break; // 7
    case 8: *wyborZnaku = 0b1111111; break; // 8
    case 9: *wyborZnaku = 0b1101111; break; // 9
}

zasilanieWyswietlacza = 0;
delay();
zasilanieWyswietlacza = 1;
*wyborSegmentu = segmenty[0];

switch (units) {
    case 0: *wyborZnaku = 0b0111111; break; // 0
    case 1: *wyborZnaku = 0b0000110; break; // 1
    case 2: *wyborZnaku = 0b1011011; break; // 2
    case 3: *wyborZnaku = 0b1001111; break; // 3
    case 4: *wyborZnaku = 0b1100110; break; // 4
    case 5: *wyborZnaku = 0b1101101; break; // 5
    case 6: *wyborZnaku = 0b1111101; break; // 6
    case 7: *wyborZnaku = 0b0000111; break; // 7
    case 8: *wyborZnaku = 0b1111111; break; // 8
    case 9: *wyborZnaku = 0b1101111; break; // 9
}

zasilanieWyswietlacza = 0;
 delay();
zasilanieWyswietlacza = 1;

*wyborSegmentu = segmenty[3];//wybieramy wskaznik na segmen diod
*wyborZnaku = diod_pointer;//wybrane diody
zasilanieWyswietlacza = 0;
    
}



//11,0592MHz = 11 059 200 Hz [cykli zegarowych na 1s]
/*Ponieważ na pojedynczy cykl maszynowy składa się 12 cykli zegarowych, to aby przy zadanej szybkości taktowania
mikrokontrolera przełożyć liczbę cykli zegarowych na liczbę cykli maszynowych musimy zwyczajnie podzielić tę
liczbę przez 12.*/
//11 059 200/12 = 921 600 [cykli maszynowych na 1s]
//50 Hz to 1000ms/50 = 20 ms
//921600/1000ms = 921,6 [cykli maszynowych na 1ms]
//921,6 * 20 = 18432 = 100%, 18=0.1%
void zmiana(unsigned char VALUE){
    TH0_LOW = (47104+(VALUE*18))/256;//reshtu
    TL0_LOW  = (47104+(VALUE*18))%256;
                            
    TH0_HIGH = (47104+18444-(VALUE*18))/256; //od 20ms*3% - 20ms*12%
    TL0_HIGH = (47104+18444-(VALUE*18))%256;//pojemnosc wysokiego stanu mlodszy bajt


}





void interupt(){
    t0_flag = !t0_flag;//flaga poprzedniego stanu(wysoki,niski), wykorzyskujemy dlia rozumieny w jakim stanie powinen pracowac uklad
    TF0 = 0;//flaga przepelnienia (65536 wartosc przepelnienia licznika)
    if(t0_flag){
    *(CS55B) = 0xFF;//wlaczamy wszystkie porty wyjscia B
    DLED = 0;
    
    TL0 = TL0_HIGH;//pojemnosc wysokiego stanu mlodszy bajt
    TH0 = TH0_HIGH;//pojemnosc wysokiego stanu starszy bajt
  }else{
    *(CS55B) = 0x00;//wylaczamy szystkie porty B
    DLED = 1;

    TL0 = TL0_LOW;//pojemnosc niskiego stanu mlodszy bajt
    TH0 = TH0_LOW;//pojemnosc niskiego stanu starszy bajt
   }
   

    }
    

//zmienna dla STATE on/OFF.
unsigned char Enableinterrupt = 0;

//Przerwanie od licznika T0
void t0_int( void ) __interrupt(1){
    
    if(Enableinterrupt == 1){
        interupt();
        counterT++;//Timer dlia liczenia sekund
        if (counterT >= 100)//jezeli timer 100 == 1000 ms, to odsylamy cyfry do transmisji
        {
            SBUF = VALUE / 100 + 48;//setki
            //while (!TI) {};
            //TI = 0;
            delay();
            SBUF = (VALUE / 10) % 10 + 48;//dziesietki
            // while (!TI) {};
            // TI = 0;
            delay();
            SBUF =  VALUE % 10 + 48;//jednostki
            // while (!TI) {};
            // TI = 0;
            delay();
            SBUF = ' ';//separator

            counterT = 0;//zerujemy timer
        }
         
       
   }

    }







unsigned int buffer = 0;
unsigned char char_count = 0;
const unsigned char max_char_count = 3;










//zmienne pomocnicze
unsigned char choise = 0;
__bit pressed = 0;
__bit myNaSettings = 0;
__bit myNaMainScreen = 0;
__bit myNaPWM = 0;
__bit myNaLED = 0;
__bit myNaOTHER = 0;
__bit myNaLOAD = 0;
__bit myNaBUZZ = 0;
__bit BUZZon = 0;
__bit myWpwm = 0;
__bit myNaVALUE= 0;
__bit myWled= 0;
__bit myWmainSCREEN = 0;
__bit mywklbuzz = 0;
__bit BUZZoff = 0;
__bit myNAstateOFF = 0;
__bit ENTERstate = 0; //enter dla state off/on
__bit stanSTATE = 0; // on/off 0/1
__bit stanSTATEB = 0; // state bufff
__bit myWyjszlyzPWm = 0;
__bit LEDstatus = 0;









 unsigned char key;

 void POINTER_CONBINE(){
    if (F1_1)
    {
       diod_pointer |= 0b000001;//jezeli led wlaczony wykonujemy operacje ALBO z wybranymi diodama co w wyniku dodaje ten bait do diod_pointer
    }
    else{
        diod_pointer &= 0b111110;//jezeli led wylaczony wykonujemy oprecje I z wybranymi diodami co w wyniku kasuje wlaczona diode
    }
    if (F2_1)
    {
        diod_pointer |= 0b000010;
    }
    else{
        diod_pointer &= 0b111101;
    }
    if (F3_1)
    {
        diod_pointer |= 0b000100;
    }
    else{
        diod_pointer &= 0b111011;
    }
    if (F4_1)
    {
        diod_pointer |= 0b001000;
    }
    else{
        diod_pointer &= 0b110111;
    }
    if (OK_1)
    {
        diod_pointer |= 0b010000;
    }
    else{
        diod_pointer &= 0b101111;
    }
    if (ER_1)
    {
        diod_pointer |= 0b100000;
    }
    else{
        diod_pointer &= 0b011111;
    }
    
}




 
void main(){

//ustawienie rejestrów kontrolnych
//--------------------------------
  SCON = 0b01010000; //ustaw parametry transmisji
   //tryb 1: 8 bitów, szybkość: T1
  TMOD = 0b00100001; //ustaw T1 w tryb 2; T0 w tryb 1
  

 TL1 = 0xFD; //ustawienie młodszego
 TH1 = 0xFD; //i starszego bajtu T1 (19200)

//inne ustawienia
 //---------------

 t0_flag = 0; // zeruj flagę przerw. t0_int
 ET0 = 1; // aktywuj przerwanie od licznika T0
 ES = 1; // aktywuj przerwanie od transmisji
 EA = 1; // aktywuj wszystkie przerwania
 TR0 = 1; // uruchom licznik T0
 TR1 = 1; // uruchom licznik T1

*(CS55D) = 0b10000000; //mb wkl port



    lcd_init();
    //initymo start menu
    zmianaLCD(2);
    myNaMainScreen = 1;
    
    while(1){
        PWM();
        zmiana(VALUE);
      
        key = ~(*key_addr);


        
        


        //*********************obluga menuPWM(2.1)*******************
        // enter po PWM
        if((key & 0b10000000 ) && myNaPWM == 1){
            if(stanSTATE == 1){
                zmianaLCD(18);
            }else{
                 zmianaLCD(10);
            }

            pressed = 1;
            stanSTATEB = stanSTATE;
            PWM_buf = VALUE;
            myNAstateOFF = 1;
            myWpwm = 1;
            myNaPWM = 0;
        }
        
        //zatwierdzenie stanu PWM I STATE oraz zapamientowanie w razie kasowania
        else if ((key & 0b10000000 ) && myWpwm == 1){
            zmianaLCD(4);
            
            if (stanSTATE == 1)
            {
                Enableinterrupt = 1;
            }
            else{
                Enableinterrupt = 0;
            }
            stanSTATEB = stanSTATE;
            PWM_buf = VALUE;
            myNAstateOFF = 0;
            myNaVALUE = 0;
            myNaPWM = 1;
            myWpwm = 0;
            pressed = 1;
        }

        
        



        //jazeli  stralka w DOL po STATE OFF
        if((key & 0b00100000) && myNAstateOFF == 1){ // mb
            if(stanSTATE == 1){
                zmianaLCD(19);
            }else{
            zmianaLCD(11);
            }
            myNAstateOFF = 0;
            myNaVALUE = 1;
            pressed = 1;
        }


        //jazeli  stralka w GORE po STATE OFF
        if((key & 0b00010000) && myWpwm == 1 && myNaVALUE == 1){
            if(stanSTATE == 0){
                zmianaLCD(10);
                }else{
                zmianaLCD(18);
                }
            myNaVALUE = 0;
            myNAstateOFF = 1;
            pressed = 1;
        }

        //jazeli  stralka wprawo i STATE ON
        if((key & 0b00001000)  && myNAstateOFF == 1){
            zmianaLCD(18);
            stanSTATE = 1; // state ON
            pressed = 1;
        }

        //jazeli  stralka wlewo i STATE OFF
         if((key & 0b00000100)  && myNAstateOFF == 1){
            zmianaLCD(10);
            stanSTATE = 0;
            pressed = 1;
        }





        // zwiekszamy VALUE na 10
        if((key & 0b00001000) && myNaVALUE == 1){
            bufer = 30;
            if(VALUE >= 120 ){
                VALUE = 120;
                if(stanSTATE == 0){
                zmianaLCD(11);
                }else{
                    zmianaLCD(19);
                }
            }else{
                VALUE += 10;
                if(stanSTATE == 0){
                zmianaLCD(11);
                }else{
                    zmianaLCD(19);
                }
            }
            
            pressed = 1;
        }

        // zmenshujemo VALUE na 10
        if((key & 0b00000100) && myNaVALUE == 1){
            bufer = 30;
            if(VALUE <= 30 ){
                VALUE = 30;
                if(stanSTATE == 0){
                zmianaLCD(11);
                }else{
                    zmianaLCD(19);
                }
            }else{
                VALUE -= 10;
                if(stanSTATE == 0){
                zmianaLCD(11);
                }else{
                    zmianaLCD(19);
                }
            }
            
            pressed = 1;

        }


        // sho jaksho zbilshyty napryklad na 34 - nazaty -10, ne bulu 24, a stalo 30
        if(VALUE < 30){
            VALUE = 30;
            if(stanSTATE == 0){
                zmianaLCD(11);
                }else{
                    zmianaLCD(19);
                }
        }

        if(VALUE > 120){
            VALUE = 120;
                if(stanSTATE == 0){
                zmianaLCD(11);
                }else{
                    zmianaLCD(19);
                }
        }
        



      zasilanieWyswietlacza = 1;
      
      *wyborSegmentu = 0x08;//strzałka w góre
      
        if(odczytZKlawiatury == 1){
            
            if(VALUE >= 120 ){
                VALUE = 120;
            }else{
                VALUE += 1;
            }
            
            if(myWpwm == 1){
                if(stanSTATE == 0){
                zmianaLCD(11);
                }else{
                    zmianaLCD(19);
					}
					           }
			if(myWmainSCREEN == 1){
            	zmianaLCD(3);
			}

             
        }

        *wyborSegmentu = 0x10;//strzałka w dol
        if(odczytZKlawiatury == 1){
          if(VALUE <= 30 ){
                VALUE = 30;
            }else{
                VALUE -= 1;
            }
            
            if(myWpwm == 1){
                if(stanSTATE == 0){
                zmianaLCD(11);
                }else{
                    zmianaLCD(19);
					}
					           }
				if(myWmainSCREEN == 1){
            	zmianaLCD(3);
			}
        }


        *wyborSegmentu = 0x04;//strzałka w prawo
        if(odczytZKlawiatury == 1){
            if(VALUE >= 120 ){
                VALUE = 120;
            }else{
                VALUE += 10;
            }
            
            if(myWpwm == 1){
                if(stanSTATE == 0){
                zmianaLCD(11);
                }else{
                    zmianaLCD(19);
					}
					           }
				if(myWmainSCREEN == 1){
            	zmianaLCD(3);
			}
        }



        *wyborSegmentu = 0x20;//strzałka w lewo
        if(odczytZKlawiatury == 1){
          if(VALUE <= 30 ){
                VALUE = 30;

            }else{
                VALUE -= 10;
            }

            if(myWpwm == 1){
                if(stanSTATE == 0){
                zmianaLCD(11);
                }else{
                    zmianaLCD(19);
					}
				}
			if(myWmainSCREEN == 1){
            	zmianaLCD(3);
			}
        }




        *wyborSegmentu = 0x3C;//strzałka w górę i strzałka w dół,wlewo i wprawo
        while(odczytZKlawiatury == 1){
            PWM();
            zasilanieWyswietlacza = 1;
            *wyborSegmentu = 0x3C;//strzałka w górę i strzałka w dół,wlewo i wprawo
            
        }
        
        PWM();
        zasilanieWyswietlacza = 0;

        //*********************obluga menuPWM(2.1)*******************







//*******************obsluga MAIN SCREEN******************************************
        //jaksho nazaty ENTER  po main screen
        if((key & 0b10000000) && myNaMainScreen == 1){
            zmianaLCD(3);
            myWmainSCREEN = 1;
            myNaMainScreen = 0;
            myNaSettings = 0;
            pressed = 1;
        }


        //jaksho nazaty strilku WNYZ po main screen
        if( (key & 0b00100000) && myNaSettings == 0 && myNaMainScreen == 1 ){
            zmianaLCD(1);
            myNaSettings = 1;
            myNaMainScreen = 0;
            pressed = 1;
        }

        //jaksho nazaty strilku WWERH po settings
        if((key & 0b00010000) && myNaSettings == 1){
            zmianaLCD(2);
            pressed = 1;
            myNaMainScreen = 1;
            myNaSettings = 0;
        }

        

        //jaksho nazaty ENTER  po settings
        if((key & 0b10000000) && myNaSettings == 1){
            zmianaLCD(4);
            pressed = 1;
            myNaSettings = 0;
            myNaPWM = 1;
        }

        //*******************obsluga MAIN SCREEN******************************************


    

       //jaksho my na OTHER i nazymajem strilku WNYZ.
        if((key & 0b00100000) && myNaOTHER == 1){
            zmianaLCD(7);
            myNaOTHER = 0;
            myNaLOAD = 1;
            pressed = 1;
        }


        //jaksho my na LED i nazymajem strilku wnyz. Perechodym na drugu czastynu menu
        if((key & 0b00100000) && myNaLED == 1 && myNaPWM == 0){
            zmianaLCD(6);
            myNaLED = 0;
            myNaOTHER = 1;
            pressed = 1;

           
        }





        //jaksho nazaty strilku WNYZ po PWM
        if((key & 0b00100000) && myNaPWM == 1){
            zmianaLCD(5);
            myNaLED = 1;
            myNaPWM = 0;
            pressed = 1;
        }







        //jaksho nazaty strilku WWERH po LED
        if((key & 0b00010000) && myNaLED == 1){
            zmianaLCD(4);
            myNaPWM = 1;
            myNaLED = 0;
            pressed  = 1;
        }

        

        //jaksho nazaty strilku WWERH po OTHER. PEREJDEM W MENU 1
        if((key & 0b00010000) && myNaOTHER == 1 ){
            zmianaLCD(5);
            pressed = 1;
            myNaLED = 1;
            myNaOTHER = 0;
             
        }


        //jaksho my na LOAD DEF i nazaly strilku WWERH
        if((key & 0b00010000) && myNaLOAD == 1){
            zmianaLCD(6);
            myNaLOAD = 0;
            myNaOTHER = 1;
            pressed = 1;
        }

        


        


        /// obsluga Buzzika /////////////////////////
        
        //jaksho my na OTHER i nazumajem ENTER
        if((key & 0b10000000) && myNaOTHER == 1) {

            if(mywklbuzz == 0){
                zmianaLCD(8);
            }else{
                 zmianaLCD(9);
            }
            
            myNaBUZZ = 1;
            myNaOTHER = 0;
            pressed = 1;
        }//jaksho enter po load def
        //Resetowanie wszyskich zmiennych
        else if ((key & 0b10000000) && myNaLOAD == 1)
        {
            Enableinterrupt = 0;
            stanSTATE = 0;
            stanSTATEB = 0;
            VALUE = 30;
            PWM_buf = 30;
            BUZZ = 1;
            BUZZon=0;
            mywklbuzz = 0;
            F1_1 = 0;
            F2_1 = 0;
            F3_1 = 0;
            F4_1 = 0;
            OK_1 = 0;
            ER_1 = 0;
            diod_pointer = 0b000000; ///!!!!
            F1_2 = 0;
            F2_2 = 0;
            F3_2 = 0;
            F4_2 = 0;
            OK_2 = 0;
            ER_2 = 0;

            
        }
        
        else if((key & 0b10000000) && myNaBUZZ == 1)
        {// wkl buzz i wychodym
            zmianaLCD(6);

            if (BUZZon)
            {
                BUZZ = 0;
                mywklbuzz = 1;
            }
            else{
                BUZZ = 1;
                mywklbuzz = 0;
            }
            
            pressed = 1;
            myNaOTHER = 1;
            myNaBUZZ = 0;
            
        }




        // nazaly strilku wprawo. BUZZ on
        if((key & 0b00001000) && myNaBUZZ == 1){
            zmianaLCD(9);
            
            pressed = 1;
            BUZZon = 1;
        }

        // nazaly strilku wliwo. BUZZ off
        if((key & 0b00000100) && myNaBUZZ == 1){
            zmianaLCD(8);
            BUZZon = 0;
            pressed = 1;
        }

        /// obsluga Buzzika /////////////////////////







        //obsluga menu LED///////////////
        //enter po led
        if((key & 0b10000000) && myNaLED == 1 && myWled == 0){

            myNaLED = 0;
            myWled = 1;
            licznikFORf = 12;
            pressed = 1;
            Fon(licznikFORf);

        }
        else if ((key & 0b10000000) && myWled == 1)//Enter W LED, zatwierdzenie wszyskich zmian i wyjscie do poprzedniego podmeniu
        {

            F1_2=F1_1;
            F2_2=F2_1;
            F3_2=F3_1;
            F4_2=F4_1;
            OK_2=OK_1;
            ER_2=ER_1;
            myWled = 0;
            myNaLED = 1;
            zmianaLCD(5);
            POINTER_CONBINE();
            pressed = 1;
        }
        
            //wnyz
         if((key & 0b00100000) && myWled == 1){
            licznikFORf++;
            if(licznikFORf == 18){
                licznikFORf = 12;//17
            }
            Fon(licznikFORf);
            pressed = 1;
         }

            //wwerh
        if((key & 0b00010000) && myWled == 1){
            licznikFORf--;

            if(licznikFORf == 11){
                licznikFORf = 17;
            }
            Fon(licznikFORf);
            pressed = 1;
        }

        // on
        if((key & 0b00001000) && myWled == 1){

            if(licznikFORf == 12){
                F1_1 = 1;
            }

            if (licznikFORf == 14){
                F3_1 = 1;
            }

            if (licznikFORf == 16){
                OK_1 = 1;
            }

            if (licznikFORf == 13){
                F2_1 = 1;
            }

            if (licznikFORf == 15){
                F4_1 = 1;
            }

            if(licznikFORf == 17){
                ER_1 = 1;
            }

            Fon(licznikFORf);
            pressed = 1;
        }

        //off
        if((key & 0b00000100) && myWled == 1){
            if (licznikFORf == 12){
                F1_1 = 0;
            }

            if (licznikFORf == 14){
                F3_1 = 0;
            }

            if (licznikFORf == 16){
                OK_1 = 0;
            }

            if (licznikFORf == 13){
                F2_1 = 0;
            }

            if (licznikFORf == 15){
                F4_1 = 0;
            }

            if(licznikFORf == 17){
                ER_1 = 0;
            }
            
            Fon(licznikFORf);
            LEDstatus = 0;
            pressed = 1;
        }


        //obsluga menu LED///////////////






        //obsluga esc/////////////////////////////

        //wychodymo z MAIN SCREEN
        if((key & 0b01000000) && myWmainSCREEN == 1){
            zmianaLCD(2);
            myNaMainScreen = 1;
            myWmainSCREEN = 0;
            pressed = 1;
        }

        //wychodymo z SETTINGS koly na PWM
        if((key & 0b01000000) && myNaPWM == 1){
            zmianaLCD(2);
            myNaPWM = 0;
            myNaMainScreen = 1;
            pressed = 1;
        }

        //wychodymo z SETTINGS koly na LED
        if((key & 0b01000000) && myNaLED == 1){
             zmianaLCD(2);
             myNaMainScreen = 1;
             myNaSettings = 0;
             myNaLED = 0;
             pressed = 1;
        }

        //wychodymo z SETTINGS koly na OTHER
        if((key & 0b01000000) && myNaOTHER == 1){
            zmianaLCD(2);
            myNaOTHER = 0;
            myNaMainScreen = 1;
            myNaSettings = 0;
            pressed = 1;
        }
        //wychodymo z SETTINGS koly na LOAD DEF
        if((key & 0b01000000) && myNaLOAD == 1){
            zmianaLCD(2);
            myNaLOAD = 0;
            myNaMainScreen = 1;
            myNaSettings = 0;
            pressed = 1;
        }

        //wychodymo z PWM
        if((key & 0b01000000) && myWpwm == 1){
            zmianaLCD(4);
            myWpwm = 0;
            myNAstateOFF = 0;
            myNaVALUE = 0;
            myNaPWM = 1;
            pressed = 1;
            stanSTATE = stanSTATEB;
            VALUE = PWM_buf;
        }

        //wychodymo z BUZZ
        if((key & 0b01000000) && myNaBUZZ == 1){
            myNaBUZZ = 0;
            myNaOTHER = 1;
            zmianaLCD(6);
            pressed = 1;
        }

        //wychodymo z LED
        //Zapisujemy wszyski zatwierdzony ledy zeby wykasowac zmiany
        if((key & 0b01000000) && myWled == 1){
            myWled = 0;
            myNaLED = 1;
            zmianaLCD(5);
            myNaMainScreen = 0;
            pressed = 1;

            F1_1=F1_2;
            F2_1=F2_2;
            F3_1=F3_2;
            F4_1=F4_2;
            OK_1=OK_2;
            ER_1=ER_2;
        }


       //obsluga esc/////////////////////////////


        //oczekiwanie dopuki zaczisznieta jaka kolwiek klawisz na klaw multp
        while(pressed){
            PWM();
            key = ~(*key_addr);
            if(key == 0b00000000){
                pressed = 0;
                delay();
            }



        }
    
    }

   


}
//funkcja przerwania transmisji szeregowej
void sio_int(void) __interrupt(4){
    if (TI) {
    TI = 0;//Jezeli TI = 1, to gotowy do pobrania, 0 odwrotnie
    /*bit flagowy stanu nadawania - moŜe spowodować zgłoszenie
    przerwania; bit jest ustawiany sprzętowo w momencie zakończe-
    nia wysyłania danej;*/
    
    }
    else {
    RI = 0;
    /*bit flagowy stanu odbioru - moŜe spowodować zgłoszenie przer-
    wania; bit jest ustawiany sprzętowo w momencie zakończenia od-
    bioru danej;*/

    buffer = buffer * 10 + (SBUF - 48);
    char_count++;

	if (char_count == max_char_count) {
    if (buffer >= 30 && buffer <= 120) {
    VALUE = buffer;
    
    if(myWpwm == 1){
                if(stanSTATE == 0){
                zmianaLCD(11);
                }else{
                    zmianaLCD(19);
					}
					           }
			if(myWmainSCREEN == 1){
            	zmianaLCD(3);
			}

	TI = 0;

    }
    buffer = 0;
    char_count = 0;
    }
}
}



