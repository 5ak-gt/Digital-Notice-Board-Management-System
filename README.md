# Digital-Notice-Board-Management-System

A console-based Digital Notice Board Management System developed using the C programming language as an Academic C Programming (ACP) project.

The system provides a digital platform for administrators to publish, update, delete, search, and manage notices, while students can access and search for important announcements.

## Features

### Administrator Module
- Administrator login authentication.
- Dashboard displaying total, active, and expired notices.
- Add and publish new notices.
- Display all published notices.
- Search notices by ID or keyword.
- Update existing notices.
- Delete notices with confirmation.
- View active and expired notices separately.
- Category-wise notice statistics.
- Administrator logout.

### Student Module
- View all published notices.
- Search notices using notice ID or keywords.
- View notice details, including title, category, publication date, expiry date, and description.

## Technologies Used

- **Programming Language:** C
- **Concepts:** Structures, Functions, File Handling, Pointers, String Handling, Conditional Statements, Loops.
- **Storage:** Binary File Handling.
- **Compiler:** GCC / MinGW.

## Data Management

The project uses file handling to store notice records persistently in a binary data file named `notices.dat`.

The system supports:
- Creating and storing notices.
- Reading saved notice records.
- Updating existing records.
- Deleting records.
- Generating unique notice IDs.
- Tracking notice expiry dates.

## How to Run the Project

### 1. Compile

Open a terminal in the project directory and execute:

```bash
gcc Digital_Notice_Board_Final.c -o noticeboard
```

### 2. Run

On Windows:

```bash
noticeboard.exe
```

Or:

```bash
./noticeboard
```

### 3. Administrator Login

The demonstration administrator password in the original source code is:

```text
ACP123
```

### 4. Student Access

Select the Student Access option from the main menu to view and search notices without administrator login.

## Project Objective

The main objective of this project is to replace traditional manual notice board management with a simple computerized system that makes publishing, accessing, updating, and managing institutional announcements easier and more organized.

## Future Enhancements

- Graphical User Interface (GUI).
- Database integration using MySQL or SQLite.
- Secure password hashing and authentication.
- Role-based user management.
- Email or notification-based announcements.
- Web-based access for students and administrators.

## Academic Project

**Project Title:** Digital Notice Board Management System  
**Project Type:** Individual Academic Project  
**Subject:** Advanced C Programming (ACP)  
**Academic Session:** 2026–2027

---

Developed as an academic project to demonstrate practical implementation of C programming concepts, structured programming, and file handling.
