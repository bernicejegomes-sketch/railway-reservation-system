# Data Structures and Core C Architecture Specification
## Railway Reservation and Waiting List Management System
**Author / Data Structures Owner:** Bernice (backend-member2)  
**Academic Context:** Second-Year Computer Engineering — Data Structures Mini Project  
**Institution:** Fr. Conceicao Rodrigues College of Engineering (CRCE)  
**Standard:** C11 (`-std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow`)

---

## 1. Executive Summary & Design Rationale

This document establishes the authoritative data-structure design, memory ownership model, operational algorithms, time complexities, and integrity invariants for the Railway Reservation and Waiting List Management System.

The project demonstrates fundamental linear data structures in pure C (C11) to solve a classic real-world resource allocation problem:
1. **Fixed-Size Contiguous Arrays:** Used for train lookup, central booking storage, event logs, and train seat maps where constant-time $O(1)$ random access and cache locality are paramount.
2. **Singly Linked Lists:** Used for the active confirmed passenger list (`ConfirmedList`), enabling dynamic insertion and removal of passenger references as tickets are booked or cancelled.
3. **Linked FIFO Queues (Pointers to Front and Rear):** Used for the train waiting list (`WaitingQueue`), guaranteeing strict First-In, First-Out (FIFO) fairness without arbitrary queue length bounds or expensive element shifting.
4. **Central Registry & Normalized ID Referencing:** The central `RailwaySystem` owns the authoritative `Booking` records. Confirmed linked lists and waiting queues store only `booking_id` references, eliminating memory duplication, preventing stale passenger data, and ensuring clean pointer lifetimes.

---

## 2. Shared C Data Structures & Type Definitions

The canonical types are declared in `backend/include/railway.h` and modularized across dedicated component headers (`train.h`, `booking.h`, `waiting_queue.h`, `confirmed_list.h`, `booking_history.h`, and `validation.h`).

### 2.1 Constants & Enumerations

```c
#define MAX_TRAINS 10
#define MAX_SEATS_PER_TRAIN 100
#define MAX_BOOKINGS 500
#define MAX_EVENTS 2000
#define NAME_LEN 80
#define ROUTE_LEN 80
#define TIME_LEN 16
#define DATE_LEN 11
#define EVENT_LEN 24

/* Booking status lifecycle */
typedef enum {
    BOOKING_CONFIRMED = 0,
    BOOKING_WAITING   = 1,
    BOOKING_CANCELLED = 2
} BookingStatus;

/* Standard error / return codes */
typedef enum {
    RAILWAY_SUCCESS               =  0,
    RAILWAY_ERR_INVALID_PARAM     = -1,
    RAILWAY_ERR_TRAIN_NOT_FOUND   = -2,
    RAILWAY_ERR_BOOKING_NOT_FOUND = -3,
    RAILWAY_ERR_CAPACITY_FULL     = -4,
    RAILWAY_ERR_ALREADY_CANCELLED = -5,
    RAILWAY_ERR_NOT_CONFIRMED     = -6,
    RAILWAY_ERR_ALLOCATION_FAILED = -7,
    RAILWAY_ERR_INVARIANT_VIOLATION = -8
} RailwayResult;
```

### 2.2 Core Models

#### `Booking`
Represents an individual passenger ticket in the central registry.
```c
typedef struct {
    unsigned long booking_id;        /* Unique, strictly monotonic ID */
    unsigned long created_sequence;  /* Monotonic sequence preserving chronological order */
    char passenger_name[NAME_LEN];   /* Bounded passenger name string */
    int age;                         /* Passenger age in years (1 to 120) */
    char journey_date[DATE_LEN];     /* ISO date format YYYY-MM-DD */
    int train_number;                /* Associated train ID */
    int seat_number;                 /* 1-based index; 0 indicates no seat (e.g. WAITING / CANCELLED) */
    BookingStatus status;            /* BOOKING_CONFIRMED, BOOKING_WAITING, or BOOKING_CANCELLED */
} Booking;
```

