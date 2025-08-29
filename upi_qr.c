#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/text_input.h>
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>
#include <storage/storage.h>
#include <stdlib.h>
#include <string.h>
#include <input/input.h>
#include "qrcode.h"

#define APP_NAME "UPI_QR"
#define SAVE_PATH "/ext/upi_qr"
#define SAVE_FILE "/ext/upi_qr/saved_upi.txt"
#define MAX_UPI_LENGTH 64
#define MAX_SAVED_ENTRIES 20

typedef struct {
    char upi_id[MAX_UPI_LENGTH];
    char name[32];
} UpiEntry;

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    TextInput* text_input;
    Submenu* submenu;
    Widget* widget;
    Popup* popup;
    
    char input_buffer[MAX_UPI_LENGTH];
    char username_buffer[32];
    char bank_buffer[32];
    char name_buffer[32];
    UpiEntry saved_entries[MAX_SAVED_ENTRIES];
    uint32_t saved_count;
    uint32_t selected_index;
    
    Storage* storage;
} UpiQrApp;

typedef enum {
    UpiQrViewMenu,
    UpiQrViewTextInput,
    UpiQrViewWidget,
    UpiQrViewPopup,
} UpiQrView;

typedef enum {
    UpiQrSceneMenu,
    UpiQrSceneUsernameInput,
    UpiQrSceneBankInput,
    UpiQrSceneNameInput,
    UpiQrSceneQrDisplay,
    UpiQrSceneQrFullscreen,
    UpiQrSceneSavedList,
    UpiQrSceneConfirmDelete,
    UpiQrSceneCount,
} UpiQrScene;

// Function prototypes
static void upi_qr_app_load_saved(UpiQrApp* app);
static void upi_qr_app_save_entries(UpiQrApp* app);
static void upi_qr_draw_qr_code(Widget* widget, const char* upi_id, void* context);

// Scene on_enter handlers
void upi_qr_scene_menu_on_enter(void* context);
void upi_qr_scene_username_input_on_enter(void* context);
void upi_qr_scene_bank_input_on_enter(void* context);
void upi_qr_scene_name_input_on_enter(void* context);
void upi_qr_scene_qr_display_on_enter(void* context);
void upi_qr_scene_qr_fullscreen_on_enter(void* context);
void upi_qr_scene_saved_list_on_enter(void* context);
void upi_qr_scene_confirm_delete_on_enter(void* context);

// Scene on_event handlers
bool upi_qr_scene_menu_on_event(void* context, SceneManagerEvent event);
bool upi_qr_scene_username_input_on_event(void* context, SceneManagerEvent event);
bool upi_qr_scene_bank_input_on_event(void* context, SceneManagerEvent event);
bool upi_qr_scene_name_input_on_event(void* context, SceneManagerEvent event);
bool upi_qr_scene_qr_display_on_event(void* context, SceneManagerEvent event);
bool upi_qr_scene_qr_fullscreen_on_event(void* context, SceneManagerEvent event);
bool upi_qr_scene_saved_list_on_event(void* context, SceneManagerEvent event);
bool upi_qr_scene_confirm_delete_on_event(void* context, SceneManagerEvent event);

// Scene on_exit handlers
void upi_qr_scene_menu_on_exit(void* context);
void upi_qr_scene_username_input_on_exit(void* context);
void upi_qr_scene_bank_input_on_exit(void* context);
void upi_qr_scene_name_input_on_exit(void* context);
void upi_qr_scene_qr_display_on_exit(void* context);
void upi_qr_scene_qr_fullscreen_on_exit(void* context);
void upi_qr_scene_saved_list_on_exit(void* context);
void upi_qr_scene_confirm_delete_on_exit(void* context);

