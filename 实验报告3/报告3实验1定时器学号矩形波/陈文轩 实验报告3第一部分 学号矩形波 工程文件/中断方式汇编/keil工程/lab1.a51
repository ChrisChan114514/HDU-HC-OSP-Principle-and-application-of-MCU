; 宏/常量定义
CLK_FREQ    EQU 12000000   ; 12MHz
HALF_PERIOD EQU 16750     ; 半周期33.5ms (周期=20+47ms)
T0_RELOAD   EQU 65535-HALF_PERIOD  ; 计算初值
PORT_PIN    EQU P1.0       ; 输出引脚

ORG 0000H
LJMP MAIN
ORG 000BH
LJMP T0_ISR

ORG 0030H
MAIN:
    MOV TMOD, #01H        ; T0方式1
    MOV TH0, #(T0_RELOAD >> 8)   ;高八位赋值
    MOV TL0, #(T0_RELOAD & 0FFH) ;低八位赋值
    SETB ET0              ; 允许T0中断
    SETB EA               ; 开中断
    SETB TR0              ; 启动T0
    CLR PORT_PIN          ; 初始低电平
    SJMP $                ; 等待中断

T0_ISR:
    CPL PORT_PIN          ; 取反输出 半周期取反一次
    MOV TH0, #(T0_RELOAD >> 8)  ; 重装初值
    MOV TL0, #(T0_RELOAD & 0FFH)
    RETI
END
