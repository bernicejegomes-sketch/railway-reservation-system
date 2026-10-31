# Railway Reservation and Waiting List Management System
## Project Bible, Technical Specification, Implementation Plan, and Git Workflow

> **Purpose of this document:** This is the single source of truth for planning, implementing, testing, integrating, and demonstrating the Data Structures mini project. Read this file before creating or modifying project files. Keep implementation consistent with this specification unless the whole team agrees to update it.

---

## 1. Project Identity

**Source-of-truth priority:** (1) the official college circular and approved problem statement, (2) this `PROJECT_BIBLE.md`, (3) `docs/API_CONTRACT.md` for exact frontend/backend message formats, (4) shared C header files for compiled data types and function signatures, and (5) code merged into `main`. If a conflict is found, do not guess: raise it, agree on the correction, update the relevant contract and then update code.

**Project title:** Railway Reservation and Waiting List Management System  
**Academic context:** Second-Year Computer Engineering — Data Structures Mini Project  
**Institution:** Fr. Conceicao Rodrigues College of Engineering (CRCE)  
**Implementation language requirement:** C for all application/backend logic  
**Frontend:** Basic HTML5, CSS3, and minimal vanilla JavaScript  
**Team size:** 3 coding members  
**Application type:** Local, menu-driven railway reservation system with a simple browser-based user interface  
**Primary goal:** Demonstrate correct use of data structures in C while providing a clear, attractive, easy-to-demonstrate interface.

### 1.1 Non-negotiable project rules

1. **All application/backend logic must be written in C.** Do not implement the backend in Python, Java, C++, Node.js, PHP, or another language.
2. Use **HTML and CSS** for the frontend. Vanilla JavaScript may be used only for frontend interactions and HTTP requests to the C backend. It must not duplicate or replace booking rules.
3. Use the C standard library and a small, documented C HTTP-server library if the team chooses browser-to-C integration. Do not introduce a large web framework.
4. The required data structures must be implemented in C using `struct`, pointers, arrays, and functions.
5. The project must remain suitable for an undergraduate Data Structures assessment. Prioritize understandable code and visible data-structure operations over unnecessary complexity.
6. Do not add an SQL database, cloud services, login provider, payment gateway, external API, or AI feature. They are not needed for this project.
7. The project must run locally and be demonstrable without an internet connection after setup.
8. Do not claim that this is connected to Indian Railways or a live railway booking service. It is an academic simulation with sample trains and routes.
9. The three members must work on separate Git branches and integrate through pull requests or reviewed merges.
10. Keep this document aligned with the college's submitted problem statement and deliverables.

---

## 2. College Requirements and Scope

The college's Data Structures mini-project circular dated 23/10/2026 requires:
- Groups of four students from the same practical batch.
- Selection of one sample problem statement or an approved similar topic.
- Implementation in C using appropriate data structures.
- A project report containing title, problem statement, objectives, data structures, solution description, and conclusion.
- Complete C source code with appropriate functions and comments.
- Demonstration of selected data structures and their operations.
- Test cases for functionality and input conditions.
- A working demonstration to faculty.
- All group members to contribute and explain the implementation.

**Team-size note:** The college circular says groups must contain **four members**. This project plan is being prepared for **three coding contributors**, as requested. If the group officially has only three members, confirm with the faculty whether that is permitted or identify the fourth group member and allocate documentation/testing/presentation duties. Do not silently assume the circular allows three members.

The selected problem statement is **Railway Reservation and Waiting List Management**. Its source requirements are to maintain passenger records and manage confirmed reservations, cancellations, waiting lists, and seat availability.

### 2.1 Project problem statement

Develop a C-based railway reservation simulation that stores train and passenger information, assigns seats, manages confirmed bookings, accepts waiting-list requests when capacity is exhausted, processes cancellations, and promotes the earliest eligible waiting passenger when a seat becomes available. A simple local browser interface should make the system easier to use and demonstrate, while all reservation decisions and data-structure operations remain in C.

### 2.2 Objectives

- Implement and demonstrate arrays, linked lists, queues, searching, sorting, pointers, structures, and file handling.
- Allocate seats without assigning the same seat twice on a train.
- Maintain confirmed passengers separately from waiting passengers.
- Enforce FIFO waiting-list order per train.
- Promote the earliest eligible waiting passenger after a confirmed cancellation.
- Support multiple independent trains.
- Maintain booking status and a basic event history.
- Display seat availability, waiting lists, booking details, and summary reports.
- Validate invalid input and handle boundary cases safely.
- Persist application data between program runs.
- Provide a clean, responsive, beginner-friendly browser UI without moving business logic out of C.

---

## 3. Technology Stack

| Layer | Technology | Responsibility |
|---|---|---|
| Backend/application logic | **C (C11 standard)** | All reservation rules, validation, seat allocation, queues, linked lists, search, sorting, reports, persistence |
| Frontend structure | HTML5 | Pages, forms, tables, navigation, content |
| Frontend styling | CSS3 | Layout, colours, responsive design, cards, status labels |
| Frontend interaction | Vanilla JavaScript (ES6+) | Form handling, fetch requests, updating UI from C backend responses; no reservation logic |
| HTTP integration | Small C HTTP server library, selected and documented before implementation | Serve API endpoints and optionally static frontend files |
| Persistence | Standard C file handling; CSV or carefully specified text files | Save and restore trains, bookings, seat state, and event history |
| Version control | Git and GitHub | Three feature branches and pull-request integration |
| Testing | C unit/integration tests or repeatable test harness; manual browser checks | Validate algorithms, APIs, and UI |
| Build | GCC or Clang with C11 flags | Compile the C program |

### 3.1 Architecture decision: C backend with HTML/CSS frontend

A browser cannot directly execute a normal compiled C program as a web server. The agreed design is:

1. Compile a C application that runs a small local HTTP server.
2. The C server serves the static HTML, CSS, and JavaScript files or the frontend is opened from the local server.
3. JavaScript sends HTTP requests to endpoints implemented in C.
4. C validates requests, executes all reservation logic, updates in-memory data structures, and returns JSON.
5. The browser displays the response.

**Do not use a JavaScript backend.** JavaScript in the browser is only a client. It must not determine which passenger gets a seat, perform queue promotion, or maintain the authoritative booking state.

### 3.2 HTTP library choice

Before implementing HTTP endpoints, the team must choose one small C-compatible HTTP server library, review its license, document its setup, and pin a version or commit. A possible option to evaluate is **Mongoose (embedded web server library)**. Do not assume the library is already installed. Do not copy unreviewed code or add multiple competing HTTP libraries.

If the course environment or faculty prefers a strictly console-based program, use the same C core and provide a menu-driven terminal interface as the required fallback/demo. The browser UI is an additional presentation layer; it must not be allowed to endanger the correctness of the C data structures.

### 3.3 Data format

- The C backend returns JSON for HTTP requests.
- A small, audited JSON parsing/serialization dependency may be used if needed; document its license and exact version in `docs/DEPENDENCIES.md`.
- Do not build a fragile JSON parser using ad-hoc string splitting.
- If an appropriate JSON dependency is not feasible, use clearly documented request formats and safe parsing, but do not accept arbitrary unvalidated input.
- Persistent data uses simple files handled by C. Use a robust write strategy (write temporary file, validate success, then replace the old file where the platform supports it).
- Never store passwords or sensitive personal data; no authentication is needed for this local classroom simulation.

---

## 4. Product Scope and User Roles

### 4.1 Roles

This is a local academic demo. It does not require separate passenger accounts or an administrator login.

- **Operator/user:** Uses the interface to add bookings, cancel tickets, search records, inspect trains, and view reports.
- **Developer/tester:** Runs test scenarios, inspects logs and data files, and verifies that internal structures remain consistent.

Optional admin login is deliberately excluded from the core scope because it does not strengthen the required data-structure demonstration enough to justify added complexity.

### 4.2 Out of scope

Do not implement:
- Live railway APIs, real train schedules, real ticket issuance, or real payments.
- OTP, email verification, user accounts, or authentication.
- AI, OCR, cloud hosting, external maps, or third-party booking systems.
- A database server such as MySQL, PostgreSQL, or MongoDB.
- React, Angular, Vue, Next.js, Node.js, Express, PHP, Python, Java, or C++.
- Complex coach/berth allocation, quotas, RAC, tatkal, dynamic pricing, refunds, or real railway rules.
- A frontend that stores the authoritative booking data in browser localStorage.
- Any business rule that exists only in JavaScript.

---

## 5. Feature Specification

Features are divided into **Core**, **Required quality**, and **Advanced but in-scope**. Implement core features first and only then complete the advanced ones.

### 5.1 Core features

#### F01 — Train management and multiple trains
- Maintain a predefined set of sample trains at startup, with an option to add trains if time permits.
- Each train has a unique train number, train name, source, destination, departure time (sample text), and capacity.
- Each train has its own seat array, confirmed-booking list, and waiting queue.
- A booking must reference a valid train.
- A seat or waiting-list position on one train must never affect another train.
- For the initial version, use a small set of predefined sample trains to avoid overcomplicating input and persistence.

#### F02 — Passenger registration and booking ID
- Capture passenger name, age, and optional gender field if approved by the team.
- Capture train number and a simple journey date string (validated format, not a real schedule lookup).
- Generate a unique booking ID in C.
- Never reuse a booking ID within saved history.
- Reject empty names, invalid ages, unknown train numbers, malformed dates if date validation is implemented, and duplicate active requests where the chosen rules define a duplicate.
- Do not collect government ID numbers, addresses, phone numbers, or other unnecessary personal information.

#### F03 — Seat allocation
- Use a fixed-capacity array per train to represent seat availability.
- When a valid booking arrives and a seat is free, allocate the lowest-numbered available seat and mark it occupied.
- Store the assigned seat number in the booking record.
- Ensure no two active confirmed bookings for the same train have the same seat number.
- If no seat is available, add the passenger to the train's waiting queue with status `WAITING` and seat number `0` (or a clearly documented sentinel such as `-1`; choose one and use it everywhere).
- Do not allocate a seat to a waiting passenger until promotion.

