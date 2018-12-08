# Flexit-SL4R-master
Control Flexit ventilation with Arduino. Tested on Flexit SL4R but may also work with models S3R, K2R, S3RK, L4X, S4R, S6R, S7R and L7X since they all use the same control board, Flexit CS50.

<img src="images/topology.png" >

This bulk of data is sent cyclically:
<img src="images/dataflow.png" >

Meaning of data in line 15 of the 16 lines:

195 
1 
0 
196 
75 
193 
1 
22      // telegram length
32 
14 
145 
128 
0 
17      // fan level. 17=level1, 34=level2, 51=level3 
0       // preheat on/off
4 
0 
25      // preset heat exchanger temperature
0       // heating element active when value >10 
100     // heating element inactive when value <100 
0 
49 
49 
0 
0 
0 
152 
136 
136 
0 
179     // checksum A
220     // checksum B