#### `ConfirmedNode` & `ConfirmedList`
Singly linked list holding booking IDs of all confirmed passengers for a specific train.
```c
typedef struct ConfirmedNode ConfirmedNode;

struct ConfirmedNode {
    unsigned long booking_id;        /* Reference to central booking registry */
    ConfirmedNode *next;             /* Pointer to successor node */
};

typedef struct {
    ConfirmedNode *head;             /* Pointer to list head */
    size_t size;                     /* Current number of confirmed passengers */
} ConfirmedList;
```

#### `WaitingNode` & `WaitingQueue`
Linked queue with head (`front`) and tail (`rear`) pointers holding waiting passenger booking IDs.
```c
typedef struct WaitingNode WaitingNode;

struct WaitingNode {
    unsigned long booking_id;        /* Reference to central booking registry */
    WaitingNode *next;             /* Pointer to subsequent waiting node */
};

typedef struct {
    WaitingNode *front;              /* Pointer to earliest waiting passenger */
    WaitingNode *rear;               /* Pointer to most recently joined passenger */
    size_t size;                     /* Total passengers currently in queue */
} WaitingQueue;
```

#### `Train`
Independent entity encapsulating train details, fixed seat array, confirmed list, and waiting queue.
```c
typedef struct {
    int train_number;                /* Primary key for train lookup */
    char train_name[NAME_LEN];       /* Human-readable name (e.g. "Konkan Kanya Express") */
    char source[ROUTE_LEN];          /* Origin station */
    char destination[ROUTE_LEN];     /* Destination station */
    char departure_time[TIME_LEN];   /* Scheduled departure time (e.g. "23:05") */
    int capacity;                    /* Total physical seats available (1 <= capacity <= MAX_SEATS_PER_TRAIN) */
    int seats[MAX_SEATS_PER_TRAIN];  /* 0 = vacant, 1 = occupied */
    ConfirmedList confirmed;         /* Linked list of confirmed bookings */
    WaitingQueue waiting;            /* Linked FIFO queue of waiting bookings */
} Train;
```

#### `BookingEvent`
Audit trail log record capturing state modifications.
```c
typedef struct {
    unsigned long sequence;          /* Event counter */
    char event_type[EVENT_LEN];      /* "BOOKING_CREATED", "TICKET_CANCELLED", "PROMOTED", etc. */
    unsigned long booking_id;        /* Associated booking */
    int train_number;                /* Associated train */
} BookingEvent;
```

#### `RailwaySystem`
Root system context encapsulating all trains, central registries, and event logs.
```c
typedef struct {
    Train trains[MAX_TRAINS];
    size_t train_count;

    Booking bookings[MAX_BOOKINGS]; /* Central registry: active and cancelled */
    size_t booking_count;

    BookingEvent events[MAX_EVENTS];
    size_t event_count;

    unsigned long next_booking_id;   /* Monotonic ID generator (e.g. starts at 1001) */
    unsigned long next_event_sequence;
} RailwaySystem;
```

---

## 3. Data Structure Choices & Detailed Analysis

| Data Structure | Component | Motivation / Why Selected | Alternatives Considered & Rejected |
|---|---|---|---|
| **Fixed-size Array (Contiguous Memory)** | `Train.seats[]` | $O(1)$ direct index access for checking seat occupancy via `seats[seat_num - 1]`. Fits in L1 CPU cache. Fixed capacity matches physical train carriages. | *Dynamic Bitset:* Unnecessary heap overhead for $N \le 100$. *Linked List of Seats:* $O(N)$ traversal to check seat availability is inefficient. |
| **Singly Linked List** | `Train.confirmed` | Efficient $O(1)$ insertion at head. Dynamic node allocation only when tickets are confirmed. Simple unlinking $O(N)$ without shifting array elements on cancellation. | *Fixed Array:* Deleting an element requires $O(N)$ array compaction and shifting. *Doubly Linked List:* Doubled pointer overhead without real benefit since traversal is unidirectional. |
| **Linked FIFO Queue (`front`, `rear`)** | `Train.waiting` | Strict $O(1)$ enqueue at `rear` and $O(1)$ dequeue at `front`. Naturally enforces FIFO fairness. Size grows dynamically. | *Circular Array Queue:* Fixed maximum size risks queue exhaustion during high demand; linked queue has no artificial queue cap. |
| **Normalized Central Array** | `RailwaySystem.bookings[]` | Single source of truth. Historical records remain intact after cancellation. Direct array lookup by ID or fast scan. Nodes store only `unsigned long booking_id`. | *Embedded copies in nodes:* High risk of data inconsistency between queue/list copies and dangling pointers upon deletion. |