#### F04 — Confirmed passenger linked list
- Maintain confirmed bookings as a singly linked list per train.
- Implement insertion, traversal, lookup, and deletion functions.
- Each node points to a booking record or contains a well-defined booking structure.
- Use one clear ownership model to avoid duplicate records or double-free errors.
- A booking should remain searchable after cancellation through booking history, even if removed from the active confirmed list.

#### F05 — FIFO waiting list
- Implement a queue per train using a linked list with `front` and `rear` pointers, or a circular array queue if the team chooses and documents it.
- The recommended implementation is a linked queue because it demonstrates pointers and avoids a fixed waiting-list capacity.
- Implement `enqueue`, `dequeue`, `peek`, `is_empty`, and display/traversal.
- When the queue becomes empty after dequeue, set both `front` and `rear` to `NULL`.
- Waiting order is FIFO per train.
- Display a passenger's current waiting position by traversing the queue. Position can change when earlier entries are removed.

#### F06 — Cancellation and automatic promotion
- Accept a booking ID.
- Validate that it exists and has `CONFIRMED` status.
- Remove the booking from the active confirmed list, mark it `CANCELLED` in the persistent booking/history record, and release the seat.
- If the same train has a waiting passenger, dequeue the first eligible entry and assign the released seat to that passenger.
- Change that passenger's status from `WAITING` to `CONFIRMED`, set the seat number, insert into the confirmed list, and record a promotion event.
- Update all relevant counts and the seat array as one logical operation.
- Do not promote a passenger from another train.
- Reject repeated cancellation of an already-cancelled booking.
- If no passenger is waiting, leave the seat available.
- Avoid memory leaks when nodes are removed.

#### F07 — Search booking and passenger details
- Search by booking ID.
- Display passenger name, train number/name, journey date if present, status, seat number if confirmed, and waiting-list position if waiting.
- Search active lists and booking history as needed.
- Clearly distinguish “not found” from “found but cancelled.”
- A cancelled booking should not be mistaken for an active booking.

#### F08 — Seat chart and availability
- Display each train's capacity, occupied seats, and vacant seats.
- Show a simple textual seat map such as `01 [Booked] 02 [Available]`.
- Display total capacity, confirmed count, waiting count, and available count.
- Ensure `confirmed_count + available_count == capacity`.
- Waiting passengers do not count as occupying seats.

#### F09 — Menu-driven C interface
- The C core must expose functions that can be called from both the console interface and HTTP handlers.
- Provide a terminal menu as a fallback and useful debugging interface:
  1. List trains
  2. Book ticket
  3. Cancel ticket
  4. Search booking
  5. View confirmed passengers
  6. View waiting list
  7. View seat chart
  8. View reports
  9. Save data
  10. Exit
- Validate menu selections and handle non-numeric input without getting stuck in an input loop.
- The console interface is optional only if faculty explicitly accepts a browser-only demonstration; retaining it is recommended.

### 5.2 Advanced features recommended for final submission

#### F10 — Booking/cancellation/promotion history
- Keep a simple append-only event log with event type, booking ID, train number, and event timestamp or a sequence number.
- Record booking created, waiting-list entry, cancellation, promotion, and data-save events.
- Use a sequence counter if platform-independent timestamp handling becomes distracting.
- Do not remove historical booking IDs when active nodes are deleted.
- Display recent history or a report of events.

#### F11 — Search and sorting
- Support search by booking ID as a primary operation.
- Optional name search can be linear.
- Sort a temporary display copy of confirmed passenger summaries by booking ID, passenger name, or seat number.
- Use a syllabus-friendly algorithm such as insertion sort or bubble sort.
- Do not reorder the authoritative waiting queue just to display a sorted list.
- Do not claim binary search is used unless the searched data is actually maintained in sorted random-access storage.

#### F12 — Reports and occupancy statistics
- Total number of trains.
- Total confirmed bookings across all trains.
- Total waiting passengers.
- Total cancelled bookings from history.
- Available seats per train.
- Occupancy percentage = (confirmed seats / train capacity) × 100.
- Booking and cancellation event counts.
- Guard against division by zero if invalid zero-capacity trains are ever possible.

#### F13 — Persistent file storage
- Save and load train metadata, booking history/current statuses, seat allocation state, and waiting-list order.
- Store files under `data/`.
- Recommended logical files:
  - `data/trains.csv`
  - `data/bookings.csv`
  - `data/waiting_list.csv`
  - `data/events.csv`
- Use a documented CSV format with escaping rules, or a documented robust text format. If CSV fields can contain commas, implement escaping or disallow commas in those fields and validate that rule.
- Do not persist pointer values, raw linked-list nodes, or raw queue pointers.
- On startup, validate loaded records and rebuild the arrays, linked lists, and queues.
- Prevent duplicated seat assignments when restoring.
- Save after successful state-changing operations or provide an explicit Save action plus safe shutdown saving.
- If loading fails or a file is malformed, show a clear error and do not silently overwrite recoverable data with empty state.

#### F14 — Validation and error handling
- Reject invalid menu options, empty names, impossible ages, unknown trains, unknown booking IDs, duplicate cancellation, invalid capacity, and malformed persisted records.
- Use bounded input functions such as `fgets` rather than unsafe `gets`.
- Avoid `scanf` patterns that leave invalid input in the input buffer.
- Check allocation failures after every `malloc`/`calloc`.
- Free allocated memory at shutdown.
- Use consistent error messages and return codes from core functions.
- Do not partially update seat, queue, and booking state if an operation fails.

#### F15 — Responsive browser frontend
- Clean dashboard with a railway-inspired but simple visual theme.
- Display overview cards for total trains, confirmed bookings, waiting passengers, and vacant seats.
- Provide a train selector and list of available trains.
- Booking form with passenger name, age, train, and journey date.
- Cancellation form using booking ID.
- Booking search form.
- Seat availability/chart view.
- Confirmed passenger and waiting-list tables.
- Reports/history view.
- Show status badges for `CONFIRMED`, `WAITING`, and `CANCELLED`.
- Display success and error messages accessibly.
- Add loading and empty states.
- Responsive layout for desktop and mobile.
- Use only HTML, CSS, and vanilla JavaScript; no frontend framework.
- JavaScript may validate basic form fields for user experience, but the C backend must repeat all validation and be authoritative.
- Do not show fake success messages. Display only responses returned by the C backend.

### 5.3 Optional features only if time permits
- Add new train via operator form.
- Name-based passenger search.
- Sort confirmed passenger display by seat number/name.
- Fare estimate based on a simple fixed fare per sample route.
- A printable ticket/booking summary.
- Optional admin PIN only if explicitly approved by the team and implemented safely. It is not necessary for the core academic objectives.

---

## 6. Data Structures and C Models

Use a shared header file for common structures and function declarations. The exact structure definitions must be agreed before parallel implementation begins.

### 6.1 Agreed baseline C model

The following is the **recommended shared baseline** for the first implementation. The team should review it once at the start and then treat it as the common model. If a change is necessary, update this file and `docs/DATA_STRUCTURES.md` before dependent modules are changed.

```c
#ifndef RAILWAY_H
#define RAILWAY_H

#include <stddef.h>

#define MAX_TRAINS 10
#define MAX_SEATS_PER_TRAIN 100
#define MAX_BOOKINGS 500
#define MAX_EVENTS 2000
#define NAME_LEN 80
#define ROUTE_LEN 80
#define TIME_LEN 16
#define DATE_LEN 11
#define EVENT_LEN 24

typedef enum {
    BOOKING_CONFIRMED,
    BOOKING_WAITING,
    BOOKING_CANCELLED
} BookingStatus;

typedef struct {
    unsigned long booking_id;
    unsigned long created_sequence; /* preserves original order */
    char passenger_name[NAME_LEN];
    int age;
    char journey_date[DATE_LEN];    /* YYYY-MM-DD */
    int train_number;
    int seat_number;                /* 0 means no seat */
    BookingStatus status;
} Booking;

typedef struct WaitingNode WaitingNode;
typedef struct ConfirmedNode ConfirmedNode;

typedef struct {
    WaitingNode *front;
    WaitingNode *rear;
    size_t size;
} WaitingQueue;

typedef struct {
    ConfirmedNode *head;
    size_t size;
} ConfirmedList;

typedef struct {
    int train_number;
    char train_name[NAME_LEN];
    char source[ROUTE_LEN];
    char destination[ROUTE_LEN];
    char departure_time[TIME_LEN];  /* sample text, not a live timetable */
    int capacity;
    int seats[MAX_SEATS_PER_TRAIN]; /* 0 = free, 1 = occupied */
    ConfirmedList confirmed;
    WaitingQueue waiting;
} Train;

struct ConfirmedNode {
    unsigned long booking_id;       /* refers to central booking registry */
    ConfirmedNode *next;
};

struct WaitingNode {
    unsigned long booking_id;       /* refers to central booking registry */
    WaitingNode *next;
};

typedef struct {
    unsigned long sequence;
    char event_type[EVENT_LEN];
    unsigned long booking_id;
    int train_number;
} BookingEvent;

typedef struct {
    Train trains[MAX_TRAINS];
    size_t train_count;

    Booking bookings[MAX_BOOKINGS]; /* includes active and cancelled history */
    size_t booking_count;

    BookingEvent events[MAX_EVENTS];
    size_t event_count;

    unsigned long next_booking_id;
    unsigned long next_event_sequence;
} RailwaySystem;

#endif
```

This is a baseline, not code to paste blindly without checking compiler warnings and header dependencies. Keep shared definitions in the agreed header(s); do not define duplicate versions in multiple `.c` files.

### 6.2 Why this ownership model is recommended

