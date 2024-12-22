#include <LedDisplay.h> //HCMS-2912 LED디스플레이를 사용하기 위한 라이브러리 / https://playground.arduino.cc/Main/LedDisplay/
#include <Keypad.h>     //키패드를 사용하기 위한 라이브러리 / https://playground.arduino.cc/Code/Keypad/
#include <BigNumber.h>  //큰 숫자를 관리하기 위한 라이브러리 https://github.com/nickgammon/BigNumber

//이하 LED 디스플레이 연결하는 핀의 설정. 
#define dataPin A4              // connects to the display's data in
#define registerSelect A0       // the display's register select pin
#define clockPin A1             // the display's clock pin
#define enable A2               // the display's chip enable pin
#define reset A3                // the display's reset pin

#define displayLength 16        // 디스플레이의 길이(BYTE)

//이하 키패드 관련 변수 설정
const int numRows = 5;        //키패드의 행 수
const int numCols = 5;        //키패드의 열 수
const int debounceTime = 20;  // 키패드가 안정되기 위한 디바운스 시간(ms)

//이하 LED 점등용 핀 변수 설정. 
const int LED_ROW1 = 13;
const int LED_ROW2 = 10;
const int LED_ROW3 = 11;
const int LED_ROW4 = 12;
const int LED_COL1 = A7;
const int LED_COL2 = A6;
const int LED_COL3 = A5;

// LED디스플레이를 사용하기 위한 상수/변수 설정.
LedDisplay myDisplay = LedDisplay(dataPin, registerSelect, clockPin, enable, reset, displayLength);
int brightness = 15;         // LED 디스플레이 초기 밝기 설정. 0~15
int scrollDirection = 1;        // 스크롤 할 때 방향  -1 = 왼쪽, 1 = 오른쪽
int cursor;                 // 커서 위치

//수학적 계산용 변수 설정
byte SignificantDigit;  //유효숫자.

//키패드 배열 설정
const char keymap[numRows][numCols] = {
  { 'C', 'A', 'M', 'P', 'I'},  //A:AC   O:ON/CE   M:Memory Recall/Clear  P:M+  I:M-
  { '7', '8', '9', '%', 'B'},   // B:backspace
  { '4', '5', '6', '*', '/'},
  { '1', '2', '3', 'R', '-'},   // R:root
  { '0', 'D', '.', '+', '='},   // D:Double zero 00, 
};

//아두이노에서 행과 열에 사용되는 핀 설정
const int rowPins[numRows] = {4, 3, 2, 1, 0}; //행 
const int colPins[numCols] = {5, 6, 7, 8, 9}; //열

//계산기의 계산에 사용되는 숫자 관련 변수 설정
BigNumber firstNumber = 0 ;      // 첫번째 입력되는 수.  계산이 이어질 때는 secondNumber가 firstNumber가 된다. 
BigNumber secondNumber = 0;     // 두번째 입력되는 수. 
BigNumber pressedNumber;          // 키패드로 눌린 숫자. 
int digitScale = 10;                  // BigNumber 자릿수.

//계산기의 연산자 및 기호
boolean decimalPoint = false;
BigNumber digit = 1;                  //소수점 이하의 숫자 입력시 몇번째 자리에 입력하는지 체크하기 위한 변수.
char currentOperator;            //현재 연산자 저장
bool isSecondNumber = false;     // 두 번째 숫자 입력 여부

void setup() {
  pinMode(A0, OUTPUT);     //디스플레이에 연결된 핀들의 모드 설정
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);

  pinMode(LED_ROW1, OUTPUT); //LED 표시 핀들의 모드 설정
  pinMode(LED_ROW2, OUTPUT);
  pinMode(LED_ROW3, OUTPUT);
  pinMode(LED_COL1, OUTPUT);
  pinMode(LED_COL2, OUTPUT);
  pinMode(LED_COL3, OUTPUT);

  myDisplay.begin();    //디스플레이 초기화 
  myDisplay.setBrightness(brightness);  
  myDisplay.clear();
  myDisplay.setCursor(4);
  myDisplay.print("Hello :)");
  delay(1000);
  myDisplay.clear();

  for (int row = 0; row < numRows; row++) {
    pinMode(rowPins[row], INPUT);         //행 핀을 입력으로 설정한다
    digitalWrite(rowPins[row], HIGH);     //풀업 저항을 작동시킨다.
  }

  for (int column = 0; column < numCols; column++) {
    pinMode(colPins[column], OUTPUT);     //열 핀을 출력으로 설정한다.
    digitalWrite(colPins[column], HIGH);  //모든 열을 비활성으로 설정한다
  }

  myDisplay.setCursor(15);  // 커서 위치를 초기화함. 
  myDisplay.print(0);       // 보통 계산기가 켜졌을 때처럼 0 을 표시함.

  BigNumber::begin();      //BigNumber 라이브러리 초기화. 
  BigNumber::setScale(digitScale);

  Serial.begin(9600); // 디버깅용 시리얼 모니터 셋업. 
  Serial.println("start");

  digitalWrite(LED_ROW1, HIGH);
  digitalWrite(LED_COL1, HIGH);
  digitalWrite(LED_COL2, HIGH);
  digitalWrite(LED_COL3, HIGH);
  delay(30000);
}

