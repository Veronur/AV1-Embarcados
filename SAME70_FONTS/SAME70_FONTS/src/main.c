/*
 * main.c
 *
 * Created: 08/04/2019 15:05:58
 *  Author: Lucca Delchiaro Costabile
 */ 

/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/

/**
 *  Informacoes para o RTC
 *  poderia ser extraida do __DATE__ e __TIME__
 *  ou ser atualizado pelo PC.
 */
#define YEAR        2019
#define MOUNTH      4
#define DAY         8
#define DAY         20
#define HOUR        15
#define MINUTE      10
#define SECOND      0


/**
* Botão
*/
#define BUTR_PIO_ID			  ID_PIOA
#define BUTR_PIO				  PIOA
#define BUTR_IDX				  4
#define BUTR_IDX_MASK			  (1 << BUTR_IDX)

#define BUTP_PIO_ID			  ID_PIOA
#define BUTP_PIO				  PIOA
#define BUTP_IDX				  3
#define BUTP_IDX_MASK			  (1 << BUTP_IDX)

#include <asf.h>
#include "tfont.h"
#include "sourcecodepro_28.h"
#include "calibri_36.h"
#include "arial_72.h"

#define PI 3.14

struct ili9488_opt_t g_ili9488_display_opt;


/************************************************************************/
/* Variaveis globais                                                           */
/************************************************************************/

volatile int N = 0;
volatile Bool butR_flag;

/************************************************************************/
/* Handlers                                                             */
/************************************************************************/


/**
*  Interrupt handler for TC1 interrupt.
*/
//void tc1_handler(void){
	//volatile uint32_t ul_dummy;
//
	///****************************************************************
	//* devemos indicar ao tc que a interrupção foi satisfeita.
	//******************************************************************/
	//ul_dummy = tc_get_status(tc0, 1);
//
	///* avoid compiler warning */
	//unused(ul_dummy);
//
//}

/**
* \brief Interrupt handler for the RTC. Refresh the display.
*/
//void RTC_Handler(void)
//{	
	///* Iniciar variaveis para setar localmente o horario atual durante a interrupcao*/
	//unsigned int hora, minuto, segundo;
	//uint32_t ul_status = rtc_get_status(RTC);
//
	///*
	//*  Verifica por qual motivo entrou
	//*  na interrupcao, se foi por segundo
	//*  ou Alarm
	//*/
	//if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		//rtc_clear_status(RTC, RTC_SCCR_SECCLR);
	//}
	//
	///* Time or date alarm */
	///* O que vai fazer na interrupcao*/
	//if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
			//rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
//
			//flag_led0 = !flag_led0;
			//
	//}
	//
	////Para o alarme continuar mesmo dps de 1 minuto.
	//if (segundo >=59){
		//minuto +=1;
	//}
	///*Escrever nas variaveis o horario atual*/
	//rtc_get_time(RTC, &hora,&minuto,&segundo);
	///*A cada interrupcao iniciar o proximo alarme*/
	//rtc_set_time_alarm(RTC,1,hora,1,minuto,1,segundo+5);
	//rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
	//rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
	//rtc_clear_status(RTC, RTC_SCCR_CALCLR);
	//rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);
	//
//}

/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/



//void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	//uint32_t ul_div;
	//uint32_t ul_tcclks;
	//uint32_t ul_sysclk = sysclk_get_cpu_hz();
//
	//uint32_t channel = TC_CHANNEL;
//
	///* Configura o PMC */
	///* O TimerCounter é meio confuso
	//o uC possui 3 TCs, cada TC possui 3 canais
	//TC0 : ID_TC0, ID_TC1, ID_TC2
	//TC1 : ID_TC3, ID_TC4, ID_TC5
	//TC2 : ID_TC6, ID_TC7, ID_TC8
	//*/
	//pmc_enable_periph_clk(ID_TC);
//
	///** Configura o TC para operar em  4Mhz e interrupçcão no RC compare */
	//tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	//tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	//tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);
//
	///* Configura e ativa interrupçcão no TC canal 0 */
	///* Interrupção no C */
	//NVIC_EnableIRQ((IRQn_Type) ID_TC);
	//tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);
