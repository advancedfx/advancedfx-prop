#!BPY

"""
Name: 'HLAE camera motion (.bvh)'
Blender: 246
Group: 'Import'
Tip: 'Import HLAE camera motion data'
"""

__author__ = "ripieces"
__url__ = "advancedfx.org"
__version__ = "0.0.0.1 (2009-09-02T18:20Z)"

__bpydoc__ = """\
HLAE camera motion Import

For more info see http://www.advancedfx.org/
"""


# Copyright (c) advancedfx.org
#
# Last changes:
# 2009-09-01 by dominik.matrixstorm.com
#
# First changes:
# 2009-09-01 by dominik.matrixstorm.com


import Blender


def SetError(error):
	print 'ERROR:', error
	Blender.Draw.PupBlock('Error!', [(error)])
	

# <summary> reads a line from file and separates it into words by splitting whitespace </summary>
# <param name="file"> file to read from </param>
# <returns> list of words </returns>
def ReadLineWords(file):
	line = file.readline()
	words = [ll for ll in line.split() if ll]	
	return words


# <summary> searches a list of words for a word by lower case comparison </summary>
# <param name="words"> list to search </param>
# <param name="word"> word to find </param>
# <returns> less than 0 if not found, otherwise the first list index </returns>
def FindWordL(words, word):
	i = 0
	word = word.lower()
	while i < len(words):
		if words[i].lower() == word:
			return i
		i += 1
	return -1


# <summary> Scans the file till a line containing a lower case match of filterWord is found </summary>
# <param name="file"> file to read from </param>
# <param name="filterWord"> word to find </param>
# <returns> False on fail, otherwise same as ReadLineWords for this line </returns>
def ReadLineWordsFilterL(file, filterWord):
	while True:
		words = ReadLineWords(file)
		if 0 < len(words):
			if 0 <= FindWordL(words, filterWord):
				return words
		else:
			return False


# <summary> Scans the file till the channels line and reads channel information </summary>
# <param name="file"> file to read from </param>
# <returns> False on fail, otherwise channel indexes as follows: [Xposition, Yposition, Zposition, Zrotation, Xrotation, Yrotation] </returns>
def ReadChannels(file):
	words = ReadLineWordsFilterL(file, 'CHANNELS')
	
	if not words:
		return False

	channels = [\
	FindWordL(words, 'Xposition'),\
	FindWordL(words, 'Yposition'),\
	FindWordL(words, 'Zposition'),\
	FindWordL(words, 'Zrotation'),\
	FindWordL(words, 'Xrotation'),\
	FindWordL(words, 'Yrotation')\
	]
	
	idx = 0
	while idx < 6:
		channels[idx] -= 2
		idx += 1
			
	for channel in channels:
		if not (0 <= channel and channel < 6):
			return False
			
	return channels
	

def ReadRootName(file):
	words = ReadLineWordsFilterL(file, 'ROOT')
	
	if not words or len(words)<2:
		return False
		
	return words[1]


def ReadFrame(file, channels):
	line = ReadLineWords(file)
	
	if len(line) < 6:
		return False;
	
	Xpos = float(line[channels[0]])
	Ypos = float(line[channels[1]])
	Zpos = float(line[channels[2]])
	Zrot = float(line[channels[3]])
	Xrot = float(line[channels[4]])
	Yrot = float(line[channels[5]])
	
	return [Xpos, Ypos, Zpos, Zrot, Xrot, Yrot]

	
def BuildTargetVector(Xpos, Ypos, Zpos, Zrot, Xrot, Yrot, forwardLen):
	MRX = Blender.Mathutils.RotationMatrix(Xrot, 3, "x")
	MRY = Blender.Mathutils.RotationMatrix(Yrot, 3, "y")
	MRZ = Blender.Mathutils.RotationMatrix(Zrot, 3, "z")
	
	lvec = Blender.Mathutils.Vector(Xpos, Ypos, Zpos)
	
	fvec = Blender.Mathutils.Vector(0, 0, -forwardLen)
	
	fvec = Blender.Mathutils.VecMultMat(fvec, MRX) # pitch
	fvec = Blender.Mathutils.VecMultMat(fvec, MRZ) # yaw
	fvec = Blender.Mathutils.VecMultMat(fvec, MRY) # roll
	
	return lvec + fvec


