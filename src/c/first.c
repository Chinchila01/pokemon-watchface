#include <pebble.h>

#define SETTINGS_KEY 1

#define GROOKEY 0
#define SCORBUNNY 1
#define SOBBLE 2

#define HOURS 0
#define MINUTES 1

typedef struct ClaySettings {
	int pokemon;
	bool cycle;
	int time_unit;
	int minutes_value;
	int last_change_minute;
	int hours_value;
	int last_change_hour;
} __attribute__((__packed__)) ClaySettings;

static Window *s_main_window;

static TextLayer *s_date_layer;
static TextLayer *s_hour_layer;
static TextLayer *s_minute_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_background_layer;

static GFont s_time_font;
static GFont s_date_font;

static GBitmap *s_pokemon_bitmap;
static GBitmap *s_pokeball_bitmap;

static BitmapLayer *s_pokemon_bitmap_layer;
static BitmapLayer *s_pokeball_bitmap_layer;

static int last_change_minute;
static int last_change_hour;

// Settings
ClaySettings settings;

static void prv_default_settings() {
	settings.pokemon = GROOKEY;
	settings.cycle = false;
	settings.time_unit = HOURS;
	settings.minutes_value = 1;
	settings.last_change_minute = 0;
	settings.hours_value = 1;
	settings.last_change_hour = 0;
}

static void prv_update_display() {

	if (settings.pokemon == GROOKEY) {
		text_layer_set_background_color(s_background_layer, GColorMalachite);
		text_layer_set_text_color(s_date_layer, GColorMalachite);
		text_layer_set_text_color(s_battery_layer, GColorMalachite);

		// Create bitmap
		s_pokemon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GROOKEY);
		bitmap_layer_set_bitmap(s_pokemon_bitmap_layer, s_pokemon_bitmap);
	} else if (settings.pokemon == SCORBUNNY) {
		text_layer_set_background_color(s_background_layer, GColorDarkCandyAppleRed);
		text_layer_set_text_color(s_date_layer, GColorDarkCandyAppleRed);
		text_layer_set_text_color(s_battery_layer, GColorDarkCandyAppleRed);

		// Create bitmap
		s_pokemon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SCORBUNNY);
		bitmap_layer_set_bitmap(s_pokemon_bitmap_layer, s_pokemon_bitmap);
	} else { // sobble
		text_layer_set_background_color(s_background_layer, GColorBlueMoon);
		text_layer_set_text_color(s_date_layer, GColorBlueMoon);
		text_layer_set_text_color(s_battery_layer, GColorBlueMoon);
		
		// Create bitmap
		s_pokemon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SOBBLE);
		bitmap_layer_set_bitmap(s_pokemon_bitmap_layer, s_pokemon_bitmap);
	}
}

static void prv_load_settings() {
	prv_default_settings();

	persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

static void prv_save_settings() {
	persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));

	prv_update_display();
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	
	Tuple *t_pokemon = dict_find(iter, MESSAGE_KEY_pokemon);
	if (t_pokemon) {
		settings.pokemon = t_pokemon->value->uint8 - 48;
	}

	Tuple *t_cycle = dict_find(iter, MESSAGE_KEY_cycle);
	if (t_cycle) {
		settings.cycle = t_cycle->value->int32 == 1;
	}

	Tuple *t_time_unit = dict_find(iter, MESSAGE_KEY_time_unit);
	if (t_time_unit) {
		settings.time_unit = t_time_unit->value->uint8 - 48;
	}

	Tuple *t_minutes_value = dict_find(iter, MESSAGE_KEY_minutes_value);
	if (t_minutes_value) {
		settings.minutes_value = t_minutes_value->value->uint8;
		settings.last_change_minute = tick_time->tm_min;
	}
	
	Tuple *t_hours_value = dict_find(iter, MESSAGE_KEY_hours_value);
	if (t_hours_value) {
		settings.hours_value = t_hours_value->value->uint8;
		settings.last_change_hour = tick_time->tm_hour;
	}

	prv_save_settings();
}

