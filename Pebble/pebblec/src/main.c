#include <pebble.h>
#include <string.h>

#define KEY_TITLE 0
#define KEY_SUMMARY 1

static Window *s_main_window;
static TextLayer *s_word_layer;
static TextLayer *s_status_layer;

static GFont s_word_font;
static GFont s_status_font;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static char * content;
static char * title;
static int start_entry;
static int next_word;

static bool flag = true;
static AppTimer *timer;
static int wpm = 300;
static int factor = 1;
static int factorEnabled = 0;

static void timer_callback() {
    // Get a tm structure
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);

    // Create a long-lived buffer
    static char *buffer;

    int start_char = start_entry, end_char;
    int len = strlen(content);
    int char_index;
    next_word = 0;
    while (start_char < len) {
        if ((content[start_char] <= '9' && content[start_char] >= '0')
                || (content[start_char] <= 'z' && content[start_char] >= 'a')
                || (content[start_char] <= 'Z' && content[start_char] >= 'A')
                || (content[start_char] == '@')
                || (content[start_char] == '#')
                || (content[start_char] == '$')
                || (content[start_char] == '%')
                || (content[start_char] == '&')
                //              || (content[start_char] == 'º')
                || (content[start_char] == '\''))
        {
            next_word = 1;
            break;
        }
        start_char++;
    }

    if(next_word == 0) {
        timer = app_timer_register(60*1000/wpm, (AppTimerCallback) timer_callback, NULL);
        return;
    }

    end_char = start_char;
    while (end_char < len &&
            ((content[end_char] <= '9' && content[end_char] >= '0')
             || (content[end_char] <= 'z' && content[end_char] >= 'a')
             || (content[end_char] <= 'Z' && content[end_char] >= 'A')
             || (content[end_char] == '@')
             || (content[end_char] == '#')
             || (content[end_char] == '$')
             || (content[end_char] == '%')
             || (content[end_char] == '&')
             //          || (content[start_char] == 'º')
             || (content[end_char] == '\''))) {
        end_char++;
    }

    if (end_char - start_char <= 2) {
        /*1 6*/
        /*2 6*/
        /*3 4*/
        /*4 4*/
        /*5 4*/
        buffer = (char*)malloc(sizeof(char)*(1 + end_char - start_char + 7));
        memset(buffer, ' ', 7);
        for (char_index = start_char; char_index < end_char; char_index++) {
            buffer[char_index - start_char + 7] = content[char_index];
        }
        // magic formula
        factor = (4 - end_char + start_char)*5 + 0*1 + 1;
        buffer[end_char - start_char + 7] = 0;
    } else {
        if (end_char - start_char <= 6) {
            buffer = (char*)malloc(sizeof(char)*(1 + end_char - start_char + 6));
            memset(buffer, ' ', 6);
            for (char_index = start_char; char_index < end_char; char_index++) {
                buffer[char_index - start_char + 6] = content[char_index];
            }
            // magic formula
            factor = (4 - end_char + start_char)*5 + 0*1 + 1;
            buffer[end_char - start_char + 6] = 0;
        } else {
            if (end_char - start_char <= 11) {
                buffer = (char*)malloc(sizeof(char)*(1 + end_char - start_char + 3));
                memset(buffer, ' ', 3);
                for (char_index = start_char; char_index < end_char; char_index++) {
                    buffer[char_index - start_char + 3] = content[char_index];
                }
                // magic formula
                factor = (4 - end_char + start_char)*5 + 0*1 + 1;
                buffer[end_char - start_char + 3] = 0;
            } else {
                buffer = (char*)malloc(sizeof(char)*(1 + end_char - start_char));
                for (char_index = start_char; char_index < end_char; char_index++) {
                    buffer[char_index - start_char] = content[char_index];
                }
                // magic formula
                factor = (4 - end_char + start_char)*5 + 0*1 + 1;
                buffer[end_char - start_char] = 0;
            }
        }
    }

    // split content to get clean word everytime it's called

    // strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);

    // Display this time on the TextLayer
    text_layer_set_text(s_word_layer, buffer);
    free(buffer);
    start_entry = end_char + 1;
    // split content to get clean word everytime it's called

    // strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);

    timer = app_timer_register(60*1000/wpm+factor*factorEnabled, (AppTimerCallback) timer_callback, NULL);
}

static void main_window_load(Window *window) {
    //Create GBitmap, then set to created BitmapLayer
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
    s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

    // Create Summary TextLayer
    s_word_layer = text_layer_create(GRect(3, 72, 139, 50));
    text_layer_set_background_color(s_word_layer, GColorClear);
    text_layer_set_text_color(s_word_layer, GColorWhite);
    text_layer_set_text(s_word_layer, "Spribblealism");
    //Create GFont
    s_word_font = 
        // fonts_get_system_font("RESOURCE_ID_ROBOTO_CONDENSED_21")
        // fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
        fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_20));
    //Apply to TextLayer
    text_layer_set_font(s_word_layer, s_word_font);
    text_layer_set_text_alignment(s_word_layer, GTextAlignmentLeft);
    // Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_word_layer));

    // Create abstract Layer
    s_status_layer = text_layer_create(GRect(3, 72, 139, 50));
    text_layer_set_background_color(s_status_layer, GColorClear);
    text_layer_set_text_color(s_status_layer, GColorWhite);
    text_layer_set_text_alignment(s_status_layer, GTextAlignmentLeft);
    text_layer_set_text(s_status_layer, "Hello World!");
    // Create second custom font, apply it and add to Window
    s_status_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
    text_layer_set_font(s_status_layer, s_status_font);
    //layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_status_layer));
}