def ReadFile(fileName, scale, forwardLen):
	file = open(fileName, 'rU')
	
	rootName = ReadRootName(file)
	if not rootName:
		SetError('Failed parsing ROOT.')
		return False
		
	print 'ROOT:', rootName

	channels = ReadChannels(file)
	if not channels:
		SetError('Failed parsing CHANNELS.')
		return False
		
	# seek to last line before Frame data:
	if not ReadLineWordsFilterL(file, 'Time:'):
		SetError('Could not locate "Frame Time:" entry.')
		return False

	# build the curve:	
	crvLoc = Blender.Curve.New(rootName +'_CLoc')
	crvTgt = Blender.Curve.New(rootName +'_CTgt')
	
	nrbLoc = False
	nrbTgt = False
			
	while True:
		frame = ReadFrame(file, channels)
		if not frame:
			break;
			
		BXP = frame[0] *scale
		BYP = frame[1] *scale
		BZP = frame[2] *scale

		BZR = frame[3]
		BXR = frame[4]
		BYR = frame[5]
		
		VLOC = [BXP, BYP, BZP, 1]

		tgt = BuildTargetVector(BXP, BYP, BZP, BZR, BXR, BYR, forwardLen)
		VTGT = [tgt[0], tgt[1], tgt[2], 1]
		
		if not nrbLoc:
			crvLoc.appendNurb(VLOC)
			nrbLoc = crvLoc[0]
			nrbLoc.setType(0) # Poly
		else:
			nrbLoc.append(VLOC)
			
		if not nrbTgt:
			crvTgt.appendNurb(VTGT)
			nrbTgt = crvTgt[0]
			nrbTgt.setType(0) # Poly
		else:
			nrbTgt.append(VTGT)
			
	# setup scene
		
	scn = Blender.Scene.GetCurrent()
	scn.objects.selected = []
	
	# 1.570796326794896619231321691639...
	halfPie = 1.5707963267
	
	obLoc = scn.objects.new(crvLoc)
	obLoc.RotX = halfPie
	obLoc.RotZ = -halfPie
	
	obTgt = scn.objects.new(crvTgt)
	obTgt.RotX = halfPie
	obTgt.RotZ = -halfPie
	
	BCS = Blender.Constraint.Settings
	BCT = Blender.Constraint.Type
	
	obAim = scn.objects.new('Empty')
	obAim.name=rootName +'_Aim'

	aCt = obAim.constraints.append(BCT.FOLLOWPATH)
	aCt.name = 'Follow'
	aCt[BCS.TARGET] = obTgt
	
	cam = Blender.Camera.New('persp', rootName);
	obCam = scn.objects.new(cam)

	aCt = obCam.constraints.append(BCT.FOLLOWPATH)
	aCt.name = 'Follow'
	aCt[BCS.TARGET] = obLoc

	aCt = obCam.constraints.append(BCT.TRACKTO)
	aCt.name = 'Track'
	aCt[BCS.TARGET] = obAim
	aCt[BCS.TRACK] = BCS.TRACKNEGZ
	aCt[BCS.UP] = BCS.UPY

	scn.update(1)
	Blender.Window.RedrawAll()
	
	return True


def load_HlaeCamMotion(fileName):
	UI_Scale = Blender.Draw.Create(0.01)
	UI_ForwardLen = Blender.Draw.Create(0.5)

	UI_block = []	
	UI_block.append(("Scale:", UI_Scale, 0.001, 10, 'Scaling'))
	UI_block.append(("Forward:", UI_ForwardLen, 0.001, 10, 'Length of forward vector (determines distance of curves)'))
	
	if not Blender.Draw.PupBlock('HLAE camera motion import', UI_block):
		return
			
	IMP_Scale = UI_Scale.val
	IMP_ForwardLen = UI_ForwardLen.val

	print 'Importing', fileName, 'Scale =', IMP_Scale, 'Forward =', IMP_ForwardLen
	
	if ReadFile(fileName, IMP_Scale, IMP_ForwardLen):
		print 'Done.'
	else:
		print 'FAILED';

	
def main():
	Blender.Window.FileSelector(load_HlaeCamMotion, 'Import', '*.bvh')

	
if __name__ == '__main__':
	main()