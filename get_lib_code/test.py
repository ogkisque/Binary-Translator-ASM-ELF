with open ("a.out", "rb") as f:
	data = f.read()
	
size1 = 0
size2 = 0
	
fl = False
mass = []
bin_mass = []
for i in data:
	if i == 0x90 and fl == False:
		fl = True
		mass.append (hex(i))
		bin_mass.append (i)
	elif i == 0x90 and fl == True:
		fl = False
		mass.append (hex(i))
		bin_mass.append (i)
	else:
		if fl:
			mass.append (hex(i))
			bin_mass.append (i)
			
mass1 = []
mass2 = []
fl = False
for i in mass:
	if fl:
		mass2.append(i)
	else:
		mass1.append(i)
		
	if i == '0xc3':
		fl = True
		
print (mass1)
print (len (mass1))
print (mass2)
print (len (mass2))

with open ("file_bin", "wb") as f1:
	f1.write(bytes(bin_mass))
	
with open ("file_bin", "rb") as f2:
	data1 = f2.read()
	
for i in data1:
	print (hex(i))
