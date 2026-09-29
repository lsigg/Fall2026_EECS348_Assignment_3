# Email priority queue tests

Run these commands from the Programming Assignment 3 directory after compiling the programs

- `empty_queue.txt`: NEXT and READ on an empty queue, with COUNT before and after
- `sender_priority.txt`: all five sender categories, with dates chosen to check that sender priority comes first
- `date_priority.txt`: same-category emails spanning year, month, and day boundaries
- `mixed_commands.txt`: repeated NEXT, insertion, removal, counts, and refilling an empty queue

Each input has a matching `.expected.txt` file with the required output

```bash
./gpt_assignment3 tests/empty_queue.txt
./claude_assignment3 < tests/empty_queue.txt
./claudeimproved_assignment3 tests/empty_queue.txt
```

Compare output with the expected result (no diff output means a match)

```bash
./claudeimproved_assignment3 tests/empty_queue.txt | diff -u tests/empty_queue.expected.txt -
```

Replace `empty_queue` with another test name to run that case

All three programs were compiled and checked against all four cases
The GPT and original Claude programs pass all four cases
The improved Claude program passes the other three cases but fails `empty_queue` because READ on an empty queue prints nothing
The expected output preserves the requirement that both NEXT and READ print `No emails to read.` when empty
