# Library Management System

## 📌 Overview

The **Library Management System** is a robust, console-based application written in C designed to streamline library operations for both administrators and students. It replaces tedious manual tracking by providing automated inventory control, role-based user portals, and an efficient book issuing/returning mechanics pipeline.

### The Problem It Solves

Traditional paper-based or unstructured ledger systems in small institutions often suffer from misplaced data, calculation errors regarding overdue books, and poor inventory transparency. This system solves these issues by automating data transactions with integrated constraints, tracking overlapping student records accurately, and eliminating calculation human error.

### Who It Is For

* **Institutional Librarians / Administrators:** Looking for a lightweight, dependency-free terminal application to manage stock, oversee student registration, and audit logs.
* **Students:** Seeking a self-service portal to browse available books, check issue statuses, and securely modify profiles.
* **C Developers & Educators:** Anyone looking for a complete, production-grade reference architecture for raw file handling and data structures in native C.

### Why It Is Useful

The application provides high-utility persistence without requiring complex SQL server installations or heavy runtimes. It handles structural constraints natively (such as managing available stock metrics dynamically when checkouts occur) and features defensive data verification routines like leap year checks and input sanitization flags.

---

## ⚡ Features

### 🛡️ Secure Role-Based Authentication

* **Admin Access:** Pre-configured secure credential guard (`admin` / `admin123`) to unlock core organizational commands.
* **Student Portals:** Registration subsystem that writes to flat databases with confirm-password validation schemes and in-app password modification handlers.

### 📚 Inventory & Stock Subsystem

* Complete **CRUD capabilities** (Create, Read, Update, Delete) bound to individual unique book IDs.
* **Dynamic Search Routing:** Search indexes optimized for explicit ID lookups, partial title keyword match (`strstr`), or partial author name filters.

### 🔄 Circulation Control & Fine Pipeline

* **Transactional Issuing:** Validates availability ceilings. Automatically decrements inventory `available` counters upon successful user checkouts.
* **Return Engine:** Restores stock levels and forwards data to an interactive fine calculation module charging a baseline fine metric (Rs. 5 per day) for overdue timelines.
* **Defensive Date Validation:** Embedded parsing algorithm enforcing syntax structural integrity ($DD-MM-YYYY$) that dynamically cross-checks invalid ranges and leap years.

### 📊 Administrative Dashboard & Auditing

* **Real-time Performance Metrics:** Instantaneous compilation metrics of overall library assets including total distinct book counts, student registration counts, and active out-of-bounds issue pipelines.
* **Transactional Logs:** Separate file stream that logs structural mutations ("Book added", "New student registered", "Book issued") for audit tracking.

---

## 🛠️ Tech Stack

* **Frontend:** Interactive Command-Line Interface (CLI) utilizing stylized ANSI text dividers for clear presentation layouts.
* **Backend Core:** Standard C (C99/C11 Compliant Execution Context) utilizing structs (`Book`, `Student`, `Issue`) to implement custom serialization layers.
* **Database / Storage:** Persistent Flat File Database Engine writing structured Comma-Separated Values (CSV format) directly into system local files (`books.txt`, `students.txt`, `issues.txt`, `logs.txt`).
* **APIs / Integrations:** Standard C POSIX library bindings (`stdio.h`, `stdlib.h`, `string.h`) handling atomic native file mutations (`fopen`, `fscanf`, `fprintf`, `remove`, `rename`).

---

## 📐 Architecture & File Database Structure

The program leverages a data storage model separating business entities into explicit record files. Data rows are stored as comma-separated entries, serving as custom database tables.

```
                    ┌─────────────────────────┐
                    │  Library_Management.c   │ (Main Execution Core)
                    └────┬───────────────┬────┘
                         │               │
            ┌────────────▼────┐     ┌────▼────────────┐
            │   Admin Menu    │     │  Student Menu   │
            └────────────┬────┘     └────┬────────────┘
                         │               │
  ┌──────────────────────┼───────────────┼──────────────────────┐
  │ File Subsystems      │               │                      │
  │                      ▼               ▼                      │
  │ 📝 books.txt    -> [ID],[Title],[Author],[Qty],[Available]  │
  │ 📝 students.txt -> [ID],[Name],[Password]                   │
  │ 📝 issues.txt   -> [BookID],[StudentID],[IssueDate],[Due]   │
  │ 📝 logs.txt     -> [Activity Message Stream]                │
  └─────────────────────────────────────────────────────────────┘

```

### Data Formats Breakdown

#### 1. Books Data Schema (`books.txt`)

Stores individual book identities and stock states.

```csv
101,C Programming,E Balagurusamy,10,9
102,History of Science and Technology,Manohar Bhardwaj,5,5

```

#### 2. Student Data Schema (`students.txt`)

Stores login profiles and registration states.

```csv
99250008,Harkirat Singh,password

```

#### 3. Circulation Issue Schema (`issues.txt`)

Tracks checkout lifecycles. The final element indicates return states ($0 = Issued, 1 = Returned$).

```csv
101,99250008,31-05-2026,30-06-2026,0

```

---

## 🚀 Getting Started

### Prerequisites

To compile and execute the system, ensure you have a standard C compiler configured (e.g., `gcc`, `clang`, or MSVC environments).

### Compilation

Open your system terminal inside the root working directory where the source code file is located and run the following command:

```bash
gcc -o library_system Library_Management_System.c

```

### Execution

Run the compiled executable application directly from the terminal prompt:

#### Windows Environment

```cmd
library_system.exe

```

#### Linux / macOS Environments

```bash
./library_system

```

---

## 📖 User Guides & Operation Flow

### 1. Administration Interface Guide

* On app launch, select option `2` (Login).
* Enter User ID: **`admin`** and Password: **`admin123`**.
* **Adding Records:** Navigate to option `2` (Add Book). Input numerical identifiers, string fields, and overall allocation numbers. The application will compute corresponding stock structures.
* **Issuing Process:** Navigate to option `7`. Supply valid existing book keys and student IDs. Provide date information strictly following the $DD-MM-YYYY$ syntax framework.

### 2. Student Subsystem Guide

* Use option `1` (Register Student) on the initial startup layout screen if you don't have an active user profile.
* Log in with your personalized identity variables.
* **Self-Service Queries:** Browse all items without administrative permissions, or execute option `3` (My Issued Books) to view specific items assigned to your profile, checking individual return tracking dates.