#include <iostream>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <fstream>
#include "wtypes.h"

#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4
//colors ----------------------
#define WORD_1 1
#define WORD_2 2
#define WORD_3 3
#define WORD_4 4
//words -----------------------

#define COLOR_1 1
#define COLOR_2 2
#define COLOR_3 3
#define COLOR_4 4
//answer ----------------------
#define TRIALS 16
#define USED 1
#define UNUSED -1

// GLOBAL ---------------------
double trial_rt[TRIALS], response_time[TRIALS];
// ----------------------------

// P R O T O T Y P E S ---------------------------------------------------------
int trials(ALLEGRO_DISPLAY* display, int color, int words, ALLEGRO_FONT* font, ALLEGRO_FONT* reminder_font, int trialresp);
void crosshair();
void reminders(ALLEGRO_FONT* reminder_font);
int feedback(int word_color, int response, ALLEGRO_FONT* font);
void wrong(ALLEGRO_FONT* font);
void instructions(ALLEGRO_FONT* font, ALLEGRO_DISPLAY* display);
// -----------------------------------------------------------------------------

void screen_size(int& horizontal, int& vertical)
{
	RECT desktop;
	
	const HWND hDesktop = GetDesktopWindow();

	GetWindowRect(hDesktop, &desktop);

	horizontal = desktop.right;
	vertical = desktop.bottom;
}

int main()
{
	al_init();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_install_keyboard();
	int usedArray[TRIALS], usedWordArray[TRIALS], temp, trialType[TRIALS], trialTypeWord[TRIALS], TRIALTYPES = 4, color[TRIALS], words[TRIALS],
		trial_resp[TRIALS], trial_acc[TRIALS], target_type[TRIALS];

	int horizontal = 0, vertical = 0;
	screen_size(horizontal, vertical);

	std::ofstream outfile;
	
	ALLEGRO_DISPLAY* display;
	ALLEGRO_FONT* font;
	ALLEGRO_FONT* inst_font;
	ALLEGRO_FONT* reminder_font;
	ALLEGRO_FONT* back_font;
	
	font = al_load_ttf_font("OpenSans-Light.ttf", 80, NULL);
	inst_font = al_load_ttf_font("OpenSans-Light.ttf", 20, NULL);
	reminder_font = al_load_ttf_font("OpenSans-Light.ttf", 30, NULL);

	outfile.open("Log.txt", std::ios_base::app);
	display = al_create_display(horizontal, vertical);

	al_clear_to_color(al_map_rgb(255, 255, 255));
	instructions(inst_font, display);
	al_flip_display();
	al_rest(5.0);
	al_clear_to_color(al_map_rgb(255, 255, 255));

	unsigned int seed = time(NULL);
	rand();
	
	for (int i = 0; i < TRIALS; i++)
	{
		usedWordArray[i] = UNUSED;
	}
	for (int i = 0; i < TRIALS; i++)
	{
		do
		{
			temp = ((int)TRIALS * rand() / (RAND_MAX + 1));
		} while (usedWordArray[temp] == USED);
		usedWordArray[temp] = USED;
		trialTypeWord[i] = temp;
	}

	for (int i = 0; i < TRIALS; i++)
	{
		if (trialTypeWord[i] < (TRIALS / TRIALTYPES))
		{
			words[i] = WORD_1;
		}
		else if (trialTypeWord[i] < 2 * (TRIALS / TRIALTYPES))
		{
			words[i] = WORD_2;
		}
		else if (trialTypeWord[i] < 3 * (TRIALS / TRIALTYPES))
		{
			words[i] = WORD_3;
		}
		else if (trialTypeWord[i] < 4 * (TRIALS / TRIALTYPES))
		{
			words[i] = WORD_4;
		}
	}

	for (int i = 0; i < TRIALS; i++)
	{
		usedArray[i] = UNUSED;
	}
	for (int i = 0; i < TRIALS; i++)
	{
		do
		{
			temp = ((int)TRIALS * rand() / (RAND_MAX + 1));
		} while (usedArray[temp] == USED);
		usedArray[temp] = USED;
		trialType[i] = temp;
	}

	for (int i = 0; i < TRIALS; i++)
	{
		if (trialType[i] < (TRIALS / TRIALTYPES))
		{
			color[i] = RED;
			
			trial_resp[i] = trials(display, color[i], words[i], font, reminder_font, i);
			target_type[i] = RED;
		}
		else if (trialType[i] < 2 * (TRIALS / TRIALTYPES))
		{
			color[i] = GREEN;
			
			trial_resp[i] = trials(display, color[i], words[i], font, reminder_font, i);
			target_type[i] = GREEN;
		}
		else if (trialType[i] < 3 * (TRIALS / TRIALTYPES))
		{
			color[i] = BLUE;
			
			trial_resp[i] = trials(display, color[i], words[i], font, reminder_font, i);
			target_type[i] = BLUE;
		}
		else if (trialType[i] < 4 * (TRIALS / TRIALTYPES))
		{
			color[i] = YELLOW;
			trial_resp[i] = trials(display, color[i], words[i], font, reminder_font, i);
			target_type[i] = YELLOW;
		}

		trial_acc[i] = feedback(color[i], trial_resp[i], font);
		
	}

	outfile << "trial\tword\tcolor\tres\tRT\tCorrect\n";

	for (int i = 0; i < TRIALS; i++)
	{
		if (trial_resp[i] <= 0)
		{
			trial_resp[i] = 0;
		}
		outfile << i << "\t" << words[i] << "\t" << color[i] << "\t" << trial_resp[i] << "\t" << trial_rt[i] << "\t" << target_type[i] << "\t" << trial_acc[i] << "\n";
	}

	outfile.close();
	al_destroy_font(font);
	al_destroy_font(inst_font);
	al_destroy_display(display);
	return 0;
}

