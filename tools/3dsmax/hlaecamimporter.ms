rollout hlaecamimporter "HLAE Cam Importer 1.0.3.3" width:200 height:188
(
	button loadandimport "Load and Import" pos:[28,116] width:144 height:18
	label lbl1 "http://advancedfx.org/" pos:[44,18] width:112 height:15 enabled:true
	label lbl2 "HLAE Cam Importer Script by msthavoc" pos:[4,4] width:192 height:15
	GroupBox grp2 "Importer Settings" pos:[5,32] width:190 height:81
	checkbox hlae_90fix "90° fix" pos:[40,74] width:120 height:16 triState:1
	edittext hlae_fov "FieldOfView" pos:[40,92] width:106 height:16 text:"90"
	button hlae_pathbutton "Path for cam file (*.bvh)" pos:[13,48] width:174 height:24
	button exitdialogbut "Exit" pos:[28,156] width:144 height:18 enabled:true
	button manual "Manual" pos:[28,136] width:144 height:18 enabled:true
	
	GLOBAL filename

	on loadandimport pressed do
	(
		if  filename == undefined 
		then (DestroyDialog(hlaecamimporter);) 
		else (
			local new_Tx, new_Ty, new_Tz, new_Rx, new_Ry, new_Rz, new_TT;
			local myrot;
			local file = openFile filename;
			local motion, frames, frametime;
			local hlaecamera;
			local camname;
			
			DestroyDialog hlaecamimporter;
	
			
			for i = 1 to 11 do ( motion = readLine file )
			if motion == "MOTION" 
			then (
				skipToString file " "
				frames = (readLine file) as Integer
				skipToString file " " 
				skipToString file " " 
				frametime = (readLine file) as Float 
				timer=0
				camname = "HLAECam"+localTime[1]+localTime[2]+"_"+localTime[4]+localTime[5]+"_"+localTime[7]+localTime[8]+localTime[9]+localTime[10]+"__"+localTime[12]+localTime[13]+"_"+localTime[15]+localTime[16]+"_"+localTime[18]+localTime[19]; 
				frameRate = 1/frametime; animationRange = interval 0 frames;
				hlaecamera=Freecamera fov:(hlae_fov.text as Float) name:camname;
				
				progressStart "Importing Cam..... [HLAE]"
				
				while (eof file != true) and not getProgressCancel()  do (
					progressUpdate(100*timer/frames)
					
					new_Tx =  ((readDelimitedString file " ") as Float)
					new_Tz =  ((readDelimitedString file " ") as Float)
					new_Ty = -((readDelimitedString file " ") as Float)
					new_Ry = -((readDelimitedString file " ") as Float)
					new_Rx =  ((readDelimitedString file " ") as Float)
					new_Rz =  ((readLine file) as Float)
					
					if 1 == hlae_90fix.triState then (
						new_TT = new_Tx
						new_Tx = new_Ty
						new_Ty = -new_TT
						new_Rz -= 90
					)
					
					myrot = (eulerangles -90 0 0) as matrix3 -- start with camera pointing into X-direction
					myrot = preRotateX myrot -new_Rx
					myrot = preRotateY myrot -new_Ry
					myrot = preRotateZ myrot -new_Rz

					-- build new key frame:
					sliderTime = timer
					set animate on
					hlaecamera.rotation = myrot
					hlaecamera.pos = ([new_Tx,new_Ty,new_Tz])
					set animate off
					
					timer+=1
				)
			
				progressEnd()
			)	
			else (
				messagebox ("not a valid HLAE cam file")
			)
			
			close file
		)
	)
	
	on hlae_pathbutton pressed do (
		filename = getOpenFileName types:"HLAE Cam(*.bvh)|*.bvh|All files|*.*|"
		hlae_pathbutton.caption = if filename == undefined then "No File Chosen, select file again..." else filename 
	)
	
	on exitdialogbut pressed do (
		DestroyDialog(hlaecamimporter);
	)
	
	on manual pressed do (
		messagebox ("1. Select the directory of your cam file from your HLAE capture\n2. Check 90° fix if you want (default = checked), this means you don't need to rotate objects or maps to fit\n3. Enter your FieldOfView, you used in HLAE. Default in HL is 90°, so here it's here also default\n4. Click on 'Load and Import' for importing the camera. \n\n A Importing Dialog appears and you can see in the background 3ds max set up the keyframes for the camera (loop from 0 to maximum frames).\nIf you selected 90° fix the script will loop a second time for rotating the cam 90 degree to fit the cam to your maps/objects.")
	)
)
CreateDialog hlaecamimporter
--print "HLAE Cam Importer Script by msthavoc loaded successfully" 