- `RailwaySystem` owns the fixed arrays of trains, booking records, and event records.
- Each `Train` owns its seat array, confirmed linked-list nodes, and waiting-queue nodes.
- Confirmed-list and waiting-queue nodes store a `booking_id`, not a separately allocated copy of a booking.
- `RailwaySystem.bookings[]` is the central record registry. A cancelled booking remains there for search/history and is removed only from active queue/list structures.
- A promotion updates the same booking record from `BOOKING_WAITING` to `BOOKING_CONFIRMED`; it does not create a second booking.
- Queue/list nodes are allocated dynamically and freed when removed. The central booking record is not freed during cancellation.
- `MAX_BOOKINGS`, `MAX_TRAINS`, and `MAX_EVENTS` are deliberate academic-demo limits. Every insertion must check capacity and return a clear error if full.
- Booking IDs are monotonically generated and never reused, including after cancellation or restart.
- `created_sequence` preserves FIFO order when restoring a queue from a file.
- Seat numbers start at `1`; seat index `seat_number - 1` maps to the array.
- `seat_number == 0` means no seat is allocated. Never use `-1` in one module and `0` in another.

### 6.3 Required shared core function signatures

The team should agree on names and signatures before parallel coding. The following is the suggested public interface; additional private helpers may remain `static` inside implementation files.

```c
/* System lifecycle */
int railway_init(RailwaySystem *system);
void railway_cleanup(RailwaySystem *system);

/* Lookups */
Train *railway_find_train(RailwaySystem *system, int train_number);
Booking *railway_find_booking(RailwaySystem *system,
                              unsigned long booking_id);

/* Main operations */
int railway_book_ticket(RailwaySystem *system,
                        const char *passenger_name,
                        int age,
                        int train_number,
                        const char *journey_date,
                        Booking *result);

int railway_cancel_ticket(RailwaySystem *system,
                          unsigned long booking_id,
                          Booking *cancelled_result,
                          Booking *promoted_result);

int railway_get_seat_chart(const RailwaySystem *system,
                           int train_number,
                           int *seat_states,
                           size_t output_capacity);

int railway_validate_invariants(const RailwaySystem *system);

/* Data structures */
int waiting_queue_enqueue(WaitingQueue *queue, unsigned long booking_id);
int waiting_queue_dequeue(WaitingQueue *queue, unsigned long *booking_id_out);
int waiting_queue_peek(const WaitingQueue *queue,
                       unsigned long *booking_id_out);
int confirmed_list_insert(ConfirmedList *list, unsigned long booking_id);
int confirmed_list_remove(ConfirmedList *list, unsigned long booking_id);

/* Persistence */
int railway_save(const RailwaySystem *system, const char *data_directory);
int railway_load(RailwaySystem *system, const char *data_directory);
```

Return values must be documented in a shared header or `docs/DATA_STRUCTURES.md`. For example, `0` may mean success and nonzero values may identify invalid input, not found, capacity full, allocation failure, or persistence failure. Do not let different modules assign contradictory meanings to the same return code.

The API layer should call these core functions rather than reimplementing booking logic. If a function signature must change, coordinate with both backend owners and update the contract before merging.

### 6.4 Booking state-transition rules

Only these transitions are allowed in the core MVP:

```text
New request
   |
   +-- free seat available --> CONFIRMED
   |
   +-- no seat available ----> WAITING

WAITING -- promotion after cancellation --> CONFIRMED
CONFIRMED -- valid cancellation ---------> CANCELLED
WAITING -- optional waiting cancellation -> CANCELLED
CANCELLED -- any further cancellation --> reject
```

Rules:
- A `CONFIRMED` booking has one valid occupied seat on its train.
- A `WAITING` booking has `seat_number == 0` and appears exactly once in its train's queue.
- A `CANCELLED` booking has `seat_number == 0` and appears in neither active structure.
- Cancelling a waiting-list passenger is optional. If implemented, it removes the queue node and marks the record cancelled without freeing a seat.
- The core operation must validate the current state before mutating anything.
- If a node allocation or persistence operation fails, do not report success. Avoid partial updates; use validation and a carefully ordered operation sequence with rollback where necessary.

### 6.5 Structure and data-structure rules

- Use C `struct` and `enum` rather than parallel, unexplained arrays of unrelated fields.
- All queue, linked-list, seat-allocation, search, and cancellation operations belong in C.
- No JavaScript should assign seat numbers or promote waiting passengers.
- Each train must own independent seat state, confirmed list, and waiting queue.
- Every allocated node must have a clear owner and cleanup path.
- Never write raw pointers to persistent files.
- Do not use a global mutable variable for every operation if a `RailwaySystem` context structure can hold the state.

---

## 7. API Contract Between Frontend and C Backend

The browser UI communicates with the C server using local HTTP endpoints. The exact HTTP library must be selected first. Use JSON request/response bodies and a consistent response envelope.

### 7.1 Response convention

Successful response example:

```json
{
  "success": true,
  "message": "Booking confirmed.",
  "data": {
    "booking_id": 1001,
    "status": "CONFIRMED",
    "seat_number": 1
  }
}
```

Error response example:

```json
{
  "success": false,
  "message": "Booking ID not found.",
  "data": null
}
```

Use appropriate HTTP status codes, such as:
- `200 OK` for successful queries/actions.
- `201 Created` for newly created booking requests.
- `400 Bad Request` for invalid input.
- `404 Not Found` for missing train or booking.
- `409 Conflict` for duplicate or invalid state transitions.
- `500 Internal Server Error` for unexpected failures.

### 7.2 Proposed endpoints

| Method | Endpoint | Purpose |
|---|---|---|
| GET | `/api/health` | Check server status |
| GET | `/api/trains` | List sample trains and availability |
| GET | `/api/trains/{train_number}` | Details and seat availability for one train |
| POST | `/api/bookings` | Create confirmed or waiting booking |
| GET | `/api/bookings/{booking_id}` | Look up booking/status |
| POST | `/api/bookings/{booking_id}/cancel` | Cancel a confirmed booking and trigger promotion |
| GET | `/api/trains/{train_number}/confirmed` | List confirmed passengers |
| GET | `/api/trains/{train_number}/waiting` | List waiting passengers in FIFO order |
| GET | `/api/trains/{train_number}/seats` | Seat chart and availability |
| GET | `/api/reports/summary` | Summary and occupancy statistics |
| GET | `/api/events` | Booking and cancellation history |
| POST | `/api/system/save` | Explicitly persist current state if needed |

These endpoints are a contract proposal, not pre-existing code. Agree on request fields and response schemas in `docs/API_CONTRACT.md` before frontend and backend work proceed independently.

### 7.3 API implementation rules

- Route handlers are thin: parse and validate the request, call a C service function, and serialize the result.
- Business rules must live in reusable C core functions, not be duplicated in route handlers.
- Avoid constructing JSON by concatenating unescaped user input.
- Do not expose internal pointers or raw memory.
- Use a consistent JSON library and document it.
- Handle malformed request bodies, unknown routes, unsupported HTTP methods, and oversized inputs.
- Bind the server to loopback (`127.0.0.1`) for a local demo unless the team deliberately configures another safe environment.
- Do not use wildcard CORS rules unnecessarily. If the UI is served from the same C server, CORS may not be needed.
- Prevent two requests from simultaneously modifying the same booking/queue state unless the server is designed for concurrency. The simplest approach for this project is a single-threaded local server or an explicit lock around state-changing operations.

---

## 8. Folder and File Structure

Create the following repository layout. Do not create every file with large amounts of code at once; build incrementally.

```text
railway-reservation-system/
├── README.md
├── PROJECT_BIBLE.md
├── .gitignore
├── Makefile
├── docs/
│   ├── PROBLEM_STATEMENT.md
│   ├── REQUIREMENTS.md
│   ├── ARCHITECTURE.md
│   ├── DATA_STRUCTURES.md
│   ├── API_CONTRACT.md
│   ├── TEST_PLAN.md
│   ├── TEAM_WORK_SPLIT.md
│   ├── GIT_WORKFLOW.md
│   ├── DEPENDENCIES.md
│   └── DEMO_SCRIPT.md
├── backend/
│   ├── include/
│   │   ├── railway.h
│   │   ├── booking.h
│   │   ├── train.h
│   │   ├── waiting_queue.h
│   │   ├── confirmed_list.h
│   │   ├── booking_history.h
│   │   ├── persistence.h
│   │   ├── reports.h
│   │   ├── validation.h
│   │   ├── api_server.h
│   │   └── api_routes.h
│   ├── src/
│   │   ├── main.c
│   │   ├── railway.c
│   │   ├── train.c
│   │   ├── booking.c
│   │   ├── waiting_queue.c
│   │   ├── confirmed_list.c
│   │   ├── booking_history.c
│   │   ├── seat_manager.c
│   │   ├── search_sort.c
│   │   ├── cancellation.c
│   │   ├── reports.c
│   │   ├── persistence.c
│   │   ├── validation.c
│   │   ├── console_ui.c
│   │   ├── api_server.c
│   │   └── api_routes.c
│   └── tests/
│       ├── test_booking.c
│       ├── test_cancellation.c
│       ├── test_waiting_queue.c
│       ├── test_seat_manager.c
│       ├── test_persistence.c
│       └── test_reports.c
├── frontend/
│   ├── index.html
│   ├── pages/
│   │   ├── trains.html
│   │   ├── booking.html
│   │   ├── lookup.html
│   │   ├── waiting-list.html
│   │   └── reports.html
│   ├── css/
│   │   ├── styles.css
│   │   ├── components.css
│   │   └── responsive.css
│   ├── js/
│   │   ├── api.js
│   │   ├── dashboard.js
│   │   ├── booking.js
│   │   ├── lookup.js
│   │   ├── trains.js
│   │   ├── waiting-list.js
│   │   └── reports.js
│   └── assets/
│       └── README.md
├── data/
│   ├── .gitkeep
│   └── README.md
├── scripts/
│   ├── build.sh
│   ├── run.sh
│   └── clean.sh
└── .github/
    └── pull_request_template.md
```

### 8.1 File-creation rules

