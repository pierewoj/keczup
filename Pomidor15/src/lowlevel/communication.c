/*
 * communication.c
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */


#include "communication.h"

//sends a message via UART
void sendMessage(char* msg, int msgLength)
{

}

//****************************************************//
//********************__USART__***********************//
//****************************************************//

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{


		if(U3_bufTxIndex==U3_bufTxMaxIndex-1)                   //Jesli wysylany zostal ostatni znak (\r)
		{
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);    //Wylacz przerwanie = koniec transmisji
			U3_bufTxIndex = 0;
		}

		else{
			USART_SendData(USART3, U3_buforTx[U3_bufTxIndex]);
			U3_bufTxIndex++;

		}
	}
}

void USART2_IRQHandler(void)
{
/*
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{


		if(U2_bufTxIndex==U2_bufTxMaxIndex-1)                   //Jesli wysylany zostal ostatni znak (\r)
		{
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);    //Wylacz przerwanie = koniec transmisji
			U2_bufTxIndex = 0;
		}

		else{
			USART_SendData(USART2, U2_buforTx[U2_bufTxIndex]);
			U2_bufTxIndex++;

		}
	}
*/
	if(USART_GetITStatus(USART2, USART_IT_RXNE )){
		U2_buforRx[U2_bufRxIndex] = USART_ReceiveData(USART2);
		U2_bufRxIndex++;

		if(U2_buforRx[U2_bufRxIndex-1] == 10 ||U2_bufRxIndex>50){
			 U2_buforRx_Size = U2_bufRxIndex-2;
			 U2_bufRxIndex = 0;
			 otrzymanoPolecenie(U2_buforRx,U2_buforRx_Size);
		}

		//a = USART_ReceiveData(USART2);
		//USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

	}
}