---

## 4. Array-Based Seat Management

Seat indices follow a strict, standardized mapping:
- **Seat numbers are 1-based:** Seat $1$ maps to array index `0`, seat $k$ maps to `seats[k - 1]`.
- **Seat states:**
  - `seats[i] == 0`: Seat is vacant (available for allocation).
  - `seats[i] == 1`: Seat is occupied by an active confirmed passenger.
- **Seat sentinel `0`:**
  - A booking with `seat_number == 0` designates that no physical seat has been assigned (applicable to `BOOKING_WAITING` and `BOOKING_CANCELLED`).
  - No negative numbers (`-1`) are used, maintaining full consistency across all modules.

### Operations:
1. **`seat_manager_find_first_vacant(Train *train)`:**
   Scans `seats[0 ... capacity - 1]` linearly from index `0`. Returns the lowest seat index $+ 1$.
2. **`seat_manager_allocate(Train *train, int seat_number)`:**
   Validates $1 \le \text{seat\_number} \le \text{capacity}$. If `seats[seat_number - 1] == 0`, sets it to `1`.
3. **`seat_manager_release(Train *train, int seat_number)`:**
   Validates $1 \le \text{seat\_number} \le \text{capacity}$. Sets `seats[seat_number - 1] = 0`.
4. **`seat_manager_count_occupied(const Train *train)`:**
   Counts elements equal to `1`. Guarantees `occupied + vacant == capacity`.

---

## 5. Singly Linked Confirmed Passenger List

Each train maintains a `ConfirmedList`.
- **Insertion (`confirmed_list_insert`):**
  Allocates a new `ConfirmedNode`, sets `node->booking_id = booking_id`, links `node->next = list->head`, sets `list->head = node`, and increments `list->size`. Time complexity: $O(1)$.
- **Searching (`confirmed_list_find`):**
  Traverses from `head` through `next` pointers comparing `node->booking_id == target_id`. Time complexity: $O(K)$ where $K$ is confirmed passenger count.
- **Deletion (`confirmed_list_remove`):**
  Maintains `current` and `prev` pointers. When matching `booking_id` is located:
  - If node is head: `list->head = current->next`.
  - If interior/tail node: `prev->next = current->next`.
  - Frees `current`, decrements `list->size`.
  - Returns `RAILWAY_SUCCESS`. Time complexity: $O(K)$.
- **Cleanup (`confirmed_list_clear`):**
  Iteratively frees all nodes, sets `head = NULL` and `size = 0`.

---

## 6. Linked FIFO Waiting Queue

Each train maintains a `WaitingQueue` to hold passengers when `capacity` is exhausted.
- **Invariant:**
  - When `size == 0`: both `front == NULL` and `rear == NULL`.
  - When `size == 1`: `front == rear` (both point to the single existing node), and `front->next == NULL`.
  - When `size > 1`: `front` points to the earliest passenger, `rear` points to the latest, and `rear->next == NULL`.
- **Enqueue (`waiting_queue_enqueue`):**
  1. Allocate a `WaitingNode` via `malloc(sizeof(WaitingNode))`.
  2. Set `node->booking_id = booking_id; node->next = NULL;`.
  3. If queue is empty (`rear == NULL`): set `front = rear = node`.
  4. Else: set `rear->next = node; rear = node;`.
  5. Increment `queue->size`. Time complexity: $O(1)$.