- Keep `backend/include/*.h` for declarations and shared type definitions.
- Keep implementation in matching `.c` files.
- Do not define the same structure in multiple headers.
- Avoid circular header dependencies; use forward declarations where appropriate.
- Do not put all logic into `main.c`.
- `main.c` initializes the system, loads files, starts the selected interface/server, and performs cleanup.
- `railway.c` coordinates core operations, but should not duplicate queue/list algorithms.
- `api_routes.c` translates HTTP requests to core function calls; it must not contain independent booking logic.
- `frontend/js/api.js` centralizes `fetch()` requests and error handling.
- `data/` contains runtime-generated data files. Do not commit real passenger data; use only fictional sample records.
- If the selected HTTP library requires vendor files, place them under a documented `third_party/` directory and add the license. Do not commit build output or local executables.

**Keep the implementation smaller if appropriate.** This is a proposed modular layout; if the team finds some modules unnecessary, it may combine closely related files with a written explanation. The core data structures and ownership boundaries must remain clear.

---

## 9. Team of Three: Work Division and Git Branches

The repository already exists on GitHub. Use these feature branch names unless the repository already has a conflicting naming convention.

| Member | Role | Branch |
|---|---|---|
| **Mireille — Frontend owner** | HTML, CSS, vanilla JavaScript, API integration, responsive UI | `frontend-member1` |
| **Bernice — C data-structures owner** | Train/booking structures, confirmed linked list, waiting queue, seat allocation, booking and cancellation core | `backend-member2` |
| **Eleen — C integration/services owner** | C HTTP server and API routes, persistence, search/sort, reports, integration tests and build integration | `backend-member3` |

These names and role assignments are the agreed team split and should also be copied into `docs/TEAM_WORK_SPLIT.md`. Branch names remain role-based (`frontend-member1`, `backend-member2`, and `backend-member3`) so existing branch workflows do not need to change.

### 9.1 Mireille — Frontend owner (`frontend-member1`)

Responsible for:
- `frontend/index.html`
- `frontend/pages/`
- `frontend/css/`
- `frontend/js/`
- `frontend/assets/README.md`
- Frontend integration with the agreed API contract
- Responsive layout, forms, tables, status badges, success/error/empty/loading states
- Browser testing and frontend section of `docs/DEMO_SCRIPT.md`

Must not:
- Implement booking allocation, cancellation rules, FIFO promotion, or authoritative state in JavaScript.
- Change C structure definitions or API schemas without coordinating with backend owners.
- Add React, Node.js, or another framework.

### 9.2 Bernice — C core/data structures owner (`backend-member2`)

Responsible for:
- `backend/include/railway.h`
- `backend/include/booking.h`
- `backend/include/train.h`
- `backend/include/waiting_queue.h`
- `backend/include/confirmed_list.h`
- `backend/include/booking_history.h`
- `backend/include/validation.h`
- `backend/src/railway.c`
- `backend/src/train.c`
- `backend/src/booking.c`
- `backend/src/waiting_queue.c`
- `backend/src/confirmed_list.c`
- `backend/src/booking_history.c`
- `backend/src/seat_manager.c`
- `backend/src/cancellation.c`
- `backend/src/validation.c`
- Related unit tests for queue, seats, booking, and cancellation
- `docs/DATA_STRUCTURES.md`

Must:
- Implement and document FIFO behavior and memory ownership.
- Keep the core independent of HTTP and browser code.
- Expose stable C functions for Eleen's API layer and Mireille's API contract.
- Write tests for seat reuse and promotion order.

### 9.3 Eleen — C services/integration owner (`backend-member3`)

Responsible for:
- `backend/include/persistence.h`
- `backend/include/reports.h`
- `backend/include/api_server.h`
- `backend/include/api_routes.h`
- `backend/src/search_sort.c`
- `backend/src/reports.c`
- `backend/src/persistence.c`
- `backend/src/console_ui.c`
- `backend/src/api_server.c`
- `backend/src/api_routes.c`
- `backend/src/main.c` integration (coordinate with Bernice before changing shared initialization/cleanup)
- `backend/tests/test_persistence.c`
- `backend/tests/test_reports.c`
- `Makefile`, `scripts/`, `docs/API_CONTRACT.md`, `docs/DEPENDENCIES.md`
- Integration testing, server startup/shutdown, and final build

Must:
- Call Bernice's core functions instead of duplicating business rules.
- Agree on API contracts before building routes.
- Document the HTTP library and JSON dependency.
- Ensure load/save does not persist pointers or corrupt queue order.

### 9.4 Shared work

All three members must:
- Review `PROJECT_BIBLE.md` before coding.
- Agree on common header files, `BookingStatus`, booking ID rules, seat sentinel, train capacity rules, and JSON response envelope.
- Review at least one pull request from another member.
- Test the integrated project on a clean checkout.
- Contribute to the report and prepare to explain the entire program.
- Never push directly to the main branch unless the repository's course workflow specifically requires it.

### 9.5 Git workflow

Suggested branch model:
- `main` — stable integrated version.
- `frontend-member1` — frontend work.
- `backend-member2` — C data structures/core.
- `backend-member3` — C services/integration.

Commands for each member after cloning:

```bash
git checkout main
git pull origin main
git checkout -b frontend-member1
# Or use backend-member2 / backend-member3 for the assigned member.
```

If the branch already exists remotely:

```bash
git fetch origin
git checkout -b frontend-member1 origin/frontend-member1
```

Commit and push:

```bash
git add .
git commit -m "Add responsive railway dashboard"
git push -u origin frontend-member1
```

Use descriptive commit messages, e.g.:
- `Implement FIFO waiting queue`
- `Add seat allocation and cancellation`
- `Add C API endpoints`
- `Build responsive booking interface`
- `Add file persistence and reports`

Then open a pull request from the member's branch into `main` (or into an agreed integration branch). At least one teammate should review it. Resolve conflicts locally, run tests, and only then merge.

**Important:** Do not all edit the same shared header or `main.c` at the same time. Bernice owns core model headers; Eleen coordinates integration changes with Bernice. Mireille should use the agreed API contract rather than waiting for the entire backend to be completed.

---

## 10. Shared Interfaces to Agree Before Parallel Coding

This is a **mandatory contract-first phase**, not optional paperwork. It exists to prevent three separate contributions from using different field names, status values, seat conventions, return codes, or JSON response formats.

Before members create separate modules, agree on these decisions in a short kickoff meeting:

1. Exact `Train` and `Booking` structures.
2. Whether the central booking registry is a linked list or a fixed-capacity array of pointers.
3. Booking ID generation and whether IDs persist across restarts.
4. Seat sentinel for no seat (`0` recommended because seats start at `1`).
5. Maximum train capacity and behavior for invalid capacities.
6. Exact date format (`YYYY-MM-DD`) and validation rules.
7. Ownership rules for booking records and list/queue nodes.
8. API request and response JSON fields.
9. Chosen HTTP server and JSON library, with versions/licenses.
10. Persistence file formats and save/load order.
11. Error codes/messages used by C core functions and API handlers.

Record the final choices in `docs/DATA_STRUCTURES.md` and `docs/API_CONTRACT.md`. Do not let separate members invent incompatible versions of the same model.

---

## 11. Visual Design Requirements

The frontend should be simple, polished, and suitable for a college demonstration.

### 11.1 Visual direction
- Theme: clean railway operations dashboard.
- Suggested palette:
  - Deep navy: `#16324F`
  - Railway blue: `#245B8F`
  - Light background: `#F4F7FB`
  - White cards: `#FFFFFF`
  - Success green: `#238636`
  - Warning amber: `#B7791F`
  - Error red: `#C0392B`
  - Main text: `#17212B`
  - Muted text: `#667085`
- Use consistent spacing, modest shadows, rounded cards, clear typography, and readable tables.
- Use system fonts; no external font download is required.
- Use simple icons only if available locally or via CSS/Unicode; the application should work offline.
- Provide visible focus states and labels for every form control.
- Do not rely on colour alone to communicate booking status.
- Provide a mobile layout with stacked cards and horizontally scrollable tables where needed.
- Avoid excessive animations, giant gradients, or overly decorative effects.

### 11.2 Dashboard layout
1. Header with project name and connection/server status.
2. Navigation: Dashboard, Trains, Book Ticket, Find Booking, Waiting Lists, Reports/History.
3. Four summary cards: trains, confirmed bookings, waiting passengers, available seats.
4. Train overview cards/table.
5. Quick booking and search actions.
6. Recent booking events.
7. Clear feedback region for API/server errors.

### 11.3 Accessibility and usability
- Use semantic HTML and appropriate form labels.
- Keep keyboard navigation functional.
- Show validation messages next to relevant controls.
- Announce success/error messages accessibly (e.g. `aria-live`).
- Confirm cancellation before submitting.
- Disable a submit button while its request is in progress to prevent accidental duplicate requests.
- Handle a backend that is not running by showing an informative error, not a fake result.

---

## 12. Build, Run, and Development Workflow

Exact commands depend on the selected HTTP library and compiler. Document the final commands in `README.md` after they have been tested.

### 12.1 Prerequisites
- GCC or Clang with C11 support.
- Git.
- A modern browser.
- `make` if the project uses the Makefile.
- The selected C HTTP library and JSON library, if used.

### 12.2 Build quality flags

Use strong compiler warnings, for example:

```bash
-std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
```

Warnings should be fixed or explicitly reviewed. Do not use warning suppression as a substitute for correcting issues.

### 12.3 Development stages

**Stage 1 — Repository and contracts**
- Create folder layout and this project bible.
- Agree on models, API contract, ownership, seat sentinel, and persistence formats.
- Add `.gitignore`, Makefile skeleton, and dependency documentation.
- Add sample trains and a basic console menu.

**Stage 2 — C data-structure core**
- Implement train records and seat arrays.
- Implement booking registry and confirmed linked lists.
- Implement FIFO waiting queues.
- Implement booking, cancellation, and automatic promotion.
- Add tests for edge cases before frontend integration.

**Stage 3 — Persistence and reporting**
- Implement save/load and validation of persisted records.
- Add search/sort and summary reports.
- Confirm the system reconstructs data structures correctly after restart.

**Stage 4 — C HTTP server**
- Select and integrate the small C HTTP library.
- Implement health, train, booking, cancellation, seat, waiting-list, and report endpoints.
- Test endpoints independently before connecting the frontend.

