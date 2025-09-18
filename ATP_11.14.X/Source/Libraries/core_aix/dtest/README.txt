		DTEST - a TPS testing tool
	      ------------------------------

This folder contains a test program that acts like a dialog manager but
does NOT create a socket and listens to it. It simulates incoming
transactions at a predefined TPS rate. It also spawns as many threads as
would have been spawned by a dialog manager in a real production
environment, when as many POS terminals are connected simultaneously.

Source code can be easily changed to create as many threads as desired. Each
thread is akin to a POS connection and logs into XIPC/XCOM. The tool
automatically generates STAN numbers by incrementing STAN by 1. A list of
genuine device_id's and merchant_id's are added as an array in the source,
that can be modified if desired.

By default, each thread will login into XIPC/XCOM and starts sending
transactions, once every second (or based on "TPS rate" / "No of threads")

Eg:- if 30 threads are created and TPS rate selected is 30

then, each of the 30 threads will login (30 XIPC/XCOM users) and
send one transaction every second.