- **Dequeue (`waiting_queue_dequeue`):**
  1. If queue is empty (`front == NULL`): return `RAILWAY_ERR_INVALID_PARAM` (queue underflow).
  2. Extract `temp = front; *booking_id_out = temp->booking_id;`.
  3. Advance `front = front->next`.
  4. If `front == NULL` (queue is now empty): set `rear = NULL`.
  5. Free `temp`, decrement `queue->size`. Time complexity: $O(1)$.
- **Peek (`waiting_queue_peek`):**
  Inspects `front->booking_id` without removal. Time complexity: $O(1)$.
- **Position Lookup (`waiting_queue_get_position`):**
  Traverses from `front` to locate `target_id`, returning 1-based index (e.g. 1st waiting, 2nd waiting). Time complexity: $O(W)$.
- **Cleanup (`waiting_queue_clear`):**
  Pops all nodes until empty, resets `front = rear = NULL`, `size = 0`.

---

## 7. Central Booking Registry & Memory Ownership

```text
+-----------------------------------------------------------------------------------+
|                                  RailwaySystem                                    |
|                                                                                   |
|  +-----------------------------------------------------------------------------+  |
|  | bookings[] (Central Registry Array: Indices 0 ... booking_count - 1)       |  |
|  | [1001: Alice, Confirmed, Seat 1] [1002: Bob, Waiting, Seat 0] [1003: ...]   |  |
|  +-----------------------------------------------------------------------------+  |
|                                         ^                       ^                 |
|                                         | ID ref                | ID ref          |
|  +-------------------------------+      |                       |                 |
|  | Train 101                     |      |                       |                 |
|  | seats: [1, 0, 0]              |      |                       |                 |
|  | confirmed.head -------------->+------+ [ConfirmedNode: 1001] |                 |
|  | waiting.front --------------->+----------------------------->+ [WaitingNode:   |
|  | waiting.rear  --------------->+                                  1002]         |
|  +-------------------------------+                                                |
+-----------------------------------------------------------------------------------+
```

### Ownership Rules:
1. **Booking Records:** Owned solely by `RailwaySystem.bookings[]`. Nodes never duplicate `Booking` struct instances.
2. **List & Queue Nodes:** Owned solely by their respective `Train`. Each node is dynamically allocated with `malloc` and deallocated with `free` upon removal or system teardown.
3. **No Dangling Pointers:** Deleting a node does not touch the registry booking record. The booking remains searchable in history with status `BOOKING_CANCELLED`.
4. **Idempotent Cleanup:** `railway_cleanup()` safely frees all linked nodes for every train and clears registry counters.

---

## 8. Algorithms

### 8.1 Ticket Booking Algorithm (`railway_book_ticket`)

```text
INPUT: system, passenger_name, age, train_number, journey_date, result_out
OUTPUT: RailwayResult status code

1. Validate inputs:
   - Check non-NULL pointers.
   - Validate passenger_name: non-empty, length < NAME_LEN, no forbidden control chars.
   - Validate age: 1 <= age <= 120.
   - Validate journey_date: YYYY-MM-DD format (10 chars, valid calendar bounds).
   - Ensure system->booking_count < MAX_BOOKINGS.

2. Locate train:
   - Lookup train by train_number. If NULL, return RAILWAY_ERR_TRAIN_NOT_FOUND.

3. Allocate new Booking record in central registry:
   - booking_id = system->next_booking_id++;
   - sequence   = system->next_event_sequence++;
   - Copy validated fields: name, age, train_number, journey_date.

4. Evaluate seat availability:
   - Call seat_manager_find_first_vacant(train).
   - IF vacant_seat > 0:
       a. seat_manager_allocate(train, vacant_seat).
       b. Set booking->seat_number = vacant_seat.
       c. Set booking->status = BOOKING_CONFIRMED.
       d. Insert booking_id into train->confirmed list (confirmed_list_insert).
       e. Record event "BOOKING_CREATED" (CONFIRMED).
   - ELSE (train is completely full):
       a. Set booking->seat_number = 0.
       b. Set booking->status = BOOKING_WAITING.
       c. Enqueue booking_id into train->waiting queue (waiting_queue_enqueue).
       d. Record event "BOOKING_CREATED" (WAITING).

5. Copy booking to result_out (if non-NULL) and increment system->booking_count.
6. Return RAILWAY_SUCCESS.
```

