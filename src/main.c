#include "pebble.h"

static Window *window;

static Layer *path_layer;

static const int HEIGHT_OFFSET = -8;

static int currentHour = 0;

static GPoint clockfaceCenter;

static TextLayer *textMinute;
static TextLayer *textSecond;
static TextLayer *textDate;

static const GPoint *HOUR_POSITION = (GPoint []){
	{0,-54},
	{27,-47},
	{47,-27},
	{54,0},
	{47,27},
	{27,47},
	{0,54},
	{-27,47},
	{-47,27},
	{-54,0},
	{-47,-27},
	{-27,-47}
};

static const GPathInfo CLOCKFACE_INNER_PATH_POINT = {
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

static const GPathInfo CLOCKFACE_OUTER_PATH_POINT = {
  12,
  (GPoint []) {
		{54,-31},
		{31,-54},
		{-0,-62},
		{-31,-54},
		{-54,-31},
		{-62,0},
		{-54,31},
		{-31,54},
		{0,62},
		{31,54},
		{54,31},
		{62,0}
	}
};

static GPath *clockfaceInner_path = NULL;
static GPath *clockfaceOuter_path = NULL;

//static int path_angle = 0;

// This is the layer update callback which is called on render updates
static void path_layer_update_callback(Layer *me, GContext *ctx) {
  (void)me;

  //gpath_rotate_to(handHour_path, (TRIG_MAX_ANGLE / 360) * path_angle);

  graphics_context_set_fill_color(ctx, GColorWhite);
	gpath_draw_filled(ctx, clockfaceOuter_path);
	
	graphics_context_set_fill_color(ctx, GColorBlack);
	gpath_draw_filled(ctx, clockfaceInner_path);
	
	graphics_context_set_fill_color(ctx, GColorWhite);
	int hPosX = HOUR_POSITION[currentHour].x+clockfaceCenter.x;
	int hPosY = HOUR_POSITION[currentHour].y+clockfaceCenter.y;
	graphics_fill_circle(ctx, GPoint(hPosX,hPosY), 2);

}

static void update_time() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	
	// Hour
	static char bufferHH[] = "00";
	strftime(bufferHH,sizeof("00"),"%l",tick_time);
	currentHour = atoi(bufferHH);
	if(currentHour==12) currentHour = 0;
	
	// Minute
	static char bufferMM[] = "00";
	strftime(bufferMM,sizeof("00"),"%M",tick_time);
  text_layer_set_text(textMinute, bufferMM);
	
	// Second
	static char bufferSS[] = "00";
	strftime(bufferSS,sizeof("00"),"%S",tick_time);
  text_layer_set_text(textSecond, bufferSS);
	
	// Date
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
  layer_add_child(window_layer, path_layer);

	clockfaceCenter = GPoint(bounds.size.w/2, (bounds.size.h/2)+HEIGHT_OFFSET);

	clockfaceOuter_path = gpath_create(&CLOCKFACE_OUTER_PATH_POINT);
	gpath_move_to(clockfaceOuter_path,clockfaceCenter);
  clockfaceInner_path = gpath_create(&CLOCKFACE_INNER_PATH_POINT);
	gpath_move_to(clockfaceInner_path,clockfaceCenter);

	textDate = text_layer_create(GRect(0,142,144,18));
	text_layer_set_background_color(textDate, GColorClear);
	text_layer_set_text_color(textDate, GColorWhite);
  text_layer_set_text(textDate,"");
  text_layer_set_font(textDate, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(textDate, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(textDate));
	
	textSecond = text_layer_create(GRect(77,71+HEIGHT_OFFSET,18,26));
	text_layer_set_background_color(textSecond, GColorClear);
	text_layer_set_text_color(textSecond, GColorWhite);
  text_layer_set_text(textSecond,"00");
  text_layer_set_font(textSecond, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(textSecond, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(textSecond));
	
	textMinute = text_layer_create(GRect(52,64+HEIGHT_OFFSET,24,40));
	text_layer_set_background_color(textMinute, GColorClear);
	text_layer_set_text_color(textMinute, GColorWhite);
  text_layer_set_text(textMinute,"00");
  text_layer_set_font(textMinute, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(textMinute, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(textMinute));

	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

}

static void deinit() {
  gpath_destroy(clockfaceOuter_path);
  gpath_destroy(clockfaceInner_path);
  layer_destroy(path_layer);
	text_layer_destroy(textMinute);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