//
	///* Inicializa o canal 0 do TC */
	//tc_start(TC, TC_CHANNEL);
//}

//void RTC_init(){
	///* Configura o PMC */
	//pmc_enable_periph_clk(ID_RTC);
//
	///* Default RTC configuration, 24-hour mode */
	//rtc_set_hour_mode(RTC, 0);
//
	///* Configura data e hora manualmente */
	//rtc_set_date(RTC, YEAR, MOUNTH, DAY, WEEK);
	//rtc_set_time(RTC, HOUR, MINUTE, SECOND);
//
	///* Configure RTC interrupts */
	//NVIC_DisableIRQ(RTC_IRQn);
	//NVIC_ClearPendingIRQ(RTC_IRQn);
	//NVIC_SetPriority(RTC_IRQn, 0);
	//NVIC_EnableIRQ(RTC_IRQn);
//
	///* Ativa interrupcao via alarme */
	//rtc_enable_interrupt(RTC,  RTC_IER_ALREN);
//
//}

void butr_callback(void)
{
	N++;
	butR_flag = true;
}

/*Inicia a configuracao da interrupcao do botao*/
void io_init(void)
{


	// Inicializa clock do periférico PIO responsavel pelo botao
	pmc_enable_periph_clk(BUTR_PIO_ID);

	// Configura PIO para lidar com o pino do botão como entrada
	// com pull-up
	pio_configure(BUTR_PIO, PIO_INPUT, BUTR_IDX_MASK, PIO_PULLUP);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: butr_callback()
	pio_handler_set(BUTR_PIO,
	BUTR_PIO_ID,
	BUTR_IDX_MASK,
	PIO_IT_RISE_EDGE,
	butr_callback);
	
	
	pio_set_debounce_filter(BUTR_PIO,BUTR_IDX_MASK,1);
	
	
	// Ativa interrupção
	pio_enable_interrupt(BUTR_PIO, BUTR_IDX_MASK);

	// Configura NVIC para receber intrrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(BUTR_PIO_ID);
	NVIC_SetPriority(BUTR_PIO_ID, 4); // Prioridade 4
}

void configure_lcd(void){
	/* Initialize display parameter */
	g_ili9488_display_opt.ul_width = ILI9488_LCD_WIDTH;
	g_ili9488_display_opt.ul_height = ILI9488_LCD_HEIGHT;
	g_ili9488_display_opt.foreground_color = COLOR_CONVERT(COLOR_WHITE);
	g_ili9488_display_opt.background_color = COLOR_CONVERT(COLOR_WHITE);

	/* Initialize LCD */
	ili9488_init(&g_ili9488_display_opt);
	ili9488_draw_filled_rectangle(0, 0, ILI9488_LCD_WIDTH-1, ILI9488_LCD_HEIGHT-1);
	
}


void font_draw_text(tFont *font, const char *text, int x, int y, int spacing) {
	char *p = text;
	while(*p != NULL) {
		char letter = *p;
		int letter_offset = letter - font->start_char;
		if(letter <= font->end_char) {
			tChar *current_char = font->chars + letter_offset;
			ili9488_draw_pixmap(x, y, current_char->image->width, current_char->image->height, current_char->image->data);
			x += current_char->image->width + spacing;
		}
		p++;
	}	
}


int main(void) {
	board_init();
	sysclk_init();	
	configure_lcd();

	/** Configura RTC */
	//RTC_init();
	// configura botao com interrupcao
	io_init();
	
	while(1) {
		char buffer[32];
		if(butR_flag){
			
			int dist= 2*PI*N;
			sprintf(buffer, "%d",dist);
			butR_flag = false;
		}
		
		
		
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
		
		//Distancia
		font_draw_text(&calibri_36, buffer, 50, 100, 1);
		
		//Tempo
		font_draw_text(&sourcecodepro_28, "OIMUNDO", 50, 50, 1);
		
		//Velocidade
		font_draw_text(&arial_72, "102456", 50, 200, 2);
		
		
		
		
	}
}