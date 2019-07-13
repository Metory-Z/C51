	 #include<reg52.h>
	 
	// 左移到头右移

	 sbit LED=P0^0;
	 sbit ADDR0=P1^0;
	 sbit ADDR1=P1^1;
	 sbit ADDR2=P1^2;
	 sbit ADDR3=P1^3;
	 sbit ENLED=P1^4;

	 void main(){
	 	unsigned int i = 0;
		unsigned char cnt = 0, flag = 1;

	 	ENLED=0;
		ADDR3=1;
		ADDR2=1;
		ADDR1=1;
		ADDR0=0;
		while (1)
		{
			if (flag == 1)
			{
				P0 = ~(0x01 << cnt);
				for (i=0; i<30000; i++);
				cnt++;
				if (cnt>=8)
					{
						cnt=0;
						flag = -flag;
					}
			}
			else
			{
				P0 = ~(0x80 >> cnt);
				for (i=0; i<30000; i++);
				cnt++;
				if (cnt>=8)
					{
						cnt=0;
						flag = -flag;
					}
			}
			
		}
}