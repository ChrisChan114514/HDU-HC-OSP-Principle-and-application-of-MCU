ORG 0H          ; 程序起始地址设置为0000H（复位向量地址）
MOV P1, #0FFH   ; 初始化P1端口：向P1写入FFH（所有引脚置高电平，LED全灭）

LOOP:           ; 主循环开始标签
    MOV A, #01H ; 将立即数01H（0000 0001B）送入累加器A（准备点亮P1.0）
    ACALL DELAY ; 调用延时子程序（保持当前状态一段时间）
    MOV P1, A   ; 将A的值输出到P1端口（P1.0=0，对应LED亮，其他引脚保持高电平）
    
    MOV A, #02H ; 将02H（0000 0010B）送入A（准备点亮P1.1）
    ACALL DELAY 
    MOV P1, A   ; P1.1=0，LED亮
    
    MOV A, #04H ; 04H（0000 0100B）→ A（准备点亮P1.2）
    ACALL DELAY
    MOV P1, A   ; P1.2=0
    
    MOV A, #08H ; 08H（0000 1000B）→ A（准备点亮P1.3）
    ACALL DELAY
    MOV P1, A   ; P1.3=0
    
    MOV A, #10H ; 10H（0001 0000B）→ A（准备点亮P1.4）
    ACALL DELAY
    MOV P1, A   ; P1.4=0
    
    MOV A, #20H ; 20H（0010 0000B）→ A（准备点亮P1.5）
    ACALL DELAY
    MOV P1, A   ; P1.5=0
    
    MOV A, #40H ; 40H（0100 0000B）→ A（准备点亮P1.6）
    ACALL DELAY
    MOV P1, A   ; P1.6=0
    
    ACALL DELAY ; 额外延时（保持P1.6状态）
    
    MOV A, #80H ; 80H（1000 0000B）→ A（准备点亮P1.7）
    ACALL DELAY
    MOV P1, A   ; P1.7=0
    
    SJMP LOOP   ; 无条件跳转回LOOP标签（无限循环）

; ====== 延时子程序 ======
DELAY:
    MOV R2, #0FFH ; 设置外层循环计数器R2=255
LOOP1:
    MOV R1, #0FFH ; 设置内层循环计数器R1=255
LOOP2:
    DJNZ R1, LOOP2 ; R1减1，若R1≠0则跳转至LOOP2（内层循环255次）
    DJNZ R2, LOOP1 ; R2减1，若R2≠0则跳转至LOOP1（外层循环255次）
    RET           ; 返回主程序
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