### 8.2 Ticket Cancellation and Promotion Algorithm (`railway_cancel_ticket`)

```text
INPUT: system, booking_id, cancelled_out, promoted_out
OUTPUT: RailwayResult status code

1. Verify booking existence in registry:
   - Find booking by booking_id. If NULL, return RAILWAY_ERR_BOOKING_NOT_FOUND.

2. Verify active status:
   - If booking->status == BOOKING_CANCELLED: return RAILWAY_ERR_ALREADY_CANCELLED.
   - If booking->status != BOOKING_CONFIRMED: return RAILWAY_ERR_NOT_CONFIRMED.
     (Note: Waiting ticket cancellation is handled separately if invoked).

3. Locate train:
   - train = railway_find_train(system, booking->train_number).
   - Verify train != NULL.

4. Release confirmed seat & remove from confirmed list:
   - freed_seat = booking->seat_number.
   - seat_manager_release(train, freed_seat).
   - confirmed_list_remove(&train->confirmed, booking_id).
   - booking->status = BOOKING_CANCELLED.
   - booking->seat_number = 0.
   - Record event "TICKET_CANCELLED".
   - If cancelled_out != NULL: *cancelled_out = *booking.

5. Check Waiting Queue for Automatic Promotion:
   - IF !waiting_queue_is_empty(&train->waiting):
       a. Dequeue earliest waiting passenger:
          waiting_queue_dequeue(&train->waiting, &promoted_id).
       b. Locate promoted_booking in central registry.
       c. Assign freed_seat to promoted passenger:
          seat_manager_allocate(train, freed_seat).
          promoted_booking->seat_number = freed_seat;
          promoted_booking->status = BOOKING_CONFIRMED;
       d. Insert promoted_id into train->confirmed list.
       e. Record event "PROMOTED".
       f. If promoted_out != NULL: *promoted_out = *promoted_booking.
   - ELSE:
       a. freed_seat remains 0 (vacant in train->seats[]).
       b. If promoted_out != NULL: memset(promoted_out, 0, sizeof(Booking)).

6. Verify post-conditions (invariants).
7. Return RAILWAY_SUCCESS.
```

---

## 9. Core System Invariants

The validation function `railway_validate_invariants(const RailwaySystem *system)` rigorously checks:

1. **Seat Mutual Exclusion:** No two confirmed bookings on the same train share the same seat number.
2. **Seat State Consistency:** For every train, seat $k$ has `seats[k - 1] == 1` if and only if there is an active confirmed booking with `seat_number == k`.
3. **Seat Number Sentinel:** Every booking with status `BOOKING_WAITING` or `BOOKING_CANCELLED` has `seat_number == 0`.
4. **Capacity Equation:** For each train, `train->confirmed.size + vacant_seats == train->capacity`.
5. **Waiting Queue Integrity:**
   - If `waiting.size == 0`: both `front == NULL` and `rear == NULL`.
   - If `waiting.size > 0`: both `front != NULL` and `rear != NULL`, and `rear->next == NULL`.
   - The number of traversed waiting nodes equals `waiting.size`.
6. **Unique Queue Membership:** No booking ID appears more than once in the waiting queue.
7. **Cross-Structure Exclusion:** A booking ID never exists simultaneously in a train's `confirmed` list and its `waiting` queue.
8. **Cancelled Exclusion:** A booking with `status == BOOKING_CANCELLED` exists in neither `confirmed` list nor `waiting` queue.
9. **Train Isolation:** Every booking referenced in a train's confirmed list or waiting queue has `booking->train_number == train->train_number`.

