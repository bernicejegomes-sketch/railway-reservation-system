# Railway Reservation and Waiting List Management

A railway reservation system with a C backend and a plain HTML, CSS and JavaScript frontend. It maintains passenger records, reserves seats, manages a waiting list for each train and handles cancellations. No external libraries or frameworks are used.

## Problem Statement

Railway reservation systems manage confirmed reservations, cancellations, waiting lists and seat availability. This system maintains passenger records and handles reservation requests and waiting-list operations appropriately.

## Features

- Three trains, each with 12 seats and up to 6 waiting-list places
- Book a ticket: a seat is confirmed if one is free, otherwise the passenger joins the waiting list
- Automatic promotion: when a confirmed ticket is cancelled, the first waiting passenger receives that seat
- Cancel any confirmed or waiting booking
- Look up any booking by its PNR (Passenger Name Record) number
- Live seat map, availability, waiting list and passenger list for each train
- Input validation on both the frontend and the backend
- Records are saved to a file after every change and reloaded at startup

## Data Structures

| Data structure | Used for | Why it fits | Time |
| --- | --- | --- | --- |
| Queue (doubly linked list) | Waiting list of each train | Passengers are served first come, first served. The double links also allow a passenger to leave from the middle of the queue. | Enqueue, dequeue and remove: O(1) |
| Hash table (separate chaining) | Passenger records by PNR | Lookup and cancellation start from a PNR, so searching every train would be wasteful. | Average O(1) insert, find and delete |
| Stack (array) | Free seat numbers of each train | Any free seat will do, so the most recently freed seat can be taken immediately. | Push and pop: O(1) |
| Array | Seat map of each train | A seat number maps directly to its passenger. | Access: O(1) |

Finding a passenger's place in the waiting list walks the queue, which takes O(w), where w is the length of the waiting list (at most 6).

## Reservation Rules

1. A booking takes a free seat if there is one, lowest seat number first on an empty train.
2. If no seat is free, the booking joins the back of the waiting list.
3. If the waiting list is also full, the booking is rejected.
4. Cancelling a confirmed ticket gives its exact seat to the first passenger in the waiting list. If nobody is waiting, the seat returns to the free stack.
5. Cancelling a waiting booking removes it from the queue, and everyone behind it moves up one place.
6. A free seat and a waiting passenger never exist together on the same train.
7. A cancelled booking is removed from the records, so its PNR is no longer found. PNRs are never reused.

## Project Structure

```
.
├── backend/
│   ├── railway.h          Data structure definitions and function declarations
│   ├── railway.c          Reservation logic, data structures and file storage
│   ├── server.c           HTTP server and JSON API
│   └── test_railway.c     Unit tests
├── frontend/
│   └── index.html         User interface
├── Makefile
└── README.md
```

## How It Works

```
Browser (frontend/index.html)
        │  HTTP requests, JSON responses
        ▼
server.c    parses requests and builds responses
        │  function calls
        ▼
railway.c   queue, hash table, stack and seat arrays
        │  saved after every change
        ▼
records.csv
```

The server handles one request at a time, so the data structures need no locking.

## Requirements

- A C compiler (`gcc` or `clang`)
- `make`
- macOS or Linux. On Windows, use WSL because the server uses POSIX sockets.
- Any modern web browser

## Running the Project

Run these commands from the project folder:

```sh
make run
```

Then open <http://localhost:8080>.

To use another port, for example 9000:

```sh
make
./railway 9000
```

Press `Ctrl+C` to stop the server. Always start it from the project folder, because it looks for `frontend/index.html` and `records.csv` there.

## Quick Demonstration

1. Book 12 tickets on one train. Every seat on the seat map fills up.
2. Book 2 more. They appear on the waiting list as WL 1 and WL 2.
3. Cancel any confirmed passenger. WL 1 takes that seat and WL 2 becomes WL 1.
4. Enter a PNR under Check PNR status to see the current status of that booking.
5. Restart the server and refresh the page. All records are still there.

## Running the Tests

```sh
make test
```

The tests run with AddressSanitizer and UndefinedBehaviorSanitizer. They cover booking order, validation, the waiting-list limit, promotion after cancellation, removal from any queue position, seat reuse, PNR lookup, and saving and loading records.

## API

`POST` bodies use `application/x-www-form-urlencoded`. Errors return a JSON object such as `{"error":"No booking found for this PNR"}`.

| Method | Path | Parameters | Result |
| --- | --- | --- | --- |
| GET | `/` | None | The web page |
| GET | `/api/state` | None | All trains with their availability, passengers and waiting lists |
| GET | `/api/pnr` | `pnr` (query string) | One passenger, or 404 |
| POST | `/api/book` | `train`, `name`, `age`, `gender` | The new passenger, or 400 with a reason |
| POST | `/api/cancel` | `pnr` | The cancelled PNR and any promoted passenger, or 404 |

Example:

```sh
curl -d "train=1&name=Asha+Verma&age=29&gender=F" http://localhost:8080/api/book
```

```json
{"pnr":1000001,"name":"Asha Verma","age":29,"gender":"F","train":1,"status":"CONFIRMED","seat":1,"position":0}
```

## Stored Data

`records.csv` is created on the first booking. Its first line is the next PNR, and each following line is one passenger:

```
pnr,train,status,seat,age,gender,name
```

`status` is `C` for confirmed or `W` for waiting. Waiting passengers are stored in queue order. If the file is damaged, the server refuses to start rather than overwrite it. To clear all bookings, stop the server and delete the file.

## Limitations

- The trains, schedule, seat count and waiting-list limit are fixed in `railway.c` and `railway.h`
- Each train has a single journey, with no travel dates or fare calculation
- There are no user accounts, so anyone using the page can cancel any booking
- The server accepts connections from the local computer only
