#include <mega128.h>
#include <delay.h>

#define FND_Null    17
#define FND_Star    14
#define FND_Sharp   15

unsigned int Port_char[]
= {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xd8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x84,0x8e,0xff,0xbf};  // 0~F�� ����ǥ 
unsigned int Port_fnd[] = {0x1f,0x2f,0x4f,0x8f,0x0f};   // FND0 ON, FND1 ON, FND2 ON, FND3 ON, ALL FND OFF    
unsigned char New_key_data = 0, Key_Num = 0;            
unsigned int buf_seg[4] = {FND_Null,FND_Null,FND_Null,FND_Null};
bit Key_off_flag = 0;

void Init_PORT(void)
{
    DDRE = 0xF0;            // FND Sel
    DDRB = 0xFF;            // FND Data Line
    DDRC = 0x0F;            // ���� 4bit Col(�Է�), ���� 4bit Row(���)
    PORTC = 0x0F;           // Port �ʱ�ȭ
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

unsigned char KeyScan(void)                             // 4x4 Ű�е� ��ĵ �Լ�, ��� ���� 10����(1~16)
{
    unsigned int Key_Scan_Line_Sel = 0xFE;              // Init_data ���� �Ϻ��� ����� ���� 
                                                        // ���� �Ϻ�(4bit)�� ����Ī(���������� ���ư��鼭)�ϸ鼭 ��� 
    unsigned char Key_Scan_Sel = 0, key_scan_num = 0;
    unsigned char Get_Key_Data = 0;                     // ���� Ű ������ 
    
    // Ű��ĵ �κ� 
    for(Key_Scan_Sel=0; Key_Scan_Sel<4; Key_Scan_Sel++)
    {    
        // �ʱ�ȭ 
        PORTC = Key_Scan_Line_Sel;
        delay_us(10);
        
        // ���� �κ� 
        Get_Key_Data = (PINC & 0xF0);                   // 74LS14�� ������ ��� 
        
        if(Get_Key_Data != 0x00)
        {
            switch(Get_Key_Data)                        // C��Ʈ ������ ���� �Ϻ�(4bit)�� ���� 
            {
                case 0x10:                              // 0001�� ������ ���� count���� 4�� ������ 1�� ���ϰ� key_scan_num������ ���� 
                    key_scan_num = Key_Scan_Sel*4+1;     
                    break;
                    
                case 0x20: 
                    key_scan_num = Key_Scan_Sel*4+2;    // 0010�� ������ ���� count���� 4�� ������ 2�� ���ϰ� key_scan_num������ ���� 
                    break;
                
                case 0x40:  
                    key_scan_num = Key_Scan_Sel*4+3;    // 0100�� ������ ���� count���� 4�� ������ 3�� ���ϰ� key_scan_num������ ���� 
                    break;
                
                case 0x80:
                    key_scan_num = Key_Scan_Sel*4+4;    // 1000�� ������ ���� count���� 4�� ������ 4�� ���ϰ� key_scan_num������ ����
                    break;  
                
                default:    
                  key_scan_num = FND_Null;              
                  break;
            }           
            return key_scan_num; 
        }    
        Key_Scan_Line_Sel = (Key_Scan_Line_Sel<<1) | 0x01;  // Init_data�� ����Ʈ��, Key_Scan_Line �̵�                
    }                                                     
    return key_scan_num;
}


unsigned char keypad_in()               // ä�͸� ���� ���� 
{
    if(Key_off_flag)                    // Key_off_flag�� 1�� ���, �� Ű�� ������ ��� 
    {
        buf_seg[3] = buf_seg[2];        // ���׸�Ʈ �� ����Ʈ �� �� �� �� <-  �� �� �� �� 
        buf_seg[2] = buf_seg[1];        // ���׸�Ʈ �� ����Ʈ �� �� �� �� <-  �� �� �� ��
        buf_seg[1] = buf_seg[0];        // ���׸�Ʈ �� ����Ʈ �� �� �� �� <-  �� �� �� ��
        Key_off_flag = ~Key_off_flag;   // Ű �Է��� �ѹ� �ޱ� ���� Key_off_flag ���� 
    }                                
    else;
        buf_seg[0] = Key_Num;           // ���� Ű���� ���׸�Ʈ�� ǥ��  �� �� �� ��
    return Key_Num;                     // �������� Key_Num ���� 
}

unsigned char Key_decode(unsigned int New_key_data)  // Ű��ĵ�� ���� ���� key_scan_num��  
{                                                    // Ű�е�� �ش��ϴ� ���� ���ڿ� ���ڷ� ��ȯ�ϱ� ���� �Լ�
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