---

## 10. Time & Space Complexity Analysis

| Operation | Algorithm / Mechanism | Time Complexity | Auxiliary Space | Explanation |
|---|---|:---:|:---:|---|
| **Train Lookup** | Linear scan over `trains[0...train_count-1]` | $O(T)$ | $O(1)$ | Small constant $T \le 10$. Direct scan avoids hash collision overhead. |
| **Find First Vacant Seat** | Linear scan over `seats[0...capacity-1]` | $O(S)$ | $O(1)$ | $S \le 100$. Scans array sequentially for the lowest index with value `0`. |
| **Seat Allocation / Release** | Direct array indexing `seats[seat - 1]` | $O(1)$ | $O(1)$ | Constant time direct read/write. |
| **Enqueue Waiting Passenger** | Tail pointer insertion `rear->next = node` | $O(1)$ | $O(1)$ | Maintained `rear` pointer avoids list traversal. |
| **Dequeue Waiting Passenger** | Head pointer unlinking `front = front->next` | $O(1)$ | $O(1)$ | Constant time pointer swing at queue head. |
| **Confirmed List Insert** | Head insertion `node->next = head` | $O(1)$ | $O(1)$ | Constant time push at list front. |
| **Confirmed List Remove** | Singly linked search and unlink | $O(K)$ | $O(1)$ | Scans $K$ confirmed nodes to unlink target ID. ($K \le S \le 100$). |
| **Find Booking in Registry** | Scan registry `bookings[0...count-1]` | $O(B)$ | $O(1)$ | $B \le 500$. Fast memory traversal. |
| **Ticket Booking (Total)** | Validate + allocate seat/queue + record | $O(S + B)$ | $O(1)$ | Dominated by vacant seat scan and registry insertion. |
| **Ticket Cancellation (Total)** | Search + release + dequeue + reallocate | $O(B + K)$ | $O(1)$ | Unlinking confirmed node and updating registry record. |
| **Invariant Verification** | Deep consistency audit across all structures | $O(T \times (S + K + W))$ | $O(1)$ | Audits all trains, seats, lists, and queues. Ideal for debug/test mode. |

---

## 11. Test Coverage Strategy

Unit tests are implemented without mock frameworks, using C assert patterns and reproducible test suites:
- **`tests/test_waiting_queue.c`:**
  - Empty queue properties (`front == rear == NULL`, `size == 0`).
  - Single enqueue and dequeue, verifying pointer reset to `NULL`.
  - Multiple enqueues verifying strict FIFO ordering (`1001, 1002, 1003`).
  - Peek operation without modifying queue state.
  - Position queries throughout queue traversal.
- **`tests/test_confirmed_list.c`:**
  - Head insertion, multiple insertions, and size tracking.
  - Searching for existing and non-existing IDs.
  - Deleting head, middle, and tail nodes.
  - Cleanup and memory deallocation.
- **`tests/test_seat_manager.c`:**
  - Sequential allocation on small test train (capacity 3).
  - Occupancy detection when fully booked.
  - Releasing seat and reallocating the same lowest vacant seat.
  - Guarding against duplicate allocation and out-of-bounds seat numbers.
- **`tests/test_booking.c`:**
  - Successful seat booking while capacity remains.
  - Automatic transition to `WAITING` status when train is full.
  - Unique booking ID generation ($1001, 1002, \dots$).
  - Rejection of invalid inputs (empty name, age $\le 0$, non-existent train).
- **`tests/test_cancellation.c`:**
  - Valid cancellation releasing seat when queue is empty.
  - Rejection of unknown booking ID or double-cancellation.
  - Cancellation with waiting passenger triggering immediate FIFO promotion.
  - Verification that promoted passenger inherits released seat number.
  - Multi-train isolation: cancelling ticket on Train 101 never promotes Train 102 passenger.