**Stage 5 — Frontend**
- Create HTML pages, CSS, and API client.
- Connect forms and tables to C API responses.
- Implement loading, success, error, and empty states.
- Verify the UI does not invent or locally simulate server results.

**Stage 6 — Integration and stabilization**
- Merge reviewed branches.
- Run all automated/manual tests on a clean checkout.
- Test save/restart, cancellation/promotion, multi-train independence, and invalid inputs.
- Fix memory leaks, stale UI state, and malformed data handling.

**Stage 7 — Report and demonstration**
- Complete the project report.
- Prepare a short demo script and sample data.
- Ensure each member can explain the C data structures and the full booking lifecycle.

---

## 13. Testing Requirements

### 13.1 Required functional tests

1. Book when seats are available → confirmed, seat assigned.
2. Fill all seats → next booking enters waiting queue.
3. Cancel a confirmed booking with a non-empty queue → first waiting passenger promoted.
4. Cancel a confirmed booking with an empty queue → seat becomes available.
5. Cancel a waiting passenger if waiting cancellation is implemented → remove that passenger without releasing a seat.
6. Search a confirmed booking → correct details.
7. Search a waiting booking → correct status and queue position.
8. Search a cancelled booking → history shows cancelled status.
9. Search an unknown ID → not-found result.
10. Repeat cancellation → rejected.
11. Invalid age, name, train number, or menu option → safely rejected.
12. Two trains → independent seat inventories and queues.
13. Save, close, restart, load → bookings, seats, statuses, and queue order restored.
14. Malformed data file → clear error; do not silently destroy data.
15. Allocation failure or capacity boundary → graceful failure without partial state update.
16. Multiple waiting passengers → promotion order remains FIFO.
17. Sorting displayed results → underlying waiting queue remains unchanged.
18. Seat occupancy report → counts match seat array and confirmed list.

### 13.2 Invariants that must always hold

- A seat belongs to at most one active confirmed booking on its train.
- Every confirmed booking has a valid, occupied seat.
- Every waiting booking has no seat and occurs exactly once in that train's queue.
- Cancelled bookings are absent from active confirmed and waiting structures.
- Confirmed count plus available seat count equals train capacity.
- Queue `front` and `rear` are both `NULL` when empty.
- Queue order is FIFO.
- A cancellation on one train cannot affect another train.
- The active booking ID is unique, and historical IDs are not reused.
- In-memory state and persisted state agree after a successful save/load cycle.

---

## 14. Data Safety, Memory Management, and Error Handling

- Use `fgets` or equivalent bounded input routines for console input.
- Parse numeric values using `strtol`/`strtoul` with error checking rather than relying on unsafe input patterns.
- Check every memory allocation.
- Free list and queue nodes when removed.
- Free all remaining allocations at shutdown.
- Avoid double-freeing booking records referenced by multiple structures.
- Validate strings and lengths before copying them into fixed-size arrays.
- Do not trust frontend validation; repeat it in C.
- Keep temporary files and backups when replacing persistent data where feasible.
- Do not commit generated personal data, executables, `.o` files, or local editor settings.
- Use fictional passenger names in demonstration data.
- Do not log unnecessary passenger details.

---

## 15. Suggested `.gitignore`

At minimum, ignore:
- `*.o`
- `*.exe`
- compiled executable names such as `railway_app`
- temporary save files such as `*.tmp`
- local editor settings when not shared intentionally
- runtime data files if they contain test sessions or generated state
- OS-specific files such as `.DS_Store`

Keep `data/.gitkeep` and a `data/README.md` with the documented format. If sample data is committed, use fictional records and make it clear that it is test data.

---

## 16. Required Documentation and Final Deliverables

Create and maintain:

1. `README.md` — overview, prerequisites, build/run instructions, features, team, and demo notes.
2. `PROJECT_BIBLE.md` — this full technical source of truth.
3. `docs/PROBLEM_STATEMENT.md` — problem, motivation, objectives, scope.
4. `docs/REQUIREMENTS.md` — functional and non-functional requirements.
5. `docs/ARCHITECTURE.md` — frontend/C-backend architecture and request flow.
6. `docs/DATA_STRUCTURES.md` — structure definitions, operations, complexity, memory ownership, invariants.
7. `docs/API_CONTRACT.md` — endpoints, request/response schemas, status codes.
8. `docs/TEST_PLAN.md` — test cases, expected outputs, evidence.
9. `docs/TEAM_WORK_SPLIT.md` — names, branches, files, contributions.
10. `docs/GIT_WORKFLOW.md` — branching, commits, pull requests, integration.
11. `docs/DEPENDENCIES.md` — compiler, HTTP library, JSON library, versions, licenses, setup.
12. `docs/DEMO_SCRIPT.md` — step-by-step demonstration and expected results.
13. Source code with comments.
14. Test files and repeatable build instructions.
15. Project report with the college's required sections.

The report should explicitly explain why each structure was chosen and demonstrate its operations with examples. Include screenshots of the frontend only as supporting evidence; the C source and data structures remain the main academic work.

---

## 17. Suggested Complexity Discussion for the Report

Use this section only after the team verifies that its implementation matches these operations.

| Operation | Likely complexity | Explanation |
|---|---|---|
| Find first free seat | `O(S)` | Scan the train's seat array; `S` is capacity |
| Add to linked waiting queue | `O(1)` | Maintain `rear` pointer |
| Remove from front of waiting queue | `O(1)` | Maintain `front` pointer |
| Search booking ID in linked registry | `O(B)` | Traverse `B` stored bookings |
| Insert at confirmed-list head | `O(1)` | Update head pointer |
| Remove confirmed record | `O(B)` | Find node then unlink it |
| Display waiting-list positions | `O(W)` | Traverse `W` waiting passengers |
| Calculate occupancy report | `O(T × S + B)` in a straightforward implementation | Depends on whether seat and booking counts are scanned separately |

Do not present these as measured performance results. They are theoretical time-complexity estimates and should be adjusted to match the final algorithms.

---

## 18. Demonstration Script

Use a small train capacity, such as three seats, so the main operations are visible quickly.

1. Start the C backend and open the frontend.
2. Show the list of sample trains and current seat availability.
3. Book three passengers on the same train; confirm seats 1, 2, and 3 are allocated.
4. Book a fourth passenger; show `WAITING` status and position 1.
5. Book a fifth passenger; show waiting position 2.
6. Search for the fourth passenger by booking ID.
7. Cancel the passenger occupying seat 2.
8. Show the fourth passenger automatically promoted and assigned seat 2.
9. Confirm the fifth passenger remains first in the waiting queue.
10. Show the updated seat chart, passenger list, waiting list, and report.
11. Demonstrate a cancellation of an unknown/already-cancelled booking and show safe error handling.
12. Save and restart the application; verify that records and queue order persist.
13. Show that a second train's seat chart and queue remain independent.
14. Briefly explain the array, confirmed linked list, FIFO queue, search, and file handling in the C code.

Do not use actual sensitive passenger data. Keep the demo reproducible with sample records.

---

## 19. Definition of Done

The project is ready for submission only when:

- [ ] It builds with the documented compiler command on a clean checkout.
- [ ] All backend business logic is implemented in C.
- [ ] The frontend uses only HTML, CSS, and minimal vanilla JavaScript.
- [ ] The confirmed list, waiting queue, and seat array are implemented and explained.
- [ ] Booking, cancellation, FIFO promotion, searching, and availability work correctly.
- [ ] Multiple trains have independent state.
- [ ] Input validation and error handling are tested.
- [ ] Persistence survives a restart without duplicating bookings or changing waiting order.
- [ ] The browser UI obtains authoritative data from the C backend.
- [ ] No fake success responses or browser-only booking state exist.
- [ ] All required documentation and test cases are complete.
- [ ] Each team member has a meaningful contribution and can explain the overall project.
- [ ] The Git branches are merged through the agreed review process.
- [ ] The final demo works on the machine that will be used for faculty evaluation.

---

## 20. Instructions for the AI Coding Assistant

Treat `PROJECT_BIBLE.md` as the project's source of truth. Before making changes:

1. Inspect the existing repository, branches, files, and build setup. Do not assume the repository is empty.
2. Read this file and the relevant documents under `docs/`.
3. Report the existing state and propose a small implementation step before making broad changes.
4. Do not replace C backend logic with another language.
5. Do not create unnecessary files or implement all modules in one huge response.
6. Implement in small, testable stages, starting with shared structures and the C core.
7. Preserve existing work and avoid destructive Git commands such as force-push, hard reset, or deleting branches unless the user explicitly approves.
8. Do not claim that compilation or tests passed unless they were actually run and the results were checked.
9. If a dependency, API, or environment setting is missing, document it and ask before inventing a substitute.
10. Keep frontend and backend contracts consistent. If a shared contract must change, update its documentation and coordinate the dependent modules.
11. Make sure each feature has a corresponding test or a documented reason why it cannot yet be tested.
12. Prefer clear beginner-friendly C code with comments over clever or unnecessarily abstract code.
13. Do not add unrequested cloud services, AI features, databases, or frameworks.
14. At the end of each work stage, summarize files changed, commands run, test results, known limitations, and the next safe step.

**First task for the AI assistant:** Inspect the existing GitHub repository checkout and report its current state. Then prepare a contract-first proposal with: (1) the exact shared C structures and ownership rules, (2) the initial public C function signatures, (3) the API request/response schemas and error codes, (4) the folder/file plan, (5) dependency choices with licenses and versions, and (6) a small integration plan. Do not immediately generate the entire application before the team reviews and approves these shared contracts.


---

## 21. Contract-First Development: Exact Procedure

The team must finish this procedure before all three members start building independent modules.

### 21.1 Step A — Inspect the existing repository

The AI coding assistant must inspect, not overwrite:
- Current branch and working-tree status.
- Existing files and directories.
- Existing `.gitignore`, README, Makefile, and dependency files.
- Git remotes and existing branch names.
- Any code already written by a group member.

It must report what exists and identify which files can be reused. It must not assume the repository is empty or run destructive Git commands.