// Scene handlers - using function pointers directly
void (*upi_qr_scene_on_enter_handlers[])(void*) = {
    [UpiQrSceneMenu] = upi_qr_scene_menu_on_enter,
    [UpiQrSceneUsernameInput] = upi_qr_scene_username_input_on_enter,
    [UpiQrSceneBankInput] = upi_qr_scene_bank_input_on_enter,
    [UpiQrSceneNameInput] = upi_qr_scene_name_input_on_enter,
    [UpiQrSceneQrDisplay] = upi_qr_scene_qr_display_on_enter,
    [UpiQrSceneQrFullscreen] = upi_qr_scene_qr_fullscreen_on_enter,
    [UpiQrSceneSavedList] = upi_qr_scene_saved_list_on_enter,
    [UpiQrSceneConfirmDelete] = upi_qr_scene_confirm_delete_on_enter,
};

bool (*upi_qr_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    [UpiQrSceneMenu] = upi_qr_scene_menu_on_event,
    [UpiQrSceneUsernameInput] = upi_qr_scene_username_input_on_event,
    [UpiQrSceneBankInput] = upi_qr_scene_bank_input_on_event,
    [UpiQrSceneNameInput] = upi_qr_scene_name_input_on_event,
    [UpiQrSceneQrDisplay] = upi_qr_scene_qr_display_on_event,
    [UpiQrSceneQrFullscreen] = upi_qr_scene_qr_fullscreen_on_event,
    [UpiQrSceneSavedList] = upi_qr_scene_saved_list_on_event,
    [UpiQrSceneConfirmDelete] = upi_qr_scene_confirm_delete_on_event,
};

void (*upi_qr_scene_on_exit_handlers[])(void*) = {
    [UpiQrSceneMenu] = upi_qr_scene_menu_on_exit,
    [UpiQrSceneUsernameInput] = upi_qr_scene_username_input_on_exit,
    [UpiQrSceneBankInput] = upi_qr_scene_bank_input_on_exit,
    [UpiQrSceneNameInput] = upi_qr_scene_name_input_on_exit,
    [UpiQrSceneQrDisplay] = upi_qr_scene_qr_display_on_exit,
    [UpiQrSceneQrFullscreen] = upi_qr_scene_qr_fullscreen_on_exit,
    [UpiQrSceneSavedList] = upi_qr_scene_saved_list_on_exit,
    [UpiQrSceneConfirmDelete] = upi_qr_scene_confirm_delete_on_exit,
};

static const SceneManagerHandlers upi_qr_scene_handlers = {
    .on_enter_handlers = upi_qr_scene_on_enter_handlers,
    .on_event_handlers = upi_qr_scene_on_event_handlers,
    .on_exit_handlers = upi_qr_scene_on_exit_handlers,
    .scene_num = UpiQrSceneCount,
};

// Callback for submenu
static void upi_qr_submenu_callback(void* context, uint32_t index) {
    UpiQrApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

// Callback for text input
static void upi_qr_text_input_callback(void* context) {
    UpiQrApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, 0);
}

// Scene implementations
void upi_qr_scene_menu_on_enter(void* context) {
    UpiQrApp* app = context;
    
    // Clear buffers when returning to menu
    memset(app->input_buffer, 0, MAX_UPI_LENGTH);
    memset(app->username_buffer, 0, 32);
    memset(app->bank_buffer, 0, 32);
    memset(app->name_buffer, 0, 32);
    
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "UPI QR Generator");
    submenu_add_item(app->submenu, "New UPI ID", 0, upi_qr_submenu_callback, app);
    submenu_add_item(app->submenu, "Saved UPI IDs", 1, upi_qr_submenu_callback, app);
    submenu_add_item(app->submenu, "About", 2, upi_qr_submenu_callback, app);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, UpiQrViewMenu);
}

