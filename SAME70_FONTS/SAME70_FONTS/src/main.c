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
#define WEEK		12
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
/* PROTOTYPES                                                           */
/************************************************************************/

static void RTT_init(uint16_t pllPreScale, uint32_t IrqNPulses);

/************************************************************************/
/* Variaveis globais                                                           */
/************************************************************************/

volatile int N = 0;
volatile Bool butR_flag;
volatile Bool f_rtt_alarme = false;
volatile Bool f_rtt_alarme1 = false;
int n=0;

/************************************************************************/
/* Handlers                                                             */
/************************************************************************/

/**
* \brief Interrupt handler for the RTC. Refresh the display.
*/
void RTT_Handler(void)
{
	uint32_t ul_status;

	/* Get RTT status */
	ul_status = rtt_get_status(RTT);

	/* IRQ due to Time has changed */
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {  }

	/* IRQ due to Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		f_rtt_alarme = true;                  // flag RTT alarme
	}
}


/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/

static float get_time_rtt(){
	uint ul_previous_time = rtt_read_timer_value(RTT);
}


static void RTT_init(uint16_t pllPreScale, uint32_t IrqNPulses)
{
	uint32_t ul_previous_time;

	/* Configure RTT for a 1 second tick interrupt */
	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);
	
	ul_previous_time = rtt_read_timer_value(RTT);
	while (ul_previous_time == rtt_read_timer_value(RTT));
	
	rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, RTT_MR_ALMIEN);
}

void butr_callback(void)
{
	N++;
	butR_flag = true;
}

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
	
	
	pio_set_debounce_filter(BUTR_PIO,BUTR_IDX_MASK,4);
	
	
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
	
	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;

	/** Configura RTC */

	io_init();

	f_rtt_alarme = true;
	f_rtt_alarme1 = true;
	
	font_draw_text(&calibri_36, "Distancia:", 10, 100, 1);
	font_draw_text(&calibri_36, "Tempo:", 10, 50, 1);
	font_draw_text(&calibri_36, "Velocidade:", 10, 150, 2);
	
	
	while(1) {
		char bufferD[32];
		char bufferV[32];

		char bufferSeg[32];
		char bufferMin[32];
		char bufferHour[32];
		int seg=0;
		int min=0;
		int hour=0;
		
		//if (f_rtt_alarme1){
			//seg=rtt_read_timer_value(RTT);
			//if (seg>60){
				//min++;
				//seg-=60;
			//}
			//if (min>60){
				//hour++;
				//min-=60;
				//
//
			//}
			//
			////Tempo
			//sprintf(bufferSeg, "%d",seg);
			//font_draw_text(&calibri_36, bufferSeg, 250, 50, 1);
			//sprintf(bufferSeg, "%d",min);
			//font_draw_text(&calibri_36, bufferMin, 200, 50, 1);
			//sprintf(bufferSeg, "%d",hour);
			//font_draw_text(&calibri_36, bufferHour, 150, 50, 1);
			//
			//uint16_t pllPreScale = (int) (((float) 32768) / 2.0);
			//uint32_t irqRTTvalue  = 2;
			//RTT_init(pllPreScale, irqRTTvalue);
			//f_rtt_alarme = false;
		//}
		
		if(butR_flag){
			
			int dist= 2*PI*N;
			sprintf(bufferD, "%d",dist);
			
			
			butR_flag = false;
		}
		
		
		
		if (f_rtt_alarme){
					
					//Distancia
					font_draw_text(&calibri_36,  bufferD, 200, 100, 1);
					
					
					
					//Velocidade
					int vel =(2*PI*N-2*PI*n)/rtt_read_timer_value(RTT);
					sprintf(bufferV, "%d",vel);
					font_draw_text(&calibri_36, bufferV, 230, 150, 2);
					n=N;
					
					
					
					
					uint16_t pllPreScale = (int) (((float) 32768) / 2.0);
					uint32_t irqRTTvalue  = 8;
					RTT_init(pllPreScale, irqRTTvalue); 
     
					f_rtt_alarme = false;
		}
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
		
		

		
		
		
		
		
		
		//Tempo
		//font_draw_text(&arial_72, "102456", 50, 200, 2);
		
		
		
		
	}
}