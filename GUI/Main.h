/******************* Section 0 : File Guard *******************/

#ifndef _MAIN_H
#define _MAIN_H

/******************* Section 1 : Includes *******************/

#include <gtk/gtk.h>
#include "Banking_Operations.h"

/******************* Section 2 : Macros Declarations *******************/



/******************* Section 3 : Macros Functions Declarations *******************/



/******************* Section 4 : User Defined Data Types & Variables Declarations *******************/

typedef struct {
    GtkWidget *window;
    GtkWidget *stack;
    
    // Main menu
    GtkWidget *btn_add_customer;
    GtkWidget *btn_edit_customer;
    GtkWidget *btn_display_customer;
    GtkWidget *btn_delete_customer;
    GtkWidget *btn_transfer_money;
    GtkWidget *btn_deposit;
    GtkWidget *btn_withdraw;
    
    // Add customer
    GtkWidget *add_name_entry;
    GtkWidget *add_id_entry;
    GtkWidget *add_phone_entry;
    GtkWidget *add_cash_entry;
    GtkWidget *add_status_label;
    
    // Edit customer
    GtkWidget *edit_id_entry;
    GtkWidget *edit_name_entry;
    GtkWidget *edit_phone_entry;
    GtkWidget *edit_status_label;
    
    // Display customer
    GtkWidget *display_id_entry;
    GtkWidget *display_info_text;
    
    // Delete customer
    GtkWidget *delete_id_entry;
    GtkWidget *delete_status_label;
    
    // Transfer money
    GtkWidget *transfer_sender_entry;
    GtkWidget *transfer_receiver_entry;
    GtkWidget *transfer_amount_entry;
    GtkWidget *transfer_status_label;
    
    // Deposit
    GtkWidget *deposit_id_entry;
    GtkWidget *deposit_amount_entry;
    GtkWidget *deposit_status_label;
    
    // Withdraw
    GtkWidget *withdraw_id_entry;
    GtkWidget *withdraw_amount_entry;
    GtkWidget *withdraw_status_label;
} AppWidgets;


/******************* Section 5 : Software Interfaces Declarations (APIs) *******************/

void show_message(GtkWidget *parent, const char *title, const char *message);
void show_error(GtkWidget *parent, const char *title, const char *message);
void on_back_button_clicked(GtkWidget *widget, gpointer data);
void on_add_customer_clicked(GtkWidget *widget, gpointer data);
void on_add_customer_submit_clicked(GtkWidget *widget, gpointer data);
void on_edit_customer_clicked(GtkWidget *widget, gpointer data);
void on_edit_customer_submit_clicked(GtkWidget *widget, gpointer data);
void on_display_customer_clicked(GtkWidget *widget, gpointer data);
void on_display_customer_submit_clicked(GtkWidget *widget, gpointer data);
void on_delete_customer_clicked(GtkWidget *widget, gpointer data);
void on_delete_customer_submit_clicked(GtkWidget *widget, gpointer data);
void on_transfer_money_clicked(GtkWidget *widget, gpointer data);
void on_transfer_money_submit_clicked(GtkWidget *widget, gpointer data);
void on_deposit_clicked(GtkWidget *widget, gpointer data);
void on_deposit_submit_clicked(GtkWidget *widget, gpointer data);
void on_withdraw_clicked(GtkWidget *widget, gpointer data);
void on_withdraw_submit_clicked(GtkWidget *widget, gpointer data);

#endif