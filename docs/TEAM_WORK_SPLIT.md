# Team Work Split and Responsibilities
## Railway Reservation and Waiting List Management System

**Academic Context:** Second-Year Computer Engineering — Data Structures Mini Project  
**Institution:** Fr. Conceicao Rodrigues College of Engineering (CRCE)

---

## 1. Team Allocation

| Member | Assigned Role | Feature Branch | Core Ownership |
|---|---|---|---|
| **Mireille** | Frontend Owner | `frontend-member1` | HTML5, CSS3, vanilla JavaScript, responsive layout, API integration, client-side views |
| **Bernice** | C Core & Data Structures Owner | `backend-member2` | Train & booking models, confirmed linked list, FIFO waiting queue, seat manager, booking & cancellation/promotion core, invariants, unit tests |
| **Eleen** | C Integration & Services Owner | `backend-member3` | C HTTP server & routes, file persistence (CSV), search & sort, summary reports, build integration (`Makefile`), console menu |

---

## 2. Detailed Member Responsibilities

### 2.1 Mireille — Frontend Owner (`frontend-member1`)
- **Files Owned:**
  - `frontend/index.html`
  - `frontend/pages/` (`trains.html`, `booking.html`, `lookup.html`, `waiting-list.html`, `reports.html`)
  - `frontend/css/` (`styles.css`, `components.css`, `responsive.css`)
  - `frontend/js/` (`api.js`, `dashboard.js`, `booking.js`, `lookup.js`, `trains.js`, `waiting-list.js`, `reports.js`)
  - `frontend/assets/README.md`
- **Scope & Boundaries:**
  - Implements clean, responsive user interface.
  - Consumes JSON endpoints defined in `docs/API_CONTRACT.md`.
  - Must NOT implement ticket allocation, queue management, or cancellation logic in JavaScript.
  - Must NOT introduce external frameworks (React, Vue, Node.js, etc.).

### 2.2 Bernice — C Core & Data Structures Owner (`backend-member2`)
- **Files Owned:**
  - Headers:
    - `backend/include/railway.h`
    - `backend/include/booking.h`
    - `backend/include/train.h`
    - `backend/include/waiting_queue.h`
    - `backend/include/confirmed_list.h`
    - `backend/include/booking_history.h`
    - `backend/include/validation.h`
  - Source Files:
    - `backend/src/railway.c`
    - `backend/src/train.c`
    - `backend/src/booking.c`
    - `backend/src/waiting_queue.c`
    - `backend/src/confirmed_list.c`
    - `backend/src/booking_history.c`
    - `backend/src/seat_manager.c`
    - `backend/src/cancellation.c`
    - `backend/src/validation.c`
  - Unit Tests:
    - `backend/tests/test_waiting_queue.c`
    - `backend/tests/test_confirmed_list.c`
    - `backend/tests/test_seat_manager.c`
    - `backend/tests/test_booking.c`
    - `backend/tests/test_cancellation.c`
  - Documentation:
    - `docs/DATA_STRUCTURES.md`
- **Scope & Boundaries:**
  - Enforces C11 compliance and compiles with `-Wall -Wextra -Wpedantic -Wconversion -Wshadow`.
  - Guarantees strict FIFO queue behavior, array seat indexing, and singly linked list pointer integrity.
  - Exposes stable, documented C APIs for Eleen's server and persistence integration.
  - Keeps core free of HTTP dependencies, database libraries, or operating system-specific lock-ins.

### 2.3 Eleen — C Integration & Services Owner (`backend-member3`)
- **Files Owned:**
  - Headers:
    - `backend/include/persistence.h`
    - `backend/include/reports.h`
    - `backend/include/api_server.h`
    - `backend/include/api_routes.h`
  - Source Files:
    - `backend/src/search_sort.c`
    - `backend/src/reports.c`
    - `backend/src/persistence.c`
    - `backend/src/console_ui.c`
    - `backend/src/api_server.c`
    - `backend/src/api_routes.c`
    - `backend/src/main.c` (coordinates with Bernice for system lifecycle)
  - Integration Tests:
    - `backend/tests/test_persistence.c`
    - `backend/tests/test_reports.c`
  - Build & Scripts:
    - `Makefile`
    - `scripts/build.sh`, `scripts/run.sh`, `scripts/clean.sh`
  - Documentation:
    - `docs/API_CONTRACT.md`
    - `docs/DEPENDENCIES.md`
- **Scope & Boundaries:**
  - Calls Bernice's core C functions; never duplicates business logic.
  - Handles CSV file serialization and deserialization under `data/`.
  - Implements embedded HTTP server using audited library and serves JSON responses.