bool upi_qr_scene_menu_on_event(void* context, SceneManagerEvent event) {
    UpiQrApp* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
            case 0: // New UPI ID
                scene_manager_next_scene(app->scene_manager, UpiQrSceneUsernameInput);
                consumed = true;
                break;
            case 1: // Saved UPI IDs
                upi_qr_app_load_saved(app);
                scene_manager_next_scene(app->scene_manager, UpiQrSceneSavedList);
                consumed = true;
                break;
            case 2: // About
                popup_reset(app->popup);
                popup_set_header(app->popup, "UPI QR v1.0", 64, 10, AlignCenter, AlignCenter);
                popup_set_text(app->popup, "Generate UPI\nPayment QR Codes", 64, 32, AlignCenter, AlignCenter);
                popup_set_timeout(app->popup, 2000);
                popup_set_context(app->popup, app);
                popup_set_callback(app->popup, NULL);
                popup_enable_timeout(app->popup);
                view_dispatcher_switch_to_view(app->view_dispatcher, UpiQrViewPopup);
                consumed = true;
                break;
        }
    }
    
    return consumed;
}

void upi_qr_scene_menu_on_exit(void* context) {
    UpiQrApp* app = context;
    submenu_reset(app->submenu);
}

void upi_qr_scene_username_input_on_enter(void* context) {
    UpiQrApp* app = context;
    
    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Enter Username:");
    text_input_set_result_callback(
        app->text_input,
        upi_qr_text_input_callback,
        app,
        app->username_buffer,
        32,
        true);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, UpiQrViewTextInput);
}

bool upi_qr_scene_username_input_on_event(void* context, SceneManagerEvent event) {
    UpiQrApp* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_next_scene(app->scene_manager, UpiQrSceneBankInput);
        consumed = true;
    }
    
    return consumed;
}

void upi_qr_scene_username_input_on_exit(void* context) {
    UNUSED(context);
}

void upi_qr_scene_bank_input_on_enter(void* context) {
    UpiQrApp* app = context;
    
    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Enter Bank (e.g., ybl, paytm):");
    text_input_set_result_callback(
        app->text_input,
        upi_qr_text_input_callback,
        app,
        app->bank_buffer,
        32,
        true);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, UpiQrViewTextInput);
}

bool upi_qr_scene_bank_input_on_event(void* context, SceneManagerEvent event) {
    UpiQrApp* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeCustom) {
        // Combine username and bank to create UPI ID
        snprintf(app->input_buffer, MAX_UPI_LENGTH, "%s@%s", app->username_buffer, app->bank_buffer);
        scene_manager_next_scene(app->scene_manager, UpiQrSceneNameInput);
        consumed = true;
    }
    
    return consumed;
}

void upi_qr_scene_bank_input_on_exit(void* context) {
    UNUSED(context);
}

void upi_qr_scene_name_input_on_enter(void* context) {
    UpiQrApp* app = context;
    
    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Enter Name (optional):");
    text_input_set_result_callback(
        app->text_input,
        upi_qr_text_input_callback,
        app,
        app->name_buffer,
        32,
        true);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, UpiQrViewTextInput);
}

bool upi_qr_scene_name_input_on_event(void* context, SceneManagerEvent event) {
    UpiQrApp* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_next_scene(app->scene_manager, UpiQrSceneQrDisplay);
        consumed = true;
    }
    
    return consumed;
}

void upi_qr_scene_name_input_on_exit(void* context) {
    UNUSED(context);
}

void upi_qr_scene_qr_display_on_enter(void* context) {
    UpiQrApp* app = context;
    
    widget_reset(app->widget);
    upi_qr_draw_qr_code(app->widget, app->input_buffer, app);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, UpiQrViewWidget);
}

bool upi_qr_scene_qr_display_on_event(void* context, SceneManagerEvent event) {
    UpiQrApp* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeBack) {
        scene_manager_search_and_switch_to_previous_scene(app->scene_manager, UpiQrSceneMenu);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) { // Save button pressed
            // Save the entry
            if(app->saved_count < MAX_SAVED_ENTRIES) {
                strcpy(app->saved_entries[app->saved_count].upi_id, app->input_buffer);
                strcpy(app->saved_entries[app->saved_count].name, 
                       strlen(app->name_buffer) > 0 ? app->name_buffer : "Unnamed");
                app->saved_count++;
                upi_qr_app_save_entries(app);
                
                popup_reset(app->popup);
                popup_set_header(app->popup, "Saved!", 64, 20, AlignCenter, AlignCenter);
                popup_set_timeout(app->popup, 1000);
                popup_set_context(app->popup, app);
                popup_set_callback(app->popup, NULL);
                popup_enable_timeout(app->popup);
                view_dispatcher_switch_to_view(app->view_dispatcher, UpiQrViewPopup);
            }
            consumed = true;
        } else if(event.event == GuiButtonTypeCenter) { // Fullscreen button pressed
            // Switch to fullscreen QR view
            scene_manager_next_scene(app->scene_manager, UpiQrSceneQrFullscreen);
            consumed = true;
        }
    }
    
    return consumed;
}