static void main_window_unload(Window *window) {
    //Destroy GBitmap
    gbitmap_destroy(s_background_bitmap);

    //Destroy BitmapLayer
    bitmap_layer_destroy(s_background_layer);

    // Destroy TextLayer
    text_layer_destroy(s_word_layer);
    text_layer_destroy(s_status_layer);
    //Unload GFont
    fonts_unload_custom_font(s_word_font);
    fonts_unload_custom_font(s_status_font);
}

static void send_request() {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
}

/* communication handler */ 

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    // Store incoming information
    free(content);
    content = malloc(256);
    title = malloc(50);

    // Read first item
    Tuple *t = dict_read_first(iterator);

    // For all items
    while(t != NULL) {
        // Which key was received?
        switch(t->key) {
            /*case KEY_TEMPERATURE:*/
            /*snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)t->value->int32);*/
            /*break;*/
            case KEY_TITLE:
                snprintf(title, sizeof(title), "%s", t->value->cstring);
                break;
            case KEY_SUMMARY:
                snprintf(content, sizeof(content), "%s", t->value->cstring);
                break;
            default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
                break;
        }

        // Look for next item
        t = dict_read_next(iterator);
    }

    // ready
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

/* clicks handler */

static void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "up_single_click_handler success!");
}

static void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "down_single_click_handler success!");
}

// pass
static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    start_entry = 0;
    APP_LOG(APP_LOG_LEVEL_INFO, "select_single_handler success!");
}

static void up_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
    wpm += 50;
    if (wpm > 2000) wpm = 2000;
    APP_LOG(APP_LOG_LEVEL_INFO, "up_multi_click_handler success!");
}

static void down_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
    wpm -= 50;
    if (wpm < 100) wpm = 100;
    APP_LOG(APP_LOG_LEVEL_INFO, "down_multi_click_handler success!");
}

static void select_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
    factorEnabled = 0 ? 1 : 0;
    APP_LOG(APP_LOG_LEVEL_INFO, "select_multi_click_handler success!");
}

// auto adjust speed
static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "select_long_click_handler success!");
}

static void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
    start_entry = 0;
    APP_LOG(APP_LOG_LEVEL_INFO, "select_long_click_release_handler success!");
}

static void config_provider(Window *window) {
    // single click / repeat-on-hold config:
    window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);

    // multi click config:
    /*window_multi_click_subscribe(BUTTON_ID_UP, 2, 10, 400, false, up_multi_click_handler);*/
    /*window_multi_click_subscribe(BUTTON_ID_DOWN, 2, 10, 400, false, down_multi_click_handler);*/
    window_multi_click_subscribe(BUTTON_ID_UP, 0, 0, 0, false, up_multi_click_handler);
    window_multi_click_subscribe(BUTTON_ID_DOWN, 0, 0, 0, false, down_multi_click_handler);
    window_multi_click_subscribe(BUTTON_ID_SELECT, 0, 0, 0, false, select_multi_click_handler);

    // long click config:
    window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);

}

static void init() {
    // Spribble init
    title = "Hello World!";
    content = "Hi, this is team PebbleReader at HackShanghai! (introduction) Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris eu consectetur eros. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Suspendisse vulputate iaculis metus ut lacinia. Nunc dapibus elit in turpis euismod volutpat. Aenean magna neque, fringilla id dui eget, imperdiet volutpat justo. Aliquam ut diam malesuada nunc consectetur vulputate eget ut augue. Nullam lacinia vestibulum lacinia. Pellentesque nisl eros, elementum nec nibh a, efficitur elementum orci. Morbi dolor nisi, mattis sit amet velit eu, maximus tempus odio. Curabitur placerat pulvinar nisi. Nulla auctor tempor viverra. Vestibulum nec urna interdum, imperdiet quam vel, viverra enim. Aenean vitae metus nibh.";

    start_entry = 0;

    // Create main Window element and assign to pointer
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
            .load = main_window_load,
            .unload = main_window_unload
            });

    // Set click config provider (as in example)
    window_set_click_config_provider(s_main_window, (ClickConfigProvider) config_provider);

    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);

    // Register with TickTimerService
    /*tick_timer_service_subscribe(SECOND_UNIT, tick_handler);*/
    timer = app_timer_register(60*1000/wpm, (AppTimerCallback) timer_callback, NULL);

    /*// Register callbacks*/
    /*app_message_register_inbox_received(inbox_received_callback);*/
    /*app_message_register_inbox_dropped(inbox_dropped_callback);*/
    /*app_message_register_outbox_failed(outbox_failed_callback);*/
    /*app_message_register_outbox_sent(outbox_sent_callback);*/

    /*// Open AppMessage*/
    /*app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());*/
}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
