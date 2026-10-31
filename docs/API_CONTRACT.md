# API Contract Specification — Version 1.0
## Railway Reservation and Waiting List Management System

**Academic Context:** Second-Year Computer Engineering — Data Structures Mini Project  
**Institution:** Fr. Conceicao Rodrigues College of Engineering (CRCE)

---

## 1. Response Envelope Format

All responses from the C backend adhere to a standardized JSON envelope:

### Success Response:
```json
{
  "success": true,
  "message": "Human-readable status summary",
  "data": { ... }
}
```

### Error Response:
```json
{
  "success": false,
  "message": "Descriptive error message",
  "data": null
}
```

---

## 2. API Endpoints

### 2.1 `GET /api/health`
- **Purpose:** Server liveness and system health check.
- **Response:**
  ```json
  {
    "success": true,
    "message": "Railway backend system operational.",
    "data": {
      "status": "healthy",
      "version": "1.0.0",
      "total_trains": 3,
      "total_bookings": 0
    }
  }
  ```

### 2.2 `GET /api/trains`
- **Purpose:** List all configured trains with real-time seat availability.
- **Core C Function:** `railway_find_train()` iterate.
- **Response Data:** Array of trains including `train_number`, `train_name`, `source`, `destination`, `departure_time`, `capacity`, `available_seats`, `confirmed_count`, `waiting_count`.

### 2.3 `POST /api/bookings`
- **Purpose:** Book a ticket (allocates seat or enqueues into waiting list).
- **Core C Function:** `railway_book_ticket()`.
- **Request Body:**
  ```json
  {
    "passenger_name": "Alice Smith",
    "age": 28,
    "train_number": 101,
    "journey_date": "2026-11-15"
  }
  ```
- **Response Status:** `201 Created`
- **Response Data (Confirmed):**
  ```json
  {
    "booking_id": 1001,
    "passenger_name": "Alice Smith",
    "train_number": 101,
    "status": "CONFIRMED",
    "seat_number": 1
  }
  ```
- **Response Data (Waiting):**
  ```json
  {
    "booking_id": 1004,
    "passenger_name": "Dave Brown",
    "train_number": 101,
    "status": "WAITING",
    "seat_number": 0,
    "waiting_position": 1
  }
  ```

### 2.4 `POST /api/bookings/{booking_id}/cancel`
- **Purpose:** Cancel a confirmed ticket and trigger automatic promotion for earliest waiting passenger.
- **Core C Function:** `railway_cancel_ticket()`.
- **Response Data:**
  ```json
  {
    "cancelled": {
      "booking_id": 1001,
      "status": "CANCELLED",
      "seat_number": 0
    },
    "promoted": {
      "booking_id": 1004,
      "status": "CONFIRMED",
      "seat_number": 1
    }
  }
  ```

### 2.5 `GET /api/bookings/{booking_id}`
- **Purpose:** Look up passenger details and booking status.
- **Core C Function:** `railway_find_booking()`.

### 2.6 `GET /api/trains/{train_number}/seats`
- **Purpose:** Retrieve visual seat map array (`0` = vacant, `1` = occupied).
- **Core C Function:** `railway_get_seat_chart()`.

### 2.7 `GET /api/trains/{train_number}/waiting`
- **Purpose:** Retrieve ordered list of waiting passengers in FIFO order.
- **Core C Function:** `waiting_queue_peek()` / traversal.

### 2.8 `GET /api/reports/summary`
- **Purpose:** Summary statistics and occupancy rates.