### 21.2 Step B — Agree on the shared C data model

Bernice proposes the C structures. Eleen checks that they can be safely serialized into API responses and persisted. Mireille checks that the returned fields are sufficient for the interface.

The team must explicitly agree on:
- Field names and types.
- Maximum lengths/capacities.
- Booking status values.
- Seat number convention.
- Unique booking ID and sequence rules.
- Ownership of booking records and list/queue nodes.
- Error/return codes.
- Which operations may change state.
- Which module owns each function.

The C structures in Section 6 are the recommended starting point. Any accepted changes must be reflected in the shared headers and `docs/DATA_STRUCTURES.md`.

### 21.3 Step C — Agree on the API contract

Eleen owns the API contract draft. Mireille verifies that the frontend can consume it without guessing. Bernice confirms that every API action maps to an existing or planned C core function.

The contract must specify for every endpoint:
- HTTP method and path.
- Required and optional request fields.
- Field types and validation.
- Successful response fields.
- Error response fields.
- HTTP status codes.
- Whether the request changes state.
- Which core C function is called.
- Expected frontend loading, success, and failure behaviour.

The detailed endpoint examples in Section 22 form the initial contract. Update `docs/API_CONTRACT.md` with the final decisions before frontend integration.

### 21.4 Step D — Build a contract test before full implementation

Before completing all features:
1. Compile a tiny C server with `GET /api/health`.
2. Return a fixed, documented JSON health response.
3. Open the frontend and call the health endpoint using `fetch()`.
4. Verify success and server-unavailable handling.
5. Add a temporary train-list endpoint using sample data.
6. Confirm the browser can parse the response.
7. Replace temporary data with the real C core when ready.

A fixed health response is acceptable for health checks. **Booking and cancellation results must never be hardcoded or faked.**

### 21.5 Step E — Freeze version 1 of the contract

After the team agrees:
- Commit `backend/include/` shared headers.
- Commit `docs/DATA_STRUCTURES.md`.
- Commit `docs/API_CONTRACT.md`.
- Add a short note in the pull request stating the contracts have been reviewed.
- Members may then implement their modules independently.

If the contract later changes, the pull request must explain why and list the dependent modules/files that need updates.

### 21.6 Contract-change checklist

Before accepting a breaking change, ask:
- Does it change a C field name/type or status enum?
- Does it change the seat sentinel or queue ordering?
- Does it change the endpoint, JSON field, or response envelope?
- Does it change persistence format?
- Does it affect tests or the demo script?
- Have both frontend and backend owners been notified?
- Have the contract documents and tests been updated?

Do not merge a change that leaves one part of the application using the old contract.

---

## 22. Detailed API Contract — Version 1 Proposal

This section makes the browser/C boundary explicit. These endpoints are local academic APIs, not public railway services.

### 22.1 Base URL and server behaviour

- Development base URL: `http://127.0.0.1:8080`
- Frontend should be served by the same C server where practical, so browser requests can use relative paths such as `/api/trains`.
- Content type for JSON requests and responses: `application/json; charset=utf-8`.
- API prefix: `/api`.
- The C server is local-only by default.
- A request body has a documented maximum size.
- Unknown endpoints return JSON errors rather than an HTML error page.
- `GET` requests must not mutate reservation state.
- `POST` requests are used for booking, cancellation, and explicit save operations.

### 22.2 Standard response envelope

Success:

```json
{
  "success": true,
  "message": "Request completed.",
  "data": {}
}
```

Failure:

```json
{
  "success": false,
  "message": "A human-readable explanation.",
  "error_code": "BOOKING_NOT_FOUND",
  "data": null
}
```

The frontend must rely on `success`, `message`, and the documented `data` fields. It must not parse error meaning from free-form text alone when an `error_code` is available.

### 22.3 `GET /api/health`

Purpose: verify that the C HTTP server is running.

Example response:

```json
{
  "success": true,
  "message": "Railway Reservation API is running.",
  "data": {
    "status": "ok",
    "application": "railway-reservation-system"
  }
}
```

This endpoint does not prove that every data file loaded correctly. If startup loading fails, the server should report a controlled degraded/error state or refuse to start, according to the agreed startup policy.

### 22.4 `GET /api/trains`

Purpose: list trains and current seat counts.

Example response:

```json
{
  "success": true,
  "message": "Trains retrieved.",
  "data": {
    "trains": [
      {
        "train_number": 1201,
        "train_name": "Coastal Express",
        "source": "Mumbai",
        "destination": "Pune",
        "departure_time": "08:30",
        "capacity": 3,
        "confirmed_count": 3,
        "waiting_count": 2,
        "available_count": 0
      }
    ]
  }
}
```

The example capacity of three is for demonstration only. Production sample capacity can be larger. All counts must be computed from authoritative C state.

### 22.5 `GET /api/trains/{train_number}`

Purpose: retrieve one train's details and counts.

- `200 OK`: train found.
- `404 Not Found`: unknown train number.
- Returned fields match the train object and calculated counts documented above.

### 22.6 `POST /api/bookings`

Purpose: create a booking request.

Request:

```json
{
  "passenger_name": "Aarav Shah",
  "age": 20,
  "train_number": 1201,
  "journey_date": "2026-11-15"
}
```

Validation:
- `passenger_name` is required, trimmed, within the agreed length, and contains no line breaks or unsupported CSV delimiters.
- `age` is an integer within the project-defined range. The team should use a clear educational rule such as 1–120.
- `train_number` must exist.
- `journey_date` must match `YYYY-MM-DD`. If full calendar validation is not implemented, clearly document the limit; do not pretend that format validation checks a real train schedule.
- Booking IDs are generated by C; clients must not choose their own booking IDs.
- The frontend must not send a seat number or booking status as authoritative input.

Confirmed response example:

```json
{
  "success": true,
  "message": "Booking confirmed.",
  "data": {
    "booking_id": 1001,
    "train_number": 1201,
    "passenger_name": "Aarav Shah",
    "status": "CONFIRMED",
    "seat_number": 1,
    "waiting_position": null
  }
}
```

Waiting-list response example:

```json
{
  "success": true,
  "message": "Train is full. Passenger added to the waiting list.",
  "data": {
    "booking_id": 1004,
    "train_number": 1201,
    "passenger_name": "Aarav Shah",
    "status": "WAITING",
    "seat_number": 0,
    "waiting_position": 1
  }
}
```

A waiting-list booking is a successfully created booking request, so it can return `201 Created` as well. Do not treat it as an HTTP failure merely because no seat is available.

### 22.7 `GET /api/bookings/{booking_id}`

Purpose: find a booking, including historical status.

Response fields:
- `booking_id`
- `passenger_name`
- `age`
- `train_number`
- `train_name`
- `journey_date`
- `status`
- `seat_number`
- `waiting_position` (`null` unless waiting)

A cancelled record should return `CANCELLED`, not `404`, because the booking exists in history. Use `404` only if the booking ID has never existed.

### 22.8 `POST /api/bookings/{booking_id}/cancel`

Purpose: cancel a confirmed booking and process promotion.

Request body may be empty. Do not accept a client-supplied seat number or promoted booking ID.

Response example when a passenger is promoted:

```json
{
  "success": true,
  "message": "Booking cancelled. Waiting passenger promoted.",
  "data": {
    "cancelled_booking_id": 1002,
    "released_seat_number": 2,
    "promoted_booking": {
      "booking_id": 1004,
      "status": "CONFIRMED",
      "seat_number": 2
    }
  }
}
```

Response example when nobody is waiting:

```json
{
  "success": true,
  "message": "Booking cancelled. Seat is now available.",
  "data": {
    "cancelled_booking_id": 1002,
    "released_seat_number": 2,
    "promoted_booking": null
  }
}
```

Errors:
- `404 BOOKING_NOT_FOUND`: ID never existed.
- `409 BOOKING_ALREADY_CANCELLED`: booking is already cancelled.
- `409 BOOKING_NOT_CONFIRMED`: the booking is waiting, not confirmed; use a separate waiting-cancellation feature if implemented.
- `500 PERSISTENCE_ERROR`: operation could not be safely saved. The backend must define and test whether it rolls back the in-memory mutation or reports a clearly recoverable unsaved state.

### 22.9 `GET /api/trains/{train_number}/confirmed`

Return confirmed passengers for that train. Each item includes booking ID, passenger name, seat number, and status. Do not return passengers belonging to other trains.

### 22.10 `GET /api/trains/{train_number}/waiting`

Return waiting passengers in **FIFO order**. Each item includes booking ID, passenger name, and one-based current waiting position. Do not sort this result alphabetically or by booking ID after retrieval; it must reflect the actual queue order.

### 22.11 `GET /api/trains/{train_number}/seats`

Example response:

```json
{
  "success": true,
  "message": "Seat chart retrieved.",
  "data": {
    "train_number": 1201,
    "capacity": 3,
    "available_count": 1,
    "confirmed_count": 2,
    "seats": [
      {"seat_number": 1, "status": "BOOKED", "booking_id": 1001},
      {"seat_number": 2, "status": "AVAILABLE", "booking_id": null},
      {"seat_number": 3, "status": "BOOKED", "booking_id": 1003}
    ]
  }
}
```

The seat chart is calculated by C. A browser must not infer seat availability from stale UI state.

### 22.12 `GET /api/reports/summary`

Return:
- Total trains.
- Total capacity.
- Total confirmed bookings.
- Total waiting passengers.
- Total cancelled bookings.
- Total vacant seats.
- Per-train occupancy percentage.

Use numeric values for counts and percentages. The frontend formats them for display.

### 22.13 `GET /api/events`

Return a bounded list of recent booking events, newest first for display. Event ordering in the UI does not alter queue ordering. Include sequence, event type, booking ID, and train number. If timestamps are not implemented, use the monotonically increasing sequence and label it as event order rather than wall-clock time.

### 22.14 `POST /api/system/save`

Optional explicit save endpoint. The server may also save after successful state-changing operations and on controlled shutdown. The endpoint should return success only after the selected persistence policy completes.

