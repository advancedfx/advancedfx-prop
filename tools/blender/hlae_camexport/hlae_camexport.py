#!BPY

"""
Name: 'HLAE camera motion (.bvh)'
Blender: 246
Group: 'Export'
Tip: 'Export motion data for HLAE'
"""

__author__ = "ripieces"
__url__ = "advancedfx.org"
__version__ = "0.0.2.2 (2011-01-23T17:50Z)"

__bpydoc__ = """\
HLAE camera motion Export

Select a single object you want to export.

For more info see http://www.advancedfx.org/
"""


# Copyright (c) advancedfx.org
#
# Last changes:
# 2011-01-23 by dominik.matrixstorm.com
#
# First changes:
# 2009-09-03 by dominik.matrixstorm.com


import Blender


# 57.29577951308232087679815481410...
RAD2DEG = 57.29577951308

def SetError(error):
	print 'ERROR:', error
	Blender.Draw.PupBlock('Error!', [(error)])
	
# <summary> Formats a float value to be suitable for bvh output </summary>
def FloatToBvhString(value):
	return "{0:f}".format(value)


def WriteHeader(file, frames, frameTime):
	file.write("HIERARCHY\n")
	file.write("ROOT MdtCam\n")
	file.write("{\n")
	file.write("\tOFFSET 0.00 0.00 0.00\n")
	file.write("\tCHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n")
	file.write("\tEnd Site\n")
	file.write("\t{\n")
	file.write("\t\tOFFSET 0.00 0.00 -1.00\n")
	file.write("\t}\n")
	file.write("}\n")
	file.write("MOTION\n")
	file.write("Frames: "+str(frames)+"\n")
	file.write("Frame Time: "+FloatToBvhString(frameTime)+"\n")
	
def LimDeg(val):
	return val

def WriteFile(fileName, render, obj, scale, startFrame, endFrame, fps):
	fps = int(fps)
	startFrame = int(startFrame)
	endFrame = int(endFrame)
	
	if not(startFrame <= endFrame):
		SetError('Voided startFrame <= endFrame')
		return False
		
	if not(1 <= fps and fps <= 120):
		SetError('Voided 1 <= fps and fps <= 120')
		return False

	if not(0.1 <= scale and scale <= 1000):
		SetError('Voided 0.1 <= scale and scale <= 1000')
		return False
	
	
	scn = Blender.Scene.GetCurrent()

	curFrame = 0	
	frameCount = 1 +endFrame - startFrame
	frameTime = 1.0 / float(fps)
	
	Rb = Blender.Mathutils.RotationMatrix(90.0, 3, 'z')
	if 'Camera' == obj.getType():
		Rb = Rb * Blender.Mathutils.RotationMatrix(-90.0, 3, 'x')

	file = open(fileName, 'wb')
	if not file:
		SetError('Could not open file '+fileName+' for writing')
		return False

	oldFrame = render.currentFrame()
	try:
		WriteHeader(file, frameCount, frameTime)
		
		lastTime = 0
		while curFrame<frameCount:
			render.currentFrame(startFrame + curFrame)
			
			curTime = Blender.sys.time()
			
			if(0.125 <= curTime - lastTime):
				lastTime = curTime

				# handle user input:
				if Blender.Window.QTest():
					evt, val = Blender.Window.QRead()
					if evt in [Blender.Draw.ESCKEY, Blender.Draw.QKEY]:
						break
						
				# update progessbar:
				Blender.Window.DrawProgressBar(float(curFrame)/float(frameCount), "Writing frame "+str(curFrame+1)+"/"+str(frameCount)+"...")

			loc = obj.getLocation('worldspace')
			rot = obj.getEuler('worldspace')
			
			rot = (Rb *Blender.Mathutils.Euler(rot[0] *RAD2DEG, rot[1] *RAD2DEG, rot[2]*RAD2DEG).toMatrix()).toEuler()
			
			X = -loc[1] *scale
			Y =  loc[2] *scale
			Z = -loc[0] *scale
			
			ZR = -rot.x
			XR = -rot.y
			YR =  rot.z
				
			ZR = LimDeg(ZR)
			XR = LimDeg(XR)				
			YR = LimDeg(YR)
			
			S = "" +FloatToBvhString(X) +" " +FloatToBvhString(Y) +" " +FloatToBvhString(Z) +" " +FloatToBvhString(ZR) +" " +FloatToBvhString(XR) +" " +FloatToBvhString(YR) + "\n"
			file.write(S)
			
			curFrame += 1
				
	finally:
		render.currentFrame(oldFrame)
		file.close()
	
	return curFrame == frameCount

def export_HlaeCamMotion(fileName):
	scn = Blender.Scene.GetCurrent()
	objs = scn.objects.selected
	
	if not(1<=len(objs)):
		SetError('Voided 1 <= len(scn.objects.selected)')
		return
	
	obj = objs[0]
	render = scn.getRenderingContext()
	
	IMP_Scale = float(100)
	IMP_StartFrame = int(render.startFrame())
	IMP_EndFrame = int(render.endFrame())
	IMP_Fps = int(render.fps)
	
	UI_Scale = Blender.Draw.Create(IMP_Scale)
	UI_StartFrame = Blender.Draw.Create(IMP_StartFrame)
	UI_EndFrame = Blender.Draw.Create(IMP_EndFrame)
	UI_Fps = Blender.Draw.Create(IMP_Fps)
	
	UI_block = []	
	UI_block.append(('Export "'+obj.name+'" ...'))
	UI_block.append(("Scale:", UI_Scale, 0.1, 1000.0, 'Scaling'))
	UI_block.append(("FPS:", UI_Fps, 1, 120, 'Frames Per Second'))
	UI_block.append(("StartFrame:", UI_StartFrame, 1, 30000, 'Frame from which the export shall start'))
	UI_block.append(("EndFrame:", UI_EndFrame, 1, 30000, 'Frame where the export shall end'))
	
	if not Blender.Draw.PupBlock('HLAE camera motion export', UI_block):
		return
		
	Blender.Window.DrawProgressBar(0.0, "Exproting ...")
			
	IMP_Scale = UI_Scale.val
	IMP_StartFrame = UI_StartFrame.val
	IMP_EndFrame = UI_EndFrame.val
	IMP_Fps = UI_Fps.val

	print 'Exporting', obj.name, 'to', fileName, 'Scale =', IMP_Scale, 'FPS =', IMP_Fps, 'StartFrame =', IMP_StartFrame, 'EndFrame =', IMP_EndFrame
	
	if WriteFile(fileName, render, obj, IMP_Scale, IMP_StartFrame, IMP_EndFrame, IMP_Fps):
		print 'Done.'
	else:
		print 'FAILED';
		
	Blender.Window.DrawProgressBar(1.0, "Finished.")
	
def main():
	if 1 != len(Blender.Scene.GetCurrent().objects.selected):
		SetError('Please select exactly one object to export.')
		return

	Blender.Window.FileSelector(export_HlaeCamMotion, 'Export', '*.bvh')

	
if __name__ == '__main__':
	main()