static void cycle_pokemon() {

	if (settings.pokemon == SOBBLE) {
		settings.pokemon = GROOKEY;
	} else {
		settings.pokemon++;
	}
}

static void update_time() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	static char s_date_buffer[8];
	strftime(s_date_buffer, sizeof(s_date_buffer),
		"%a,%d", tick_time);

	static char s_hour_buffer[4];
	static char s_minute_buffer[4];
	strftime(s_hour_buffer, sizeof(s_hour_buffer),
	 clock_is_24h_style() ? "%H" : "%I", tick_time);
	strftime(s_minute_buffer, sizeof(s_minute_buffer), "%M", tick_time);

	text_layer_set_text(s_hour_layer, s_hour_buffer);
	text_layer_set_text(s_minute_layer, s_minute_buffer);
	text_layer_set_text(s_date_layer, s_date_buffer);

	if (settings.cycle) {

		if (settings.time_unit == HOURS) {
			int value_to_compare = settings.last_change_hour >= (24 - settings.hours_value) ?
				(settings.last_change_hour - 24) : settings.last_change_hour;
			
			if ((tick_time->tm_hour - value_to_compare) == settings.hours_value) {
				settings.last_change_hour = tick_time->tm_hour;
				cycle_pokemon();
				prv_save_settings();
			}
		} else if (settings.time_unit == MINUTES) {
			int value_to_compare = settings.last_change_minute >= (60 - settings.minutes_value) ?
				(settings.last_change_minute - 60) : settings.last_change_minute;
			
			if ((tick_time->tm_min - settings.last_change_minute) == settings.minutes_value) {
				settings.last_change_minute = tick_time->tm_min;
				cycle_pokemon();
				prv_save_settings();
			}
		}
	}
}