// END OF MAIN -------------------------------------------------------------------------------------------------------------------------

int trials(ALLEGRO_DISPLAY* display, int color, int words, ALLEGRO_FONT* font, ALLEGRO_FONT* reminder_font, int trialresp)
{
	ALLEGRO_KEYBOARD_STATE kbdstate;
	ALLEGRO_COLOR word_color, back_word_color = al_map_rgb(0,0,0);

	int horizontal = 0, vertical = 0;

	screen_size(horizontal, vertical);

	int wordX = horizontal / 2, wordY = vertical / 2;
	double start_time, stimulus_duration, start_timeout, timeout_duration, response = 0;
	
	stimulus_duration = 1.0;
	timeout_duration = 1.0;
	const char* stroop_words[4];
	//al_clear_to_color(al_map_rgb(255, 255, 255));
	al_clear_to_color(al_map_rgb(255, 255, 255));


	if (color == RED)
	{
		word_color = al_map_rgb(255, 0, 0);
	}
	else if (color == GREEN)
	{
		word_color = al_map_rgb(0, 255, 0);
	}
	else if (color == BLUE)
	{
		word_color = al_map_rgb(0, 0, 255);
	}
	else if (color == YELLOW)
	{
		word_color = al_map_rgb(255, 255, 0);
	}

	if (words == WORD_1)
	{
		
		al_draw_text(font, word_color, wordX, wordY, ALLEGRO_ALIGN_CENTRE, "RED");
		
	}
	else if (words == WORD_2)
	{
		
		al_draw_text(font, word_color, wordX, wordY, ALLEGRO_ALIGN_CENTRE, "GREEN");
		
	}
	else if (words == WORD_3)
	{
		
		al_draw_text(font, word_color, wordX, wordY, ALLEGRO_ALIGN_CENTRE, "BLUE");
		
	}
	else if (words == WORD_4)
	{
		
		al_draw_text(font, word_color, wordX, wordY, ALLEGRO_ALIGN_CENTRE, "YELLOW");
		
	}

	start_time = al_get_time();
	reminders(reminder_font);
	al_flip_display();
	al_rest(.5);
	al_clear_to_color(al_map_rgb(255, 255, 255));
	

	while (al_get_time() - start_time < stimulus_duration)
	{
		
		al_get_keyboard_state(&kbdstate);

		if (al_key_down(&kbdstate, ALLEGRO_KEY_D))
		{
			response = RED, response_time[trialresp] = al_get_time();
		}
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_J))
		{
			response = BLUE, response_time[trialresp] = al_get_time();
		}
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F))
		{
			response = GREEN, response_time[trialresp] = al_get_time();
		}
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_K))
		{
			response = YELLOW, response_time[trialresp] = al_get_time();
		}

	}
	
	al_clear_to_color(al_map_rgb(255, 255, 255));
	crosshair();
	reminders(reminder_font);
	al_flip_display();
	start_timeout = al_get_time();
	
	while (al_get_time() - start_timeout < stimulus_duration)
	{
		
		al_get_keyboard_state(&kbdstate);

		if (al_key_down(&kbdstate, ALLEGRO_KEY_D))
		{
			response = RED, response_time[trialresp] = al_get_time();
		}
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_J))
		{
			response = BLUE, response_time[trialresp] = al_get_time();
		}
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F))
		{
			response = GREEN, response_time[trialresp] = al_get_time();
		}
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_K))
		{
			response = YELLOW, response_time[trialresp] = al_get_time();
		}

	}
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_rest(.5);

	//start_time = al_get_time();

	trial_rt[trialresp] = response_time[trialresp] - start_time;
	return response;
}

