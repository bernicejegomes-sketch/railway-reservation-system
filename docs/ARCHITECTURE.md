# Architecture Overview
## Railway Reservation and Waiting List Management System

**Academic Context:** Second-Year Computer Engineering — Data Structures Mini Project  
**Institution:** Fr. Conceicao Rodrigues College of Engineering (CRCE)

---

## 1. System Architecture

The project employs a layered architecture separating presentation (browser), service routing (C HTTP server), and authoritative business logic / data structures (pure C core).

```text
+-------------------------------------------------------------------------------+
|                             CLIENT / FRONTEND                                  |
|   HTML5 Semantic Pages (Dashboard, Booking, Trains, Waiting List, Reports)   |
|   CSS3 (Responsive layout, status tags, cards)                                |
|   Vanilla JavaScript (Form events, async fetch, DOM rendering)                 |
+---------------------------------------+---------------------------------------+
                                        | JSON over HTTP (Port 8080)
                                        v
+-------------------------------------------------------------------------------+
|                                  C BACKEND                                    |
|                                                                               |
|  +-------------------------------------------------------------------------+  |
|  | C HTTP Server & Router (`api_server.c`, `api_routes.c`)                |  |
|  | - Parses incoming REST HTTP requests                                    |  |
|  | - Encodes JSON responses using agreed response envelope                 |  |
|  +------------------------------------+------------------------------------+  |
|                                       | Internal C API Calls                  |
|                                       v                                       |
|  +-------------------------------------------------------------------------+  |
|  | Bernice's Core Data Structures Engine                                   |  |
|  | - `RailwaySystem`: Root context managing trains, bookings, events       |  |
|  | - `Train`: Per-train seat array (`seats[]`), confirmed list, waiting    |  |
|  | - `WaitingQueue`: Linked FIFO queue (`front`, `rear`, `size`)           |  |
|  | - `ConfirmedList`: Singly linked list of active confirmed passenger IDs |  |
|  | - `seat_manager`: First-fit vacancy locator & allocation logic         |  |
|  | - `cancellation`: Two-way cancellation and automatic waiting promotion  |  |
|  | - `validation`: Parameter checking, bounds check, invariant auditor     |  |
|  +------------------------------------+------------------------------------+  |
|                                       | Persistence & Query                   |
|                                       v                                       |
|  +-------------------------------------------------------------------------+  |
|  | Services & Integration Layer (Eleen)                                    |  |
|  | - `persistence.c`: CSV serialization/deserialization to `data/`         |  |
|  | - `reports.c`: Occupancy analytics & booking metrics                    |  |
|  | - `search_sort.c`: Passenger sorting (bubble/insertion) and lookup     |  |
|  +-------------------------------------------------------------------------+  |
+-------------------------------------------------------------------------------+
```

---

## 2. Key Architectural Guarantees

1. **Pure C Domain Logic:** No booking allocation, waiting list queuing, or cancellation logic is implemented in JavaScript or external dependencies.
2. **Zero Global Mutable State:** All functions operate on explicit system context pointers (`RailwaySystem *system` or `Train *train`).
3. **Decoupled Ownership:** The central registry owns `Booking` structs. Linked nodes hold references (`booking_id`), avoiding duplicate memory and sync hazards.
4. **Resilient Invariants:** Core invariants guarantee that confirmed seat count, vacant seat count, and train capacity are always in mathematical balance.