void upi_qr_scene_qr_display_on_exit(void* context) {
    UpiQrApp* app = context;
    widget_reset(app->widget);
    // Don't clear buffers here - they're needed for fullscreen view
    // Buffers will be cleared when returning to main menu
}

// Fullscreen QR scene - shows QR code taking up entire screen
void upi_qr_scene_qr_fullscreen_on_enter(void* context) {
    UpiQrApp* app = context;
    
    // Generate UPI payment string - simplified format
    char upi_payment_string[256];
    char encoded_name[64];
    const char* payee_name = (strlen(app->name_buffer) > 0) ? app->name_buffer : "Payment";
    
    // URL encode the payee name (replace spaces with %20)
    int j = 0;
    for(int i = 0; payee_name[i] && j < (int)sizeof(encoded_name) - 3; i++) {
        if(payee_name[i] == ' ') {
            encoded_name[j++] = '%';
            encoded_name[j++] = '2';
            encoded_name[j++] = '0';
        } else {
            encoded_name[j++] = payee_name[i];
        }
    }
    encoded_name[j] = '\0';
    
    snprintf(
        upi_payment_string,
        sizeof(upi_payment_string),
        "upi://pay?pa=%s&pn=%s&cu=INR",
        app->input_buffer,
        encoded_name
    );
    

    
    // Generate actual QR code for fullscreen
    uint8_t qr_version = 3;
    uint16_t buffer_size = qrcode_getBufferSize(qr_version);
    uint8_t* qr_buffer = malloc(buffer_size);
    
    if(qr_buffer) {
        QRCode qrcode;
        int8_t result = qrcode_initBytes(&qrcode, qr_buffer, MODE_BYTE, qr_version, ECC_LOW, 
                                        (uint8_t*)upi_payment_string, strlen(upi_payment_string));
        
        if(result >= 0) {
            // Calculate QR code display parameters - maximize for fullscreen
            uint8_t qr_size = qrcode.size;
            uint8_t max_display_size = 64; // Use full screen height
            uint8_t module_size = max_display_size / qr_size;
            if(module_size == 0) module_size = 1;
            
            uint8_t display_size = qr_size * module_size;
            uint8_t start_x = (128 - display_size) / 2;
            uint8_t start_y = 0; // Start at top
            
            // Draw QR code modules
            for(uint8_t y = 0; y < qr_size; y++) {
                for(uint8_t x = 0; x < qr_size; x++) {
                    if(qrcode_getModule(&qrcode, x, y)) {
                        // Draw filled rectangle for black modules
                        for(uint8_t dy = 0; dy < module_size; dy++) {
                            for(uint8_t dx = 0; dx < module_size; dx++) {
                                widget_add_frame_element(app->widget, 
                                    start_x + x * module_size + dx, 
                                    start_y + y * module_size + dy, 
                                    1, 1, 0);
                            }
                        }
                    }
                }
            }
        } else {
            // Fallback to text display if QR generation fails
            widget_add_string_element(app->widget, 64, 32, AlignCenter, AlignCenter, FontPrimary, "QR Gen Failed");
        }
        
        free(qr_buffer);
    } else {
        // Fallback if memory allocation fails
        widget_add_string_element(app->widget, 64, 32, AlignCenter, AlignCenter, FontPrimary, "Memory Error");
    }
    
    view_dispatcher_switch_to_view(app->view_dispatcher, UpiQrViewWidget);
}

