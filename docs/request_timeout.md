### logic for Request Timeout 408 when polling

| pollin/out | timeout_yes | time_out_no |
| --- | --- | --- |
| 00 | set bool timeout to true | check if client timed out by calculating current_time - init_time |
| 01 | send response with status code 408 | - |
| 10 | - | after first recv(), set the init_time for the timeout |
| 11 | set bool timeout to true | - |