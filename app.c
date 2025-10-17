#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DONOR_FILE "donors.csv"
#define RECEIVER_FILE "receivers.csv"

// Forward declarations
static void activate(GtkApplication *app, gpointer user_data);
static void mainpage(GtkApplication *app);
static void on_save_donor_clicked(GtkButton *button, gpointer user_data);
static void on_save_receiver_clicked(GtkButton *button, gpointer user_data);

// Struct for login widgets
typedef struct {
    GtkEntry *entry1;
    GtkEntry *entry2;
    GtkWidget *window;
    GtkApplication *app;
} AppWidgets;

// Struct for the registration form widgets
typedef struct {
    GtkWidget *name_entry;
    GtkWidget *age_entry;
    GtkWidget *blood_combo;
    GtkWidget *contact_entry;
    GtkWidget *parent_window;
} RegistrationWidgets;

// Struct to pass data for the donor search
typedef struct {
    GtkApplication *app;
    GtkWidget *blood_combo;
} FindDonorWidgets;

// Struct to pass data for logout
typedef struct {
    GtkApplication *app;
    GtkWidget *main_menu_window;
} LogoutWidgets;


// --- DATA HANDLING FUNCTIONS ---

// Appends a new record to a specified data file.
void append_to_file(const char *filename, const char *name, const char *age, const char *blood_group, const char *contact) {
    FILE *fp = fopen(filename, "a");
    if (fp == NULL) {
        g_printerr("Error: Could not open file %s for writing.\n", filename);
        return;
    }
    fprintf(fp, "%s,%s,%s,%s\n", name, age, blood_group, contact);
    fclose(fp);
}


// --- UI CREATION: LIST VIEWS ---

// Creates and populates a TreeView with filtered data from a file
void create_filtered_list_view_window(GtkApplication *app, const gchar *title, const char *filename, const gchar *filter_blood_group) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);

    GtkListStore *list_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
    g_object_unref(list_store);

    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);

    const char *column_titles[] = {"Name", "Age", "Blood Group", "Contact"};
    for (int i = 0; i < 4; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(column_titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    }

    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            char *name = strtok(line, ",");
            char *age = strtok(NULL, ",");
            char *blood = strtok(NULL, ",");
            char *contact = strtok(NULL, "\n");

            if (name && age && blood && contact) {
                // Add to list only if blood group matches filter
                if (g_strcmp0(blood, filter_blood_group) == 0) {
                    GtkTreeIter iter;
                    gtk_list_store_append(list_store, &iter);
                    gtk_list_store_set(list_store, &iter, 0, name, 1, age, 2, blood, 3, contact, -1);
                }
            }
        }
        fclose(fp);
    }

    gtk_widget_show_all(window);
}

// Creates and populates a TreeView with all data from a file
void create_list_view_window(GtkApplication *app, const gchar *title, const char *filename) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);

    GtkListStore *list_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
    g_object_unref(list_store);

    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);

    const char *column_titles[] = {"Name", "Age", "Blood Group", "Contact"};
    for (int i = 0; i < 4; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(column_titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    }

    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            char *name = strtok(line, ",");
            char *age = strtok(NULL, ",");
            char *blood = strtok(NULL, ",");
            char *contact = strtok(NULL, "\n");

            if (name && age && blood && contact) {
                GtkTreeIter iter;
                gtk_list_store_append(list_store, &iter);
                gtk_list_store_set(list_store, &iter, 0, name, 1, age, 2, blood, 3, contact, -1);
            }
        }
        fclose(fp);
    }

    gtk_widget_show_all(window);
}

// Custom callbacks to free allocated memory for widgets
static void free_reg_widgets(GtkWidget *widget, gpointer data) {
    g_free(data);
}
static void free_find_donor_widgets(GtkWidget *widget, gpointer data) {
    g_free(data);
}
static void free_logout_widgets(GtkWidget *widget, gpointer data) {
    g_free(data);
}


// --- UI CREATION: REGISTRATION & SEARCH FORMS ---

void create_registration_window(GtkApplication *app, const gchar *title, GCallback save_callback_func) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *name_label = gtk_label_new("Name:");
    GtkWidget *name_entry = gtk_entry_new();
    GtkWidget *age_label = gtk_label_new("Age:");
    GtkWidget *age_entry = gtk_entry_new();
    GtkWidget *blood_label = gtk_label_new("Blood Group:");
    GtkWidget *blood_combo = gtk_combo_box_text_new();
    GtkWidget *contact_label = gtk_label_new("Contact No:");
    GtkWidget *contact_entry = gtk_entry_new();
    GtkWidget *save_button = gtk_button_new_with_label("Save");

    const char *blood_groups[] = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};
    for(int i = 0; i < G_N_ELEMENTS(blood_groups); i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(blood_combo), blood_groups[i]);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(blood_combo), 0);

    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), age_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), age_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), blood_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), blood_combo, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), contact_label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), contact_entry, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), save_button, 1, 4, 1, 1);

    RegistrationWidgets *reg_widgets = g_malloc(sizeof(RegistrationWidgets));
    reg_widgets->name_entry = name_entry;
    reg_widgets->age_entry = age_entry;
    reg_widgets->blood_combo = blood_combo;
    reg_widgets->contact_entry = contact_entry;
    reg_widgets->parent_window = window;

    g_signal_connect(save_button, "clicked", save_callback_func, reg_widgets);
    g_signal_connect(window, "destroy", G_CALLBACK(free_reg_widgets), reg_widgets);

    gtk_widget_show_all(window);
}


