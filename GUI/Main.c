// gcc `pkg-config --cflags gtk+-3.0` -o Main Main.c Banking_Operations.c `pkg-config --libs gtk+-3.0`
// ./Main

#include "Main.h"
// Global banking system

System *Bank;
FILE *data_base;
Error_States retval;

int main(int argc, sint8 *argv[]) {
    // Initialize banking system
    Bank = System_init(&retval);
    if (!Bank) {
        fprintf(stderr, "Failed to initialize banking system\n");
        return 1;
    }
    
    data_base = fopen(FILE_PATH, "a+");
    if (!data_base) {
        fprintf(stderr, "Failed to open database file\n");
        free(Bank);
        return 1;
    }
    
    Load_Customers_From_File(Bank, data_base);
    
    // Initialize GTK
    gtk_init(&argc, &argv);
    
    AppWidgets *widgets = g_slice_new(AppWidgets);
    
    // Create main window
    widgets->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(widgets->window), "Bank Customer Management System");
    gtk_window_set_default_size(GTK_WINDOW(widgets->window), 600, 400);
    gtk_container_set_border_width(GTK_CONTAINER(widgets->window), 10);
    g_signal_connect(widgets->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Create stack for navigation
    widgets->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(widgets->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(widgets->stack), 300);
    
    // Main Menu
    GtkWidget *main_menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(main_menu_box), gtk_label_new("Bank Customer Management System"));
    
    widgets->btn_add_customer = gtk_button_new_with_label("Add New Customer");
    widgets->btn_edit_customer = gtk_button_new_with_label("Edit Customer");
    widgets->btn_display_customer = gtk_button_new_with_label("Display Customer");
    widgets->btn_delete_customer = gtk_button_new_with_label("Delete Customer");
    widgets->btn_transfer_money = gtk_button_new_with_label("Transfer Money");
    widgets->btn_deposit = gtk_button_new_with_label("Deposit Money");
    widgets->btn_withdraw = gtk_button_new_with_label("Withdraw Money");
    
    gtk_box_pack_start(GTK_BOX(main_menu_box), widgets->btn_add_customer, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_menu_box), widgets->btn_edit_customer, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_menu_box), widgets->btn_display_customer, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_menu_box), widgets->btn_delete_customer, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_menu_box), widgets->btn_transfer_money, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_menu_box), widgets->btn_deposit, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_menu_box), widgets->btn_withdraw, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(widgets->stack), main_menu_box, "main_menu", "Main Menu");
    
    // Connect button signals
    g_signal_connect(widgets->btn_add_customer, "clicked", G_CALLBACK(on_add_customer_clicked), widgets);
    g_signal_connect(widgets->btn_edit_customer, "clicked", G_CALLBACK(on_edit_customer_clicked), widgets);
    g_signal_connect(widgets->btn_display_customer, "clicked", G_CALLBACK(on_display_customer_clicked), widgets);
    g_signal_connect(widgets->btn_delete_customer, "clicked", G_CALLBACK(on_delete_customer_clicked), widgets);
    g_signal_connect(widgets->btn_transfer_money, "clicked", G_CALLBACK(on_transfer_money_clicked), widgets);
    g_signal_connect(widgets->btn_deposit, "clicked", G_CALLBACK(on_deposit_clicked), widgets);
    g_signal_connect(widgets->btn_withdraw, "clicked", G_CALLBACK(on_withdraw_clicked), widgets);
    
    // Add Customer View
    GtkWidget *add_customer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(add_customer_box), gtk_label_new("Add New Customer"));
    
    widgets->add_name_entry = gtk_entry_new();
    widgets->add_id_entry = gtk_entry_new();
    widgets->add_phone_entry = gtk_entry_new();
    widgets->add_cash_entry = gtk_entry_new();
    
    gtk_box_pack_start(GTK_BOX(add_customer_box), gtk_label_new("Full Name:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(add_customer_box), widgets->add_name_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(add_customer_box), gtk_label_new("ID (6 digits):"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(add_customer_box), widgets->add_id_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(add_customer_box), gtk_label_new("Phone Number:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(add_customer_box), widgets->add_phone_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(add_customer_box), gtk_label_new("Initial Cash Amount:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(add_customer_box), widgets->add_cash_entry, FALSE, FALSE, 0);
    
    GtkWidget *add_submit_btn = gtk_button_new_with_label("Add Customer");
    g_signal_connect(add_submit_btn, "clicked", G_CALLBACK(on_add_customer_submit_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(add_customer_box), add_submit_btn, FALSE, FALSE, 0);
    
    widgets->add_status_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(add_customer_box), widgets->add_status_label, FALSE, FALSE, 0);
    
    GtkWidget *add_back_btn = gtk_button_new_with_label("Back to Main Menu");
    g_signal_connect(add_back_btn, "clicked", G_CALLBACK(on_back_button_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(add_customer_box), add_back_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(widgets->stack), add_customer_box, "add_customer", "Add Customer");
    
    // Edit Customer View
    GtkWidget *edit_customer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(edit_customer_box), gtk_label_new("Edit Customer Information"));
    
    widgets->edit_id_entry = gtk_entry_new();
    widgets->edit_name_entry = gtk_entry_new();
    widgets->edit_phone_entry = gtk_entry_new();
    
    gtk_box_pack_start(GTK_BOX(edit_customer_box), gtk_label_new("Customer ID:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(edit_customer_box), widgets->edit_id_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(edit_customer_box), gtk_label_new("New Name:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(edit_customer_box), widgets->edit_name_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(edit_customer_box), gtk_label_new("New Phone Number:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(edit_customer_box), widgets->edit_phone_entry, FALSE, FALSE, 0);
    
    GtkWidget *edit_submit_btn = gtk_button_new_with_label("Update Customer");
    g_signal_connect(edit_submit_btn, "clicked", G_CALLBACK(on_edit_customer_submit_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(edit_customer_box), edit_submit_btn, FALSE, FALSE, 0);
    
    widgets->edit_status_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(edit_customer_box), widgets->edit_status_label, FALSE, FALSE, 0);
    
    GtkWidget *edit_back_btn = gtk_button_new_with_label("Back to Main Menu");
    g_signal_connect(edit_back_btn, "clicked", G_CALLBACK(on_back_button_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(edit_customer_box), edit_back_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(widgets->stack), edit_customer_box, "edit_customer", "Edit Customer");
    
    // Display Customer View
    GtkWidget *display_customer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(display_customer_box), gtk_label_new("Display Customer Information"));
    
    widgets->display_id_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(display_customer_box), gtk_label_new("Customer ID:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(display_customer_box), widgets->display_id_entry, FALSE, FALSE, 0);
    
    GtkWidget *display_submit_btn = gtk_button_new_with_label("Show Customer");
    g_signal_connect(display_submit_btn, "clicked", G_CALLBACK(on_display_customer_submit_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(display_customer_box), display_submit_btn, FALSE, FALSE, 0);
    
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    widgets->display_info_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(widgets->display_info_text), FALSE);
    gtk_container_add(GTK_CONTAINER(scrolled_window), widgets->display_info_text);
    gtk_box_pack_start(GTK_BOX(display_customer_box), scrolled_window, TRUE, TRUE, 0);
    
    GtkWidget *display_back_btn = gtk_button_new_with_label("Back to Main Menu");
    g_signal_connect(display_back_btn, "clicked", G_CALLBACK(on_back_button_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(display_customer_box), display_back_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(widgets->stack), display_customer_box, "display_customer", "Display Customer");
    
    // Delete Customer View
    GtkWidget *delete_customer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(delete_customer_box), gtk_label_new("Delete Customer"));
    
    widgets->delete_id_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(delete_customer_box), gtk_label_new("Customer ID to Delete:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(delete_customer_box), widgets->delete_id_entry, FALSE, FALSE, 0);
    
    GtkWidget *delete_submit_btn = gtk_button_new_with_label("Delete Customer");
    g_signal_connect(delete_submit_btn, "clicked", G_CALLBACK(on_delete_customer_submit_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(delete_customer_box), delete_submit_btn, FALSE, FALSE, 0);
    
    widgets->delete_status_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(delete_customer_box), widgets->delete_status_label, FALSE, FALSE, 0);
    
    GtkWidget *delete_back_btn = gtk_button_new_with_label("Back to Main Menu");
    g_signal_connect(delete_back_btn, "clicked", G_CALLBACK(on_back_button_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(delete_customer_box), delete_back_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(widgets->stack), delete_customer_box, "delete_customer", "Delete Customer");
    
    // Transfer Money View
    GtkWidget *transfer_money_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(transfer_money_box), gtk_label_new("Transfer Money Between Customers"));
    
    widgets->transfer_sender_entry = gtk_entry_new();
    widgets->transfer_receiver_entry = gtk_entry_new();
    widgets->transfer_amount_entry = gtk_entry_new();
    
    gtk_box_pack_start(GTK_BOX(transfer_money_box), gtk_label_new("Sender ID:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(transfer_money_box), widgets->transfer_sender_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(transfer_money_box), gtk_label_new("Receiver ID:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(transfer_money_box), widgets->transfer_receiver_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(transfer_money_box), gtk_label_new("Amount to Transfer:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(transfer_money_box), widgets->transfer_amount_entry, FALSE, FALSE, 0);
    
    GtkWidget *transfer_submit_btn = gtk_button_new_with_label("Transfer Money");
    g_signal_connect(transfer_submit_btn, "clicked", G_CALLBACK(on_transfer_money_submit_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(transfer_money_box), transfer_submit_btn, FALSE, FALSE, 0);
    
    widgets->transfer_status_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(transfer_money_box), widgets->transfer_status_label, FALSE, FALSE, 0);
    
    GtkWidget *transfer_back_btn = gtk_button_new_with_label("Back to Main Menu");
    g_signal_connect(transfer_back_btn, "clicked", G_CALLBACK(on_back_button_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(transfer_money_box), transfer_back_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(widgets->stack), transfer_money_box, "transfer_money", "Transfer Money");
    
    // Deposit View
    GtkWidget *deposit_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(deposit_box), gtk_label_new("Deposit Money"));
    
    widgets->deposit_id_entry = gtk_entry_new();
    widgets->deposit_amount_entry = gtk_entry_new();
    
    gtk_box_pack_start(GTK_BOX(deposit_box), gtk_label_new("Customer ID:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(deposit_box), widgets->deposit_id_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(deposit_box), gtk_label_new("Amount to Deposit:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(deposit_box), widgets->deposit_amount_entry, FALSE, FALSE, 0);
    
    GtkWidget *deposit_submit_btn = gtk_button_new_with_label("Deposit Money");
    g_signal_connect(deposit_submit_btn, "clicked", G_CALLBACK(on_deposit_submit_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(deposit_box), deposit_submit_btn, FALSE, FALSE, 0);
    
    widgets->deposit_status_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(deposit_box), widgets->deposit_status_label, FALSE, FALSE, 0);
    
    GtkWidget *deposit_back_btn = gtk_button_new_with_label("Back to Main Menu");
    g_signal_connect(deposit_back_btn, "clicked", G_CALLBACK(on_back_button_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(deposit_box), deposit_back_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(widgets->stack), deposit_box, "deposit", "Deposit Money");
    
    // Withdraw View
    GtkWidget *withdraw_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(withdraw_box), gtk_label_new("Withdraw Money"));
    
    widgets->withdraw_id_entry = gtk_entry_new();
    widgets->withdraw_amount_entry = gtk_entry_new();
    
    gtk_box_pack_start(GTK_BOX(withdraw_box), gtk_label_new("Customer ID:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(withdraw_box), widgets->withdraw_id_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(withdraw_box), gtk_label_new("Amount to Withdraw:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(withdraw_box), widgets->withdraw_amount_entry, FALSE, FALSE, 0);
    
    GtkWidget *withdraw_submit_btn = gtk_button_new_with_label("Withdraw Money");
    g_signal_connect(withdraw_submit_btn, "clicked", G_CALLBACK(on_withdraw_submit_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(withdraw_box), withdraw_submit_btn, FALSE, FALSE, 0);
    
    widgets->withdraw_status_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(withdraw_box), widgets->withdraw_status_label, FALSE, FALSE, 0);
    
    GtkWidget *withdraw_back_btn = gtk_button_new_with_label("Back to Main Menu");
    g_signal_connect(withdraw_back_btn, "clicked", G_CALLBACK(on_back_button_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(withdraw_box), withdraw_back_btn, FALSE, FALSE, 0);
    
    gtk_stack_add_titled(GTK_STACK(widgets->stack), withdraw_box, "withdraw", "Withdraw Money");
    
    // Add stack to window and show
    gtk_container_add(GTK_CONTAINER(widgets->window), widgets->stack);
    gtk_widget_show_all(widgets->window);
    
    gtk_main();
    
    // Cleanup
    fclose(data_base);
    free(Bank);
    g_slice_free(AppWidgets, widgets);
    
    return 0;
}


void show_message(GtkWidget *parent, const sint8 *title, const sint8 *message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void show_error(GtkWidget *parent, const sint8 *title, const sint8 *message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_back_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(widgets->stack), "main_menu");
}

// Add Customer Functions
void on_add_customer_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(widgets->stack), "add_customer");
}

void on_add_customer_submit_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(widgets->add_name_entry));
    const gchar *id_str = gtk_entry_get_text(GTK_ENTRY(widgets->add_id_entry));
    const gchar *phone_str = gtk_entry_get_text(GTK_ENTRY(widgets->add_phone_entry));
    const gchar *cash_str = gtk_entry_get_text(GTK_ENTRY(widgets->add_cash_entry));
    
    if (strlen(name) == 0 || strlen(id_str) == 0 || strlen(phone_str) == 0 || strlen(cash_str) == 0) {
        gtk_label_set_text(GTK_LABEL(widgets->add_status_label), "Please fill all fields");
        return;
    }
    
    uint32 id = atoi(id_str);
    uint32 phone = atoi(phone_str);
    uint32 cash = atoi(cash_str);
    
    retval = Create_Customer(Bank, data_base, name, id, phone, cash);
    
    if (retval == OP_Success) {
        gtk_label_set_text(GTK_LABEL(widgets->add_status_label), "Customer added successfully!");
        gtk_entry_set_text(GTK_ENTRY(widgets->add_name_entry), "");
        gtk_entry_set_text(GTK_ENTRY(widgets->add_id_entry), "");
        gtk_entry_set_text(GTK_ENTRY(widgets->add_phone_entry), "");
        gtk_entry_set_text(GTK_ENTRY(widgets->add_cash_entry), "");
    } else {
        gtk_label_set_text(GTK_LABEL(widgets->add_status_label), "Failed to add customer. Check ID/phone uniqueness.");
    }
}

// Edit Customer Functions
void on_edit_customer_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(widgets->stack), "edit_customer");
}

void on_edit_customer_submit_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    
    const gchar *id_str = gtk_entry_get_text(GTK_ENTRY(widgets->edit_id_entry));
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(widgets->edit_name_entry));
    const gchar *phone_str = gtk_entry_get_text(GTK_ENTRY(widgets->edit_phone_entry));
    
    if (strlen(id_str) == 0 || strlen(name) == 0 || strlen(phone_str) == 0) {
        gtk_label_set_text(GTK_LABEL(widgets->edit_status_label), "Please fill all fields");
        return;
    }
    
    uint32 id = atoi(id_str);
    uint32 phone = atoi(phone_str);
    
    retval = Edit_Customer(Bank, data_base, id, name, phone);
    
    if (retval == OP_Success) {
        gtk_label_set_text(GTK_LABEL(widgets->edit_status_label), "Customer updated successfully!");
        gtk_entry_set_text(GTK_ENTRY(widgets->edit_id_entry), "");
        gtk_entry_set_text(GTK_ENTRY(widgets->edit_name_entry), "");
        gtk_entry_set_text(GTK_ENTRY(widgets->edit_phone_entry), "");
    } else {
        gtk_label_set_text(GTK_LABEL(widgets->edit_status_label), "Failed to update customer. Check ID/phone.");
    }
}

// Display Customer Functions
void on_display_customer_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(widgets->stack), "display_customer");
}

void on_display_customer_submit_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    
    const gchar *id_str = gtk_entry_get_text(GTK_ENTRY(widgets->display_id_entry));
    
    if (strlen(id_str) == 0) {
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->display_info_text)), "Please enter a customer ID", -1);
        return;
    }
    
    uint32 id = atoi(id_str);
    sint8 output[1024] = {0};
    
    retval = Display_Customer_information(Bank, id, output);
    
    if (retval == OP_Success) {
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->display_info_text)), output, -1);
    } else {
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->display_info_text)), "Customer not found", -1);
    }
}

