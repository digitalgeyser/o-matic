compx = 87;	// Size of compartments, X
compy = 185;	// Size of compartments, Y
wall = 1.15;		// Width of wall
nox = 2;		// Number of compartments, X
noy = 1;		// Number of compartments, Y
deep = 190;		// Depth of compartments

tolerance=.05;	// Tolerance around lid.  If it's too tight, increase this. 
			// If it's too loose, decrease it.
lidoffset=3;		// This is how far away from the box to print the lid
lidheight=8;		// Height of lid (duh)
showlid=false;	// Whether or not to render the lid
showbox=true;	// Whether or not to render the box

if (showbox) {
difference() {
	cube ( size = [nox * (compx + wall) + wall, noy * (compy + wall) + wall, (deep + wall)], center = false);
    union() {
	for ( ybox = [ 0 : noy - 1])
		{
             for( xbox = [ 0 : nox - 1])
			{
			translate([ xbox * ( compx + wall ) + wall, ybox * ( compy + wall ) + wall, wall])
			cube ( size = [ compx, compy, deep+1 ]);
			}
		}
	};
    
    translate([compx, compy/2, 30])
    rotate ([0,90,0])
    cylinder (r=19, h=20, center=true);
}
}

if (showlid) {
translate ([0,noy*(compy+wall)+wall+lidoffset,0])
difference() {
	cube ( size = [nox * (compx + wall) + 3 * wall + 2* tolerance, noy * (compy + wall) + 3 * wall + 2*tolerance, lidheight], center = false);

	translate ([wall,wall,wall])
	cube ( size = [nox * (compx + wall) +wall+tolerance, noy * (compy + wall) + wall + tolerance, lidheight+1], center = false);
	}

}