void uart3_sendArray(unsigned short int *arr, unsigned short int n){
	U3_bufTxMaxIndex = n;
	int i;
	for(i=0;i<n;i++){
		U3_buforTx[i]=*arr;
		arr++;
	}
	U3_bufTxIndex=0;
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

void uart2_sendArray(unsigned short int *arr, unsigned short int n){
	U2_bufTxMaxIndex = n;
	int i;
	for(i=0;i<n;i++){
		U2_buforTx[i]=*arr;
		arr++;
	}
	usart_data_number = n;
	U2_bufTxIndex=0;
	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

void dynamixel_ustawPozycje(double procent){
//0.2 - poziomo z tylu
//0.8 - poziomo przod

	if(procent<0 || procent>1)return;
	if(procent<0.6)
		RAMKA=0;
	else
		RAMKA=1;
	unsigned int a = procent * 0x03FF;
	int i=0;

	unsigned short int dynamixelData[11]={0xFF,0xFF,0xFE,0x07,0x03,0x1E,0x00,0x01,0x00,0x00,0xD6};
	dynamixelData[6] = (a & 0x00FF)%0xFF;
	dynamixelData[7] = (a>>8)%0xFF;
	dynamixelData[10]=~(dynamixelData[2]+dynamixelData[3]+dynamixelData[4]+dynamixelData[5]+dynamixelData[6]+dynamixelData[7]+dynamixelData[8]+dynamixelData[9]);
	dynamixelData[10]=dynamixelData[10]&0xFF;
	uart3_sendArray(dynamixelData,12);
}

//processes a message received by UART. Messages shuold end with '\n'.
void messageProcessor(char* MSG, int MSG_size)
{
	if(MSG_size<1)
		return;
	if(MSG[0]!='C')
		return;

	//Komendy bez argumentow
	if(MSG[1]=='0'){ //Komenda 'STOP'
		setPrawySilnik(0);
		setLewySilnik(0);
		stop();
		stan=0;
		return;
	}

	if(MSG[1]=='1'){ //Komenda 'START'
		stan=1;
		return;
	}

	if(MSG[1]=='5'){ //Komenda 'OPUSC'
		dynamixel_ustawPozycje(0.82);
		return;
	}

	if(MSG[1]=='6'){ //Komenda 'PODNIES'
		dynamixel_ustawPozycje(0.51);
		return;
	}

	//Komendy z argumentem liczbowym (INT)
	if(MSG_size<4)
		return;
	int mnoznik=1,wartosc_argumentu=0,poczatek_argumentu,i=0; //zmienne pomocnicze
	if(MSG[3]=='-'){
		poczatek_argumentu = 4;
		mnoznik=-1;
	}
	else{
		poczatek_argumentu=3;
		mnoznik=1;
	}
	int tmp=1;
	for(i=MSG_size-1; i>=poczatek_argumentu; i--){
		if(MSG[i]<'0' || MSG[i]>'9')
			return;
		wartosc_argumentu += tmp*(MSG[i]-'0');
		tmp*=10;

	}
	wartosc_argumentu*=mnoznik; //jezeli minus

	if(MSG[1]=='2'){ //Komenda "GLOBAL VMAX"
		V_max = wartosc_argumentu/100.;
		return;
	}

	if(MSG[1]=='3'){ //Komenda "PWM R"
		setPrawySilnik(wartosc_argumentu/100.);
		return;
	}

	if(MSG[1]=='4'){ //Komenda "PWM L"
		setLewySilnik(wartosc_argumentu/100.);
		return;
	}
	if(MSG[1]=='7'){ //Komenda "PWM L"
		stan=wartosc_argumentu;

		return;
	}
	if(MSG[1]=='8'){ //Komenda "Vobr_max"
		Vobr_max=wartosc_argumentu;
		return;
	}
	if(MSG[1]=='P'){ //Komenda "Kp"
		kp=wartosc_argumentu/100.;
		return;
	}
	if(MSG[1]=='I'){ //Komenda "Ti"
		Ti=wartosc_argumentu;
		return;
	}
	if(MSG[1]=='D'){ //Komenda "Td"
		Td=wartosc_argumentu/100.;
		return;
	}
}

void spacja(unsigned short int *tab, int *i){
	tab[*i]=' ';
	(*i)++;
}

void printINT(int a, unsigned short int *tab, int *i){
	int cyfra;
	int reszta=a;
	if(a<0){ //minus
		reszta=-a;
		a=-a;
	    tab[*i]='-';
	    (*i)++;
	}

	if(a>=100000){ //za duza liczba
		tab[*i]='?';
		(*i)++;
		return;
	}

	if(a>=10000){ //cyfra dziesiatek-tysiecy
		cyfra=reszta/10000;
		reszta=reszta%10000;
		tab[*i]=cyfra+'0';
		(*i)++;
	}
	if(a>=1000){ //cyfra tysiecy
		cyfra=reszta/1000;
		reszta=reszta%1000;
		tab[*i]=cyfra+'0';
		(*i)++;
	}
	if(a>=100){ //cyfra setek
		cyfra=reszta/100;
		reszta=reszta%100;
		tab[*i]=cyfra+'0';
		(*i)++;
	}
	if(a>=10){ //cyfra dzisiatek
		cyfra=reszta/10;
		reszta=reszta%10;
		tab[*i]=cyfra+'0';
		(*i)++;
	}
	if(a>=0){ //cyfra jednosci
		tab[*i]=reszta+'0';
		(*i)++;
	}

}

void melduj(){
	unsigned short int MESSAGE[300];
	int i=0,j=0;
/*
	//KTIRY
	for(;i<20;i++)
		MESSAGE[i]=KTIRy[i+4]+'0';
	spacja(MESSAGE,&i);

	//PWM_L
	int PWM_L_PROC = (int) (PWM_L*100); //PROCENT
	printINT(PWM_L_PROC , MESSAGE, &i);
	spacja(MESSAGE,&i);

	//PWM_R
	int PWM_R_PROC = (int) (PWM_R*100); //PROCENT
	printINT(PWM_R_PROC , MESSAGE, &i);
	spacja(MESSAGE,&i);

	//SHARPY
	int k=0;
	for(;k<4;k++){
		printINT(SHARP_AVG[k],MESSAGE,&i);
		spacja(MESSAGE,&i);
	}

	//KIER
	printINT((int)KIER,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//POSX
	printINT((int)(POSX) ,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//POSY
	printINT((int)(POSY) ,MESSAGE,&i);
	spacja(MESSAGE,&i);



	//RAMKA
	printINT(RAMKA,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//VL
	printINT(V_L,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//VR
	printINT(V_R,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//stan
	printINT(stan,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//wieziemyPuszke
	printINT(wieziemyPuszke,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//przedNamiPuszka
	printINT(przedNamiPuszka,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//ramkaTimer
	printINT(KOMPASX_minVal,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//cofanieTimer
	printINT(KOMPASX_maxVal,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//doc_POSX
	printINT((int)(doc_POSX),MESSAGE,&i);
	spacja(MESSAGE,&i);

	//doc_POSY
	printINT((int)(doc_POSY),MESSAGE,&i);
	spacja(MESSAGE,&i);

	//doc_kierunek
	printINT((int)(doc_KIER),MESSAGE,&i);
	spacja(MESSAGE,&i);

	//nast_POSX
	printINT(KOMPASY_minVal,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//nast_POSY
	printINT(KOMPASY_maxVal,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//nastIstnieje
	printINT(podStan,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//jestesmyUSiebie
	printINT(jestesmyUSiebie,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//loopCounter
	printINT(loopCounter,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//TIME
	printINT(TIME/1000,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//KOMPAS_X
	printINT((int)KOMPAS_X,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//KOMPAS_Y
	printINT((int)KOMPAS_Y,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//KOMPAS_Z
	printINT((int)KOMPAS_Z,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//CZASY ODWIEDZENIA SKRZYZOWAN
	int tmp1,tmp2;
	for(tmp1=0;tmp1<5;tmp1++)
		for(tmp2=0;tmp2<5;tmp2++){
			printINT(TIME_ENEMY[tmp2][tmp1]/1000,MESSAGE,&i);
			spacja(MESSAGE,&i);
		}

	//BATERIA
	printINT((int)BATERIA,MESSAGE,&i);
	spacja(MESSAGE,&i);



	//Ti
	printINT((int)Ti,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//Td
	printINT((int)100*Td,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//kp
	printINT((int)100*kp,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//signal P
	printINT((int)100*signal_p,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//signal I
	printINT((int)100*signal_i,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//signal D
	printINT((int)100*signal_d,MESSAGE,&i);
	spacja(MESSAGE,&i);

	//dane z ultradŸwiêków                              //*****!!######  ZMIANA ->
	printINT((int)ultraMM[0],MESSAGE,&i);
	spacja(MESSAGE,&i);
	printINT((int)ultraMM[1],MESSAGE,&i);
	spacja(MESSAGE,&i);
	printINT((int)ultraMM[2],MESSAGE,&i);
	spacja(MESSAGE,&i);
														//*****!!###### KONIEC ZMIANY
	//znaki konca wiadomosci i wysylanie
	MESSAGE[i]=13;
	i++;
	MESSAGE[i]=10;
	i++;
	uart2_sendArray(MESSAGE,i+1);

	data_number = i;*/
}
//****************************************************//
//***************__I2C_protocol__*********************//
//****************************************************//

//write one byte to slave device; adress - device adress, data - array name, length - length of the array
//function used only in gyro config (once at the begining of the main program)
void __i2c_write(uint8_t address, uint8_t* data, uint32_t length)
{
   uint32_t dummy;

   I2C1_CR1_START_bb = 1;              // request a start
   while (I2C1_SR1_SB_bb == 0);        // wait for start to finish
   dummy = I2C1->SR1;                  // read of SR1 clears the flag
   I2C1->DR = address;                 // transfer address
   while (I2C1_SR1_ADDR_bb == 0);      // wait for address transfer
   dummy = I2C1->SR1;                  // clear the flag
   dummy = I2C1->SR2;

   while (length > 0)                  // transfer whole block
   {
      while (I2C1_SR1_TxE_bb == 0);      // wait for DR empty
      I2C1->DR = *data++;               // transfer one byte, increment pointer
      length--;
   }

   while (I2C1_SR1_TxE_bb == 0 || I2C1_SR1_BTF_bb == 1);   // wait for bus not-busy
   I2C1_CR1_STOP_bb = 1;               // request a stop
}

//read 6 bytes from slave device; address - device address, reg_address - first reg. address
//data - name of the array to store measured values
void __i2c_read(uint8_t address,uint8_t reg_address, uint8_t* data)
{

		uint32_t dummy;
		uint8_t iter = 0;

		I2C1_CR1_START_bb = 1;
		while( I2C1_SR1_SB_bb == 0);
		I2C1->DR = address;
		while(I2C1_SR1_ADDR_bb == 0);
		dummy = I2C1->SR2;
		while(I2C1_SR1_TxE_bb == 0);
		I2C1->DR = reg_address | 0x80;        //0x80 - causes auto-incrementation (readed block of data)
		while(I2C1_SR1_BTF_bb == 0);
		I2C1_CR1_START_bb = 1;
		while(I2C1_SR1_SB_bb == 0);
		I2C1->DR = address | 0x01;
		while(I2C1_SR1_ADDR_bb == 0);
		dummy = I2C1->SR2;

		while( iter < 6 )
		{
		   if( iter == 5 )
			   I2C1_CR1_ACK_bb = 0;

		   while( I2C1_SR1_RxNE_bb == 0);
		   data[iter] = I2C1->DR;
		   iter++;

		}

		I2C1_CR1_STOP_bb = 1;
		I2C1_CR1_ACK_bb = 1;
}

//****************************************************//
//***************__I2C_protocol_END__*****************//
//****************************************************//