// Delete Customer Functions
void on_delete_customer_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(widgets->stack), "delete_customer");
}

void on_delete_customer_submit_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    
    const gchar *id_str = gtk_entry_get_text(GTK_ENTRY(widgets->delete_id_entry));
    
    if (strlen(id_str) == 0) {
        gtk_label_set_text(GTK_LABEL(widgets->delete_status_label), "Please enter a customer ID");
        return;
    }
    
    uint32 id = atoi(id_str);
    
    retval = Delete_Customer(Bank, data_base, id);
    
    if (retval == OP_Success) {
        gtk_label_set_text(GTK_LABEL(widgets->delete_status_label), "Customer deleted successfully!");
        gtk_entry_set_text(GTK_ENTRY(widgets->delete_id_entry), "");
    } else {
        gtk_label_set_text(GTK_LABEL(widgets->delete_status_label), "Failed to delete customer");
    }
}

// Transfer Money Functions
void on_transfer_money_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(widgets->stack), "transfer_money");
}

void on_transfer_money_submit_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    
    const gchar *sender_str = gtk_entry_get_text(GTK_ENTRY(widgets->transfer_sender_entry));
    const gchar *receiver_str = gtk_entry_get_text(GTK_ENTRY(widgets->transfer_receiver_entry));
    const gchar *amount_str = gtk_entry_get_text(GTK_ENTRY(widgets->transfer_amount_entry));
    
    if (strlen(sender_str) == 0 || strlen(receiver_str) == 0 || strlen(amount_str) == 0) {
        gtk_label_set_text(GTK_LABEL(widgets->transfer_status_label), "Please fill all fields");
        return;
    }
    
    uint32 sender = atoi(sender_str);
    uint32 receiver = atoi(receiver_str);
    uint32 amount = atoi(amount_str);
    
    retval = Customer_Transfer_Money(Bank, data_base, sender, receiver, amount);
    
    if (retval == OP_Success) {
        gtk_label_set_text(GTK_LABEL(widgets->transfer_status_label), "Transfer completed successfully!");
        gtk_entry_set_text(GTK_ENTRY(widgets->transfer_sender_entry), "");
        gtk_entry_set_text(GTK_ENTRY(widgets->transfer_receiver_entry), "");
        gtk_entry_set_text(GTK_ENTRY(widgets->transfer_amount_entry), "");
    } else {
        gtk_label_set_text(GTK_LABEL(widgets->transfer_status_label), "Transfer failed. Check IDs and balance.");
    }
}

