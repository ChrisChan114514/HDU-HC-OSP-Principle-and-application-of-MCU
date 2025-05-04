ORG 0000H       ; 程序存储器起始地址
AJMP MAIN       ; 绝对跳转

ORG 0100H       ; 将后续代码定位到0100H地址
MAIN:           ; 主程序入口标签
    CLR P1.0    ; 清除P1端口的第0位（P1.0输出低电平）
    LCALL DELAY ; 长调用DELAY子程序（实现延时）
    SETB P1.0   ; 置位P1端口的第0位（P1.0输出高电平）
    LCALL DELAY ; 再次调用DELAY子程序
    SJMP MAIN   ; 短跳转回MAIN标签（无限循环）

; 学号后两位=47，延时公式：(47 + 50)*2 = 194ms
DELAY:
    MOV R7, #194      ; 外层循环194次（每次约1ms）
DLY_LOOP:
    MOV R6, #250      ; 内层循环250次
    NOP               ; 微调（1μs）
DLY_INNER:
    DJNZ R6, DLY_INNER ; 内层循环250次 × 2μs = 500μs
    DJNZ R7, DLY_LOOP  ; 外层循环194次 × (500μs + 其他) ≈ 194ms
    RET


END             ; 程序结束标记