### 22.15 Error code catalogue

Use consistent codes, for example:
- `INVALID_REQUEST`
- `INVALID_PASSENGER_NAME`
- `INVALID_AGE`
- `INVALID_JOURNEY_DATE`
- `TRAIN_NOT_FOUND`
- `BOOKING_NOT_FOUND`
- `BOOKING_ALREADY_CANCELLED`
- `BOOKING_NOT_CONFIRMED`
- `TRAIN_CAPACITY_FULL` (use only when the waiting list itself cannot accept a request)
- `BOOKING_CAPACITY_REACHED`
- `MEMORY_ALLOCATION_FAILED`
- `PERSISTENCE_ERROR`
- `INVALID_ROUTE`
- `METHOD_NOT_ALLOWED`
- `INTERNAL_ERROR`

The API should not expose stack traces, filesystem paths, memory addresses, or internal implementation details.

---

## 23. Persistence Format and Startup/Shutdown Contract

Persistence is a core feature. The format must be documented before implementation because both runtime state and the waiting queue must be restored consistently.

### 23.1 Proposed files

```text
data/
├── trains.csv
├── bookings.csv
├── events.csv
└── README.md
```

The seat array and confirmed/waiting linked nodes are rebuilt from train and booking records; do not serialize pointers or raw in-memory structures.

### 23.2 Proposed CSV headers

`trains.csv`:

```text
train_number,train_name,source,destination,departure_time,capacity
```

`bookings.csv`:

```text
booking_id,created_sequence,passenger_name,age,journey_date,train_number,seat_number,status
```

`events.csv`:

```text
sequence,event_type,booking_id,train_number
```

The team must decide one field-escaping policy. For a beginner-friendly first version, validate and reject commas, quotes, carriage returns, and newlines in text fields so a simple CSV parser remains safe. If the team wants to allow those characters, implement proper CSV quoting and escaping instead. Never silently split a name into extra columns.

### 23.3 Startup load order

1. Initialize all arrays, counts, queue pointers, and list heads to safe empty values.
2. Load and validate train metadata.
3. Load booking records and validate unique IDs, train references, status values, seat ranges, and capacities.
4. Clear seat arrays and rebuild seat occupancy from confirmed bookings.
5. Rebuild each train's confirmed linked list from confirmed booking records.
6. Rebuild each waiting queue from `WAITING` records sorted by `created_sequence` in ascending order.
7. Verify every system invariant.
8. Load event history and restore next ID/sequence counters.
9. Start the HTTP server only after state is valid.

If the team loads events before bookings, it must still restore the next ID counter correctly. Never reset `next_booking_id` to a value that would reuse an ID in the saved history.

### 23.4 Save procedure

- Validate invariants before saving.
- Write each file to a temporary file first.
- Check every write and close operation.
- Replace the previous file only after the temporary file is complete, where the platform supports safe replacement.
- Document that replacing several separate files is not a fully atomic multi-file transaction. If a save fails partway through, preserve backups or report a recoverable error rather than pretending the save succeeded.
- Do not erase the old valid file before a new one has been written successfully.
- Use only fictional demo data in the repository.

### 23.5 Persistence acceptance criteria

- A restart restores all trains, active confirmed bookings, waiting-list order, cancelled history, and ID counters.
- The same seat is not allocated to two bookings after reload.
- A promoted passenger is not also left in the waiting queue after reload.
- Malformed records are reported clearly.
- A failed save is not shown as successful in the UI.
- No pointer values are present in the data files.

---

## 24. Detailed C Module Boundaries

This section clarifies what belongs in each backend module. It is intentionally explicit to reduce merge conflicts.

| Module | Owns | Must not own |
|---|---|---|
| `railway.c` | Initialize/cleanup system, coordinate public operations, invariant validation | JSON parsing, HTML rendering |
| `train.c` | Train lookup, train initialization, per-train metadata and counts | Booking HTTP routes |
| `booking.c` | Booking record creation, ID generation, status lookup | HTML form validation only |
| `seat_manager.c` | Find/allocate/release seats, seat chart calculation | Waiting queue ordering |
| `confirmed_list.c` | Confirmed linked-list insertion/removal/traversal | Booking record ownership |
| `waiting_queue.c` | Queue enqueue/dequeue/peek/traversal and front/rear invariants | Seat allocation |
| `cancellation.c` | Validate cancellation, coordinate release/promotion/state updates | JSON serialization |
| `booking_history.c` | Booking registry and event history lookup/append | HTTP routing |
| `search_sort.c` | Search and sort display copies/results | Reordering the waiting queue |
| `persistence.c` | Validate/read/write persistence files and rebuild state | Frontend response formatting |
| `reports.c` | Aggregate counts and occupancy statistics | Mutating booking state |
| `validation.c` | Shared input validation helpers | UI-only styling |
| `console_ui.c` | Read menu input, call core functions, print results | Duplicate business rules |
| `api_server.c` | HTTP library setup, lifecycle, static file serving if supported | Reservation policy |
| `api_routes.c` | Parse endpoint request, call core functions, serialize response | Independent copy of booking logic |
| `main.c` | Configure paths, initialize/load, choose server/console mode, shutdown/save/cleanup | All application logic in one file |

### 24.1 Public versus private functions

- Put cross-module declarations in headers.
- Keep helper functions `static` when they are used only inside one `.c` file.
- Avoid putting function implementations in headers.
- Include header guards in every header.
- Include the headers a `.c` file actually needs; do not rely on accidental transitive includes.
- Use `const` for parameters that must not be modified.
- Return status codes and fill output parameters rather than returning pointers to temporary local variables.
- Compile with warnings enabled after every module is added.

### 24.2 Memory ownership rules

- The central booking array owns booking records by value.
- List/queue nodes own only their own allocated node memory and store a booking ID.
- Removing a list/queue node frees that node, not the booking registry record.
- `railway_cleanup()` frees all remaining confirmed and waiting nodes for every train.
- A failed enqueue must not leave a booking marked `WAITING` if the queue node was never created.
- A failed confirmed-list insertion must not leave a booking marked `CONFIRMED` with no list node.
- Test these failure paths where practical.

---

## 25. Frontend-to-Backend Integration Rules

### 25.1 One shared API client

`frontend/js/api.js` should:
- Define or centralize the API base path.
- Set JSON request headers where a body is sent.
- Parse the standard response envelope.
- Handle non-2xx status codes.
- Detect network errors and server-unavailable conditions.
- Return structured results to page modules.
- Avoid duplicate `fetch()` implementations in every page.

### 25.2 UI state rules

- The C backend is the source of truth for trains, bookings, statuses, seats, and queue order.
- After booking/cancellation, refresh the relevant data from the API.
- Do not update a seat chart optimistically unless the UI can reliably reconcile it with the server response; the simplest MVP should refresh from C after each mutation.
- Disable submit buttons while a request is pending.
- Confirm cancellation before sending the request.
- Show an error when the server is unreachable.
- Do not keep authoritative booking records in `localStorage`.
- Do not hardcode sample booking success into frontend scripts.
- Static sample train data may be used in a clearly labelled mockup during UI development, but it must be removed or gated off before final integration.

### 25.3 Suggested integration sequence

1. Build `/api/health` and test it with browser `fetch()`.
2. Build `/api/trains` with sample C data.
3. Connect the train list/dashboard.
4. Connect seat-chart endpoint.
5. Connect booking form.
6. Connect booking lookup.
7. Connect cancellation and promotion result.
8. Connect confirmed/waiting tables.
9. Connect reports and events.
10. Test the full cancellation/promotion sequence from the UI.

---

## 26. Requirement Traceability Matrix

Use this table during development and in the project report.

| College/project requirement | Implemented by | Verification |
|---|---|---|
| C programming | `backend/src/*.c` | Build with C11 compiler |
| Structures and pointers | `railway.h`, list/queue modules | Explain node layout and ownership |
| Array for seats | `seat_manager.c` | Seat chart and uniqueness tests |
| Linked list for confirmed bookings | `confirmed_list.c` | Insert/search/delete/traverse tests |
| FIFO waiting list | `waiting_queue.c` | Enqueue/dequeue order test |
| Booking and cancellation | `booking.c`, `cancellation.c` | Book/cancel scenarios |
| Automatic promotion | `cancellation.c` | Cancel with 2+ waiting passengers |
| Passenger lookup | `booking.c`, `search_sort.c` | Existing and unknown ID tests |
| Multiple trains | `train.c`, per-train state | Cross-train isolation test |
| Search/sort | `search_sort.c` | Results sorted without mutating queue |
| Persistence | `persistence.c` | Save/restart/load tests |
| Reports | `reports.c` | Compare reports with known state |
| Simple browser UI | `frontend/` | Manual browser tests |
| C backend API | `api_server.c`, `api_routes.c` | Endpoint tests |
| Project report | `docs/` and final report | All required sections present |
| Test cases | `backend/tests/`, `docs/TEST_PLAN.md` | Reproducible results |
| Team contribution | branch history and PRs | Each member explains own and shared code |

---

## 27. Expanded Testing Matrix

In addition to the test list in Section 13, cover the following groups.

### 27.1 Queue tests
- Enqueue into an empty queue: `front` and `rear` refer to the new node.
- Enqueue a second item: FIFO order is preserved.
- Dequeue from a two-item queue: first item is returned.
- Dequeue the last item: both `front` and `rear` become `NULL`.
- Dequeue an empty queue: return a controlled error.
- Repeated enqueue/dequeue cycles: no stale pointer remains.
- Queue allocation failure: state remains valid.

### 27.2 Seat and booking tests
- First booking receives the lowest free seat.
- Booking a full train creates a waiting record.
- Cancellation with no waiting passenger frees the exact seat.
- Cancellation with waiting passengers promotes exactly the first one.
- A second cancellation promotes the next passenger, preserving order.
- A promoted booking has one seat and no queue node.
- A cancelled booking cannot be promoted later.
- Booking ID is unique even after cancellation and restart.
- Full booking registry returns a controlled error.
- Invalid train number causes no state change.

