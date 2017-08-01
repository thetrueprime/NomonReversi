from ldtp import * 
from ldtputils import * 
import Tkinter as tk
buttontouse = "";
winnametouse = "";
nomons = [];
clicked = 0
root=tk.Tk()
def adaptProgress():
	i = 0
	amount = len(nomons)
	for nomon in nomons:
		nomon[3] = (100*i)/amount
		i=i+1

def isSelected(non):
	if non[3]>90 or non[3] <10:
		return True;
	return False;	

		
def clickevent():
	global root
	global nomons
	global clicked
	if(clicked == 0):
		for nomon in nomons:
			if isSelected(nomon):
				print("Carrying Through")
			else:
				print("removed")
				nomons.remove(nomon)
		if len(nomons)==1:
			clicked = 1
		adaptProgress()
	else:
		
		app = nomons[0][4]
		obj = nomons[0][5]
		click(app,obj);
		root.destroy()

def key(event):
	print "pressed", repr(event.char)

def callback(event):
	print "clicked at", event.x, event.y
	clickevent();


w = tk.Canvas(root,width=3000,height=3000)
w.bind("<Key>", key)
w.bind("<Button-1>", callback)
w.pack(expand=1)
#w.create_line(0, 0, 200, 100)
#w.create_line(0, 100, 200, 0, fill="red", dash=(4, 4))

#w.create_rectangle(50, 25, 150, 75, fill="blue")
i = 0
geometry = root.winfo_geometry();
xoffset = 0;
yoffset = 0;
print("geometry:"+str(geometry));
print("xoffset = "+str(xoffset));
print("yoffset = "+str(yoffset));
def isinValidButton(app,obj):
	try:
		prop = getobjectproperty(app,obj,"parent");
		print(prop)
		if "ukn" in prop:
			return True
		else:
			return False
	except:
		return True
	 #return bool(re.search(r'\d', obj))
def isNumberButton(app,obj):
	try:
		prop = getobjectproperty(app,obj,"parent");
		print(prop)
		if "pnl" in prop:
			return True
		else:
			return False
	except:
		return True
	 #return bool(re.search(r'\d', obj))

def reactivate():
	global i
	for app in getwindowlist():
		print(app);
		if "frm" in app:
			app.replace("frm","");
		print("now"+app);
		if buttontouse=="":
			winname = app;
			print("setwinname");
		if "dlg" not in app and "frmudtp" not in app:
			print("not dlg")
			for obj in getobjectlist(app):
				if "btn" in obj :
					if(not isinValidButton(app,obj) and not isNumberButton(app,obj)):
						if i == 100 :
							#i=0;
							break; 
						size = getobjectsize(app,obj);
						if size[0]>0 and size[1] >0:
							print(str(i)+str(size));
							i = i+1;
							radius = size[2]/2;	
							if (size[3]/2)<radius:
								radius=size[3]/2;
							
							nomon = [size[0]-xoffset,size[1]-yoffset,radius,0,app,obj,isNumberButton(app,obj)];					
							nomons.append(nomon);
		adaptProgress()
	

reactivate()

progress = 0

def hubloop():
	w.delete("all")
	#print("running");
	w.create_rectangle(0,0,30000,30000,fill="white");

	for nomon in nomons:
		cprogress = nomon[3]
		if nomon[6]:
			w.create_arc(nomon[0],nomon[1],nomon[0]+nomon[2]*2,nomon[1]+nomon[2]*2,start=0,extent=360*cprogress/100,fill="red");
		else:
			w.create_arc(nomon[0],nomon[1],nomon[0]+nomon[2]*2,nomon[1]+nomon[2]*2,start=0,extent=360*cprogress/100,fill="blue");
			nomon[3] = cprogress+1;
			#print("extent:"+str(360*cprogress/100));
			if cprogress == 100:
				nomon[3] = 0
		
	
	#global progress
	#progress = progress+1;
	#if progress==100 :
	#	progress = 0
	root.after(50,hubloop);

root.after(50,hubloop)
root.wait_visibility(root)
root.attributes("-fullscreen",True)
root.wm_attributes('-alpha',0.3)
root.mainloop()
