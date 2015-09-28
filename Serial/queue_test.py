import Queue

rdict = {}

q = Queue.Queue()
for i in range(5):
    q.put(i)

while not q.empty():
    data = q.get()
    print(data)
    if data == 0:
        # do something
	rdict['hdr'] = data
	rdict['val1'] = q.get()
	rdict['val2'] = q.get()
	rdict['val3'] = q.get()
	rdict['ftr'] = q.get()
	#q.put(data)
	#for i in range(5):
	    #rdict[i] = q.get()
	break        
    else:
        q.put(data)

print("q should be correct, let's check...")
print(rdict)
