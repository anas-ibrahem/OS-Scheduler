
# Test case I

```
#id arrival runtime priority
1	10	15	8
2	11	1	6
3	11	10	5
4	20	28	0
```
## **round robin** 

>Each process gets a fixed time slice (quantum). If a process doesn’t finish in that time, it's placed back in the queue

here the quantum is 3 in this test case:

- process 1 starts running at time 10 for 3 seconds
- process 2 runs and finishes execution at timestamp 14 
- process 3 runs at timestamp 14 for 3 seconds and gets enqueued back at the end of the circular queue
- process 1 reruns at timestamp 17 from, finishes at timestamp 20 and gets enqueued at the end of the circular queue
- process 4 arrives at timestamp 20
- process 3 reruns till timestamp 23
- process 4 starts running and runs at 26 
- process 1 again at timestamp 26
- context switching between 1 , 3, and 4 until they all finish 


```
At time x process y state arr w total z remain y wait k
At time 10 process 1 started arr 10 total 15 remain 15 wait 0
At time 13 process 1 stopped arr 10 total 15 remain 12 wait 0
At time 13 process 2 started arr 11 total 1 remain 1 wait 2
At time 14 process 2 finished arr 11 total 1 remain 0 wait 2 TA 1 WTA 1
At time 14 process 3 started arr 11 total 10 remain 10 wait 3
At time 17 process 3 stopped arr 11 total 10 remain 7 wait 3
At time 17 process 1 resumed arr 10 total 15 remain 12 wait 4
At time 20 process 1 stopped arr 10 total 15 remain 9 wait 4
At time 20 process 3 resumed arr 11 total 10 remain 7 wait 6
At time 23 process 3 stopped arr 11 total 10 remain 4 wait 6
At time 23 process 4 started arr 20 total 28 remain 28 wait 3
At time 26 process 4 stopped arr 20 total 28 remain 25 wait 3
At time 26 process 1 resumed arr 10 total 15 remain 9 wait 10
At time 29 process 1 stopped arr 10 total 15 remain 6 wait 10
At time 29 process 3 resumed arr 11 total 10 remain 4 wait 12
At time 32 process 3 stopped arr 11 total 10 remain 1 wait 12
At time 32 process 4 resumed arr 20 total 28 remain 25 wait 9
At time 35 process 4 stopped arr 20 total 28 remain 22 wait 9
At time 35 process 1 resumed arr 10 total 15 remain 6 wait 16
At time 38 process 1 stopped arr 10 total 15 remain 3 wait 16
At time 38 process 3 resumed arr 11 total 10 remain 1 wait 18
At time 39 process 3 finished arr 11 total 10 remain 0 wait 18 TA 25 WTA 2.5
At time 39 process 4 resumed arr 20 total 28 remain 22 wait 13
At time 42 process 4 stopped arr 20 total 28 remain 19 wait 13
At time 42 process 1 resumed arr 10 total 15 remain 3 wait 20
At time 45 process 1 finished arr 10 total 15 remain 0 wait 20 TA 35 WTA 2.33
At time 45 process 4 resumed arr 20 total 28 remain 19 wait 16
At time 48 process 4 stopped arr 20 total 28 remain 16 wait 16
At time 48 process 4 resumed arr 20 total 28 remain 16 wait 16
At time 51 process 4 stopped arr 20 total 28 remain 13 wait 16
At time 51 process 4 resumed arr 20 total 28 remain 13 wait 16
At time 54 process 4 stopped arr 20 total 28 remain 10 wait 16
At time 54 process 4 resumed arr 20 total 28 remain 10 wait 16
At time 57 process 4 stopped arr 20 total 28 remain 7 wait 16
At time 57 process 4 resumed arr 20 total 28 remain 7 wait 16
At time 60 process 4 stopped arr 20 total 28 remain 4 wait 16
At time 60 process 4 resumed arr 20 total 28 remain 4 wait 16
At time 63 process 4 stopped arr 20 total 28 remain 1 wait 16
At time 63 process 4 resumed arr 20 total 28 remain 1 wait 16
At time 64 process 4 finished arr 20 total 28 remain 0 wait 16 TA 41 WTA 1.46
```


```
CPU utilization = 85.94%
Avg WTA = 1.82
Avg Waiting = 14
Std WTA = 1.82
```

---

## **highest priority first** 

>it chooses the process with highest priority and runs it until it finishes (non preemptive)

order of running 
- process 1  
- process 4 
- process 3
- process 2 
notices in HPF how process 2 was executed last 

**output files**
```
At time x process y state arr w total z remain y wait k
At time 10 process 1 started arr 10 total 15 remain 15 wait 0
At time 25 process 1 finished arr 10 total 15 remain 0 wait 0 TA 15 WTA 1
At time 25 process 4 started arr 20 total 28 remain 28 wait 5
At time 53 process 4 finished arr 20 total 28 remain 0 wait 5 TA 28 WTA 1
At time 53 process 3 started arr 11 total 10 remain 10 wait 42
At time 63 process 3 finished arr 11 total 10 remain 0 wait 42 TA 10 WTA 1
At time 63 process 2 started arr 11 total 1 remain 1 wait 52
At time 64 process 2 finished arr 11 total 1 remain 0 wait 52 TA 1 WTA 1
```

```
CPU utilization = 85.94%
Avg WTA = 1
Avg Waiting = 24.75
Std WTA = 1
```

----

## **shortest remaining time next** 
>it always chooses the process which has smallest remainint time and context switch to it 

- process 1 runs for one second
- process 2 runs and finishes at timestamp 12
- process 3 runs until it finishes at timestamp 22 
- process 1 runs and finishes at timestamp 36
- then finally process 4 runs 

notice how process 1 had to wait for 10 more seconds to start execution 

results

```
At time x process y state arr w total z remain y wait k
At time 10 process 1 started arr 10 total 15 remain 15 wait 0
At time 11 process 1 stopped arr 10 total 15 remain 14 wait 0
At time 11 process 2 started arr 11 total 1 remain 1 wait 0
At time 12 process 2 finished arr 11 total 1 remain 0 wait 0 TA 1 WTA 1
At time 12 process 3 started arr 11 total 10 remain 10 wait 1
At time 22 process 3 finished arr 11 total 10 remain 0 wait 1 TA 10 WTA 1
At time 22 process 1 resumed arr 10 total 15 remain 14 wait 11
At time 36 process 1 finished arr 10 total 15 remain 0 wait 11 TA 26 WTA 1.73
At time 36 process 4 started arr 20 total 28 remain 28 wait 16
At time 64 process 4 finished arr 20 total 28 remain 0 wait 16 TA 28 WTA 1
```

```
CPU utilization = 84.38%
Avg WTA = 1.18
Avg Waiting = 7
Std WTA = 1.18
```

## comparison between different algorithms
- round robin had highest WTA
- highest priority first had the highest Average wait time 

---

# Test case II

```
#id arrival runtime priority
1	10	15	2
2	11	1	5
3	11	10	0
```

----

# Test case III

```
#id arrival runtime priority
1	1	13	7
2	6	5	1
3	9	2	3
```

---

# Test case IV

```
#id arrival runtime priority
1	11	19	1
2	12	18	2
3	21	4	1
4	26	2	9
5	32	8	0
```


---
# Test case V

```
#id arrival runtime priority
1	9	16	0
2	19	7	10
3	24	29	6
4	31	10	8
5	38	9	7
6	45	23	4
```
