# Bank-Customer-Management-System

This is a simple Bank Customer Management System written in the C programming language.
The program provides a set of essential banking operations through a terminal-based interface.
All customer data is stored in a plain text file (data_base.txt) and is automatically loaded and saved during the program’s execution.

Main Features (In Order):
    1.    Create Customer
Add a new customer with name, ID, phone number, and initial balance.
    2.    Edit Customer
Modify existing customer details such as name or phone number.
    3.    Display Customer
View customer information by entering their unique ID.
    4.    Delete Customer
Permanently remove a customer from the database using their ID.
    5.    Transfer Between 2 Customers
Send money from one customer’s account to another.
    6.    Deposit Money
Add money to a customer’s account balance.
    7.    Withdraw Money
Subtract money from a customer’s account balance if sufficient funds exist.

Project Files:
    •    Main.c              –> Main program logic and user interface handling.
    •    Bank_Operations.c   –> Business logic for all banking operations.
    •    Bank_Operations.h   –> Header file for function declarations and operation prototypes.
    •    Platform.h          –> Platform-specific data types and constants (e.g., uint32, sint32).
    •    Std_Libraries.h     –> Standard C library includes.
    •    data_base.txt       –> Stores all customer data persistently in plain text format.
    •    Main                –> The compiled executable file of the project.

Compilation:

To compile the program, use the following command:

in Terminal :
#gcc -o Main Main.c Bank_Operations.c

in GUI :
gcc `pkg-config --cflags gtk+-3.0` -o Main Main.c Banking_Operations.c `pkg-config --libs gtk+-3.0`

How It Works:
    •    Customer data is loaded from data_base.txt when the program starts.
    •    All operations are performed in memory using structured data types.
    •    Any change (create, update, delete, transfer, etc.) is saved back to the file immediately.
    •    You can open data_base.txt manually to verify stored customer records.

Technical Information:
    •    Language: C (C99)
    •    Platform: Linux/macOS (requires gcc)
    •    Interface: Terminal (command-line)
    •    Data Storage: Flat file (data_base.txt)
    •    Type Definitions: Custom fixed-width types defined in Platform.h

Author:
Mohammed Elahmady, Hatem Ayman, Mayar Basha, Al'aa Abd Elhady
