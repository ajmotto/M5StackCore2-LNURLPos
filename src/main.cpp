#include <M5Core2.h>
#include <string.h>
#include <qrcode.h>
#include <Bitcoin.h>
//#include <Base64.h>
#include <Hash.h>
#include <Conversion.h>


////////////////////////////////////////////////////////
////////CHANGE! USE LNURLPoS EXTENSION IN LNBITS////////
////////////////////////////////////////////////////////
String server = "https://njvijay-umbrel.ngrok.io";
String posId = "JhCX7ACbchA27C4Qb7NNZ9";
String key = "KzGWWEYeJaZVMVkTBGARRt";
String currency = "USD";

////////////////////////////////////////////////////////
////Note: See lines 75, 97, to adjust to keypad size////
////////////////////////////////////////////////////////

//////////////VARIABLES///////////////////

String dataId = "";
bool paid = false;
bool shouldSaveConfig = false;
bool down = false;
const char* spiffcontent = "";
String spiffing; 
String lnurl;
String choice;
String payhash;
String key_val;
String cntr = "0";
String inputs="10";
int keysdec;
int keyssdec;
float temp;  
String fiat;
float satoshis;
String nosats;
float conversion;
String virtkey;
String payreq;
int randomPin;
bool settle = false;
String preparedURL;

SHA256 h;

//UI Design

#define orange 0xFC64
#define dark 0x02F3
#define darkOrange 0xC18A
#define red 0xFC7F
#define purple 0xC18A
#define light 0xCF3E

int posX2[4];
int n2=4;
int y2=3;
int boxSize2=45;
int space2=3;
int start2=65;
String amount="";
float numbers2[3]={0,0,0};
int fase2=0;
char buttons2[4][3]={{'0',' ','Q'},{'1','4','7'},{'2','5','8'},{'3','6','9'}};
char operation2=' ';
bool irq2 = false;

int posX[7];

int boxSize=26;
int space=2;
int start=21;

int screenTouched = 1;
int hashPressed = 0;
int QRScreen = 0;

void numberDisplay(String amnt) {
    M5.Lcd.fillRoundRect(posX2[0],3,188,80,4,dark);
    M5.Lcd.drawString("Enter the amount",posX2[0]+5,10,2);
    M5.Lcd.drawString(amnt+" USD",posX2[0]+5,40,4); 
    //amount = amnt;

}

void resetDisplay(){
  numberDisplay("0.00");
  amount="";  
}

void startCalc()
  {

    resetDisplay();
    M5.Lcd.fillRect(0,0,319,239,WHITE);
        //M5.Lcd.pushImage(240,160,64,64,calc[moode]);
     M5.Lcd.setTextColor(light);
     M5.Lcd.setTextFont(4);
    for(int i=0;i<n2;i++) {
        posX2[i]=(start2+(i*boxSize2)+(space2*i));
        Serial.printf("Posx %d: %d", i, posX2[i]);
    }
    //Top calculator Display
    numberDisplay("0.00");

   for(int i=0;i<n2;i++)
    for(int j=0;j<y2;j++){

    if(j<y2 && i <1 )
    M5.Lcd.fillRoundRect(posX2[i],posX2[j]+22,boxSize2,boxSize2,4,darkOrange);
    else
    M5.Lcd.fillRoundRect(posX2[i],posX2[j]+22,boxSize2,boxSize2,4,dark);

    M5.Lcd.drawString(String(buttons2[i][j]), posX2[i]+16, posX2[j]+34);
    
    }}


//////////LNURL AND CRYPTO///////////////
void to_upper(char * arr){
  for (size_t i = 0; i < strlen(arr); i++)
  {
    if(arr[i] >= 'a' && arr[i] <= 'z'){
      arr[i] = arr[i] - 'a' + 'A';
    }
  }
}

void showPin()
{
  QRScreen = 1;
  //M5.Lcd.setTextSize(4);
  M5.Lcd.drawString(String(randomPin), 100, 120 );
}

void encode_data(byte output[8], byte nonce[8], int pin, int amount_in_cents){
  SHA256 h;
  h.write(nonce, 8);
  h.write((byte *)key.c_str(), key.length());
  h.end(output);
  output[0] = output[0] ^ ((byte)(pin & 0xFF));
  output[1] = output[1] ^ ((byte)(pin >> 8));
  for(int i=0; i<4; i++){
    output[2+i] = output[2+i] ^ ((byte)(amount_in_cents & 0xFF));
    amount_in_cents = amount_in_cents >> 8;
  }
}