// Deposit Functions
void on_deposit_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(widgets->stack), "deposit");
}

void on_deposit_submit_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    
    const gchar *id_str = gtk_entry_get_text(GTK_ENTRY(widgets->deposit_id_entry));
    const gchar *amount_str = gtk_entry_get_text(GTK_ENTRY(widgets->deposit_amount_entry));
    
    if (strlen(id_str) == 0 || strlen(amount_str) == 0) {
        gtk_label_set_text(GTK_LABEL(widgets->deposit_status_label), "Please fill all fields");
        return;
    }
    
    uint32 id = atoi(id_str);
    uint32 amount = atoi(amount_str);
    
    retval = Customer_Deposit_Money(Bank, data_base, id, amount);
    
    if (retval == OP_Success) {
        gtk_label_set_text(GTK_LABEL(widgets->deposit_status_label), "Deposit completed successfully!");
        gtk_entry_set_text(GTK_ENTRY(widgets->deposit_id_entry), "");
        gtk_entry_set_text(GTK_ENTRY(widgets->deposit_amount_entry), "");
    } else {
        gtk_label_set_text(GTK_LABEL(widgets->deposit_status_label), "Deposit failed. Check customer ID.");
    }
}

// Withdraw Functions
void on_withdraw_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(widgets->stack), "withdraw");
}

void on_withdraw_submit_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    
    const gchar *id_str = gtk_entry_get_text(GTK_ENTRY(widgets->withdraw_id_entry));
    const gchar *amount_str = gtk_entry_get_text(GTK_ENTRY(widgets->withdraw_amount_entry));
    
    if (strlen(id_str) == 0 || strlen(amount_str) == 0) {
        gtk_label_set_text(GTK_LABEL(widgets->withdraw_status_label), "Please fill all fields");
        return;
    }
    
    uint32 id = atoi(id_str);
    uint32 amount = atoi(amount_str);
    
    retval = Customer_Withdraw_Money(Bank, data_base, id, amount);
    
    if (retval == OP_Success) {
        gtk_label_set_text(GTK_LABEL(widgets->withdraw_status_label), "Withdrawal completed successfully!");
        gtk_entry_set_text(GTK_ENTRY(widgets->withdraw_id_entry), "");
        gtk_entry_set_text(GTK_ENTRY(widgets->withdraw_amount_entry), "");
    } else {
        gtk_label_set_text(GTK_LABEL(widgets->withdraw_status_label), "Withdrawal failed. Check ID/balance.");
    }
}
