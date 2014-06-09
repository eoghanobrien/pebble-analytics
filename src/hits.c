#include <pebble.h>
#include <messageresult.h>

static Window *window;
static TextLayer *count_layer;
static TextLayer *title_layer;
static TextLayer *website_layer;
static int current_env = 0;
static bool allow_click = true;
static bool isDark = true;

enum {
  KEY_ENV   = 0,
  KEY_COUNT = 1,
  KEY_TITLE = 2,
  KEY_WEBSITE = 3,
};

static void fetch_data(void) {
  allow_click = false;
  Tuplet env_tuple = TupletInteger(KEY_ENV, current_env);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (iter == NULL) {
    return;
  }
  dict_write_tuplet(iter, &env_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
}


static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (allow_click) {
    if (current_env == 0) {
      current_env = 4;
    }
    else {
      current_env--;
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Up Button Pressed: CURRENT ENV: %d", current_env);
    text_layer_set_text(count_layer, "");
    text_layer_set_text(title_layer, "Updating...");
    text_layer_set_text(website_layer, "");
    fetch_data();
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (allow_click) {
    if (current_env == 4) {
      current_env = 0;
    }
    else {
      current_env++;
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Down Button Pressed: CURRENT ENV: %d", current_env);
    text_layer_set_text(count_layer, "");
    text_layer_set_text(title_layer, "Updating...");
    text_layer_set_text(website_layer, "");
    fetch_data();
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(title_layer, "Refreshing...");
  fetch_data();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
  allow_click = true;
}


void out_failed_handler(DictionaryIterator *failed, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed Outbox Handler");
  log_app_message_result(app_message_error);
}


void in_received_handler(DictionaryIterator *received, void *context) {
  Tuple *count_tuple = dict_find(received, KEY_COUNT);
  if (count_tuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Count: %s", count_tuple->value->cstring);
    text_layer_set_text(count_layer, count_tuple->value->cstring);
  }

  Tuple *title_tuple = dict_find(received, KEY_TITLE);
  if (title_tuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Title: %s", title_tuple->value->cstring);
    text_layer_set_text(title_layer, title_tuple->value->cstring);
  }

  Tuple *website_tuple = dict_find(received, KEY_WEBSITE);
  if (website_tuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Website: %s", website_tuple->value->cstring);
    text_layer_set_text(website_layer, website_tuple->value->cstring);
  }
  
  light_enable_interaction();
}

void in_dropped_handler(AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Dropped Inbox Handler");
  log_app_message_result(app_message_error);
}

static void app_message_init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  const int inbound_size = 128;
  const int outbound_size = 16;
  app_message_open(inbound_size, outbound_size);
  fetch_data();
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  /* HITS COUNT LAYER */
  count_layer = text_layer_create((GRect) { .origin = { 0, 25 }, .size = { bounds.size.w, 50 } });
  text_layer_set_text(count_layer, "");
  text_layer_set_font(count_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(count_layer, GTextAlignmentCenter);
  text_layer_set_text_color(count_layer, GColorWhite);
  text_layer_set_background_color(count_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(count_layer));

  /* HITS TITLE LAYER */
  title_layer = text_layer_create((GRect) { .origin = { 0, 80 }, .size = { bounds.size.w, 30 } });
  text_layer_set_text(title_layer, "Updating...");
  text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
  text_layer_set_text_color(title_layer, GColorWhite);
  text_layer_set_background_color(title_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(title_layer));

  /* WEBSITE TITLE LAYER */
  website_layer = text_layer_create((GRect) { .origin = { 0, 115 }, .size = { bounds.size.w, 21 } });
  text_layer_set_text(website_layer, "");
  text_layer_set_font(website_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(website_layer, GTextAlignmentCenter);
  text_layer_set_text_color(website_layer, GColorWhite);
  text_layer_set_background_color(website_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(website_layer));

  fetch_data();
}

static void window_unload(Window *window) {
  text_layer_destroy(count_layer);
  text_layer_destroy(title_layer);
  text_layer_destroy(website_layer);
}

static void init(void) {
  window = window_create();

  app_message_init();

  window_set_background_color(window, GColorBlack);
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}