### 27.3 Multi-train tests
- Fill Train A; Train B still has its own capacity.
- Cancel on Train A; no Train B record changes.
- Waiting passenger on Train A cannot receive a seat on Train B.
- Reports aggregate all trains correctly.

### 27.4 API tests
- Valid JSON request.
- Missing required field.
- Wrong JSON field type.
- Malformed JSON.
- Oversized body.
- Unknown route.
- Unsupported method.
- Unknown train/booking ID.
- Cancellation repeated.
- Server unavailable from browser.
- C backend returns a valid JSON error for every handled failure.

### 27.5 Persistence tests
- Empty first run creates or initializes data safely.
- Save/load confirmed records.
- Save/load waiting records and FIFO order.
- Save/load cancelled history.
- Invalid or duplicate seat assignment in file is detected.
- Invalid train reference in file is detected.
- Failed save does not report success.
- Restart does not reset ID sequence.

### 27.6 Frontend tests
- Empty form submission shows a clear message.
- API errors are displayed.
- Waiting-list booking is presented as successful but not confirmed.
- Cancellation requires confirmation.
- Successful cancellation refreshes seat and waiting data.
- Empty waiting list has a helpful empty state.
- Small-screen layout remains usable.
- No hardcoded success is shown when the C server is stopped.

---

## 28. Dependency Selection and Setup Policy

Do not choose dependencies only because an AI coding tool suggests them.

Before adding a C HTTP server or JSON library, document in `docs/DEPENDENCIES.md`:
- Exact library name.
- Version or pinned commit.
- Official source.
- License.
- Build/link instructions.
- Required system packages.
- Whether files are vendored or installed externally.
- Any platform limitations on Windows, Linux, or macOS.

### 28.1 HTTP library acceptance criteria
- C-compatible API.
- Small enough for a student project.
- Works on the team's target operating system.
- Can serve `GET` and `POST` endpoints.
- Supports request-body limits and response headers.
- License is compatible with the intended submission.
- Can serve static frontend files or can be paired with a simple, documented static-file approach.
- Does not require a Node.js or Python server.

Mongoose is a candidate to evaluate, not a mandatory dependency. The team must check current licensing and setup before adoption. Another small C-compatible library may be chosen if better suited to the repository and lab computers.

### 28.2 JSON library acceptance criteria
- Parses JSON safely.
- Serializes escaped strings correctly.
- Has a clear C API and license.
- Builds in the target environment.
- Is pinned/documented.
- Does not cause the project to use another backend language.

Do not implement a custom JSON parser by splitting strings on commas or colons. JSON strings can contain escaped punctuation, and ad-hoc parsing is unreliable.

### 28.3 Fallback if browser/API integration becomes too risky
The C core and terminal menu remain the primary academic deliverable. If the HTTP dependency cannot be built on the faculty's machine, demonstrate the fully working C console program and present the browser frontend as a separate static design only if the faculty accepts that limitation. Do not misrepresent a static page as connected to the C backend. Prefer testing the local C server early to avoid reaching this fallback.

---

## 29. Build and Run Acceptance Checklist

The final `README.md` must contain tested commands for the actual chosen environment. Do not copy placeholder commands without verifying them.

It must explain:
1. Compiler and dependency prerequisites.
2. How to build the C program.
3. How to run the application/server.
4. The local URL to open.
5. How to stop the server.
6. Where persistent files are stored.
7. How to reset only the fictional demo data safely.
8. How to run tests.
9. How to troubleshoot port conflicts, missing libraries, or file permissions.
10. How to run the terminal fallback if included.

Use C11 and warnings such as:

```bash
-std=c11 -Wall -Wextra -Wpedantic
```

Add `-Wconversion` and `-Wshadow` if the team can address the extra warnings. Do not suppress warnings globally just to make the build appear clean.

Do not hardcode an absolute developer-specific filesystem path. Resolve `data/` and frontend paths from a documented working directory or a configured project path.

---

## 30. Risk Register and Mitigation

| Risk | Why it matters | Mitigation |
|---|---|---|
| Shared structures drift | Modules do not compile or fields disagree | Freeze shared headers before parallel implementation |
| API fields drift | UI cannot parse C responses | Contract examples and API smoke tests |
| Queue promotion bug | Wrong passenger receives a seat | Dedicated FIFO tests with at least 2 waiting passengers |
| Seat array and list disagree | Duplicate/missing seats | Validate invariants after mutations and during tests |
| Memory leak/double free | Crash or corrupted state | Central ownership policy; cleanup tests |
| Persistence corrupts queue order | Wrong passenger promoted after restart | Persist creation sequence and rebuild FIFO queue |
| C HTTP library setup takes too long | Frontend integration blocked | Test `/api/health` during the first stage |
| JSON parsing is unsafe | Malformed requests crash or corrupt state | Use a documented C JSON library |
| Frontend fakes backend data | Demo looks functional but isn't integrated | Remove mocks and test with server stopped/running |
| Scope grows too large | Core operations remain unfinished | Complete core booking lifecycle before optional features |
| Git merge conflicts | Integration delays | File ownership, small PRs, reviewed shared-header changes |
| College requires four members | Group may not meet circular | Confirm with faculty and document fourth member if applicable |

---

## 31. Report Structure and Viva Preparation

The final report should follow the college's required sections and may use this expanded outline:

1. Title page and team details.
2. Problem statement.
3. Objectives.
4. Scope and assumptions.
5. System requirements and technology stack.
6. System architecture and request flow.
7. Data structures used and why each was selected.
8. Algorithms:
   - Seat allocation.
   - Booking insertion.
   - FIFO enqueue/dequeue.
   - Cancellation.
   - Waiting-list promotion.
   - Searching and sorting.
   - Save/load reconstruction.
9. Flowcharts or pseudocode for booking and cancellation.
10. File structure and module descriptions.
11. Test cases with input, expected output, actual output, and result.
12. Screenshots of the working frontend and console/API demonstration.
13. Complexity analysis based on the actual algorithms.
14. Limitations and future enhancements.
15. Conclusion.
16. References for any external library documentation.

### 31.1 Viva questions every member should be ready to answer
- Why is an array suitable for seat availability?
- Why is a queue suitable for the waiting list?
- What makes the waiting queue FIFO?
- Why use a linked list for confirmed passengers?
- What are `front` and `rear`?
- What happens when the final queue node is removed?
- How do you prevent duplicate seat assignment?
- What is the difference between a cancelled booking and a deleted record?
- How does the application restore FIFO order after restart?
- What is the time complexity of searching the booking registry?
- Why is the browser JavaScript not the backend?
- What is an API contract?
- What happens if the C server is not running?
- How do you prevent two modules from defining conflicting structures?
- How do you test that cancelling a ticket promotes the correct passenger?

All three members should understand the full booking lifecycle, not only their assigned files.

---

## 32. Final Implementation Order and Exit Gates

Do not move to the next stage until the listed exit criteria are met.

### Stage 1 — Repository and specification
Deliver:
- `PROJECT_BIBLE.md`
- Initial directory structure.
- Shared C model proposal.
- `docs/DATA_STRUCTURES.md`.
- `docs/API_CONTRACT.md`.
- Dependency decision record.
- Three feature branches.

**Exit gate:** All three members agree on structures, status values, seat sentinel, API examples, and ownership rules.

### Stage 2 — C core without HTTP
Deliver:
- Train initialization.
- Seat array operations.
- Booking registry.
- Confirmed linked list.
- Waiting queue.
- Booking, cancellation, promotion, and search.
- Console menu.

**Exit gate:** Core unit tests pass, including cancellation promotion and FIFO ordering.

### Stage 3 — Persistence and reports
Deliver:
- File formats.
- Save/load.
- Search/sort.
- Summary reports and event history.

**Exit gate:** Restart tests pass and invariants remain true after reload.

### Stage 4 — C HTTP integration
Deliver:
- Chosen HTTP library.
- `/api/health`.
- Train list and seat endpoints.
- Booking, lookup, cancellation endpoints.
- Waiting/confirmed and report endpoints.

**Exit gate:** Requests and errors return documented JSON and call the real C core.

### Stage 5 — Frontend integration
Deliver:
- Dashboard and navigation.
- Booking form.
- Lookup and cancellation.
- Seat chart and waiting-list display.
- Reports/history.
- Loading/error/empty states.

**Exit gate:** Full booking/cancellation/promotion scenario works from the browser against the C backend.

### Stage 6 — Stabilization
Deliver:
- Regression tests.
- Clean build.
- Data persistence checks.
- Memory cleanup.
- Updated README and documentation.

**Exit gate:** A clean checkout can build and run using documented instructions.

### Stage 7 — Report and demo
Deliver:
- Final report.
- Test evidence.
- Demo data.
- Demo script.
- Member contribution summary.

**Exit gate:** All members can run and explain the project.

---

## 33. Final Guidance for the AI Coding Assistant

Treat this file as a living specification, but do not change its decisions silently.

Before each coding stage:
1. State the stage and the expected outcome.
2. Identify which files will be created or changed.
3. Check the shared contracts and ownership.
4. Make the smallest useful implementation.
5. Compile and test the changed module.
6. Report exact commands and actual results.
7. List limitations and the next step.

When editing the repository:
- Inspect files before overwriting them.
- Keep C as the only backend implementation language.
- Keep browser JavaScript limited to presentation and API calls.
- Do not invent APIs, dependencies, versions, or successful test results.
- Do not generate an entire application in one unreviewable change.
- Do not change shared structures or API response shapes without updating the documents and notifying dependent members.
- Do not add SQL, cloud services, AI features, authentication, payment processing, or frameworks outside this specification.
- Do not force-push, reset shared branches, or delete branches without explicit approval.
- Prefer simple code that the team can explain during viva.

At the end of each stage, report:
- Files created/modified.
- Key decisions made.
- Commands executed.
- Test results, including failures.
- Known limitations.
- Git branch and commit state.
- The next safe implementation step.

**The first deliverable is not the whole application. It is an inspected repository plus reviewed shared C structures, reviewed API contract, and a small working C health endpoint.**