static void battery_state_handler(BatteryChargeState charge) {

	static char s_battery_buffer[5];
	snprintf(s_battery_buffer, sizeof(s_battery_buffer),
	 "%d%c", charge.charge_percent,
	  charge.is_charging ? '*' : ' ');

	text_layer_set_text(s_battery_layer, s_battery_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

static void main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	int WINDOW_WIDTH = 144;
	int WINDOW_HEIGHT = 168;
	int TIME_FONT_WIDTH = 36;
	int TIME_FONT_HEIGHT = 48;

	// Load custom font
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_18));

	// Create background layer - ALWAYS FIRST
	int BACKGROUND_HEIGHT = WINDOW_HEIGHT/2;
	int backgroundY = (0.5 * WINDOW_HEIGHT) - BACKGROUND_HEIGHT/2;

	s_background_layer = text_layer_create(
		GRect(0, backgroundY, bounds.size.w, BACKGROUND_HEIGHT)
	);

	//text_layer_set_background_color(s_background_layer, GColorMalachite);
	layer_add_child(window_layer, text_layer_get_layer(s_background_layer));

	// Create hours layer
	int hoursY = (0.5 * WINDOW_HEIGHT) - TIME_FONT_HEIGHT - 5;
	
	s_hour_layer = text_layer_create(
		GRect(0.5 * bounds.size.w, hoursY, 2 * TIME_FONT_WIDTH, TIME_FONT_HEIGHT)
	);

	text_layer_set_background_color(s_hour_layer, GColorClear);
	text_layer_set_text_color(s_hour_layer, GColorBlack);
	text_layer_set_font(s_hour_layer, s_time_font);
	text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);
	text_layer_set_overflow_mode(s_hour_layer, GTextOverflowModeWordWrap);

	layer_add_child(window_layer, text_layer_get_layer(s_hour_layer));

	// Create minutes layer
	int minutesY = (0.5 * WINDOW_HEIGHT) - 10;
	
	s_minute_layer = text_layer_create(
		GRect(0.5 * bounds.size.w, minutesY, 2 * TIME_FONT_WIDTH, TIME_FONT_HEIGHT)
	);

	text_layer_set_background_color(s_minute_layer, GColorClear);
	text_layer_set_text_color(s_minute_layer, GColorBlack);
	text_layer_set_font(s_minute_layer, s_time_font);
	text_layer_set_text_alignment(s_minute_layer, GTextAlignmentCenter);
	text_layer_set_overflow_mode(s_minute_layer, GTextOverflowModeWordWrap);

	layer_add_child(window_layer, text_layer_get_layer(s_minute_layer));

	// Create date layer
	s_date_layer = text_layer_create(
		GRect(0, 0, bounds.size.w, 18)
	);

	text_layer_set_background_color(s_date_layer, GColorClear);
	//text_layer_set_text_color(s_date_layer, GColorMalachite);
	text_layer_set_font(s_date_layer, s_date_font);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);

	layer_add_child(window_layer, text_layer_get_layer(s_date_layer));

	// Create battery layer
	s_battery_layer = text_layer_create(
		GRect(0, 0, bounds.size.w, 18)
	);

	text_layer_set_background_color(s_battery_layer, GColorClear);
	//text_layer_set_text_color(s_battery_layer, GColorMalachite);
	text_layer_set_font(s_battery_layer, s_date_font);
	text_layer_set_text_alignment(s_battery_layer, GTextAlignmentLeft);

	layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));

	// Create pokemon bitmap
	int POKEMON_BITMAP_HEIGHT = 90;
	int POKEMON_BITMAP_WIDTH = 72;
	// Create bitmap

	int grookeyY = (0.5 * bounds.size.h) - (0.5 * POKEMON_BITMAP_HEIGHT);

	s_pokemon_bitmap_layer = bitmap_layer_create(GRect(3, grookeyY, POKEMON_BITMAP_WIDTH, POKEMON_BITMAP_HEIGHT));
	bitmap_layer_set_compositing_mode(s_pokemon_bitmap_layer, GCompOpSet);

	layer_add_child(window_get_root_layer(window),
		bitmap_layer_get_layer(s_pokemon_bitmap_layer));

	// Create pokeball bitmap
	int POKEBALL_HEIGHT = 25;
	int POKEBALL_WIDTH = 25;

	s_pokeball_bitmap = gbitmap_create_with_resource(RESOURCE_ID_POKEBALL);

	int pokeballX = (0.5 * bounds.size.w) - (0.5 * POKEBALL_WIDTH);
	int pokeballY = bounds.size.h - POKEBALL_HEIGHT - 5;

	s_pokeball_bitmap_layer = bitmap_layer_create(GRect(pokeballX, pokeballY, POKEBALL_WIDTH, POKEBALL_HEIGHT));
	bitmap_layer_set_compositing_mode(s_pokeball_bitmap_layer, GCompOpSet);
	bitmap_layer_set_bitmap(s_pokeball_bitmap_layer, s_pokeball_bitmap);

	layer_add_child(window_get_root_layer(window),
		bitmap_layer_get_layer(s_pokeball_bitmap_layer));

	prv_update_display();
}

static void main_window_unload(Window *window) {
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_hour_layer);
	text_layer_destroy(s_minute_layer);
	text_layer_destroy(s_battery_layer);
	text_layer_destroy(s_background_layer);

	fonts_unload_custom_font(s_time_font);
	fonts_unload_custom_font(s_date_font);

	gbitmap_destroy(s_pokemon_bitmap);
	bitmap_layer_destroy(s_pokemon_bitmap_layer);

	gbitmap_destroy(s_pokeball_bitmap);
	bitmap_layer_destroy(s_pokeball_bitmap_layer);
}

static void init() {
	// Load settings
	prv_load_settings();

	// Open AppMessage connection
	app_message_register_inbox_received(prv_inbox_received_handler);
	app_message_open(128, 128);
	
	// Create main Window element and assign to pointer
	s_main_window = window_create();
	window_set_background_color(s_main_window, GColorBlack);

	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	window_stack_push(s_main_window, true);
	
	update_time();
	battery_state_handler(battery_state_service_peek());

	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	battery_state_service_subscribe(battery_state_handler);
}

static void deinit() {
	app_message_deregister_callbacks();
	tick_timer_service_unsubscribe();
	battery_state_service_unsubscribe();

	// Destroy window
	window_destroy(s_main_window);
}

int main (void) {

	init();
	app_event_loop();
	deinit();
}
