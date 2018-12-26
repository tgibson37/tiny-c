import sys
import cairo
import shlex


def comment(cmt):
	cmt = cmt[:-1]
	#print "in comment: ", cmt
def draw(x):
	global ctx
	'''
	hard way: very long list of if-elif's
		if x[0] == "moveto":
			ctx.move_to( int(x[1]), int(x[2]) )
		elif x[0] == "lineto":
			ctx.line_to( int(x[1]), int(x[2]) )
			ctx.stroke()
	easy way: do ALL cmds with int or float only args:
		given x = [name,1,2] produce string = "ctx.<name>(1, 2)"
		then eval(string)
	'''
	cmd=x[0]
	argslist=x[1:]
	args= ','.join(argslist)
	string = "ctx." + cmd + "(" + args + ")"
	#print "in draw: ", string
	eval(string)

def window(x):
	global surface, ctx, width, height
	width = int(x[1])
	height= int(x[2])
	#print "in window: ", x
	surface = cairo.ImageSurface(cairo.FORMAT_RGB24, width, height)
	ctx = cairo.Context(surface)
	ctx.rectangle(0,0,width,height)
	ctx.set_source_rgb(1,1,1)
	ctx.fill()
	ctx.set_source_rgb(0,0,0)
def show():
	global surface
	#print "in show: ", pngname
	ctx.stroke()
	surface.write_to_png(pngname)
	from subprocess import call
	if len(show_app)>0:
		call([show_app, pngname])
def usage():
	print "Usage: ", sys.argv[0], "drawfile"

# It all starts here
if len(sys.argv)>1:
	drawname = sys.argv[1]
	drawfile = sys.argv[1] + ".draw"
	pngname = sys.argv[1] + ".png"
else:
	usage()
	sys.exit()
show_app = "display"
M_PI = 3.14159265358979323846
with open(drawfile) as f:
    for line in f:
		x = shlex.split(line)
		if ord(line[:1])==35:    # decimal 35 is hash
			comment(line)
		elif x[0]=="arc":
			r1 = float(x[1])
			r2 = float(x[2])
			r3 = float(x[3])
			d4 = int(x[4])
			r4 = float(d4*(M_PI/180))
			d5 = int(x[5])
			r5 = float(d5*(M_PI/180))
			ctx.arc(r1,r2,r3,r4,r5)
		elif x[0]=="arc_negative":
			r1 = float(x[1])
			r2 = float(x[2])
			r3 = float(x[3])
			d4 = int(x[4])
			r4 = float(d4*(M_PI/180))
			d5 = int(x[5])
			r5 = float(d5*(M_PI/180))
			ctx.arc_negative(r1,r2,r3,r4,r5)


		elif x[0]=="show":
			if len(x)>1:
				image_display=x[1]
			show()
		elif x[0]=="showapp":
			if len(x)>1:
				show_app = x[1]
			else:
				show_app = ""
		elif x[0]=="showtext":
			ctx.show_text(x[1])
		elif x[0]=="window":
			window(x)
		else:
			draw(x)
		'''
		elif x[0]=="":
			ctx.
		elif x[0]=="setrgb":
			r1 = float(x[1])/100
			r2 = float(x[2])/100
			r3 = float(x[3])/100
			ctx.set_source_rgb(r1,r2,r3)
		'''
#print("done")
