/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGPUVolumeRayCastMapper_CompositeFS.glsl

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Fragment program part with ray cast and composite method.

uniform sampler3D dataSetTexture; //Mehdi
uniform sampler1D opacityTexture;//Mehdi


uniform vec3 lowBounds;//Mehdi
uniform vec3 highBounds;//Mehdi


//uniform int Number_Of_Volumes;
const int Number_Of_Volumes = 1;


// Entry position (global scope)
vec3 pos;
vec3 posX;



// Incremental vector in texture space (global scope)
vec3 rayDir;

float tMax;

// from cropping vs no cropping
vec4 initialColor();

// from 1 vs 4 component shader.
float scalarFromValue(vec4 value);
vec4 colorFromValue(int volNumber,vec4 value);


// from noshade vs shade.
void initShade();
vec4 shade(int volNumber, vec4 value);



vec3 clipMin;

uniform vec3 ClippingplanesNormal;
uniform vec3 ClippingplanesOrigins;
uniform int clipped;

void clip (const int i, vec3 position)
{
	
	clipMin=vec3(-10.0, -10.0, -10.0);

	float partX=ClippingplanesNormal.x*(position.x-ClippingplanesOrigins.x);
	float partY=ClippingplanesNormal.y*(position.y-ClippingplanesOrigins.y);
	float partZ=ClippingplanesNormal.z*(position.z-ClippingplanesOrigins.z);

	float temp;
		
	if(ClippingplanesNormal.x!=0.0)
	{
		temp=((ClippingplanesNormal.x*ClippingplanesOrigins.x)-partY-partZ)/ClippingplanesNormal.x;
		//temp=findXonPlane(ClippingplanesNormal, ClippingplanesOrigins, pos);
		if(temp>pos.x)
		{
			if(temp>clipMin.x)
				clipMin.x=temp;

		}		
		else
		{
			if(temp<clipMin.x)
				clipMin.x=temp;

		}
		if (ClippingplanesNormal.x<0.0) clipMin.x=-clipMin.x;

	}
	if(ClippingplanesNormal.y!=0.0)
	{
		temp=((ClippingplanesNormal.y*ClippingplanesOrigins.y)-partX-partZ)/ClippingplanesNormal.y;
		//temp=findYonPlane(ClippingplanesNormal, ClippingplanesOrigins, pos);
		if(temp>pos.y)
		{
			if(temp>clipMin.y)
				clipMin.y=temp;

		}
		
		else
		{
			if(temp<=clipMin.y)
				clipMin.y=temp;

		}
		if (ClippingplanesNormal.y<0.0) clipMin.y=-clipMin.y;
	}
	
	if(ClippingplanesNormal.z!=0.0)
	{
		temp=((ClippingplanesNormal.z*ClippingplanesOrigins.z)-partX-partY)/ClippingplanesNormal.z;
		//temp=findZonPlane(ClippingplanesNormal, ClippingplanesOrigins, pos);
		if(temp>pos.z)
		{
			if(temp>clipMin.z)
				clipMin.z=temp;

		}
		
		else
		{
			if(temp<=clipMin.z)
				clipMin.z=temp;

		}
		if (ClippingplanesNormal.z<0.0) clipMin.z=-clipMin.z;
	}
		
		
}



void trace(void)
{
  
  bool inside=true;
  

  vec4 destColor=initialColor();
  float remainOpacity=1.0-destColor.a;
  
  vec4 color;
  vec4 opacity;
  

  
  initShade();

  //float t=0.0;
  



  // We NEED two nested loops. It is trick to work around hardware
  // limitation about the maximum number of loops.

    //hier war eine w schleife(inside)
for(float t = 0.0; t < tMax;)
      {	
		vec4 value;
		float scalar;
		
		
			//Texture 1
			
			if(all(greaterThanEqual(pos,lowBounds))
			&& all(lessThanEqual(pos,highBounds)))
			{
				//if(isPointOnTheLineZ(end,start,pos))
				if(clipped==1);
					clip(0,pos);
		
				if(all(lessThan(clipMin,pos))||(clipped==0))
				{

					value=texture3D(dataSetTexture,pos);
					scalar=scalarFromValue(value);
					// opacity is the sampled texture value in the 1D opacity texture at scalarValue
 
					opacity=texture1D(opacityTexture,scalar);//Mehdi

					if(opacity.a>0.0)
					{
						color=shade(0,value);
						color=color*opacity.a;
						destColor=destColor+color*remainOpacity;
						remainOpacity=remainOpacity*(1.0-opacity.a);
					}
				}
			}
			
				
						
			pos=pos+rayDir;
			t+=1.0;
			inside=(t<tMax) && (remainOpacity>=0.0039)
			&& (all(greaterThanEqual(pos,lowBounds))
			&& all(lessThanEqual(pos,highBounds))); 
	if(!inside)
	{
		break;
	}
	}
	
	
	gl_FragColor = destColor;
	gl_FragColor.a = 1.0-remainOpacity;

}