////VERY KINDLY DONATED BY SNIGIREV!/////

void makeLNURL(){
  randomPin = random(1000,9999);
  byte nonce[8];
  for(int i = 0; i < 8;i++){
    nonce[i] = random(9);
  }
  byte payload[8];
  encode_data(payload, nonce, randomPin, amount.toInt());
  preparedURL = server + "/lnurlpos/api/v1/lnurl/";
  preparedURL += toHex(nonce,8);
  preparedURL += "/";
  preparedURL += toHex(payload, 8);
  preparedURL += "/";
  preparedURL += posId;
  Serial.println(preparedURL);
  char Buf[200];
  preparedURL.toCharArray(Buf, 200);
  char *url = Buf;
  byte * data = (byte *)calloc(strlen(url)*2, sizeof(byte));
  size_t len = 0;
  int res = convert_bits(data, &len, 5, (byte *)url, strlen(url), 8, 1);
  Serial.printf("Convert Bits = %d",res);
  char * charLnurl = (char *)calloc(strlen(url)*2, sizeof(byte));
  bech32_encode(charLnurl, "lnurl", data, len);
  to_upper(charLnurl);
  lnurl = charLnurl;
  Serial.println(lnurl);
}



void setup(void) {
  
  M5.begin();
  M5.Lcd.fillScreen(TFT_BLACK);
  //M5.Lcd.setSwapBytes(true);
  //resetCalc();
  startCalc();
  //M5.Lcd.fillScreen(BLACK);
  //M5.Lcd.setCursor(0,6,2);
  //M5.Lcd.setTextColor(0x0775);
  //M5.Lcd.println("LNURLPOS");
  //makeLNURL();
  //m5.Lcd.qrcode(lnurl,50,10,220,6U);
}

void loop(void) {
  M5.update();

  int y = 0;
  int x = 0;

  if (!QRScreen) {
      TouchPoint_t pos= M5.Touch.getPressPoint();
      y=pos.y;
      x=pos.x;
  }


  //Serial.println("Entering loop");
  //startCalc();

  if (x > 0) {
    if (y > 0 and y < 80) resetDisplay();

    for (int i = 0; i < y2; i++)
      for (int j = 0; j < n2; j++) {
        if (x >posX2[j] && x<posX2[j]+boxSize2 && y>posX2[i]+22 && y<posX2[i]+boxSize2+22 ){
          //startCalc();
          //M5.Lcd.fillRoundRect(posX2[j],posX2[i]+22,boxSize2,boxSize2,4,darkOrange);
            
         if(buttons2[j][i]=='0' || buttons2[j][i]=='1' || buttons2[j][i]=='2' || buttons2[j][i]=='3' || buttons2[j][i]=='4' || buttons2[j][i]=='5' || buttons2[j][i]=='6' || buttons2[j][i]=='7' || buttons2[j][i]=='8' || buttons2[j][i]=='9' ) {
            if (screenTouched == 1) {
              amount = amount + String(buttons2[j][i]);
              float currentVal = amount.toFloat()/100;
              Serial.printf("Keypad pressed: %s", amount);
              Serial.println();
              numberDisplay(String(currentVal));
              screenTouched = 0;
              x = 0;
              y = 0;
              Serial.println("Screen Touch reset");

            }
         }

         if(buttons2[j][i]=='Q') {
           if (screenTouched == 1) {
             if (amount.toInt() == 0) {
               Serial.println("Amount value is zero");
             } else {
               M5.Lcd.fillScreen(TFT_BLACK);
               makeLNURL();
               m5.Lcd.qrcode(lnurl,50,10,220,6U);
               QRScreen = 1;
             }
             hashPressed = 1;
             screenTouched = 0;
             x = 0;
             y = 0;
           }
         }

        }
      }
  } else {
    screenTouched = 1;
    hashPressed = 0;
  }

  if (M5.BtnA.isPressed()) {
    QRScreen = 0;
    M5.Lcd.fillScreen(TFT_WHITE);
    startCalc();
  }

  if (M5.BtnB.isPressed()) {
    M5.Lcd.fillScreen(TFT_BLACK);
    //QRScreen = 0;
    showPin();
  }

  if (M5.BtnC.isPressed()) {
    if (amount.toInt() == 0) {
               Serial.println("Amount value is zero");
             } else {
               M5.Lcd.fillScreen(TFT_BLACK);
               makeLNURL();
               m5.Lcd.qrcode(lnurl,50,10,220,6U);
               QRScreen = 1;
             }
  }



}