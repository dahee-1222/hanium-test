#include <mega128.h>
#include <delay.h>

#define FND_Null    17
#define FND_Star    14
#define FND_Sharp   15

unsigned int Port_char[]
= {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xd8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x84,0x8e,0xff,0xbf};  // 0~F의 문자표 
unsigned int Port_fnd[] = {0x1f,0x2f,0x4f,0x8f,0x0f};   // FND0 ON, FND1 ON, FND2 ON, FND3 ON, ALL FND OFF    
unsigned char New_key_data = 0, Key_Num = 0;            
unsigned int buf_seg[4] = {FND_Null,FND_Null,FND_Null,FND_Null};
bit Key_off_flag = 0;

void Init_PORT(void)
{
    DDRE = 0xF0;            // FND Sel
    DDRB = 0xFF;            // FND Data Line
    DDRC = 0x0F;            // 상위 4bit Col(입력), 하위 4bit Row(출력)
    PORTC = 0x0F;           // Port 초기화
    PORTE = Port_fnd[4];    // ALL FND OFF
}

void Print_Segment(unsigned int* seg_value)
{
    PORTE = Port_fnd[0];
    PORTB = Port_char[seg_value[0]];
    delay_ms(1);
    
    PORTE = Port_fnd[1];
    PORTB = Port_char[seg_value[1]];
    delay_ms(1);
    
    PORTE = Port_fnd[2];
    PORTB = Port_char[seg_value[2]];
    delay_ms(1);
    PORTE = Port_fnd[3];
    PORTB = Port_char[seg_value[3]];
    delay_ms(1);
}

unsigned char KeyScan(void)                             // 4x4 키패드 스캔 함수, 출력 값은 10진수(1~16)
{
    unsigned int Key_Scan_Line_Sel = 0xFE;              // Init_data 하위 니블의 출력을 결정 
                                                        // 하위 니블(4bit)을 스위칭(연속적으로 돌아가면서)하면서 출력 
    unsigned char Key_Scan_Sel = 0, key_scan_num = 0;
    unsigned char Get_Key_Data = 0;                     // 실제 키 데이터 
    
    // 키스캔 부분 
    for(Key_Scan_Sel=0; Key_Scan_Sel<4; Key_Scan_Sel++)
    {    
        // 초기화 
        PORTC = Key_Scan_Line_Sel;
        delay_us(10);
        
        // 수신 부분 
        Get_Key_Data = (PINC & 0xF0);                   // 74LS14의 데이터 취득 
        
        if(Get_Key_Data != 0x00)
        {
            switch(Get_Key_Data)                        // C포트 내용의 상위 니블(4bit)만 받음 
            {
                case 0x10:                              // 0001이 맞으면 현재 count값에 4를 곱한후 1을 더하고 key_scan_num변수에 저장 
                    key_scan_num = Key_Scan_Sel*4+1;     
                    break;
                    
                case 0x20: 
                    key_scan_num = Key_Scan_Sel*4+2;    // 0010이 맞으면 현재 count값에 4를 곱한후 2를 더하고 key_scan_num변수에 저장 
                    break;
                
                case 0x40:  
                    key_scan_num = Key_Scan_Sel*4+3;    // 0100이 맞으면 현재 count값에 4를 곱한후 3을 더하고 key_scan_num변수에 저장 
                    break;
                
                case 0x80:
                    key_scan_num = Key_Scan_Sel*4+4;    // 1000이 맞으면 현재 count값에 4를 곱한후 4를 더하고 key_scan_num변수에 저장
                    break;  
                
                default:    
                  key_scan_num = FND_Null;              
                  break;
            }           
            return key_scan_num; 
        }    
        Key_Scan_Line_Sel = (Key_Scan_Line_Sel<<1) | 0x01;  // Init_data를 시프트함, Key_Scan_Line 이동                
    }                                                     
    return key_scan_num;
}


unsigned char keypad_in()               // 채터링 방지 구문 
{
    if(Key_off_flag)                    // Key_off_flag가 1일 경우, 즉 키를 눌렀을 경우 
    {
        buf_seg[3] = buf_seg[2];        // 세그먼트 값 시프트 ■ □ □ □ <-  □ ■ □ □ 
        buf_seg[2] = buf_seg[1];        // 세그먼트 값 시프트 □ ■ □ □ <-  □ □ ■ □
        buf_seg[1] = buf_seg[0];        // 세그먼트 값 시프트 □ □ ■ □ <-  □ □ □ ■
        Key_off_flag = ~Key_off_flag;   // 키 입력을 한번 받기 위한 Key_off_flag 반전 
    }                                
    else;
        buf_seg[0] = Key_Num;           // 누른 키값을 세그먼트에 표현  □ □ □ ■
    return Key_Num;                     // 전역변수 Key_Num 리턴 
}

unsigned char Key_decode(unsigned int New_key_data)  // 키스캔을 통해 받은 key_scan_num를  
{                                                    // 키패드상에 해당하는 실제 숫자와 문자로 변환하기 위한 함수
    switch(New_key_data)
    {
        case 1 :
            Key_Num = 1;
            break;
        case 2 :
            Key_Num = 2;   
            break;
        case 3 :
            Key_Num = 3;  
            break;
        case 4 :
            Key_Num = 10; 
            break;
        case 5 :
            Key_Num = 4;     
            break;
        case 6 :
            Key_Num = 5;  
            break;
        case 7 :
            Key_Num = 6;
            break;
        case 8 :
            Key_Num = 11; 
            break;
        case 9 :
            Key_Num = 7;  
            break;
        case 10 :
            Key_Num = 8;
            break;
        case 11 :
            Key_Num = 9; 
            break;
        case 12 :
            Key_Num = 12;  
            break;
        case 13 :
            Key_Num = FND_Star;
            break;
        case 14 :
            Key_Num = 0;    
            break;
        case 15 :
            Key_Num = FND_Sharp;
            break;
        case 16 :
            Key_Num = 13;    
            break;       
    }    
    return Key_Num;
}

void main()
{
    Init_PORT();      
    
    while(1)
    {   
        New_key_data = KeyScan();  
        if(New_key_data)
        { 
            Key_Num = Key_decode(New_key_data); 
            keypad_in();         
        }   
        else
        Key_off_flag=1;
        Print_Segment(buf_seg);       
    }
}