// --- BUTTON CLICK HANDLERS ---

// Callback for the search donor button
static void on_search_donor_clicked(GtkButton *button, gpointer user_data) {
    FindDonorWidgets *widgets = (FindDonorWidgets *)user_data;
    const gchar *blood_group = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets->blood_combo));
    if (blood_group) {
        create_filtered_list_view_window(widgets->app, "Available Donors", DONOR_FILE, blood_group);
        g_free((gpointer)blood_group);
    }
}

// Callbacks for main menu buttons
static void on_add_donor_clicked(GtkButton *button, gpointer app) {
    create_registration_window(GTK_APPLICATION(app), "Donor Registration", G_CALLBACK(on_save_donor_clicked));
}
static void on_view_donors_clicked(GtkButton *button, gpointer app) {
    create_list_view_window(GTK_APPLICATION(app), "Registered Donors", DONOR_FILE);
}
static void on_add_receiver_clicked(GtkButton *button, gpointer app) {
    create_registration_window(GTK_APPLICATION(app), "Receiver Registration", G_CALLBACK(on_save_receiver_clicked));
}
static void on_view_receivers_clicked(GtkButton *button, gpointer app) {
    create_list_view_window(GTK_APPLICATION(app), "Registered Receivers", RECEIVER_FILE);
}

// Callback for the Find a Donor button
static void on_find_donor_clicked(GtkButton *button, gpointer app) {
    GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window), "Find a Donor");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget *label = gtk_label_new("Select Blood Group:");
    GtkWidget *blood_combo = gtk_combo_box_text_new();
    GtkWidget *search_button = gtk_button_new_with_label("Search");

    const char *blood_groups[] = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};
    for(int i = 0; i < G_N_ELEMENTS(blood_groups); i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(blood_combo), blood_groups[i]);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(blood_combo), 0);

    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), blood_combo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), search_button, FALSE, FALSE, 10);
    
    FindDonorWidgets *find_widgets = g_malloc(sizeof(FindDonorWidgets));
    find_widgets->app = GTK_APPLICATION(app);
    find_widgets->blood_combo = blood_combo;

    g_signal_connect(search_button, "clicked", G_CALLBACK(on_search_donor_clicked), find_widgets);
    g_signal_connect(window, "destroy", G_CALLBACK(free_find_donor_widgets), find_widgets);

    gtk_widget_show_all(window);
}

// Callback for the Check Blood Stock button
static void on_check_stock_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    const char *blood_groups[] = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};
    int counts[G_N_ELEMENTS(blood_groups)] = {0};

    FILE *fp = fopen(DONOR_FILE, "r");
    if (fp != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            char *name = strtok(line, ",");
            char *age = strtok(NULL, ",");
            char *blood = strtok(NULL, ",");

            if (blood) {
                for (int i = 0; i < G_N_ELEMENTS(blood_groups); i++) {
                    if (g_strcmp0(blood, blood_groups[i]) == 0) {
                        counts[i]++;
                        break;
                    }
                }
            }
        }
        fclose(fp);
    }
    
    GString *stock_message = g_string_new("Current Blood Stock (Donors):\n\n");
    for (int i = 0; i < G_N_ELEMENTS(blood_groups); i++) {
        g_string_append_printf(stock_message, "%s: %d\n", blood_groups[i], counts[i]);
    }

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", stock_message->str);
    g_string_free(stock_message, TRUE);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Callback for the Logout button
static void on_logout_clicked(GtkButton *button, gpointer user_data) {
    LogoutWidgets *widgets = (LogoutWidgets*)user_data;
    gtk_window_close(GTK_WINDOW(widgets->main_menu_window));
    activate(widgets->app, NULL);
}

// Callbacks for "Save" buttons in registration forms
static void on_save_donor_clicked(GtkButton *button, gpointer user_data) {
    RegistrationWidgets *widgets = (RegistrationWidgets *)user_data;
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(widgets->name_entry));
    const gchar *age = gtk_entry_get_text(GTK_ENTRY(widgets->age_entry));
    const gchar *blood_group = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets->blood_combo));
    const gchar *contact = gtk_entry_get_text(GTK_ENTRY(widgets->contact_entry));
    
    append_to_file(DONOR_FILE, name, age, blood_group, contact);
    g_free((gpointer)blood_group);

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(widgets->parent_window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Donor saved successfully!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    gtk_window_close(GTK_WINDOW(widgets->parent_window));
}

