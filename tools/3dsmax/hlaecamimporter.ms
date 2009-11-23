rollout hlaecamimporter "HLAE Cam Importer 1.02" width:200 height:188
(

 	rollout ImportingDialog "Importing Cam..... [HLAE]" width:150 height:50
	(
		GroupBox grp2 "Importing Dialog" pos:[15,3] width:120 height:44
		label lbl3 "Please wait..." pos:[34,22] width:81 height:15
	)
	
	button loadandimport "Load and Import" pos:[28,116] width:144 height:18
	label lbl1 "http://advancedfx.org/" pos:[44,18] width:112 height:15 enabled:true
	label lbl2 "HLAE Cam Importer Script by msthavoc" pos:[4,4] width:192 height:15
	GroupBox grp2 "Importer Settings" pos:[5,32] width:190 height:81
	checkbox hlae_90fix "90° fix" pos:[40,74] width:120 height:16 triState:1
	edittext hlae_fov "FieldOfView" pos:[40,92] width:106 height:16 text:"90"
	button hlae_pathbutton "Path for cam file (*.bvh)" pos:[13,48] width:174 height:24
	button exitdialogbut "Exit" pos:[28,156] width:144 height:18 enabled:true
	GLOBAL filename
	
	
	

	button manual "Manual" pos:[28,136] width:144 height:18 enabled:true
	on loadandimport pressed do
	(
	if  filename == undefined 
	then (DestroyDialog(hlaecamimporter);) 
	else (
			local file = openFile filename;
			local motion, frames, frametime, fps;
			local current_x, current_y, current_z;
			local new_x, new_y, new_z, new_h, new_p, new_b;
			local position, lposition;
			local hlaecamera;
			local camname;
			local startrotation;
			local globalposition_cam;
			startrotation = ((eulerAngles -90 0 0) as quat)
			lrotation = (eulerangles 0 0 0)
	
			current_x=0.0
			current_y=0.0
			current_z=0.0
			DestroyDialog hlaecamimporter;
	
			
			for i = 1 to 11 do ( motion = readLine file )
			if motion == "MOTION" 
			then (
				CreateDialog ImportingDialog;
				skipToString file " "
				frames = (readLine file) as Integer
				skipToString file " " 
				skipToString file " " 
				frametime = (readLine file) as Float 
				timer=0
				lposition=[0.0,0.0,0.0]
				camname = "HLAECam"+localTime[1]+localTime[2]+"_"+localTime[4]+localTime[5]+"_"+localTime[7]+localTime[8]+localTime[9]+localTime[10]+"__"+localTime[12]+localTime[13]+"_"+localTime[15]+localTime[16]+"_"+localTime[18]+localTime[19]; 
				frameRate = 1/frametime; animationRange = interval 0 frames;
				hlaecamera=Freecamera fov:(hlae_fov.text as Float) name:camname;
				max tool animmode; set animate on
				sliderTime = 0
				rotate hlaecamera (angleaxis 180 [0,0,1]) 
				max tool animmode; set animate off
			
				while eof file != true  do 
					(  
						new_x = (readDelimitedString file " ") 
						new_z = (readDelimitedString file " ") 
						new_y = (readDelimitedString file " ") 
						new_b = (readDelimitedString file " ") 
						new_p = (readDelimitedString file " ") 
						new_h = (readLine file)
						new_x = new_x as Float
						new_y = new_y as Float
						new_z = new_z as Float
						new_h = new_h as Float
						new_p = new_p as Float
						new_b = new_b as Float
						position =  ([new_x,new_y,new_z])  
						sliderTime = timer
						max tool animmode; set animate on
						move hlaecamera lposition
						hlaecamera.rotation=(quat 0 -0.707107 -0.707107 0)
						hlaecamera.transform = preRotateY hlaecamera.transform new_h
						hlaecamera.transform = preRotateX hlaecamera.transform (-1*new_p)
						hlaecamera.transform = preRotateZ hlaecamera.transform new_b
						move hlaecamera position							
						max tool animmode; set animate off
						lposition=position*-1
						timer+=1
					) 
			
					if hlae_90fix.triState == 1 then(for i = 0 to frames do (sliderTime=i;max tool animmode;set animate on;about [0,0,hlaecamera.position.z] rotate hlaecamera (EulerAngles 0 0 90);max tool animmode;set animate off;))
					DestroyDialog ImportingDialog
				)
				
			else (messagebox ("not a valid HLAE cam file"))
			
	
			close file
	
		)
	
	)
	on hlae_pathbutton pressed do
	(
	filename = getOpenFileName types:"HLAE Cam(*.bvh)|*.bvh|All files|*.*|"
	hlae_pathbutton.caption = if filename == undefined then "No File Chosen, select file again..." else filename 
	)
	on exitdialogbut pressed do
	(
	DestroyDialog(hlaecamimporter);
	)
	on manual pressed do
	(
	messagebox ("1. Select the directory of your cam file from your HLAE capture\n2. Check 90° fix if you want (default = checked), this means you don't need to rotate objects or maps to fit\n3. Enter your FieldOfView, you used in HLAE. Default in HL is 90°, so here it's here also default\n4. Click on 'Load and Import' for importing the camera. \n\n A Importing Dialog appears and you can see in the background 3ds max set up the keyframes for the camera (loop from 0 to maximum frames).\nIf you selected 90° fix the script will loop a second time for rotating the cam 90 degree to fit the cam to your maps/objects.")
	)
)
CreateDialog hlaecamimporter
--print "HLAE Cam Importer Script by msthavoc loaded successfully" 