void loop() {
  char key = getKey();
  if (key != 0) {
    switch (key) {
      case 'A':  //AC
        firstNumber = 0;
        secondNumber = 0;
        decimalPoint = false;
        digit = 1;
        isSecondNumber = false;
        myDisplay.clear();
        myDisplay.setCursor(15);
        myDisplay.print(0);
        break;

      case 'O':  // ON/CE
        break;

      case 'M':  // Memory
        break;

      case 'P':  // Memory+
        break;

      case 'I':  // Memory-
        break;

      case '%':
        break;

      case 'B':  // Backspace
        break;

      case '.':
        decimalPoint = true;
        break;

      case 'R':  // Root
        if (isSecondNumber) {
          secondNumber = secondNumber.sqrt();
          displayNumber(secondNumber);
        } else {
          firstNumber = firstNumber.sqrt();
          displayNumber(firstNumber);
        }
        break;

      case '=':
        calculate();
        break;

      case '+':
      case '-':
      case '*':
      case '/':
        if (isSecondNumber) {
          calculate();
        }
        currentOperator = key;
        isSecondNumber = true;
        decimalPoint = false;
        digit = 1;
        break;

      default:
        pressedNumber = BigNumber(key - '0');
        numberDisplay();
        break;
    }
  }
  delay(50);
}

void numberDisplay() {
  if (isSecondNumber) {
    if (decimalPoint) {
      secondNumber = secondNumber + pressedNumber / BigNumber(10).pow(digit);
      digit++;
    } else {
      secondNumber = secondNumber * 10 + pressedNumber;
    }
    displayNumber(secondNumber);
  } else {
    if (decimalPoint) {
      firstNumber = firstNumber + pressedNumber / BigNumber(10).pow(digit);
      digit++;
    } else {
      firstNumber = firstNumber * 10 + pressedNumber;
    }
    displayNumber(firstNumber);
  }
}

void displayNumber(BigNumber number) {
  String numStr = number.toString();
  if (numStr.length() > displayLength) {
    numStr = numStr.substring(0, displayLength);
  }
  myDisplay.clear();
  myDisplay.setCursor(displayLength - numStr.length());
  myDisplay.print(numStr);
  Serial.print("Display: ");
  Serial.println(numStr);
}

void calculate() {
  if (isSecondNumber) {
    switch (currentOperator) {
      case '+':
        firstNumber = firstNumber + secondNumber;
        break;
      case '-':
        firstNumber = firstNumber - secondNumber;
        break;
      case '*':
        firstNumber = firstNumber * secondNumber;
        break;
      case '/':
        firstNumber = firstNumber / secondNumber;
        break;
    }
    displayNumber(firstNumber);
    secondNumber = 0;
    isSecondNumber = false;
  }
}

char getKey() {
  char key = 0; //0은 눌린 키가 없음을 의미
  for (int column = 0; column < numCols; column++) {
    digitalWrite(colPins[column], LOW);    //현재 열을 활성으로 설정한다.
    for (int row = 0; row < numRows; row++) {  //모든 행을 검사하여 눌린 키가 있는지 확인
      if (digitalRead(rowPins[row]) == LOW) {
        delay(debounceTime);
        while (digitalRead(rowPins[row]) == LOW);  //키를 놓을 때까지 기다린다
        key = keymap[row][column];      //눌린 키를 저장한다
      }
    }
    digitalWrite(colPins[column], HIGH);    //현재 열을 비활성으로 설정한다.
  }
  return key;   //눌린 키 또는 0을 리턴한다.
}