static void on_save_receiver_clicked(GtkButton *button, gpointer user_data) {
    RegistrationWidgets *widgets = (RegistrationWidgets *)user_data;
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(widgets->name_entry));
    const gchar *age = gtk_entry_get_text(GTK_ENTRY(widgets->age_entry));
    const gchar *blood_group = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets->blood_combo));
    const gchar *contact = gtk_entry_get_text(GTK_ENTRY(widgets->contact_entry));

    append_to_file(RECEIVER_FILE, name, age, blood_group, contact);
    g_free((gpointer)blood_group);

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(widgets->parent_window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Receiver saved successfully!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    gtk_window_close(GTK_WINDOW(widgets->parent_window));
}


// --- MAIN APPLICATION WINDOWS ---

// Creates the main dashboard window after login
static void mainpage(GtkApplication *app) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Main Menu");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 30);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label), "<span size='xx-large' weight='bold'>Management System</span>");

    GtkWidget *add_donor_button = gtk_button_new_with_label("Add Donor");
    GtkWidget *view_donors_button = gtk_button_new_with_label("View Donors");
    GtkWidget *add_receiver_button = gtk_button_new_with_label("Add Receiver");
    GtkWidget *view_receivers_button = gtk_button_new_with_label("View Receivers");
    GtkWidget *find_donor_button = gtk_button_new_with_label("Find a Donor");
    GtkWidget *check_stock_button = gtk_button_new_with_label("Check Blood Stock");
    GtkWidget *logout_button = gtk_button_new_with_label("Logout");

    gtk_box_pack_start(GTK_BOX(box), title_label, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(box), add_donor_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), view_donors_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), add_receiver_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), view_receivers_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), find_donor_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), check_stock_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), logout_button, FALSE, FALSE, 10);
    
    LogoutWidgets *logout_widgets = g_malloc(sizeof(LogoutWidgets));
    logout_widgets->app = app;
    logout_widgets->main_menu_window = window;
    
    g_signal_connect(add_donor_button, "clicked", G_CALLBACK(on_add_donor_clicked), app);
    g_signal_connect(view_donors_button, "clicked", G_CALLBACK(on_view_donors_clicked), app);
    g_signal_connect(add_receiver_button, "clicked", G_CALLBACK(on_add_receiver_clicked), app);
    g_signal_connect(view_receivers_button, "clicked", G_CALLBACK(on_view_receivers_clicked), app);
    g_signal_connect(find_donor_button, "clicked", G_CALLBACK(on_find_donor_clicked), app);
    g_signal_connect(check_stock_button, "clicked", G_CALLBACK(on_check_stock_clicked), NULL);
    g_signal_connect(logout_button, "clicked", G_CALLBACK(on_logout_clicked), logout_widgets);
    g_signal_connect(window, "destroy", G_CALLBACK(free_logout_widgets), logout_widgets);

    gtk_widget_show_all(window);
}

// Callback for the login button
static void on_button_clicked(GtkButton *button, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    const gchar *username = gtk_entry_get_text(widgets->entry1);
    const gchar *password = gtk_entry_get_text(widgets->entry2);

    if (g_strcmp0(username, "sumanyu") == 0 && g_strcmp0(password, "7689") == 0) {
        g_print("Login successful\n");
        gtk_window_close(GTK_WINDOW(widgets->window));
        mainpage(widgets->app);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(widgets->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid username or password!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

// Custom callback to free the AppWidgets struct.
static void free_app_widgets(GtkWidget *widget, gpointer data) {
    g_free(data);
}

// Function called when the application is activated to create the login window
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window, *button, *fixed, *entry1, *entry2;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Blood Donor and Receiver Management System");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    entry1 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry1), "Username");
    gtk_fixed_put(GTK_FIXED(fixed), entry1, 40, 150);

    entry2 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry2), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry2), FALSE);
    gtk_fixed_put(GTK_FIXED(fixed), entry2, 40, 200);

    button = gtk_button_new_with_label("Login");
    gtk_widget_set_size_request(button, 70, 30);
    gtk_fixed_put(GTK_FIXED(fixed), button, 80, 250);

    AppWidgets *widgets = g_malloc(sizeof(AppWidgets));
    widgets->entry1 = GTK_ENTRY(entry1);
    widgets->entry2 = GTK_ENTRY(entry2);
    widgets->window = window;
    widgets->app = app;

    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_button_clicked), widgets);
    g_signal_connect(window, "destroy", G_CALLBACK(free_app_widgets), widgets);

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, "window { background-image: url('b.jpg'); background-size: cover; background-position: center; }", -1, NULL);
    GtkStyleContext *style_context = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider);

    gtk_widget_show_all(window);
}

// --- MAIN FUNCTION ---
int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("c.project", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

