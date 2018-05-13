To compile and execute this project, unzip all the files into a single directory.
Then, simply type make.

If everything worked well, then you must be seeing a prompt asking you whether you want your output on terminal or file.
If not, our program might fail under below circumstances, which are beyond our control:

****************************************************************************************************************************
Problem 1:
	 Another student has a file called "logFile.txt" in /tmp directory.  
Remedy: 
	 Try changing file path on line number 191 in main.cpp and in makefile (or) execute on a different CSE machine.

Problem 2:
	Another student has already created shared memory object with same name as we used, and accidentally forgot to remove it!
Remedy:
	You can change the all the shared semaphore names or simply execute on different CSE machine.


**************************************************************************************************************************** 
	Please contact us at karthikVenkataramanaPemmaraju@my.unt.edu or bharathramagoni@my.unt.edu for futher issues.