bool upi_qr_scene_qr_fullscreen_on_event(void* context, SceneManagerEvent event) {
    UpiQrApp* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(app->scene_manager);
        consumed = true;
    }
    
    return consumed;
}

void upi_qr_scene_qr_fullscreen_on_exit(void* context) {
    UpiQrApp* app = context;
    
    // Clear the widget
    widget_reset(app->widget);
}

void upi_qr_scene_saved_list_on_enter(void* context) {
    UpiQrApp* app = context;
    
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Saved UPI IDs");
    
    for(uint32_t i = 0; i < app->saved_count; i++) {
        char item_text[96];
        snprintf(item_text, sizeof(item_text), "%s - %s", 
                 app->saved_entries[i].name, 
                 app->saved_entries[i].upi_id);
        submenu_add_item(app->submenu, item_text, i, upi_qr_submenu_callback, app);
    }
    
    if(app->saved_count == 0) {
        submenu_add_item(app->submenu, "No saved entries", 999, upi_qr_submenu_callback, app);
    } else {
        submenu_add_item(app->submenu, "[Hold Back to Delete]", 998, upi_qr_submenu_callback, app);
    }
    
    view_dispatcher_switch_to_view(app->view_dispatcher, UpiQrViewMenu);
}

bool upi_qr_scene_saved_list_on_event(void* context, SceneManagerEvent event) {
    UpiQrApp* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event < app->saved_count) {
            app->selected_index = event.event;
            strcpy(app->input_buffer, app->saved_entries[event.event].upi_id);
            strcpy(app->name_buffer, app->saved_entries[event.event].name);
            scene_manager_next_scene(app->scene_manager, UpiQrSceneQrDisplay);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        if(app->saved_count > 0) {
            // Show delete menu
            scene_manager_next_scene(app->scene_manager, UpiQrSceneConfirmDelete);
            consumed = true;
        }
    }
    
    return consumed;
}

void upi_qr_scene_saved_list_on_exit(void* context) {
    UpiQrApp* app = context;
    submenu_reset(app->submenu);
}

void upi_qr_scene_confirm_delete_on_enter(void* context) {
    UpiQrApp* app = context;
    
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Delete Entry?");
    
    for(uint32_t i = 0; i < app->saved_count; i++) {
        char item_text[96];
        snprintf(item_text, sizeof(item_text), "Delete: %s", 
                 app->saved_entries[i].name);
        submenu_add_item(app->submenu, item_text, i, upi_qr_submenu_callback, app);
    }
    
    view_dispatcher_switch_to_view(app->view_dispatcher, UpiQrViewMenu);
}

bool upi_qr_scene_confirm_delete_on_event(void* context, SceneManagerEvent event) {
    UpiQrApp* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event < app->saved_count) {
            // Delete entry
            for(uint32_t i = event.event; i < app->saved_count - 1; i++) {
                app->saved_entries[i] = app->saved_entries[i + 1];
            }
            app->saved_count--;
            upi_qr_app_save_entries(app);
            
            popup_reset(app->popup);
            popup_set_header(app->popup, "Deleted!", 64, 20, AlignCenter, AlignCenter);
            popup_set_timeout(app->popup, 1000);
            popup_set_context(app->popup, app);
            popup_set_callback(app->popup, NULL);
            popup_enable_timeout(app->popup);
            view_dispatcher_switch_to_view(app->view_dispatcher, UpiQrViewPopup);
            
            scene_manager_search_and_switch_to_previous_scene(app->scene_manager, UpiQrSceneSavedList);
        }
        consumed = true;
    }
    
    return consumed;
}

void upi_qr_scene_confirm_delete_on_exit(void* context) {
    UpiQrApp* app = context;
    submenu_reset(app->submenu);
}

