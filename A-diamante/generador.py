from random import *

a = int(raw_input())
i=1;

print "p edge",a,"797"
while (i <= a):
    b = i
    while ( b <= a ):
        if (i == b): 
        	print 'a',i,b,'0'
        if(randint(1,100) < randint(1,100)):
            r = randint(1,a)
            print 'a',i,b,r
            print 'a',b,i,r
     	b = b+1
    i= i+1