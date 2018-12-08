# Flexit-SL4R-master
Control Flexit ventilation with Arduino. Tested on Flexit SL4R but may also work with models S3R, K2R, S3RK, L4X, S4R, S6R, S7R and L7X since they all use the same control board, Flexit CS50.

<img src="images/topology.png" >

This bulk of data is sent cyclically:
<img src="images/dataflow.png" >

Meaning of data in line 15 of the 16 lines:

195 <br/>
1 <br/>
0 <br/>
196 <br/>
75 <br/>
193 <br/>
1 <br/>
22      // telegram length <br/>
32 <br/>
14 <br/>
145 <br/>
128 <br/>
0 <br/>
17      // fan level. 17=level1, 34=level2, 51=level3 <br/>
0       // preheat on/off <br/>
4 <br/>
0 <br/>
25      // preset heat exchanger temperature <br/>
0       // heating element active when value >10 <br/>
100     // heating element inactive when value <100 <br/>
0 <br/>
49 <br/>
49 <br/>
0 <br/>
0 <br/>
0 <br/>
152 <br/>
136 <br/>
136 <br/>
0 <br/>
179     // checksum A <br/>
220     // checksum B <br/>
