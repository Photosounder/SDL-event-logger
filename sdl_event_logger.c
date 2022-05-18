#include "rl.h"

void test_control_window()
{
	static double knob_value = NAN;

	// GUI layout (so far just one knob under the image)
	static gui_layout_t layout={0};
	const char *layout_src[] = {
		"elem 0", "type none", "label Test control", "pos	0	0", "dim	3	3;6", "off	0	1", "",
		"elem 10", "type knob", "label Knob", "knob 0 0.5 1 linear %.3f", "pos	0;6	-1", "dim	2", "off	0	1", "",
	};

	make_gui_layout(&layout, layout_src, sizeof(layout_src)/sizeof(char *), "Test control");

	// GUI window
	static flwindow_t window={0};
	flwindow_init_defaults(&window);
	window.bg_opacity = 0.94;
	window.shadow_strength = 0.5*window.bg_opacity;
	draw_dialog_window_fromlayout(&window, cur_wind_on, &cur_parent_area, &layout, 0);	// this handles and displays the window that contains the control

	// GUI controls
	ctrl_knob_fromlayout(&knob_value, &layout, 10);			// this both displays the control and updates the gain value
}

void log_window()
{
	// GUI layout (so far just one knob under the image)
	static gui_layout_t layout={0};
	const char *layout_src[] = {
		"elem 0", "type none", "label Event log", "pos	0	0", "dim	12	7;6", "off	0	1", "",
		"elem 10", "type textedit", "pos	0;6	-1", "dim	11	6", "off	0	1", "",
	};

	make_gui_layout(&layout, layout_src, sizeof(layout_src)/sizeof(char *), "Test control");

	// GUI window
	static flwindow_t window={0};
	flwindow_init_defaults(&window);
	window.bg_opacity = 0.94;
	window.shadow_strength = 0.5*window.bg_opacity;
	draw_dialog_window_fromlayout(&window, cur_wind_on, &cur_parent_area, &layout, 0);	// this handles and displays the window that contains the control

	// GUI controls

	// Console parameters
	textedit_t *te=NULL;
	te = get_textedit_fromlayout(&layout, 10);
	te->read_only = 1;
	te->draw_string_mode = ALIG_LEFT | MONOSPACE;
	te->scroll_mode = 1;
	te->scroll_mode_scale_def = 120. * 4.5;
	

	// Scroll down console
	if (te->vert_scroll.down == 0)
		te->scroll_pos.y = -1e30;

	te->string = print_log.buf;
	te->alloc_size = print_log.as;

	ctrl_textedit_fromlayout(&layout, 10);
}

void event_logger_root()
{
	int i;
	static int test_wind_on=0, log_wind_on=0;

	// GUI windows
	window_register(1, test_control_window, NULL, make_rect_off(xy(zc.limit_u.x-0.25, 8.), xy(3., 3.), xy(1., 1.)), &test_wind_on, 0);
	window_register(1, log_window, NULL, make_rect_off(XY0, xy(24., 16.), xy(0.5, 0.5)), &log_wind_on, 0);
}

void main_loop()
{
	static int exit_flag=0, init=1;
	SDL_Event event;

	if (init)
	{
		init = 0;

		fb.use_drawq = 0;	// OpenCL draw queue

		sdl_graphics_init_autosize("SDL Event Logger", SDL_WINDOW_RESIZABLE, 0);
		SDL_MaximizeWindow(fb.window);

		zc = init_zoom(&mouse, drawing_thickness);
		calc_screen_limits(&zc);
		mouse = init_mouse();

		vector_font_load_from_header();

		#ifdef __EMSCRIPTEN__
		SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#screen");
		#endif

		// Print notice
		fprintf_rl(stdout, "Middle-click or press Alt-Z to toggle the warp mode which is where the problem is.\n\n");
	}

	#ifdef __EMSCRIPTEN__
	#else
	while (exit_flag==0)
	#endif
	{
		//********Input handling********

		mouse_pre_event_proc(&mouse);
		keyboard_pre_event_proc(&mouse);
		sdl_handle_window_resize(&zc);

		while (SDL_PollEvent(&event))
		{
			// Print relevant events to log
			//if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) fprintf_rl(stdout, "MOUSEBUTTON%-4s button %d, clicks %d, time %d\n", event.type==SDL_MOUSEBUTTONDOWN ? "DOWN" : "UP", event.button.button, event.button.clicks, event.button.timestamp);
			//if (event.type == SDL_MOUSEWHEEL) fprintf_rl(stdout, "SDL_MOUSEWHEEL %d, time %d\n", event.wheel.y, event.wheel.timestamp);
			if (event.type == SDL_MOUSEMOTION) fprintf_rl(stdout, "SDL_MOUSEMOTION x %4d y %4d, xrel %4d yrel %4d, time %6d\n", event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel, event.motion.timestamp);

			dropfile_event_proc(event);
			sdl_mouse_event_proc(&mouse, event, &zc);
			sdl_keyboard_event_proc(&mouse, event);

			if (event.type==SDL_QUIT)
				exit_flag = 1;
		}

		if (mouse.key_state[RL_SCANCODE_RETURN] == 2 && get_kb_alt())
			sdl_toggle_borderless_fullscreen();

		textedit_add(cur_textedit, NULL);	// processes the new keystrokes in the current text editor

		mouse_post_event_proc(&mouse, &zc);

		//-------------input-----------------

		event_logger_root();

		gui_layout_edit_toolbar(mouse.key_state[RL_SCANCODE_F6]==2);
		window_manager();

		mousecursor_logic_and_draw();

		sdl_flip_fb();
	}
}

int main(int argc, char *argv[])
{
	#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
	#else
	main_loop();
	#endif

	sdl_quit_actions();

	return 0;
}