// Widget button callback
static void upi_qr_widget_button_callback(GuiButtonType result, InputType type, void* context) {
    UpiQrApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

// UPI QR code drawing with actual QR code generation
static void upi_qr_draw_qr_code(Widget* widget, const char* upi_id, void* context) {
    // Generate UPI payment string - simplified format
    UpiQrApp* app = (UpiQrApp*)context;
    char upi_payment_string[256];
    char encoded_name[64];
    const char* payee_name = (strlen(app->name_buffer) > 0) ? app->name_buffer : "Payment";
    
    // URL encode the payee name (replace spaces with %20)
    int j = 0;
    for(int i = 0; payee_name[i] && j < (int)sizeof(encoded_name) - 3; i++) {
        if(payee_name[i] == ' ') {
            encoded_name[j++] = '%';
            encoded_name[j++] = '2';
            encoded_name[j++] = '0';
        } else {
            encoded_name[j++] = payee_name[i];
        }
    }
    encoded_name[j] = '\0';
    
    snprintf(
        upi_payment_string,
        sizeof(upi_payment_string),
        "upi://pay?pa=%s&pn=%s&cu=INR",
        upi_id,
        encoded_name
    );
    

    
    // Generate actual QR code
    uint8_t qr_version = 3; // Start with version 3 for UPI strings
    uint16_t buffer_size = qrcode_getBufferSize(qr_version);
    uint8_t* qr_buffer = malloc(buffer_size);
    
    if(qr_buffer) {
        QRCode qrcode;
        int8_t result = qrcode_initBytes(&qrcode, qr_buffer, MODE_BYTE, qr_version, ECC_LOW, 
                                        (uint8_t*)upi_payment_string, strlen(upi_payment_string));
        
        if(result >= 0) {
            // Calculate QR code display parameters - maximize size for screen
            uint8_t qr_size = qrcode.size;
            uint8_t max_display_size = 55; // Larger QR code taking most of screen
            uint8_t module_size = max_display_size / qr_size;
            if(module_size == 0) module_size = 1;
            
            uint8_t display_size = qr_size * module_size;
            uint8_t start_x = (128 - display_size) / 2;
            uint8_t start_y = 5; // Start near top
            
            // Draw QR code modules
            for(uint8_t y = 0; y < qr_size; y++) {
                for(uint8_t x = 0; x < qr_size; x++) {
                    if(qrcode_getModule(&qrcode, x, y)) {
                        // Draw filled rectangle for black modules
                        for(uint8_t dy = 0; dy < module_size; dy++) {
                            for(uint8_t dx = 0; dx < module_size; dx++) {
                                widget_add_frame_element(widget, 
                                    start_x + x * module_size + dx, 
                                    start_y + y * module_size + dy, 
                                    1, 1, 0);
                            }
                        }
                    }
                }
            }
            
        } else {
            // Fallback to text display if QR generation fails
            widget_add_string_element(widget, 64, 30, AlignCenter, AlignCenter, FontSecondary, "QR Gen Failed");
        }
        
        free(qr_buffer);
    } else {
        // Fallback if memory allocation fails
        widget_add_string_element(widget, 64, 30, AlignCenter, AlignCenter, FontSecondary, "Memory Error");
    }
    
    // Add buttons - Left for Save, Center for Fullscreen
    widget_add_button_element(widget, GuiButtonTypeLeft, "Save", upi_qr_widget_button_callback, context);
    widget_add_button_element(widget, GuiButtonTypeCenter, "Full", upi_qr_widget_button_callback, context);
}

// File operations
static void upi_qr_app_load_saved(UpiQrApp* app) {
    File* file = storage_file_alloc(app->storage);
    
    if(storage_file_open(file, SAVE_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
        char buffer[256];
        size_t bytes_read;
        app->saved_count = 0;
        
        while((bytes_read = storage_file_read(file, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            
            // Manual string parsing to replace strtok
            char* current = buffer;
            char* line_end;
            
            while(current && *current && app->saved_count < MAX_SAVED_ENTRIES) {
                // Find end of current line
                line_end = strchr(current, '\n');
                if(line_end) {
                    *line_end = '\0';
                }
                
                // Process the line
                if(*current) {
                    char* separator = strchr(current, '|');
                    if(separator) {
                        *separator = '\0';
                        strncpy(app->saved_entries[app->saved_count].name, current, 31);
                        app->saved_entries[app->saved_count].name[31] = '\0';
                        strncpy(app->saved_entries[app->saved_count].upi_id, separator + 1, MAX_UPI_LENGTH - 1);
                        app->saved_entries[app->saved_count].upi_id[MAX_UPI_LENGTH - 1] = '\0';
                        app->saved_count++;
                    }
                }
                
                // Move to next line
                if(line_end) {
                    current = line_end + 1;
                } else {
                    break;
                }
            }
        }
        storage_file_close(file);
    }
    
    storage_file_free(file);
}

static void upi_qr_app_save_entries(UpiQrApp* app) {
    // Ensure directory exists
    storage_simply_mkdir(app->storage, SAVE_PATH);
    
    File* file = storage_file_alloc(app->storage);
    
    if(storage_file_open(file, SAVE_FILE, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        for(uint32_t i = 0; i < app->saved_count; i++) {
            char buffer[128];
            int len = snprintf(buffer, sizeof(buffer), "%s|%s\n", 
                     app->saved_entries[i].name,
                     app->saved_entries[i].upi_id);
            storage_file_write(file, buffer, len);
        }
        storage_file_close(file);
    }
    
    storage_file_free(file);
}

// View dispatcher callbacks
static bool upi_qr_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    UpiQrApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool upi_qr_back_event_callback(void* context) {
    furi_assert(context);
    UpiQrApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

// App lifecycle
UpiQrApp* upi_qr_app_alloc() {
    UpiQrApp* app = malloc(sizeof(UpiQrApp));
    
    app->gui = furi_record_open(RECORD_GUI);
    app->storage = furi_record_open(RECORD_STORAGE);
    
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&upi_qr_scene_handlers, app);
    
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, upi_qr_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, upi_qr_back_event_callback);
    
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    
    // Create views
    app->submenu = submenu_alloc();
    app->text_input = text_input_alloc();
    app->widget = widget_alloc();
    app->popup = popup_alloc();
    
    view_dispatcher_add_view(app->view_dispatcher, UpiQrViewMenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, UpiQrViewTextInput, text_input_get_view(app->text_input));
    view_dispatcher_add_view(app->view_dispatcher, UpiQrViewWidget, widget_get_view(app->widget));
    view_dispatcher_add_view(app->view_dispatcher, UpiQrViewPopup, popup_get_view(app->popup));
    
    // Initialize data
    app->saved_count = 0;
    app->selected_index = 0;
    memset(app->input_buffer, 0, MAX_UPI_LENGTH);
    memset(app->username_buffer, 0, 32);
    memset(app->bank_buffer, 0, 32);
    memset(app->name_buffer, 0, 32);
    
    // Load saved entries
    upi_qr_app_load_saved(app);
    
    return app;
}

void upi_qr_app_free(UpiQrApp* app) {
    // Remove views
    view_dispatcher_remove_view(app->view_dispatcher, UpiQrViewMenu);
    view_dispatcher_remove_view(app->view_dispatcher, UpiQrViewTextInput);
    view_dispatcher_remove_view(app->view_dispatcher, UpiQrViewWidget);
    view_dispatcher_remove_view(app->view_dispatcher, UpiQrViewPopup);
    
    // Free views
    submenu_free(app->submenu);
    text_input_free(app->text_input);
    widget_free(app->widget);
    popup_free(app->popup);
    
    // Free core
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    
    // Close records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);
    
    free(app);
}

int32_t upi_qr_app(void* p) {
    UNUSED(p);
    UpiQrApp* app = upi_qr_app_alloc();
    
    scene_manager_next_scene(app->scene_manager, UpiQrSceneMenu);
    
    view_dispatcher_run(app->view_dispatcher);
    
    upi_qr_app_free(app);
    
    return 0;
}