// END OF TRIALS ----------------------------------------------------------------------------------------------------------------------

void crosshair()
{
	int horizontal = 0, vertical = 0;
	screen_size(horizontal, vertical);
	al_draw_line((horizontal / 2) - 10, vertical / 2, (horizontal / 2) + 10, vertical / 2, al_map_rgb(255, 0, 0), 3);
	al_draw_line(horizontal / 2, (vertical / 2) - 10, horizontal / 2, (vertical / 2) + 10, al_map_rgb(255, 0, 0), 3);
}

void wrong(ALLEGRO_FONT* font)
{
	int horizontal = 0, vertical = 0;
	screen_size(horizontal, vertical);
	al_draw_text(font,al_map_rgb(255,0,0),horizontal / 2, vertical * .75,ALLEGRO_ALIGN_CENTRE,"X");
}

void reminders(ALLEGRO_FONT* reminder_font)
{
	int horizontal = 0, vertical = 0;
	screen_size(horizontal, vertical);
	al_draw_text(reminder_font, al_map_rgb(169, 169, 169), horizontal / 2, vertical * .25, ALLEGRO_ALIGN_CENTRE, "D = RED    F = GREEN    J = BLUE    K = YELLOW");
}

int feedback(int word_color, int response, ALLEGRO_FONT* font)
{
	int correct;
	int horizontal = 0, vertical = 0;
	screen_size(horizontal, vertical);

	if (word_color == response)
	{
		correct = 1;
		al_draw_text(font, al_map_rgb(255, 0, 0), horizontal / 2, vertical * .75, ALLEGRO_ALIGN_CENTRE, "CORRECT");
		al_flip_display();
		al_rest(.5);
		al_clear_to_color(al_map_rgb(255, 255, 255));
		al_flip_display();
	}
	else
	{
		correct = 0;
		wrong(font);
		al_flip_display();
		al_rest(.5);
		al_clear_to_color(al_map_rgb(255, 255, 255));
		al_flip_display();
	}

	return correct;
}

void instructions(ALLEGRO_FONT* inst_font, ALLEGRO_DISPLAY* display)
{
	int width = al_get_display_width(display), height = al_get_display_height(display);

	al_draw_text(inst_font, al_map_rgb(0,0,0), width / 2, height * .25, ALLEGRO_ALIGN_CENTRE, "ANSWER THE COLOR THAT THE WORD SAYS");
	al_draw_text(inst_font, al_map_rgb(0, 0, 0), width / 2, height * .75, ALLEGRO_ALIGN_CENTRE, "NOT THE COLOR OF THE WORD");
}
