### logic for Request Timeout 408 when polling
#### 408 --  The client did not produce a request within the time that the serve was prepared to wait. The client MAY repeat the request without modifications at any later time.

#### the following will happen in next loop after the client timeout:
| pollin/out | timeout_yes | time_out_no |
| --- | --- | --- |
| 00 | delete the client and clean pfds | continue with the for loop for client sockets |
| 10 or 11 | generate 408 response and set events to POLLOUT and continue | after first recv(), set the first_recv_time. For the following recv which belongs to the same request, don't modify the first_recv_time |
| 01 or 11 | set events to POLLIN, update the last_active to the time() after response is sent and init the first_recv_time | set events to POLLIN, update the last_active to the time() after response is sent and init the first_recv_time|
