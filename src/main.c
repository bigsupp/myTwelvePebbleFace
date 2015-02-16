#include "pebble.h"

static Window *window;

static Layer *path_layer;

static const int HEIGHT_OFFSET = -6;

static TextLayer *textMinute;
static TextLayer *textSecond;
static TextLayer *textDate;

static const GPathInfo HAND_HOUR_PATH_POINT = {
  12,
  (GPoint []) {
		{52,-30},
		{30,-52},
		{-0,-60},
		{-30,-52},
		{-52,-30},
		{-60,0},
		{-52,30},
		{-30,52},
		{0,60},
		{30,52},
		{52,30},
		{60,0}
	}
};

static GPath *handHour_path = NULL;

static int path_angle = 0;
static bool outline_mode = true;

// This is the layer update callback which is called on render updates
static void path_layer_update_callback(Layer *me, GContext *ctx) {
  (void)me;

  // You can rotate the path before rendering
  gpath_rotate_to(handHour_path, (TRIG_MAX_ANGLE / 360) * path_angle);

  if (outline_mode) {
    // draw outline uses the stroke color
    graphics_context_set_stroke_color(ctx, GColorWhite);
    gpath_draw_outline(ctx, handHour_path);
  } else {
    // draw filled uses the fill color
    graphics_context_set_fill_color(ctx, GColorWhite);
    gpath_draw_filled(ctx, handHour_path);
  }
}

static void update_time() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	
	static char bufferMM[] = "00";
	strftime(bufferMM,sizeof("00"),"%M",tick_time);
  text_layer_set_text(textMinute, bufferMM);
	
	static char bufferSS[] = "00";
	strftime(bufferSS,sizeof("00"),"%S",tick_time);
  text_layer_set_text(textSecond, bufferSS);
	
	static char bufferDate[16];
	strftime(bufferDate, sizeof(bufferDate), "%d %b %Y", tick_time);
	text_layer_set_text(textDate, bufferDate);

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

static void init() {
  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  path_layer = layer_create(bounds);
  layer_set_update_proc(path_layer, path_layer_update_callback);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, path_layer);

  // Pass the corresponding GPathInfo to initialize a GPath
	handHour_path = gpath_create(&HAND_HOUR_PATH_POINT);
	gpath_move_to(handHour_path, GPoint(bounds.size.w/2, (bounds.size.h/2)+HEIGHT_OFFSET ));

	textDate = text_layer_create(GRect(0,142,144,18));
	text_layer_set_background_color(textDate, GColorClear);
	text_layer_set_text_color(textDate, GColorWhite);
  text_layer_set_text(textDate,"");
  text_layer_set_font(textDate, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(textDate, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(textDate));
	
	textSecond = text_layer_create(GRect(77,72+HEIGHT_OFFSET,18,26));
	text_layer_set_background_color(textSecond, GColorClear);
	text_layer_set_text_color(textSecond, GColorWhite);
  text_layer_set_text(textSecond,"00");
  text_layer_set_font(textSecond, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(textSecond, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(textSecond));
	
	textMinute = text_layer_create(GRect(52,65+HEIGHT_OFFSET,24,40));
	text_layer_set_background_color(textMinute, GColorClear);
	text_layer_set_text_color(textMinute, GColorWhite);
  text_layer_set_text(textMinute,"00");
  text_layer_set_font(textMinute, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(textMinute, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(textMinute));

	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

}

static void deinit() {
  gpath_destroy(handHour_path);
  layer_destroy(path_layer);
	text_layer_destroy(textMinute);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
